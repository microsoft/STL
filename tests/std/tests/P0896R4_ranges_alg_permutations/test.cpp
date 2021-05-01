// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstdio>
#include <random>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;

#pragma warning(disable : 6294) // Ill-defined for-loop: initial condition does not satisfy test. Loop body not executed

// Validate that (next|prev)_permutation_result alias in_found_result
STATIC_ASSERT(same_as<ranges::next_permutation_result<int>, ranges::in_found_result<int>>);
STATIC_ASSERT(same_as<ranges::prev_permutation_result<int>, ranges::in_found_result<int>>);

// Validate dangling story
STATIC_ASSERT(
    same_as<decltype(ranges::next_permutation(borrowed<false>{})), ranges::next_permutation_result<ranges::dangling>>);
STATIC_ASSERT(same_as<decltype(ranges::next_permutation(borrowed<true>{})), ranges::next_permutation_result<int*>>);
STATIC_ASSERT(
    same_as<decltype(ranges::prev_permutation(borrowed<false>{})), ranges::prev_permutation_result<ranges::dangling>>);
STATIC_ASSERT(same_as<decltype(ranges::prev_permutation(borrowed<true>{})), ranges::prev_permutation_result<int*>>);

constexpr int perm1[][1] = {{0}};
constexpr int perm4[][4] = {{0, 1, 2, 3}, {0, 1, 3, 2}, {0, 2, 1, 3}, {0, 2, 3, 1}, {0, 3, 1, 2}, {0, 3, 2, 1},
    {1, 0, 2, 3}, {1, 0, 3, 2}, {1, 2, 0, 3}, {1, 2, 3, 0}, {1, 3, 0, 2}, {1, 3, 2, 0}, {2, 0, 1, 3}, {2, 0, 3, 1},
    {2, 1, 0, 3}, {2, 1, 3, 0}, {2, 3, 0, 1}, {2, 3, 1, 0}, {3, 0, 1, 2}, {3, 0, 2, 1}, {3, 1, 0, 2}, {3, 1, 2, 0},
    {3, 2, 0, 1}, {3, 2, 1, 0}};
constexpr int perm5[][5] = {{0, 1, 2, 3, 4}, {0, 1, 2, 4, 3}, {0, 1, 3, 2, 4}, {0, 1, 3, 4, 2}, {0, 1, 4, 2, 3},
    {0, 1, 4, 3, 2}, {0, 2, 1, 3, 4}, {0, 2, 1, 4, 3}, {0, 2, 3, 1, 4}, {0, 2, 3, 4, 1}, {0, 2, 4, 1, 3},
    {0, 2, 4, 3, 1}, {0, 3, 1, 2, 4}, {0, 3, 1, 4, 2}, {0, 3, 2, 1, 4}, {0, 3, 2, 4, 1}, {0, 3, 4, 1, 2},
    {0, 3, 4, 2, 1}, {0, 4, 1, 2, 3}, {0, 4, 1, 3, 2}, {0, 4, 2, 1, 3}, {0, 4, 2, 3, 1}, {0, 4, 3, 1, 2},
    {0, 4, 3, 2, 1}, {1, 0, 2, 3, 4}, {1, 0, 2, 4, 3}, {1, 0, 3, 2, 4}, {1, 0, 3, 4, 2}, {1, 0, 4, 2, 3},
    {1, 0, 4, 3, 2}, {1, 2, 0, 3, 4}, {1, 2, 0, 4, 3}, {1, 2, 3, 0, 4}, {1, 2, 3, 4, 0}, {1, 2, 4, 0, 3},
    {1, 2, 4, 3, 0}, {1, 3, 0, 2, 4}, {1, 3, 0, 4, 2}, {1, 3, 2, 0, 4}, {1, 3, 2, 4, 0}, {1, 3, 4, 0, 2},
    {1, 3, 4, 2, 0}, {1, 4, 0, 2, 3}, {1, 4, 0, 3, 2}, {1, 4, 2, 0, 3}, {1, 4, 2, 3, 0}, {1, 4, 3, 0, 2},
    {1, 4, 3, 2, 0}, {2, 0, 1, 3, 4}, {2, 0, 1, 4, 3}, {2, 0, 3, 1, 4}, {2, 0, 3, 4, 1}, {2, 0, 4, 1, 3},
    {2, 0, 4, 3, 1}, {2, 1, 0, 3, 4}, {2, 1, 0, 4, 3}, {2, 1, 3, 0, 4}, {2, 1, 3, 4, 0}, {2, 1, 4, 0, 3},
    {2, 1, 4, 3, 0}, {2, 3, 0, 1, 4}, {2, 3, 0, 4, 1}, {2, 3, 1, 0, 4}, {2, 3, 1, 4, 0}, {2, 3, 4, 0, 1},
    {2, 3, 4, 1, 0}, {2, 4, 0, 1, 3}, {2, 4, 0, 3, 1}, {2, 4, 1, 0, 3}, {2, 4, 1, 3, 0}, {2, 4, 3, 0, 1},
    {2, 4, 3, 1, 0}, {3, 0, 1, 2, 4}, {3, 0, 1, 4, 2}, {3, 0, 2, 1, 4}, {3, 0, 2, 4, 1}, {3, 0, 4, 1, 2},
    {3, 0, 4, 2, 1}, {3, 1, 0, 2, 4}, {3, 1, 0, 4, 2}, {3, 1, 2, 0, 4}, {3, 1, 2, 4, 0}, {3, 1, 4, 0, 2},
    {3, 1, 4, 2, 0}, {3, 2, 0, 1, 4}, {3, 2, 0, 4, 1}, {3, 2, 1, 0, 4}, {3, 2, 1, 4, 0}, {3, 2, 4, 0, 1},
    {3, 2, 4, 1, 0}, {3, 4, 0, 1, 2}, {3, 4, 0, 2, 1}, {3, 4, 1, 0, 2}, {3, 4, 1, 2, 0}, {3, 4, 2, 0, 1},
    {3, 4, 2, 1, 0}, {4, 0, 1, 2, 3}, {4, 0, 1, 3, 2}, {4, 0, 2, 1, 3}, {4, 0, 2, 3, 1}, {4, 0, 3, 1, 2},
    {4, 0, 3, 2, 1}, {4, 1, 0, 2, 3}, {4, 1, 0, 3, 2}, {4, 1, 2, 0, 3}, {4, 1, 2, 3, 0}, {4, 1, 3, 0, 2},
    {4, 1, 3, 2, 0}, {4, 2, 0, 1, 3}, {4, 2, 0, 3, 1}, {4, 2, 1, 0, 3}, {4, 2, 1, 3, 0}, {4, 2, 3, 0, 1},
    {4, 2, 3, 1, 0}, {4, 3, 0, 1, 2}, {4, 3, 0, 2, 1}, {4, 3, 1, 0, 2}, {4, 3, 1, 2, 0}, {4, 3, 2, 0, 1},
    {4, 3, 2, 1, 0}};
