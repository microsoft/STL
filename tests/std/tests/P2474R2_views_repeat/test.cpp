// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <ranges>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

using namespace std;

template <class T, class B>
concept CanViewRepeat = requires(T&& v, B b) { views::repeat(forward<T>(v), b); };

template <class R>
concept CanSize = requires(R& r) { ranges::size(r); };

template <class R>
concept CanTakeDrop = requires(R r) {
    forward<R>(r) | views::take(1);
    forward<R>(r) | views::drop(1);
};

struct non_default {
    int value{};
    constexpr non_default(int v) : value(v) {}
    constexpr bool operator==(const non_default&) const = default;
};
struct move_only {
    int value{};
    constexpr move_only(int v) : value(v) {}
    move_only(move_only&&)                            = default;
    move_only& operator=(move_only&&)                 = default;
    constexpr bool operator==(const move_only&) const = default;
};

template <class T>
constexpr conditional_t<!is_copy_constructible_v<T> && is_move_constructible_v<T>, T&&, const T&> //
    move_if_move_only(T& val) {
    return move(val);
}

template <class T, class B = unreachable_sentinel_t>
constexpr void test_common(T val, B bound = unreachable_sentinel) {
    constexpr bool bounded = !same_as<B, unreachable_sentinel_t>;
    using R                = ranges::repeat_view<T, B>;

    static_assert(same_as<ranges::range_value_t<R>, T>);
    static_assert(same_as<ranges::range_reference_t<R>, const T&>);
    static_assert(same_as<ranges::range_reference_t<const R>, const T&>);

    static_assert(ranges::random_access_range<R>);
    static_assert(!ranges::contiguous_range<R>);
    static_assert(ranges::common_range<R> == bounded);

    static_assert(ranges::view<R>);
    static_assert(movable<R> == movable<T>);
    static_assert(copy_constructible<R> == copy_constructible<T>);
    static_assert(copyable<R> == copyable<T>);
    static_assert(default_initializable<R> == default_initializable<T>);
    static_assert(is_nothrow_copy_constructible_v<R> == is_nothrow_copy_constructible_v<T>); // strengthened
    static_assert(is_nothrow_copy_assignable_v<R> == is_nothrow_copy_assignable_v<T>); // strengthened
    static_assert(is_nothrow_move_constructible_v<R> == is_nothrow_move_constructible_v<T>); // strengthened
    static_assert(is_nothrow_move_assignable_v<R> == is_nothrow_move_assignable_v<T>); // strengthened

    if constexpr (!bounded) {
        static_assert(same_as<ranges::range_difference_t<R>, ptrdiff_t>);
        static_assert(same_as<ranges::sentinel_t<R>, unreachable_sentinel_t>);
    } else if constexpr (_Signed_integer_like<B>) {
        static_assert(same_as<ranges::range_difference_t<R>, B>);
    } else {
        static_assert(same_as<ranges::range_difference_t<R>, ranges::_Iota_diff_t<B>>);
    }

    static_assert(ranges::common_range<const R> == bounded);
    static_assert(same_as<ranges::iterator_t<const R>, ranges::iterator_t<R>>);

    static_assert(same_as<ranges::range_difference_t<const R>, ranges::range_difference_t<R>>);

    static_assert(CanSize<R> == bounded);

    same_as<R> auto rng = views::repeat(move_if_move_only(val), bound);
    if constexpr (copyable<T>) {
        static_assert(noexcept(views::repeat(val, bound)) == is_nothrow_copy_constructible_v<T>); // strengthened
        static_assert(noexcept(rng | views::drop(1)) == is_nothrow_copy_constructible_v<T>); // strengthened
        static_assert(noexcept(rng | views::take(1)) == is_nothrow_copy_constructible_v<T>); // strengthened
    }
    static_assert(noexcept(views::repeat(move(val), bound)) == is_nothrow_move_constructible_v<T>); // strengthened
    static_assert(noexcept(move(rng) | views::drop(1)) == is_nothrow_move_constructible_v<T>); // strengthened
    static_assert(noexcept(move(rng) | views::take(1)) == is_nothrow_move_constructible_v<T>); // strengthened

    if constexpr (bounded) {
        B i = 0;
        for (const auto& v : rng) {
            assert(v == val);
            ++i;
        }
        assert(i == bound);

        static_assert(noexcept(rng.end())); // strengthened
        assert(cmp_equal(rng.size(), bound));
        static_assert(noexcept(rng.size())); // strengthened

        constexpr int amount   = 3;
        const auto dist        = ranges::distance(rng);
        const auto take_amount = ranges::min(dist, amount);

        const same_as<R> auto take = rng | views::take(amount);
        assert(cmp_equal(take.size(), take_amount));

        const auto drop_amount     = dist - ranges::min(dist, amount);
        const same_as<R> auto drop = rng | views::drop(amount);
        assert(cmp_equal(drop.size(), drop_amount));
    } else {
        auto it = rng.begin();
        for (int i = 0; i < 10; ++i) {
            assert(*it == val);
            ++it;
        }
        static_assert(noexcept(rng.end()));

        using ReconR = ranges::repeat_view<T, ranges::range_difference_t<R>>;

        const same_as<ReconR> auto take = move_if_move_only(rng) | views::take(3);
        assert(take.size() == 3);

        const same_as<R> auto drop = move_if_move_only(rng) | views::drop(3);
        static_assert(!CanSize<decltype(drop)>);
    }

    static_assert(noexcept(rng.begin())); // strengthened
    using I = ranges::iterator_t<R>;
    static_assert(same_as<typename I::iterator_concept, random_access_iterator_tag>);
    static_assert(same_as<typename I::iterator_category, random_access_iterator_tag>);

    static_assert(is_nothrow_default_constructible_v<I>);
    assert(I{} == I{});
    assert(!(I{} != I{}));
    assert(!(I{} < I{}));
    assert(!(I{} > I{}));
    assert(I{} <= I{});
    assert(I{} >= I{});
    assert(I{} <=> I{} == 0);
    assert(I{} - I{} == 0);

    const I first  = rng.begin();
    const I second = ranges::next(first);

    assert(*first == *second);
    assert(&*first == &*second);
    static_assert(noexcept(*first));

    {
        I tmp = first;
        static_assert(is_nothrow_copy_constructible_v<I>);

        I i = move(tmp);
        static_assert(is_nothrow_move_constructible_v<I>);

        tmp = i;
        static_assert(is_nothrow_copy_assignable_v<I>);

        i = move(tmp);
        static_assert(is_nothrow_move_assignable_v<I>);
    }

    assert(!(first == second));
    assert(!(second == first));
    static_assert(noexcept(first == second)); // strengthened

    assert(first != second);
    assert(second != first);
    static_assert(noexcept(first != second)); // strengthened

    assert(first < second);
    assert(!(second < first));
    static_assert(noexcept(first < second)); // strengthened

    assert(!(first > second));
    assert(second > first);
    static_assert(noexcept(first > second)); // strengthened

    assert(first <= second);
    assert(!(second <= first));
    static_assert(noexcept(first <= second)); // strengthened

    assert(!(first >= second));
    assert(second >= first);
    static_assert(noexcept(first >= second)); // strengthened

    assert((first <=> second) < 0);
    assert((second <=> first) > 0);
    static_assert(noexcept(first <=> second)); // strengthened

    {
        I i = first;

        assert(&++i == &i);
        assert(i == second);
        static_assert(noexcept(++i)); // strengthened

        i = first;

        assert(i++ == first);
        assert(i == second);
        static_assert(noexcept(i++)); // strengthened

        assert(&--i == &i);
        assert(i == first);
        static_assert(noexcept(--i)); // strengthened

        i = second;

        assert(i-- == second);
        assert(i == first);
        static_assert(noexcept(i--)); // strengthened

        assert(i + 1 == second);
        static_assert(noexcept(i + 1)); // strengthened

        assert(1 + i == second);
        static_assert(noexcept(1 + i)); // strengthened

        assert(&(i += 1) == &i);
        assert(i == second);
        static_assert(noexcept(i += 1)); // strengthened
        assert(i - 1 == first);
        static_assert(noexcept(i - 1)); // strengthened

        assert(&(i -= 1) == &i);
        assert(i == first);
        static_assert(noexcept(i -= 1)); // strengthened

        assert(second - first == 1);
        static_assert(noexcept(second - first)); // strengthened

        assert(first[1] == *second);
        assert(second[-1] == *first);
        static_assert(noexcept(first[1])); // strengthened
    }

    const same_as<ranges::sentinel_t<R>> auto last = rng.end();
    const bool is_empty                            = bound == 0;
    assert((first == last) == is_empty);
    static_assert(noexcept(first == last)); // strengthened
    assert((first != last) != is_empty);
    static_assert(noexcept(first != last)); // strengthened
    if constexpr (bounded) {
        assert(cmp_equal(last - first, rng.size()));
        static_assert(noexcept(last - first)); // strengthened
    }

    const same_as<ranges::const_iterator_t<R>> auto cfirst = rng.cbegin();
    assert(cfirst == first);
    const same_as<ranges::const_sentinel_t<R>> auto clast = rng.cend();
    if constexpr (ranges::common_range<R>) {
        assert(clast == last);
        assert(cmp_equal(clast - cfirst, rng.size()));
    } else {
        static_assert(same_as<remove_const_t<decltype(clast)>, unreachable_sentinel_t>);
    }
}

