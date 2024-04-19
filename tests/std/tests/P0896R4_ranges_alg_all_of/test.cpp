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

constexpr auto is_even = [](auto const& x) { return x % 2 == 0; };
constexpr auto is_odd  = [](auto const& x) { return x % 2 != 0; };

struct instantiator {
    static constexpr P input[3] = {{0, 13}, {2, 13}, {4, 13}};

    template <ranges::input_range Read>
    static constexpr void call() {
        using ranges::all_of;
        { // Validate iterator + sentinel overload
            Read wrapped_input1{input};
            Read wrapped_input2{input};
            Read wrapped_input3{input};
            Read wrapped_input4{input};

            auto result = all_of(wrapped_input1.begin(), wrapped_input1.end(), is_even, get_first);
            static_assert(same_as<decltype(result), bool>);
            assert(result);
            assert(!all_of(wrapped_input2.begin(), wrapped_input2.end(), is_even, get_second));
            assert(!all_of(wrapped_input3.begin(), wrapped_input3.end(), is_odd, get_first));
            assert(all_of(wrapped_input4.begin(), wrapped_input4.end(), is_odd, get_second));
        }
        { // Validate range overload
            Read wrapped_input1{input};
            Read wrapped_input2{input};
            Read wrapped_input3{input};
            Read wrapped_input4{input};

            auto result = all_of(wrapped_input1, is_even, get_first);
            static_assert(same_as<decltype(result), bool>);
            assert(result);
            assert(!all_of(wrapped_input2, is_even, get_second));
            assert(!all_of(wrapped_input3, is_odd, get_first));
            assert(all_of(wrapped_input4, is_odd, get_second));
        }
    }
};

int main() {
    static_assert((test_in<instantiator, const P>(), true));
    test_in<instantiator, const P>();
}
