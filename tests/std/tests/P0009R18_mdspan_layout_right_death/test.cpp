// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cstddef>
#include <mdspan>
#include <span>

#include <test_death.hpp>

using namespace std;

// TRANSITION, Test Construction From extents_type

void test_construction_from_other_right_mapping() {
    layout_right::mapping<dextents<int, 1>> m1{dextents<int, 1>{256}};
    // Value of other.required_span_size() must be representable as a value of type index_type
    layout_right::mapping<dextents<unsigned char, 1>> m2{m1};
}

void test_construction_from_other_left_mapping() {
    layout_left::mapping<dextents<int, 1>> m1{dextents<int, 1>{256}};
    // Value of other.required_span_size() must be representable as a value of type index_type
    layout_right::mapping<dextents<unsigned char, 1>> m2{m1};
}

#pragma warning(push) // TRANSITION, "/analyze:only" BUG?
#pragma warning(disable : 28020) // The expression '0<=_Param_(1)&&_Param_(1)<=1-1' is not true at this call
void test_construction_from_other_stride_mapping_1() {
    using Ext = extents<int, 2, 4>;
    layout_stride::mapping<Ext> m1{Ext{}, array{3, 1}};
    // For all r in the range [0, extents_type::rank()), other.stride(r) must be equal to
    // extents().rev-prod-of-extents(r)
    layout_right::mapping<Ext> m2{m1};
}

void test_construction_from_other_stride_mapping_2() {
    layout_stride::mapping<dextents<int, 1>> m1{dextents<int, 1>{256}, array{1}};
    // Value of other.required_span_size() must be representable as a value of type index_type
    layout_right::mapping<dextents<unsigned char, 1>> m2{m1};
}
#pragma warning(pop) // TRANSITION, "/analyze:only" BUG?

void test_stride_function() {
    layout_right::mapping<extents<int, 3>> m;
    // Value of i must be less than extents_type::rank()
    (void) m.stride(1);
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;
    exec.add_death_tests({
        // TRANSITION Construction From extents_type
        test_construction_from_other_right_mapping,
        test_construction_from_other_left_mapping,
        test_construction_from_other_stride_mapping_1,
        test_construction_from_other_stride_mapping_2,
        test_stride_function,
    });
    return exec.run(argc, argv);
}
