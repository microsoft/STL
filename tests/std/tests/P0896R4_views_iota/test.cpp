// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <functional>
#include <ranges>
#include <type_traits>
#include <utility>

using namespace std;

template <class W, class B>
concept CanViewIota = requires(W w, B b) {
    views::iota(w, b);
};

template <class R>
concept CanSize = requires(R& r) {
    ranges::size(r);
};

struct empty_type {};

template <class T>
constexpr void test_integral() {
    constexpr T low        = 0;
    constexpr T high       = 8;
    constexpr T expected[] = {0, 1, 2, 3, 4, 5, 6, 7};

    {
        // Validate bounded (both upper and lower bounds exist) iota_view
        using R = ranges::iota_view<T, T>;

        // Validate type properties
        static_assert(same_as<ranges::range_value_t<R>, T>);
        static_assert(same_as<ranges::range_reference_t<R>, T>);

        static_assert(ranges::random_access_range<R>);
        static_assert(!ranges::contiguous_range<R>);
        static_assert(ranges::common_range<R>);

        static_assert(ranges::view<R>);
        static_assert(semiregular<R>);
        static_assert(is_nothrow_copy_constructible_v<R>);
        static_assert(is_nothrow_copy_assignable_v<R>);
        static_assert(is_nothrow_move_constructible_v<R>);
        static_assert(is_nothrow_move_assignable_v<R>);

        if constexpr (sizeof(T) < sizeof(int)) {
            static_assert(same_as<ranges::range_difference_t<R>, int>);
        } else {
            static_assert(same_as<ranges::range_difference_t<R>, long long>);
        }

        // iota_view is always a simple-view, i.e., const and non-const are always valid ranges with the same iterators:
        static_assert(ranges::common_range<const R>);
        static_assert(
            same_as<ranges::iterator_t<const ranges::iota_view<T, T>>, ranges::iterator_t<ranges::iota_view<T, T>>>);

        static_assert(same_as<ranges::range_reference_t<const R>, T>);
        static_assert(same_as<ranges::range_difference_t<const R>, ranges::range_difference_t<R>>);

        const same_as<R> auto rng = views::iota(low, high);
        static_assert(noexcept(views::iota(low, high))); // strengthened

        assert(ranges::equal(rng, expected));
        static_assert(noexcept(rng.begin())); // strengthened
        static_assert(noexcept(rng.end())); // strengthened

        assert(rng.size() == 8u);
        static_assert(noexcept(rng.size() == 8u)); // strengthened

        using I = ranges::iterator_t<R>;
        static_assert(same_as<typename I::iterator_concept, random_access_iterator_tag>);
        static_assert(same_as<typename iterator_traits<I>::iterator_category, input_iterator_tag>);

        assert(I{} == I{T{0}});
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

        assert(*first == T{0});
        assert(*second == T{1});
        static_assert(noexcept(*first));

        assert(first == I{T{0}});
        assert(second == I{T{1}});
        static_assert(noexcept(noexcept(I{T{0}}))); // strengthened

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
        assert(last - first == 8);
        static_assert(noexcept(last - first)); // strengthened
    }

    {
        // Validate unbounded (upper bound is unreachable) iota_view
        using R = ranges::iota_view<T>;
        static_assert(same_as<ranges::range_value_t<R>, T>);
        static_assert(same_as<ranges::range_reference_t<R>, T>);

        static_assert(ranges::random_access_range<R>);
        static_assert(!ranges::contiguous_range<R>);
        static_assert(!ranges::common_range<R>);
        static_assert(same_as<ranges::sentinel_t<ranges::iota_view<T>>, unreachable_sentinel_t>);

        // Bounded and unbounded iota_view have the same iterator type, however:
        static_assert(same_as<ranges::iterator_t<ranges::iota_view<T, T>>, ranges::iterator_t<ranges::iota_view<T>>>);

        if constexpr (sizeof(T) < sizeof(int)) {
            static_assert(same_as<ranges::range_difference_t<R>, int>);
        } else {
            static_assert(same_as<ranges::range_difference_t<R>, long long>);
        }

        {
            const same_as<R> auto rng = views::iota(low);

            auto i = low;
            for (const auto& e : rng) {
                assert(e == i);
                if (++i == high) {
                    break;
                }
            }
        }

        static_assert(!CanSize<ranges::iota_view<T>>);
    }
}

int main() {
    // Validate standard signed integer types
    static_assert((test_integral<signed char>(), true));
    test_integral<signed char>();
    static_assert((test_integral<short>(), true));
    test_integral<short>();
    static_assert((test_integral<int>(), true));
    test_integral<int>();
    static_assert((test_integral<long>(), true));
    test_integral<long>();
    static_assert((test_integral<long long>(), true));
    test_integral<long long>();

    // Validate standard unsigned integer types
    static_assert((test_integral<unsigned char>(), true));
    test_integral<unsigned char>();
    static_assert((test_integral<unsigned short>(), true));
    test_integral<unsigned short>();
    static_assert((test_integral<unsigned int>(), true));
    test_integral<unsigned int>();
    static_assert((test_integral<unsigned long>(), true));
    test_integral<unsigned long>();
    static_assert((test_integral<unsigned long long>(), true));
    test_integral<unsigned long long>();

    // Validate other integer types
#ifndef __clang__ // TRANSITION, LLVM-48173
    static_assert(!CanViewIota<bool, bool>);
#endif // TRANSITION, LLVM-48173
    static_assert((test_integral<char>(), true));
    test_integral<char>();
    static_assert((test_integral<wchar_t>(), true));
    test_integral<wchar_t>();
#ifdef __cpp_char8_t
    static_assert((test_integral<char8_t>(), true));
    test_integral<char8_t>();
#endif // __cpp_char8_t
    static_assert((test_integral<char16_t>(), true));
    test_integral<char16_t>();
    static_assert((test_integral<char32_t>(), true));
    test_integral<char32_t>();

    // Some non-integer coverage:
    {
        // Pointers
        empty_type objects[] = {{}, {}, {}};
        const auto address   = [](auto& x) { return &x; };
        assert(ranges::equal(
            views::iota(begin(as_const(objects)), end(objects)), objects, ranges::equal_to{}, identity{}, address));
        assert(ranges::equal(
            views::iota(begin(objects), end(as_const(objects))), objects, ranges::equal_to{}, identity{}, address));
    }
    {
        // Iterator and sentinel of a non-common range
        const int some_ints[] = {0, 1, 2, 3, 4, 5, 6, 7};
        const int even_ints[] = {0, 2, 4, 6};

        const auto even  = [](int x) { return x % 2 == 0; };
        const auto deref = [](auto x) -> decltype(auto) { return *x; };

        auto f = some_ints | views::filter(even);
        auto r = views::iota(ranges::begin(f), ranges::end(f));

        assert(ranges::equal(r, even_ints, ranges::equal_to{}, deref));
    }
}
