// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

#include <ranges>
#include <span>

#include <test_death.hpp>
using namespace std;

[[maybe_unused]] constexpr auto lambda = [x = 42](int) { return x == 42; };
using TWV                              = decltype(ranges::take_while_view{span<int, 0>{}, lambda});

void test_view_predicate() {
    TWV r;
    (void) r.pred(); // value-initialized take_while_view has no predicate
}

void test_view_end() {
    TWV r;
    (void) r.end(); // N4868 [range.take_while.view] forbids calling end on a take_while_view that holds no predicate
}

void test_view_const_end() {
    const TWV r;
    (void) r.end(); // N4868 [range.take_while.view] forbids calling end on a take_while_view that holds no predicate
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

    exec.add_death_tests({
        test_view_predicate,
        test_view_end,
        test_view_const_end,
    });

    return exec.run(argc, argv);
}
