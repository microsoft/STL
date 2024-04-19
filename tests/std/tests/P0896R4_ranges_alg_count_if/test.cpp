// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

constexpr auto is_even = [](auto const& x) { return x % 2 == 0; };
constexpr auto is_odd  = [](auto const& x) { return x % 2 != 0; };

using namespace std;
using P = pair<int, int>;

struct instantiator {
    static constexpr P input[5] = {{0, 47}, {1, 99}, {2, 99}, {3, 47}, {4, 99}};

    template <ranges::input_range Read>
    static constexpr void call() {
        using ranges::count_if;
        { // Validate iterator + sentinel overload
            Read wrapped_input{input};

            auto result = count_if(wrapped_input.begin(), wrapped_input.end(), is_odd, get_first);
            static_assert(same_as<decltype(result), ranges::range_difference_t<Read>>);
            assert(result == 2);
        }
        { // Validate range overload
            Read wrapped_input{input};

            auto result = count_if(wrapped_input, is_even, get_first);
            static_assert(same_as<decltype(result), ranges::range_difference_t<Read>>);
            assert(result == 3);
        }
    }
};

int main() {
    static_assert((test_in<instantiator, P const>(), true));
    test_in<instantiator, P const>();
}
