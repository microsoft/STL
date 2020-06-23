// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <range_algorithm_support.hpp>
#include <ranges>
#include <utility>

constexpr void smoke_test() {
    using ranges::swap_ranges, ranges::swap_ranges_result, ranges::iterator_t;
    using std::same_as;

    // Validate that move_result aliases in_in_result
    STATIC_ASSERT(same_as<swap_ranges_result<int, double>, ranges::in_in_result<int, double>>);

    {
        // // Validate range overload
        // int range1[] = {13, 53, 1876};
        // int range2[] = {34, 243, 9366};
        // // int const expected_output1[] = {34, 243, 9366};
        // // int const expected_output2[] = {13, 53, 1876};
        // // auto result = swap_ranges(move_only_range{range1}, move_only_range{range2});
        // auto result = swap_ranges(range1, range2);
        // using I1    = iterator_t<std::array<int, 3>>;
        // using I2    = iterator_t<std::array<int, 3>>;
        // STATIC_ASSERT(same_as<decltype(result), swap_ranges_result<I1, I2>>);
        // STATIC_ASSERT(same_as<decltype(result),
        //     swap_ranges_result<iterator_t<move_only_range<int const>>, iterator_t<move_only_range<int>>>>);
        // assert(result.in == move_only_range{range1}.end());
        // assert(result.out == move_only_range{range2}.end());
        // assert(ranges::equal(range1, expected_output1));
        // assert(ranges::equal(range2, expected_output2));
    } { // Validate iterator + sentinel overload
        // int range1[] = {13, 53, 1876};
        // int range2[] = {34, 243, 9366};
        // move_only_range wrapped_range1{range1};
        // move_only_range wrapped_range2{range2};
        // int const expected_output1[] = {34, 243, 9366};
        // int const expected_output2[] = {13, 53, 1876};
        // auto result =
        //     swap_ranges(wrapped_range1.begin(), wrapped_range1.end(), wrapped_range2.begin(), wrapped_range2.end());
        // STATIC_ASSERT(same_as<decltype(result),
        //     swap_ranges_result<iterator_t<move_only_range<int const>>, iterator_t<move_only_range<int>>>>);
        // assert(result.in == wrapped_range1.end());
        // assert(result.out == wrapped_range2.end());
        // assert(ranges::equal(range1, expected_output1));
        // assert(ranges::equal(range2, expected_output2));
    }
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class In1, class In2>
    static void call(In1 in1 = {}, In2 in2 = {}) {
        (void) ranges::swap_ranges(in1, in2);
        (void) ranges::swap_ranges(ranges::begin(in1), ranges::end(in1), ranges::begin(in2), ranges::end(in2));
    }
};

template void test_in_in<instantiator>();
