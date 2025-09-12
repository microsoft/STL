// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#if defined(__clang__) && defined(_M_ARM64EC) // TRANSITION, LLVM-158341
int main() {}
#else // ^^^ workaround / no workaround vvv

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

#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({
        test_begin_after_initial_suspend_point,
        test_begin_after_moving_from,
        test_end_iterator_dereference,
        test_end_iterator_incrementation,
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0

    return exec.run(argc, argv);
}

#endif // ^^^ no workaround ^^^
