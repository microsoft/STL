// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;
using P = pair<int, int>;

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::rotate(borrowed<false>{}, nullptr_to<int>)), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::rotate(borrowed<true>{}, nullptr_to<int>)), ranges::subrange<int*>>);

struct instantiator {
    static constexpr P expected[5] = {{3, 47}, {4, 99}, {0, 99}, {1, 47}, {2, 99}};

    template <ranges::forward_range ReadWrite>
    static constexpr void call() {
        using ranges::rotate, ranges::subrange, ranges::equal, ranges::iterator_t;
        { // Validate iterator overload
            P input[5] = {{0, 99}, {1, 47}, {2, 99}, {3, 47}, {4, 99}};
            ReadWrite wrapped_input{input};

            auto result = rotate(wrapped_input.begin(), next(wrapped_input.begin(), 3), wrapped_input.end());
            STATIC_ASSERT(same_as<decltype(result), subrange<iterator_t<ReadWrite>>>);
            assert(result.begin() == next(wrapped_input.begin(), 2));
            assert(result.end() == wrapped_input.end());
            assert(equal(expected, input));
        }
        { // Validate range overload
            P input[5] = {{0, 99}, {1, 47}, {2, 99}, {3, 47}, {4, 99}};
            ReadWrite wrapped_input{input};

            auto result = rotate(wrapped_input, next(wrapped_input.begin(), 3));
            STATIC_ASSERT(same_as<decltype(result), subrange<iterator_t<ReadWrite>>>);
            assert(result.begin() == next(wrapped_input.begin(), 2));
            assert(result.end() == wrapped_input.end());
            assert(equal(expected, input));
        }
    }
};

int main() {
#if defined(__clang__) || defined(__EDG__) // TRANSITION, VSO-938163
    STATIC_ASSERT((test_fwd<instantiator, P>(), true));
#endif // TRANSITION, VSO-938163
    test_fwd<instantiator, P>();
}
