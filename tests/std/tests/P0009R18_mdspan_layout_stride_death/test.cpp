// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cstddef>
#include <mdspan>
#include <span>

#include <test_death.hpp>
#include <test_mdspan_support.hpp>

using namespace std;

void test_default_construction() {
    using Ext = extents<signed char, dynamic_extent, 4, 5, 7>;
    // Value of layout_stride::mapping<extents_type>().required_span_size() must be
    // representable as a value of type index_type
    [[maybe_unused]] layout_stride::mapping<Ext> m{}; // NB: strides are [140, 35, 7, 1]
}

void test_construction_from_extents_and_array_1() {
    // Value of s[i] must be greater than 0 for all i in the range [0, rank_)
    [[maybe_unused]] layout_stride::mapping<dextents<int, 1>> m1{extents<int, 1>{}, array<int, 1>{-1}};
}

void test_construction_from_extents_and_array_2() {
    using Ext = extents<signed char, 120>;
    // REQUIRED-SPAN-SIZE(e, s) must be representable as a value of type index_type
    [[maybe_unused]] layout_stride::mapping<Ext> m{Ext{}, array<int, 1>{2}};
}

void test_construction_from_extents_and_array_3() {
    using Ext = extents<short, 2, 3, 5>;
    const array<short, 3> a{29, 2, 6};
    // Incorrect strides
    [[maybe_unused]] layout_stride::mapping<Ext> m{Ext{}, a};
}

void test_construction_from_extents_and_span_1() {
    array<int, 1> a{-1};
    // Value of s[i] must be greater than 0 for all i in the range [0, rank_)
    [[maybe_unused]] layout_stride::mapping<dextents<int, 1>> m{extents<int, 1>{}, span{a}};
}

void test_construction_from_extents_and_span_2() {
    using Ext = extents<unsigned char, 130>;
    array<ConvertibleToInt<int>, 1> a{{{.val = 2}}};
    const span s{a};
    // REQUIRED-SPAN-SIZE(e, s) must be representable as a value of type index_type
    [[maybe_unused]] layout_stride::mapping<Ext> m{Ext{}, s};
}

void test_construction_from_extents_and_span_3() {
    using Ext = extents<unsigned char, 3, 3, 1, 1>;
    array<long long, 4> a{3, 1, 8, 3};
    const span s{a};
    // Incorrect strides
    [[maybe_unused]] layout_stride::mapping<Ext> m{Ext{}, s};
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

void test_stride_with_empty_extents() {
    layout_stride::mapping<extents<int>> m;
    // The argument to stride must be nonnegative and less than extents_type::rank()
#pragma warning(push)
#pragma warning(disable : 28020) // yay, /analyze catches this mistake at compile time!
    (void) m.stride(0);
#pragma warning(pop)
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({
        test_default_construction,
        test_construction_from_extents_and_array_1,
        test_construction_from_extents_and_array_2,
        test_construction_from_extents_and_array_3,
        test_construction_from_extents_and_span_1,
        test_construction_from_extents_and_span_2,
        test_construction_from_extents_and_span_3,
        test_construction_from_strided_layout_mapping,
        test_call_operator,
        test_stride_with_empty_extents,
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0

    return exec.run(argc, argv);
}
