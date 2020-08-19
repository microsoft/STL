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

struct instantiator {
    static constexpr P input[5] = {{0, 99}, {1, 47}, {2, 99}, {3, 47}, {4, 99}};

    template <ranges::input_range Read>
    static constexpr void call() {
        using ranges::count;
        { // Validate iterator + sentinel overload
            Read wrapped_input{input};

            auto result = count(wrapped_input.begin(), wrapped_input.end(), 47, get_second);
            STATIC_ASSERT(same_as<decltype(result), ranges::range_difference_t<Read>>);
            assert(result == 2);
        }
        { // Validate range overload
            Read wrapped_input{input};

            auto result = count(wrapped_input, 99, get_second);
            STATIC_ASSERT(same_as<decltype(result), ranges::range_difference_t<Read>>);
            assert(result == 3);
        }
    }
};

int main() {
    STATIC_ASSERT((test_in<instantiator, P const>(), true));
    test_in<instantiator, P const>();
}
