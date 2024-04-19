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

constexpr auto matches = [](int const val) { return val == 47; };

// Validate dangling story
static_assert(same_as<decltype(ranges::replace_if(borrowed<false>{}, matches, 5)), ranges::dangling>);
static_assert(same_as<decltype(ranges::replace_if(borrowed<true>{}, matches, 5)), int*>);

struct instantiator {
    static constexpr P output[5] = {{0, 99}, {47, 1}, {2, 99}, {47, 1}, {4, 99}};

    template <ranges::input_range Read>
    static constexpr void call() {
        using ranges::replace_if;
        { // Validate iterator + sentinel overload
            P input[5] = {{0, 99}, {1, 47}, {2, 99}, {3, 47}, {4, 99}};
            Read wrapped_input{input};

            auto result = replace_if(wrapped_input.begin(), wrapped_input.end(), matches, P{47, 1}, get_second);
            static_assert(same_as<decltype(result), ranges::iterator_t<Read>>);
            assert(result == wrapped_input.end());
            assert(ranges::equal(output, input));
        }
        { // Validate range overload
            P input[5] = {{0, 99}, {1, 47}, {2, 99}, {3, 47}, {4, 99}};
            Read wrapped_input{input};

            auto result = replace_if(wrapped_input, matches, P{47, 1}, get_second);
            static_assert(same_as<decltype(result), ranges::iterator_t<Read>>);
            assert(result == wrapped_input.end());
            assert(ranges::equal(output, input));
        }
    }
};

int main() {
    static_assert((test_in<instantiator, P>(), true));
    test_in<instantiator, P>();
}
