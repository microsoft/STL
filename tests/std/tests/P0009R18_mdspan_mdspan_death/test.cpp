// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <mdspan>

#include <test_death.hpp>

using namespace std;

constexpr array<int, 256> some_ints{};

void test_construction_from_other_mdspan() {
    mdspan mds1{some_ints.data(), 8, 2, 8};
    // For each rank index r of extents_type, static_extent(r) == dynamic_extent || static_extent(r) == other.extent(r)
    // must be true
    [[maybe_unused]] mdspan<const int, extents<int, 8, 8, 2>> mds2{mds1};
}

#ifdef __cpp_multidimensional_subscript // TRANSITION, P2128R6
void test_access_with_invalid_multidimensional_index_1() {
    mdspan mds{some_ints.data(), 4, 4};
    // I must be a multidimensional index in extents()
    (void) mds[3, 4];
}

void test_access_with_nonrepresentable_index_1() {
    mdspan mds{some_ints.data(), dextents<unsigned char, 2>{2, 3}};
    (void) mds[256u, -255];
}
#endif // __cpp_multidimensional_subscript

void test_access_with_invalid_multidimensional_index_2() {
    mdspan mds{some_ints.data(), 5, 5};
    // I must be a multidimensional index in extents()
    (void) mds[array{4, 5}];
}

void test_access_with_nonrepresentable_index_2() {
    mdspan mds{some_ints.data(), dextents<unsigned char, 2>{2, 3}};
    (void) mds[array{256, -255}];
}

void test_size_when_index_type_is_signed() {
    mdspan mds{some_ints.data(), dextents<signed char, 3>{8, 8, 4}};
    // The size of the multidimensional index space extents() must be representable as a value of type size_type
    (void) mds.size();
}

void test_size_when_index_type_is_unsigned() {
    mdspan mds{some_ints.data(), dextents<unsigned char, 3>{8, 8, 4}};
    // The size of the multidimensional index space extents() must be representable as a value of type size_type
    (void) mds.size();
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({
        test_construction_from_other_mdspan,
#ifdef __cpp_multidimensional_subscript // TRANSITION, P2128R6
        test_access_with_invalid_multidimensional_index_1,
        test_access_with_nonrepresentable_index_1,
#endif // __cpp_multidimensional_subscript
        test_access_with_invalid_multidimensional_index_2,
        test_access_with_nonrepresentable_index_2,
        test_size_when_index_type_is_signed,
        test_size_when_index_type_is_unsigned,
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0

    return exec.run(argc, argv);
}