struct move_tester {
    int x         = -1;
    move_tester() = default;
    constexpr move_tester(move_tester&& other) noexcept {
        other.x = 1;
    }
};

struct forward_tester {
    int x            = -1;
    forward_tester() = default;
    constexpr forward_tester(forward_tester& other) : x(1) {
        other.x = 1;
    }
    constexpr forward_tester(forward_tester&& other) : x(2) {
        other.x = 2;
    }
};

struct tuple_tester {
    forward_tester y;
    forward_tester z;
};

struct rvalue_only_to_sentinel {
    template <class Sent>
    constexpr operator Sent() && noexcept {
        if constexpr (is_same_v<Sent, unreachable_sentinel_t>) {
            return {};
        } else {
            return Sent{1};
        }
    }
};


template <class IntLike>
constexpr void test_iterator_arithmetic() {
    auto rv    = views::repeat(0, IntLike{20u});
    auto first = rv.begin();
    auto last  = rv.end();
    assert(last - first == 20);
    first += 2;
    last -= 3;
    assert(last - first == 15);
}

template <class T>
constexpr void test_piecewise_construction_from_empty_braces() {
    ranges::repeat_view<T> r{piecewise_construct, {}, {}};
    assert(r.front() == T{});
}

#ifndef _M_CEE // TRANSITION, VSO-1659496
template <class T>
struct holder {
    T t;
};

