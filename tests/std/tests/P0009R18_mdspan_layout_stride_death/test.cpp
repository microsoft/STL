// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cstddef>
#include <mdspan>
#include <span>

#include <test_death.hpp>

using namespace std;

void test_construction_from_extents_and_array() {
    // Value of s[i] must be greater than 0 for all i in the range [0, rank_)
    [[maybe_unused]] layout_stride::mapping<dextents<int, 1>> m1{extents<int, 1>{}, array<int, 1>{-1}};
}

void test_construction_from_extents_and_span() {
    array<int, 1> s{-1};
    // Value of s[i] must be greater than 0 for all i in the range [0, rank_)
    [[maybe_unused]] layout_stride::mapping<dextents<int, 1>> m1{extents<int, 1>{}, span{s}};
}

void test_construction_from_strided_layout_mapping() {
    layout_right::mapping<extents<int, 256>> m1;
    // Value of other.required_span_size() must be representable as a value of type index_type
    [[maybe_unused]] layout_stride::mapping<dextents<unsigned char, 1>> m2{m1};
}

void test_call_operator() {
    layout_stride::mapping<extents<unsigned int, 5, 3, 4>> m;
    // Value of extents_type::index-cast(i) must be a multidimensional index in extents_
    (void) m(4, 3, 3);
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;
    exec.add_death_tests({
        // TRANSITION more tests
        test_construction_from_extents_and_array,
        test_construction_from_extents_and_span,
        test_construction_from_strided_layout_mapping,
        test_call_operator,
    });
    return exec.run(argc, argv);
}