constexpr int perm6[][6] = {{0, 0, 1, 1, 2, 2}, {0, 0, 1, 2, 1, 2}, {0, 0, 1, 2, 2, 1}, {0, 0, 2, 1, 1, 2},
    {0, 0, 2, 1, 2, 1}, {0, 0, 2, 2, 1, 1}, {0, 1, 0, 1, 2, 2}, {0, 1, 0, 2, 1, 2}, {0, 1, 0, 2, 2, 1},
    {0, 1, 1, 0, 2, 2}, {0, 1, 1, 2, 0, 2}, {0, 1, 1, 2, 2, 0}, {0, 1, 2, 0, 1, 2}, {0, 1, 2, 0, 2, 1},
    {0, 1, 2, 1, 0, 2}, {0, 1, 2, 1, 2, 0}, {0, 1, 2, 2, 0, 1}, {0, 1, 2, 2, 1, 0}, {0, 2, 0, 1, 1, 2},
    {0, 2, 0, 1, 2, 1}, {0, 2, 0, 2, 1, 1}, {0, 2, 1, 0, 1, 2}, {0, 2, 1, 0, 2, 1}, {0, 2, 1, 1, 0, 2},
    {0, 2, 1, 1, 2, 0}, {0, 2, 1, 2, 0, 1}, {0, 2, 1, 2, 1, 0}, {0, 2, 2, 0, 1, 1}, {0, 2, 2, 1, 0, 1},
    {0, 2, 2, 1, 1, 0}, {1, 0, 0, 1, 2, 2}, {1, 0, 0, 2, 1, 2}, {1, 0, 0, 2, 2, 1}, {1, 0, 1, 0, 2, 2},
    {1, 0, 1, 2, 0, 2}, {1, 0, 1, 2, 2, 0}, {1, 0, 2, 0, 1, 2}, {1, 0, 2, 0, 2, 1}, {1, 0, 2, 1, 0, 2},
    {1, 0, 2, 1, 2, 0}, {1, 0, 2, 2, 0, 1}, {1, 0, 2, 2, 1, 0}, {1, 1, 0, 0, 2, 2}, {1, 1, 0, 2, 0, 2},
    {1, 1, 0, 2, 2, 0}, {1, 1, 2, 0, 0, 2}, {1, 1, 2, 0, 2, 0}, {1, 1, 2, 2, 0, 0}, {1, 2, 0, 0, 1, 2},
    {1, 2, 0, 0, 2, 1}, {1, 2, 0, 1, 0, 2}, {1, 2, 0, 1, 2, 0}, {1, 2, 0, 2, 0, 1}, {1, 2, 0, 2, 1, 0},
    {1, 2, 1, 0, 0, 2}, {1, 2, 1, 0, 2, 0}, {1, 2, 1, 2, 0, 0}, {1, 2, 2, 0, 0, 1}, {1, 2, 2, 0, 1, 0},
    {1, 2, 2, 1, 0, 0}, {2, 0, 0, 1, 1, 2}, {2, 0, 0, 1, 2, 1}, {2, 0, 0, 2, 1, 1}, {2, 0, 1, 0, 1, 2},
    {2, 0, 1, 0, 2, 1}, {2, 0, 1, 1, 0, 2}, {2, 0, 1, 1, 2, 0}, {2, 0, 1, 2, 0, 1}, {2, 0, 1, 2, 1, 0},
    {2, 0, 2, 0, 1, 1}, {2, 0, 2, 1, 0, 1}, {2, 0, 2, 1, 1, 0}, {2, 1, 0, 0, 1, 2}, {2, 1, 0, 0, 2, 1},
    {2, 1, 0, 1, 0, 2}, {2, 1, 0, 1, 2, 0}, {2, 1, 0, 2, 0, 1}, {2, 1, 0, 2, 1, 0}, {2, 1, 1, 0, 0, 2},
    {2, 1, 1, 0, 2, 0}, {2, 1, 1, 2, 0, 0}, {2, 1, 2, 0, 0, 1}, {2, 1, 2, 0, 1, 0}, {2, 1, 2, 1, 0, 0},
    {2, 2, 0, 0, 1, 1}, {2, 2, 0, 1, 0, 1}, {2, 2, 0, 1, 1, 0}, {2, 2, 1, 0, 0, 1}, {2, 2, 1, 0, 1, 0},
    {2, 2, 1, 1, 0, 0}};
