// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

#include <array>
#include <mdspan>

#include <test_death.hpp>

using namespace std;

constexpr array<int, 128> some_ints{};

void test_construction_from_other_mdspan() {
    auto mds1 = mdspan{some_ints.data(), 8, 2, 8};
    // For each rank index r of extents_type, static_extent(r) == dynamic_extent || static_extent(r) == other.extent(r)
    // must be true
    [[maybe_unused]] mdspan<const int, extents<int, 8, 8, 2>> mds2{mds1};
}

#ifdef __cpp_multidimensional_subscript // TRANSITION, P2128R6
void test_access_with_invalid_multidimensional_index_1() {
    auto mds = mdspan{some_ints.data(), 4, 4};
    // I must be a multidimensional index in extents()
    (void) mds[3, 4];
}
#endif // __cpp_multidimensional_subscript

void test_access_with_invalid_multidimensional_index_2() {
    auto mds = mdspan{some_ints.data(), 5, 5};
    // I must be a multidimensional index in extents()
    (void) mds[array{4, 5}];
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;
    exec.add_death_tests({
        test_construction_from_other_mdspan,
#ifdef __cpp_multidimensional_subscript // TRANSITION, P2128R6
        test_access_with_invalid_multidimensional_index_1,
#endif // __cpp_multidimensional_subscript
        test_access_with_invalid_multidimensional_index_2,
    });
    return exec.run(argc, argv);
}
