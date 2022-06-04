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
STATIC_ASSERT(same_as<decltype(ranges::remove(borrowed<false>{}, 42)), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::remove(borrowed<true>{}, 42)), ranges::subrange<int*>>);

struct instantiator {
    static constexpr P expected[3] = {{0, 99}, {2, 99}, {4, 99}};

    template <ranges::forward_range Read>
    static constexpr void call() {
        using ranges::remove, ranges::subrange, ranges::equal, ranges::iterator_t;

        size_t projectionCounter = 0;
        auto projection          = [&projectionCounter](const P& val) {
            ++projectionCounter;
            return val.second;
        };

        { // Validate iterator + sentinel overload
            P input[5] = {{0, 99}, {1, 47}, {2, 99}, {3, 47}, {4, 99}};
            Read wrapped_input{input};

            auto result = remove(wrapped_input.begin(), wrapped_input.end(), 47, projection);
            STATIC_ASSERT(same_as<decltype(result), subrange<iterator_t<Read>>>);
            assert(result.begin() == next(wrapped_input.begin(), 3));
            assert(result.end() == wrapped_input.end());
            assert(equal(expected, span{input}.first<3>()));
            assert(projectionCounter == ranges::size(input));
        }

        projectionCounter = 0;

        { // Validate range overload
            P input[5] = {{0, 99}, {1, 47}, {2, 99}, {3, 47}, {4, 99}};
            Read wrapped_input{input};

            auto result = remove(wrapped_input, 47, projection);
            STATIC_ASSERT(same_as<decltype(result), subrange<iterator_t<Read>>>);
            assert(result.begin() == next(wrapped_input.begin(), 3));
            assert(result.end() == wrapped_input.end());
            assert(equal(expected, span{input}.first<3>()));
            assert(projectionCounter == ranges::size(input));
        }
    }
};

int main() {
    STATIC_ASSERT((test_fwd<instantiator, P>(), true));
    test_fwd<instantiator, P>();
}
