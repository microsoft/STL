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
STATIC_ASSERT(same_as<decltype(ranges::unique(borrowed<false>{})), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::unique(borrowed<true>{})), ranges::subrange<int*>>);

struct instantiator {
    static constexpr P expected[4] = {{0, 99}, {1, 47}, {3, 99}, {4, 47}};

    template <ranges::forward_range ReadWrite>
    static constexpr void call() {
        using ranges::unique, ranges::subrange, ranges::iterator_t;
        { // Validate iterator + sentinel overload
            P input[6] = {{0, 99}, {1, 47}, {2, 47}, {3, 99}, {4, 47}, {5, 47}};
            ReadWrite wrapped_input{input};
            size_t comparisonsCounter = 0;
            constexpr auto countedEq  = [&comparisonsCounter](const int a, const int b) {
                ++comparisonsCounter;
                return a == b;
            };

            auto result = unique(wrapped_input.begin(), wrapped_input.end(), countedEq, get_second);
            STATIC_ASSERT(same_as<decltype(result), subrange<iterator_t<ReadWrite>>>);
            assert(result.begin() == next(wrapped_input.begin(), 4));
            assert(result.end() == wrapped_input.end());
            assert(ranges::equal(expected, span{input}.first<4>()));
            assert(comparisonsCounter == ranges::size(input) - 1);
        }
        { // Validate range overload
            P input[6] = {{0, 99}, {1, 47}, {2, 47}, {3, 99}, {4, 47}, {5, 47}};
            ReadWrite wrapped_input{input};
            size_t comparisonsCounter = 0;
            constexpr auto countedEq  = [&comparisonsCounter](const int a, const int b) {
                ++comparisonsCounter;
                return a == b;
            };

            auto result = unique(wrapped_input, countedEq, get_second);
            STATIC_ASSERT(same_as<decltype(result), subrange<iterator_t<ReadWrite>>>);
            assert(result.begin() == next(wrapped_input.begin(), 4));
            assert(result.end() == wrapped_input.end());
            assert(ranges::equal(expected, span{input}.first<4>()));
            assert(comparisonsCounter == ranges::size(input) - 1);
        }
    }
};

int main() {
    STATIC_ASSERT((test_fwd<instantiator, P>(), true));
    test_fwd<instantiator, P>();
}
