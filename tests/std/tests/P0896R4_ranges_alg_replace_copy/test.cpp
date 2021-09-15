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

// Validate that replace_copy_result aliases in_out_result
STATIC_ASSERT(same_as<ranges::replace_copy_result<int, double>, ranges::in_out_result<int, double>>);

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::replace_copy(borrowed<false>{}, nullptr_to<int>, 42, 5)),
    ranges::replace_copy_result<ranges::dangling, int*>>);
STATIC_ASSERT(same_as<decltype(ranges::replace_copy(borrowed<true>{}, nullptr_to<int>, 42, 5)),
    ranges::replace_copy_result<int*, int*>>);

struct instantiator {
    static constexpr P input[5]    = {{0, 99}, {1, 47}, {2, 99}, {3, 47}, {4, 99}};
    static constexpr P expected[5] = {{0, 99}, {47, 1}, {2, 99}, {47, 1}, {4, 99}};

    template <ranges::input_range Read, indirectly_writable<ranges::range_reference_t<Read>> Write>
    static constexpr void call() {
        using ranges::replace_copy, ranges::replace_copy_result, ranges::equal, ranges::iterator_t;
        { // Validate iterator + sentinel overload
            P output[5] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};
            Read wrapped_input{input};

            auto result =
                replace_copy(wrapped_input.begin(), wrapped_input.end(), Write{output}, 47, P{47, 1}, get_second);
            STATIC_ASSERT(same_as<decltype(result), replace_copy_result<iterator_t<Read>, Write>>);
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == output + 5);
            assert(equal(output, expected));
        }
        { // Validate range overload
            P output[5] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};
            Read wrapped_input{input};

            auto result = replace_copy(wrapped_input, Write{output}, 47, P{47, 1}, get_second);
            STATIC_ASSERT(same_as<decltype(result), replace_copy_result<iterator_t<Read>, Write>>);
            assert(result.in == wrapped_input.end());
            assert(result.out.peek() == output + 5);
            assert(equal(output, expected));
        }
    }
};

int main() {
#ifndef _PREFAST_ // TRANSITION, GH-1030
    STATIC_ASSERT((input_range_output_iterator_permutations<instantiator, P const, P>(), true));
#endif // TRANSITION, GH-1030
    input_range_output_iterator_permutations<instantiator, P const, P>();
}
