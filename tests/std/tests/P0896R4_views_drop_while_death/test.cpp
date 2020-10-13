// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

#include <cassert>
#include <cstddef>
#include <ranges>
#include <vector>

#include <test_death.hpp>
using namespace std;

static int some_ints[] = {0, 1, 2, 3};

[[maybe_unused]] constexpr auto lambda = [x = 42](int) { return x == 42; };
using DWV                              = decltype(ranges::drop_while_view{some_ints, lambda});

void test_view_predicate() {
    DWV r;
    (void) r.pred(); // value-initialized drop_while_view has no predicate
}

void test_view_begin() {
    DWV r;
    (void)
        r.begin(); // N4861 [range.drop_while.view]/3 forbids calling begin on a drop_while_view that holds no predicate
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

    exec.add_death_tests({
        test_view_predicate,
        test_view_begin,
    });

    return exec.run(argc, argv);
}
