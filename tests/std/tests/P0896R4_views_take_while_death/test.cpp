// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

#include <cassert>
#include <cstddef>
#include <ranges>
#include <span>

#include <test_death.hpp>
using namespace std;

[[maybe_unused]] constexpr auto lambda = [x = 42](int) { return x == 42; };
using DWV                              = decltype(ranges::take_while_view{span<int, 0>{}, lambda});

void test_view_predicate() {
    DWV r;
    (void) r.pred(); // value-initialized take_while_view has no predicate
}

void test_view_end() {
    DWV r;
    (void) r.end(); // N4861 [range.take_while.view] forbids calling end on a take_while_view that holds no predicate
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

    exec.add_death_tests({
        test_view_predicate,
        test_view_end,
    });

    return exec.run(argc, argv);
}