constexpr int perm8[][8] = {{0, 0, 0, 0, 0, 0, 0, 0}};

struct int_wrapper {
    int val = 10;

    constexpr int_wrapper() = default;
    constexpr int_wrapper(int x) : val{x} {}
    constexpr int_wrapper(const int_wrapper&) = default;
    constexpr int_wrapper(int_wrapper&& that) : val{exchange(that.val, -1)} {}
    constexpr int_wrapper& operator=(const int_wrapper&) = default;

    constexpr int_wrapper& operator=(int_wrapper&& that) {
        val = exchange(that.val, -1);
        return *this;
    }
    auto operator<=>(const int_wrapper&) const = default;
};

constexpr auto get_val = [](auto&& x) { return static_cast<const int_wrapper&>(x).val; };

template <auto& Expected>
struct next_perm_instantiator {
    template <ranges::bidirectional_range R>
    static constexpr void call() {
        using ranges::next_permutation, ranges::next_permutation_result, ranges::equal, ranges::iterator_t;

        constexpr auto number_of_permutations = static_cast<int>(ranges::size(Expected));

        { // Validate range overload
            int_wrapper input[ranges::size(Expected[0])];
            ranges::copy(Expected[0], input);

            for (int i = 1; i < number_of_permutations; ++i) {
                R range{input};
                const same_as<next_permutation_result<iterator_t<R>>> auto result =
                    next_permutation(range, ranges::less{}, get_val);
                assert(result.in == range.end());
                assert(result.found);
                assert(equal(input, Expected[i], ranges::equal_to{}, get_val));
            }

            R range{input};
            const same_as<next_permutation_result<iterator_t<R>>> auto result =
                next_permutation(range, ranges::less{}, get_val);
            assert(result.in == range.end());
            assert(!result.found);
            assert(equal(input, Expected[0]));
        }

        { // Validate iterator overload
            int_wrapper input[ranges::size(Expected[0])];
            ranges::copy(Expected[0], input);

            for (int i = 1; i < number_of_permutations; ++i) {
                R range{input};
                const same_as<next_permutation_result<iterator_t<R>>> auto result =
                    next_permutation(range.begin(), range.end(), ranges::less{}, get_val);
                assert(result.in == range.end());
                assert(result.found);
                assert(equal(input, Expected[i]));
            }

            R range{input};
            const same_as<next_permutation_result<iterator_t<R>>> auto result =
                next_permutation(range.begin(), range.end(), ranges::less{}, get_val);
            assert(result.in == range.end());
            assert(!result.found);
            assert(equal(input, Expected[0]));
        }
    }
};

