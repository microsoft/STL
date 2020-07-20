// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;

// Validate that swap_ranges_result aliases in_out_result
STATIC_ASSERT(same_as<ranges::swap_ranges_result<int, double>, ranges::in_in_result<int, double>>);

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::swap_ranges(borrowed<false>{}, borrowed<false>{})),
    ranges::swap_ranges_result<ranges::dangling, ranges::dangling>>);
STATIC_ASSERT(same_as<decltype(ranges::swap_ranges(borrowed<true>{}, borrowed<false>{})),
    ranges::swap_ranges_result<int*, ranges::dangling>>);
STATIC_ASSERT(same_as<decltype(ranges::swap_ranges(borrowed<false>{}, borrowed<true>{})),
    ranges::swap_ranges_result<ranges::dangling, int*>>);
STATIC_ASSERT(
    same_as<decltype(ranges::swap_ranges(borrowed<true>{}, borrowed<true>{})), ranges::swap_ranges_result<int*, int*>>);

struct instantiator {
    static constexpr int expected_output1[3] = {34, 243, 9366};
    static constexpr int expected_output2[3] = {13, 53, 1876};

    template <ranges::input_range ReadWrite1, ranges::input_range ReadWrite2>
    static constexpr void call() {
        using ranges::swap_ranges, ranges::swap_ranges_result, ranges::iterator_t, ranges::equal;
        { // Validate iterator + sentinel overload
            int input1[3] = {13, 53, 1876};
            int input2[3] = {34, 243, 9366};
            ReadWrite1 wrapped_input1{input1};
            ReadWrite2 wrapped_input2{input2};

            auto result =
                swap_ranges(wrapped_input1.begin(), wrapped_input1.end(), wrapped_input2.begin(), wrapped_input2.end());
            STATIC_ASSERT(
                same_as<decltype(result), swap_ranges_result<iterator_t<ReadWrite1>, iterator_t<ReadWrite2>>>);
            assert(result.in1 == wrapped_input1.end());
            assert(result.in2 == wrapped_input2.end());
            assert(equal(input1, expected_output1));
            assert(equal(input2, expected_output2));
        }
        { // Validate range overload
            int input1[3] = {13, 53, 1876};
            int input2[3] = {34, 243, 9366};
            ReadWrite1 wrapped_input1{input1};
            ReadWrite2 wrapped_input2{input2};

            auto result = swap_ranges(wrapped_input1, wrapped_input2);
            STATIC_ASSERT(
                same_as<decltype(result), swap_ranges_result<iterator_t<ReadWrite1>, iterator_t<ReadWrite2>>>);
            assert(result.in1 == wrapped_input1.end());
            assert(result.in2 == wrapped_input2.end());
            assert(equal(input1, expected_output1));
            assert(equal(input2, expected_output2));
        }
    }
};

int main() {
#if defined(__clang__) || defined(__EDG__) // TRANSITION, VSO-938163
    STATIC_ASSERT((test_in_in<instantiator, int, int>(), true));
#endif // TRANSITION, VSO-938163
    test_in_in<instantiator, int, int>();
}
