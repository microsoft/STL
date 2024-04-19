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

// Validate that replace_copy_if_result aliases in_out_result
static_assert(same_as<ranges::replace_copy_if_result<int, double>, ranges::in_out_result<int, double>>);

// Validate dangling story
static_assert(same_as<decltype(ranges::replace_copy_if(borrowed<false>{}, nullptr_to<int>, matches, 5)),
    ranges::replace_copy_if_result<ranges::dangling, int*>>);
static_assert(same_as<decltype(ranges::replace_copy_if(borrowed<true>{}, nullptr_to<int>, matches, 5)),
    ranges::replace_copy_if_result<int*, int*>>);

struct instantiator {
    static constexpr P input[5]    = {{0, 99}, {1, 47}, {2, 99}, {3, 47}, {4, 99}};
    static constexpr P expected[5] = {{0, 99}, {47, 1}, {2, 99}, {47, 1}, {4, 99}};

    template <ranges::input_range Read, indirectly_writable<ranges::range_reference_t<Read>> Write>
    static constexpr void call() {
        using ranges::replace_copy_if, ranges::replace_copy_if_result, ranges::equal, ranges::iterator_t;
        { // Validate iterator + sentinel overload
            P output[5] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};
            Read wrapped_input{input};

            auto result = replace_copy_if(
                wrapped_input.begin(), wrapped_input.end(), Write{output}, matches, P{47, 1}, get_second);
            static_assert(same_as<decltype(result), replace_copy_if_result<iterator_t<Read>, Write>>);
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == output + 5);
            assert(equal(output, expected));
        }
        { // Validate range overload
            P output[5] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};
            Read wrapped_input{input};

            auto result = replace_copy_if(wrapped_input, Write{output}, matches, P{47, 1}, get_second);
            static_assert(same_as<decltype(result), replace_copy_if_result<iterator_t<Read>, Write>>);
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == output + 5);
            assert(equal(output, expected));
        }
    }
};

int main() {
#ifndef _PREFAST_ // TRANSITION, GH-1030
    static_assert((input_range_output_iterator_permutations<instantiator, P const, P>(), true));
#endif // TRANSITION, GH-1030
    input_range_output_iterator_permutations<instantiator, P const, P>();
}
