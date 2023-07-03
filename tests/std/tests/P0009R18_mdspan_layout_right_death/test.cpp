// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

#include <array>
#include <cstddef>
#include <mdspan>
#include <span>

#include <test_death.hpp>

using namespace std;

void test_construction_from_extents_type_with_signed_index_type() {
    using Ext = dextents<signed char, 3>;
    // The size of the multidimensional index space e must be representable as a value of type index_type
    [[maybe_unused]] layout_right::mapping<Ext> m{Ext{5, 4, 7}};
}

void test_construction_from_extents_type_with_unsigned_index_type() {
    using Ext = dextents<unsigned char, 3>;
    // The size of the multidimensional index space e must be representable as a value of type index_type
    [[maybe_unused]] layout_right::mapping<Ext> m{Ext{5, 10, 6}};
}

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

void test_construction_from_other_stride_mapping_1() {
    using Ext = extents<int, 2, 4>;
    layout_stride::mapping<Ext> m1{Ext{}, array{1, 2}};
    // For all r in the range [0, extents_type::rank()), other.stride(r) must be equal to
    // extents().rev-prod-of-extents(r)
    layout_right::mapping<Ext> m2{m1};
}

void test_construction_from_other_stride_mapping_2() {
    layout_stride::mapping<dextents<int, 1>> m1{dextents<int, 1>{256}, array{1}};
    // Value of other.required_span_size() must be representable as a value of type index_type
    layout_right::mapping<dextents<unsigned char, 1>> m2{m1};
}

void test_call_operator() {
    layout_right::mapping<extents<int, 5, 4, 3>> m;
    // Value of extents_type::index-cast(i) must be a multidimensional index in extents_
    (void) m(4, 3, 3);
}

void test_stride_function() {
    layout_right::mapping<extents<int, 3>> m;
    // Value of i must be less than extents_type::rank()
    (void) m.stride(1);
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;
    exec.add_death_tests({
        test_construction_from_extents_type_with_signed_index_type,
        test_construction_from_extents_type_with_unsigned_index_type,
        test_construction_from_other_right_mapping,
        test_construction_from_other_left_mapping,
        test_construction_from_other_stride_mapping_1,
        test_construction_from_other_stride_mapping_2,
        test_call_operator,
        test_stride_function,
    });
    return exec.run(argc, argv);
}
