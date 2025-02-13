// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cstddef>
#include <mdspan>
#include <span>

#include <test_death.hpp>
#include <test_mdspan_support.hpp>

using namespace std;

void test_static_extent_function_with_invalid_index() {
    using E = extents<int, 3>;
    // Index must be less than rank()
#pragma warning(push)
#pragma warning(disable : 28020) // yay, /analyze catches this mistake at compile time!
    (void) E::static_extent(1);
#pragma warning(pop)
}

void test_extent_function_with_invalid_index() {
    extents<int, 3> e;
    // Index must be less than rank()
#pragma warning(push)
#pragma warning(disable : 28020) // yay, /analyze catches this mistake at compile time!
    (void) e.extent(1);
#pragma warning(pop)
}

void test_construction_from_other_extents_with_invalid_values() {
    extents<int, dynamic_extent, dynamic_extent> e1{1, 2};
    // Value of other.extent(r) must be equal to extent(r) for each r for which extent(r) is a static extent
    [[maybe_unused]] extents<int, 1, 1> e2{e1};
}

void test_construction_from_other_extents_with_unrepresentable_as_index_type_values() {
    extents<int, dynamic_extent> e1{256};
    // Value of other.extent(r) must be representable as a value of type index_type for every rank index r
    [[maybe_unused]] extents<unsigned char, dynamic_extent> e2{e1};
}

void test_construction_from_pack_with_invalid_values() {
    // Value of exts_arr[r] must be equal to extent(r) for each r for which extent(r) is a static extent
    [[maybe_unused]] extents<int, 1, 2> e{1, 1};
}

void test_construction_from_pack_with_unrepresentable_as_index_type_values_1() {
    // Either sizeof...(exts) must be equal to 0 or each element of exts must be nonnegative and must be representable
    // as value of type index_type
    [[maybe_unused]] extents<unsigned char, 1, dynamic_extent> e{1, 256};
}

void test_construction_from_pack_with_unrepresentable_as_index_type_values_2() {
    // Either sizeof...(exts) must be equal to 0 or each element of exts must be nonnegative and must be representable
    // as value of type index_type
    [[maybe_unused]] extents<unsigned char, 1, dynamic_extent> e{ConvertibleToInt<unsigned char>{.val = 1}, 256};
}

void test_construction_from_pack_with_unrepresentable_as_index_type_values_3() {
    static_assert(signed_integral<char>, "This test assumes that it isn't being compiled with /J");
    // Either sizeof...(exts) must be equal to 0 or each element of exts must be nonnegative and must be representable
    // as value of type index_type
    [[maybe_unused]] extents<signed char, 1, dynamic_extent> e{static_cast<char>(-1)};
}

void test_construction_from_span_with_invalid_values() {
    int vals[] = {1, 2};
    span s{vals};
    // Value of exts[r] must be equal to extent(r) for each r for which extent(r) is a static extent
    [[maybe_unused]] extents<int, 1, 1> e{s};
}

void test_construction_from_span_with_unrepresentable_as_index_type_values() {
    int vals[] = {256};
    span s{vals};
    // Either N must be zero or exts[r] must be nonnegative and must be representable as value of type index_type for
    // every rank index r
    [[maybe_unused]] extents<unsigned char, dynamic_extent> e{s};
}

void test_construction_from_array_with_invalid_values() {
    array a = {1, 2};
    // Value of exts[r] must be equal to extent(r) for each r for which extent(r) is a static extent
    [[maybe_unused]] extents<int, 1, 1> e{a};
}

void test_construction_from_array_with_unrepresentable_as_index_type_values() {
    array a = {256};
    // Either N must be zero or exts[r] must be nonnegative and must be representable as value of type index_type for
    // every rank index r
    [[maybe_unused]] extents<unsigned char, dynamic_extent> e{a};
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({
        test_static_extent_function_with_invalid_index,
        test_extent_function_with_invalid_index,
        test_construction_from_other_extents_with_invalid_values,
        test_construction_from_other_extents_with_unrepresentable_as_index_type_values,
        test_construction_from_pack_with_invalid_values,
        test_construction_from_pack_with_unrepresentable_as_index_type_values_1,
        test_construction_from_pack_with_unrepresentable_as_index_type_values_2,
        test_construction_from_pack_with_unrepresentable_as_index_type_values_3,
        test_construction_from_span_with_invalid_values,
        test_construction_from_span_with_unrepresentable_as_index_type_values,
        test_construction_from_array_with_invalid_values,
        test_construction_from_array_with_unrepresentable_as_index_type_values,
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0

    return exec.run(argc, argv);
}