struct incomplete;

struct adl_validator {
    constexpr explicit adl_validator(holder<incomplete>*) noexcept {}
};

constexpr void test_piecewise_construction_for_adl() {
    constexpr holder<incomplete>* arg_ptr = nullptr;
    // Intentionally std::-qualify make_tuple to avoid ADL.
    {
        (void) ranges::repeat_view<adl_validator>{piecewise_construct, std::make_tuple(arg_ptr)};
    }
    {
        ranges::repeat_view<adl_validator, ptrdiff_t> r{piecewise_construct, std::make_tuple(arg_ptr)};
        assert(r.size() == 0);
    }
    {
        ranges::repeat_view<adl_validator, ptrdiff_t> r{
            piecewise_construct, std::make_tuple(arg_ptr), std::make_tuple(ptrdiff_t{42})};
        assert(r.size() == 42);
    }
}
#endif // ^^^ no workaround ^^^

template <class Sent>
constexpr void test_make_sentinel_from_rvalue() {
    ranges::repeat_view<int, Sent> r{piecewise_construct, make_tuple(42), make_tuple(rvalue_only_to_sentinel{})};
    assert(r.front() == 42);
    if constexpr (!is_same_v<Sent, unreachable_sentinel_t>) {
        assert(r.size() == 1);
    }
}