template <auto& Expected>
struct prev_perm_instantiator {
    template <ranges::bidirectional_range R>
    static constexpr void call() {
        using ranges::prev_permutation, ranges::prev_permutation_result, ranges::equal, ranges::iterator_t;

        constexpr auto number_of_permutations = static_cast<int>(ranges::size(Expected));

        { // Validate range overload
            int_wrapper input[ranges::size(Expected[0])];
            ranges::copy(Expected[number_of_permutations - 1], input);

            for (int i = number_of_permutations - 1; i-- > 0;) {
                R range{input};
                const same_as<prev_permutation_result<iterator_t<R>>> auto result =
                    prev_permutation(range, ranges::less{}, get_val);
                assert(result.in == range.end());
                assert(result.found);
                assert(equal(input, Expected[i]));
            }

            R range{input};
            const same_as<prev_permutation_result<iterator_t<R>>> auto result =
                prev_permutation(range, ranges::less{}, get_val);
            assert(result.in == range.end());
            assert(!result.found);
            assert(equal(input, Expected[number_of_permutations - 1]));
        }

        { // Validate iterator overload
            int_wrapper input[ranges::size(Expected[0])];
            ranges::copy(Expected[number_of_permutations - 1], input);

            for (int i = number_of_permutations - 1; i-- > 0;) {
                R range{input};
                const same_as<prev_permutation_result<iterator_t<R>>> auto result =
                    prev_permutation(range.begin(), range.end(), ranges::less{}, get_val);
                assert(result.in == range.end());
                assert(result.found);
                assert(equal(input, Expected[i]));
            }

            R range{input};
            const same_as<prev_permutation_result<iterator_t<R>>> auto result =
                prev_permutation(range.begin(), range.end(), ranges::less{}, get_val);
            assert(result.in == range.end());
            assert(!result.found);
            assert(equal(input, Expected[number_of_permutations - 1]));
        }
    }
};

struct empty_range_test {
    template <ranges::bidirectional_range R>
    static constexpr void call() {
        using ranges::next_permutation, ranges::next_permutation_result, ranges::prev_permutation,
            ranges::prev_permutation_result, ranges::iterator_t;

        { // Validate range overload, next_permutation
            R range{};
            const same_as<next_permutation_result<iterator_t<R>>> auto result =
                next_permutation(range, ranges::less{}, get_val);
            assert(result.in == range.end());
            assert(!result.found);
        }

        { // Validate iterator overload, next_permutation
            R range{};
            const same_as<next_permutation_result<iterator_t<R>>> auto result =
                next_permutation(range.begin(), range.end(), ranges::less{}, get_val);
            assert(result.in == range.end());
            assert(!result.found);
        }

        { // Validate range overload, prev_permutation
            R range{};
            const same_as<prev_permutation_result<iterator_t<R>>> auto result =
                prev_permutation(range, ranges::less{}, get_val);
            assert(result.in == range.end());
            assert(!result.found);
        }

        { // Validate iterator overload, prev_permutation
            R range{};
            const same_as<prev_permutation_result<iterator_t<R>>> auto result =
                prev_permutation(range.begin(), range.end(), ranges::less{}, get_val);
            assert(result.in == range.end());
            assert(!result.found);
        }
    }
};

int main() {
    STATIC_ASSERT((test_bidi<empty_range_test, int_wrapper>(), true));
    test_bidi<empty_range_test, int_wrapper>();

    STATIC_ASSERT((test_bidi<next_perm_instantiator<perm1>, int_wrapper>(), true));
    STATIC_ASSERT((test_bidi<next_perm_instantiator<perm4>, int_wrapper>(), true));
    STATIC_ASSERT((test_bidi<next_perm_instantiator<perm8>, int_wrapper>(), true));
    test_bidi<next_perm_instantiator<perm1>, int_wrapper>();
    test_bidi<next_perm_instantiator<perm4>, int_wrapper>();
    test_bidi<next_perm_instantiator<perm5>, int_wrapper>();
    test_bidi<next_perm_instantiator<perm6>, int_wrapper>();
    test_bidi<next_perm_instantiator<perm8>, int_wrapper>();

    STATIC_ASSERT((test_bidi<prev_perm_instantiator<perm1>, int_wrapper>(), true));
    STATIC_ASSERT((test_bidi<prev_perm_instantiator<perm4>, int_wrapper>(), true));
    STATIC_ASSERT((test_bidi<prev_perm_instantiator<perm8>, int_wrapper>(), true));
    test_bidi<prev_perm_instantiator<perm1>, int_wrapper>();
    test_bidi<prev_perm_instantiator<perm4>, int_wrapper>();
    test_bidi<prev_perm_instantiator<perm5>, int_wrapper>();
    test_bidi<prev_perm_instantiator<perm6>, int_wrapper>();
    test_bidi<prev_perm_instantiator<perm8>, int_wrapper>();
}
