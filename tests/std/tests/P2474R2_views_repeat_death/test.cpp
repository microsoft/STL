// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

#include <ranges>

#include <test_death.hpp>

using namespace std;
using ranges::repeat_view;

void test_view_negative() {
    [[maybe_unused]] auto v = repeat_view(1, -1); // bound must be positive
}

void test_iter_decrement() {
    repeat_view v(1, 0);
    auto it = v.begin();
    --it; // can't decrement bound past 0
}
void test_iter_increment() {
    repeat_view v(1);
    auto it = v.begin();
    it += (numeric_limits<ptrdiff_t>::max)() - 1;
    it++;
    it++; // integer overflow
}

void test_iter_subtract_zero() {
    repeat_view v(1, 3);
    auto it = v.begin();
    it -= 4; // can't subtract bound past 0
}

void test_iter_subtract_overflow() {
    repeat_view v(1);
    auto it = v.begin();
    it += (numeric_limits<ptrdiff_t>::max)() - 1;
    it -= -2; // integer overflow
}

void test_iter_add_zero() {
    repeat_view v(1, 3);
    auto it = v.begin();
    it += -4; // can't subtract bound past 0
}

void test_iter_add_overflow() {
    repeat_view v(1);
    auto it = v.begin();
    it += (numeric_limits<ptrdiff_t>::max)() - 1;
    it += 2; // integer overflow
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({
        test_view_negative,
        test_iter_decrement,
        test_iter_increment,
        test_iter_subtract_zero,
        test_iter_subtract_overflow,
        test_iter_add_zero,
        test_iter_add_overflow,
    });
#else // ^^^ test everything / test only _CONTAINER_DEBUG_LEVEL case vvv
    exec.add_death_tests({
        test_view_negative,
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0
    return exec.run(argc, argv);
}