constexpr bool test() {
    using namespace string_literals;

    test_common(7, 5);
    test_common(7, 2);
    test_common(7, 0);
    test_common(3);
    test_common<non_default>(3);
    test_common<move_only>(3);
    test_common("woof"s, 5);
    test_common("woof"s, 2);
    test_common("meow"s);

    static_assert(!CanViewRepeat<const move_tester&, int>);
    static_assert(CanViewRepeat<move_tester&&, int>);
    static_assert(CanSize<ranges::repeat_view<int, _Signed128>>);

    {
        move_tester to_move;
        (void) views::repeat(move(to_move));
        assert(to_move.x == 1);
        using repeat_move = ranges::repeat_view<move_tester>;
        static_assert(CanTakeDrop<repeat_move>);
        static_assert(!CanTakeDrop<const repeat_move&>);
    }
    {
        forward_tester to_copy;
        forward_tester to_move;
        ranges::repeat_view<tuple_tester> r(piecewise_construct, forward_as_tuple(to_copy, move(to_move)));
        const auto& i = *r.begin();
        assert(i.y.x == 1);
        assert(i.z.x == 2);
        assert(to_copy.x == 1);
        assert(to_move.x == 2);
    }

    // GH-4251: <ranges>: repeat_view<T, unsigned int> emits truncation warnings
    test_iterator_arithmetic<unsigned char>();
    test_iterator_arithmetic<unsigned short>();
    test_iterator_arithmetic<unsigned int>();
    test_iterator_arithmetic<unsigned long>();
    test_iterator_arithmetic<unsigned long long>();
    test_iterator_arithmetic<signed char>();
    test_iterator_arithmetic<short>();
    test_iterator_arithmetic<int>();
    test_iterator_arithmetic<long>();
    test_iterator_arithmetic<long long>();
    test_iterator_arithmetic<char>();
#ifdef __cpp_char8_t
    test_iterator_arithmetic<char8_t>();
#endif // defined(__cpp_char8_t)
    test_iterator_arithmetic<char16_t>();
    test_iterator_arithmetic<char32_t>();
    test_iterator_arithmetic<wchar_t>();
    test_iterator_arithmetic<_Signed128>();

    // GH-5387: "<ranges>: Cannot construct repeat_view via {piecewise_construct, {}, {}}"
    test_piecewise_construction_from_empty_braces<int>();
    test_piecewise_construction_from_empty_braces<string>();
#ifndef _M_CEE // TRANSITION, VSO-1659496
    test_piecewise_construction_for_adl();
#endif // ^^^ no workaround ^^^
    test_make_sentinel_from_rvalue<int>();
    test_make_sentinel_from_rvalue<_Signed128>();
    test_make_sentinel_from_rvalue<unreachable_sentinel_t>();

    return true;
}

// Check LWG-3875 "std::ranges::repeat_view<T, IntegerClass>::iterator may be ill-formed"
static_assert(CanViewRepeat<string, long long>);
static_assert(CanViewRepeat<string, unsigned long long>);
static_assert(CanViewRepeat<string, _Signed128>);
static_assert(
    !CanViewRepeat<string, _Unsigned128>); // _Unsigned128 does not satisfy 'integer-like-with-usable-difference-type'

// Check LWG-4053 "Unary call to std::views::repeat does not decay the argument" (affects CTAD only due to LWG-4054)
static_assert(is_same_v<decltype(ranges::repeat_view(ranges::repeat_view(42))), ranges::repeat_view<int>>);
static_assert(is_same_v<decltype(ranges::repeat_view("Hello world!")), ranges::repeat_view<const char*>>);
static_assert(is_same_v<decltype(ranges::repeat_view(test)), ranges::repeat_view<bool (*)()>>);

// Check LWG-4054 "Repeating a repeat_view should repeat the view"
static_assert(is_same_v<decltype(views::repeat(views::repeat(42))), ranges::repeat_view<ranges::repeat_view<int>>>);
static_assert(is_same_v<decltype(views::repeat("Hello world!")), ranges::repeat_view<const char*>>);
static_assert(is_same_v<decltype(views::repeat(test)), ranges::repeat_view<bool (*)()>>);

// Check GH-3392 "<ranges>: views::repeat(...) | views::take(...) is not always a valid range"
static_assert(ranges::range<decltype(views::repeat('3', 100ull) | views::take(3))>);
static_assert(ranges::range<decltype(views::repeat('3', 100ull) | views::drop(3))>);

int main() {
    assert(test());
    static_assert(test());
}
