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
STATIC_ASSERT(same_as<decltype(ranges::replace(borrowed<false>{}, 42, 5)), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::replace(borrowed<true>{}, 42, 5)), int*>);

struct instantiator {
    static constexpr P output[5] = {{0, 99}, {47, 1}, {2, 99}, {47, 1}, {4, 99}};

    template <ranges::input_range Read>
    static constexpr void call() {
        using ranges::replace, ranges::iterator_t;
        { // Validate iterator + sentinel overload
            P input[5] = {{0, 99}, {1, 47}, {2, 99}, {3, 47}, {4, 99}};
            Read wrapped_input{input};

            auto result = replace(wrapped_input.begin(), wrapped_input.end(), 47, P{47, 1}, get_second);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<Read>>);
            assert(result == wrapped_input.end());
            assert(ranges::equal(output, input));
        }
        { // Validate range overload
            P input[5] = {{0, 99}, {1, 47}, {2, 99}, {3, 47}, {4, 99}};
            Read wrapped_input{input};

            auto result = replace(wrapped_input, 47, P{47, 1}, get_second);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<Read>>);
            assert(result == wrapped_input.end());
            assert(ranges::equal(output, input));
        }
    }
};

int main() {
    STATIC_ASSERT((test_in<instantiator, P>(), true));
    test_in<instantiator, P>();
}
