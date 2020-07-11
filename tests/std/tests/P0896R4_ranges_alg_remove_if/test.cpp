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

constexpr auto matches = [](int const val) { return val == 47; };

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::remove_if(borrowed<false>{}, matches)), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::remove_if(borrowed<true>{}, matches)), ranges::subrange<int*>>);

struct instantiator {
    static constexpr P expected[3] = {{0, 99}, {2, 99}, {4, 99}};

    template <ranges::forward_range Read>
    static constexpr void call() {
        using ranges::remove_if, ranges::subrange, ranges::iterator_t;
        { // Validate iterator + sentinel overload
            P input[5] = {{0, 99}, {1, 47}, {2, 99}, {3, 47}, {4, 99}};
            Read wrapped_input{input};
            size_t comparisonsCounter = 0;
            auto projection           = [&comparisonsCounter](const P& data) {
                ++comparisonsCounter;
                return data.second;
            };

            auto result = remove_if(wrapped_input.begin(), wrapped_input.end(), matches, projection);
            STATIC_ASSERT(same_as<decltype(result), subrange<iterator_t<Read>>>);
            assert(result.begin() == next(wrapped_input.begin(), 3));
            assert(result.end() == wrapped_input.end());
            assert(ranges::equal(expected, span{input}.first<3>()));
            assert(comparisonsCounter == ranges::size(input));
        }
        { // Validate range overload
            P input[5] = {{0, 99}, {1, 47}, {2, 99}, {3, 47}, {4, 99}};
            Read wrapped_input{input};
            size_t comparisonsCounter = 0;
            auto projection           = [&comparisonsCounter](const P& data) {
                ++comparisonsCounter;
                return data.second;
            };

            auto result = remove_if(wrapped_input, matches, projection);
            STATIC_ASSERT(same_as<decltype(result), subrange<iterator_t<Read>>>);
            assert(result.begin() == next(wrapped_input.begin(), 3));
            assert(result.end() == wrapped_input.end());
            assert(ranges::equal(expected, span{input}.first<3>()));
            assert(comparisonsCounter == ranges::size(input));
        }
    }
};

int main() {
    STATIC_ASSERT((test_fwd<instantiator, P>(), true));
    test_fwd<instantiator, P>();
}
