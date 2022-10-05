// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <limits>
#include <ranges>
#include <type_traits>
#include <utility>

using namespace std;

template <template <class> class Trait, class R, class T, class B>
constexpr bool nothrow_view() {
    return Trait<R>::value == Trait<T>::value && Trait<B>::value;
}
template <template <class> class Trait, class T, class B = int>
constexpr bool is_nothrow() {
    return Trait<T>::value && Trait<B>::value;
}

template <class W, class B>
concept CanViewRepeat = requires(W&& w, B b) {
    views::repeat(forward<W>(w), b);
};

template <class R>
concept CanSize = requires(R& r) {
    ranges::size(r);
};

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
    static_assert(semiregular<R>);
    static_assert(default_initializable<R> == default_initializable<T>);
    static_assert(nothrow_view<is_nothrow_copy_constructible, R, T, B>()); // strengthened
    static_assert(nothrow_view<is_nothrow_copy_assignable, R, T, B>()); // strengthened
    static_assert(nothrow_view<is_nothrow_move_constructible, R, T, B>()); // strengthened
    static_assert(nothrow_view<is_nothrow_move_assignable, R, T, B>()); // strengthened

    if constexpr (!bounded) {
        static_assert(same_as<ranges::range_difference_t<R>, ptrdiff_t>);
    } else if constexpr (_Signed_integer_like<B>) {
        static_assert(same_as<ranges::range_difference_t<R>, B>);
    } else {
        static_assert(same_as<ranges::range_difference_t<R>, ranges::_Iota_diff_t<B>>);
    }

    static_assert(ranges::common_range<const R> == bounded);
    static_assert(
        same_as<ranges::iterator_t<const ranges::repeat_view<T, B>>, ranges::iterator_t<ranges::repeat_view<T, B>>>);

    static_assert(same_as<ranges::range_difference_t<const R>, ranges::range_difference_t<R>>);
    static_assert(same_as<ranges::sentinel_t<ranges::repeat_view<T>>, unreachable_sentinel_t>);

    static_assert(CanSize<ranges::repeat_view<T, B>> == bounded);

    const same_as<R> auto rng = views::repeat(val, bound);
    static_assert(
        noexcept(views::repeat(val, bound)) == is_nothrow<is_nothrow_copy_constructible, T, B>()); // strengthened

    if constexpr (bounded) {
        B i = 0;
        for (const auto& v : rng) {
            assert(v == val);
            ++i;
        }
        assert(i == bound);

        static_assert(noexcept(rng.end()) == is_nothrow<is_nothrow_copy_constructible, B>()); // strengthened
        assert(cmp_equal(rng.size(), bound));
        static_assert(noexcept(rng.size()) == is_nothrow<is_nothrow_copy_constructible, B>()); // strengthened

        constexpr int amount   = 3;
        const auto size        = ranges::distance(rng);
        const auto take_amount = ranges::min(size, amount);

        const same_as<R> auto take = rng | views::take(amount);
        assert(cmp_equal(take.size(), take_amount));

        const auto drop_amount     = size - ranges::min(size, amount);
        const same_as<R> auto drop = rng | views::drop(amount);
        assert(cmp_equal(drop.size(), drop_amount));
    } else {
        static_assert(noexcept(rng.end()));

        using ReconR = ranges::repeat_view<T, ranges::range_difference_t<R>>;

        const same_as<ReconR> auto take = rng | views::take(3);
        assert(take.size() == 3);

        const same_as<R> auto drop = rng | views::drop(3);
        static_assert(!CanSize<decltype(drop)>);
    }

    static_assert(noexcept(rng.begin())); // strengthened
    using I = ranges::iterator_t<R>;
    static_assert(same_as<typename I::iterator_concept, random_access_iterator_tag>);

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

    assert(first <=> second < 0);
    assert(second <=> first > 0);
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
    assert(!(first == last));
    static_assert(noexcept(first == last)); // strengthened
    assert(first != last);
    static_assert(noexcept(first != last)); // strengthened
    if constexpr (bounded) {
        assert(cmp_equal(last - first, rng.size()));
        static_assert(noexcept(last - first)); // strengthened
    }
}

struct move_tester {
    int x         = -1;
    move_tester() = default;
    constexpr move_tester(move_tester&& other) {
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

    // clang doesn't support parenthesized initialization of aggregates
    // TRANSITION ???
    template <class T, class U>
    constexpr tuple_tester(T&& a, U&& b) : y{std::forward<T>(a)}, z{std::forward<U>(b)} {}
};

constexpr bool test() {
    using namespace std::string_literals;

    test_common(7, 5);
    test_common(7, 2);
    test_common(3);
    test_common("woof"s, 5);
    test_common("woof"s, 2);
    test_common("meow"s);

    static_assert(!CanViewRepeat<const move_tester&, int>);
    static_assert(CanViewRepeat<move_tester&&, int>);
    static_assert(CanSize<ranges::repeat_view<int, _Signed128>>);

    {
        move_tester to_move;
        (void) views::repeat(std::move(to_move));
        assert(to_move.x == 1);
    }
    {
        forward_tester to_copy;
        forward_tester to_move;
        ranges::repeat_view<tuple_tester> r(piecewise_construct, forward_as_tuple(to_copy, std::move(to_move)));
        const auto& i = *r.begin();
        assert(i.y.x == 1);
        assert(i.z.x == 2);
        assert(to_copy.x == 1);
        assert(to_move.x == 2);
    }
    return true;
}

int main() {

    assert(test());
    static_assert(test());
}
