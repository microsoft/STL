// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

constexpr void smoke_test() {
    using ranges::swap_ranges, ranges::swap_ranges_result, ranges::iterator_t;
    using std::same_as;
    using I1 = iterator_t<move_only_range<int>>;
    using I2 = iterator_t<move_only_range<int>>;

    // Validate that swap_ranges_result aliases in_in_result
    STATIC_ASSERT(same_as<ranges::swap_ranges_result<int, double>, ranges::in_in_result<int, double>>);

    { // Validate ranges overload
        using R                      = std::array<int, 3>;
        R range1                     = {13, 53, 1876};
        R range2                     = {34, 243, 9366};
        int const expected_output1[] = {34, 243, 9366};
        int const expected_output2[] = {13, 53, 1876};
        auto result                  = swap_ranges(move_only_range{range1}, move_only_range{range2});
        STATIC_ASSERT(same_as<decltype(result), swap_ranges_result<I1, I2>>);
        assert(result.in1 == move_only_range{range1}.end());
        assert(result.in2 == move_only_range{range2}.end());
        assert(ranges::equal(range1, expected_output1));
        assert(ranges::equal(range2, expected_output2));
    }
    { // Validate iterator + sentinel overload
        int range1[] = {13, 53, 1876};
        int range2[] = {34, 243, 9366};
        move_only_range wrapped_range1{range1};
        move_only_range wrapped_range2{range2};
        int const expected_output1[] = {34, 243, 9366};
        int const expected_output2[] = {13, 53, 1876};
        auto result =
            swap_ranges(wrapped_range1.begin(), wrapped_range1.end(), wrapped_range2.begin(), wrapped_range2.end());
        STATIC_ASSERT(same_as<decltype(result), swap_ranges_result<I1, I2>>);
        assert(result.in1 == wrapped_range1.end());
        assert(result.in2 == wrapped_range2.end());
        assert(ranges::equal(range1, expected_output1));
        assert(ranges::equal(range2, expected_output2));
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
