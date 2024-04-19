// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <span>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;
using P = pair<int, int>;

// Validate dangling story
static_assert(same_as<decltype(ranges::unique(borrowed<false>{})), ranges::dangling>);
static_assert(same_as<decltype(ranges::unique(borrowed<true>{})), ranges::subrange<int*>>);

struct instantiator {
    static constexpr P expected[4] = {{0, 99}, {1, 47}, {3, 99}, {4, 47}};

    static constexpr auto make_counter(size_t& counter) {
        return [&counter](const int a, const int b) {
            ++counter;
            return a == b;
        };
    }

    template <ranges::forward_range ReadWrite>
    static constexpr void call() {
        using ranges::unique, ranges::subrange, ranges::equal, ranges::size, ranges::iterator_t;

        size_t comparisonCounter = 0;
        const auto countedEq     = make_counter(comparisonCounter);

        { // Validate iterator + sentinel overload
            P input[6] = {{0, 99}, {1, 47}, {2, 47}, {3, 99}, {4, 47}, {5, 47}};
            ReadWrite wrapped_input{input};

            auto result = unique(wrapped_input.begin(), wrapped_input.end(), countedEq, get_second);
            static_assert(same_as<decltype(result), subrange<iterator_t<ReadWrite>>>);
            assert(result.begin() == next(wrapped_input.begin(), 4));
            assert(result.end() == wrapped_input.end());
            assert(equal(expected, span{input}.first<4>()));
            assert(comparisonCounter == size(input) - 1);
        }

        { // Validate already unique range returns empty subrange
            P input[4] = {{0, 99}, {1, 47}, {3, 99}, {4, 47}};
            ReadWrite wrapped_input{input};

            auto result = unique(wrapped_input.begin(), wrapped_input.end(), countedEq, get_second);
            static_assert(same_as<decltype(result), subrange<iterator_t<ReadWrite>>>);
            assert(result.empty());
        }

        comparisonCounter = 0;

        { // Validate range overload
            P input[6] = {{0, 99}, {1, 47}, {2, 47}, {3, 99}, {4, 47}, {5, 47}};
            ReadWrite wrapped_input{input};

            auto result = unique(wrapped_input, countedEq, get_second);
            static_assert(same_as<decltype(result), subrange<iterator_t<ReadWrite>>>);
            assert(result.begin() == next(wrapped_input.begin(), 4));
            assert(result.end() == wrapped_input.end());
            assert(equal(expected, span{input}.first<4>()));
            assert(comparisonCounter == size(input) - 1);
        }

        { // Validate already unique range returns empty subrange
            P input[4] = {{0, 99}, {1, 47}, {3, 99}, {4, 47}};
            ReadWrite wrapped_input{input};

            auto result = unique(wrapped_input, countedEq, get_second);
            static_assert(same_as<decltype(result), subrange<iterator_t<ReadWrite>>>);
            assert(result.empty());
        }
    }
};

int main() {
    static_assert((test_fwd<instantiator, P>(), true));
    test_fwd<instantiator, P>();
}
