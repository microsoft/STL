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

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;
    exec.add_death_tests({
        test_construction_from_other_mdspan,
    });
    return exec.run(argc, argv);
}
