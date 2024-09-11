// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

#include <generator>
#include <utility>

#include <test_death.hpp>

std::generator<int> gen() {
    co_return;
}

void test_begin_after_initial_suspend_point() {
    auto g = gen();
    (void) g.begin();
    (void) g.begin();
}

void test_begin_after_moving_from() {
    auto g  = gen();
    auto g2 = std::move(g);
    (void) g.begin();
}

void test_end_iterator_dereference() {
    auto g = gen();
    auto i = g.begin();
    (void) *i;
}

void test_end_iterator_incrementation() {
    auto g = gen();
    auto i = g.begin();
    ++i;
}

int main(int argc, char** argv) {
    std_testing::death_test_executive exec;

    exec.add_death_tests({
        test_begin_after_initial_suspend_point,
        test_begin_after_moving_from,
        test_end_iterator_dereference,
        test_end_iterator_incrementation,
    });

    return exec.run(argc, argv);
}
