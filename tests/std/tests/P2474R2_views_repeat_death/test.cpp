// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

#include <cstddef>
#include <limits>
#include <ranges>

#include <test_death.hpp>

using namespace std;
using ranges::repeat_view;

void test_view_lvalue_negative() {
    const int lvalue        = 1;
    [[maybe_unused]] auto v = repeat_view(lvalue, -1); // bound must be positive
}

void test_view_rvalue_negative() {
    [[maybe_unused]] auto v = repeat_view(1, -1); // bound must be positive
}

void test_iter_decrement() {
    repeat_view v(1, 1);
    auto it = v.begin();
    --it; // can't decrement bound past 0
}

void test_iter_decrement_overflow() {
    repeat_view v(1);
    auto it = v.begin();
    it += (numeric_limits<ptrdiff_t>::min)();
    --it; // integer overflow
}

void test_iter_increment() {
    repeat_view v(1);
    auto it = v.begin();
    it += (numeric_limits<ptrdiff_t>::max)();
    ++it; // integer overflow
}

void test_iter_subtract_zero() {
    repeat_view v(1, 1);
    auto it = v.begin();
    it -= 1; // can't subtract bound past 0
}

void test_iter_subtract_pos_overflow() {
    repeat_view v(1);
    auto it = v.begin();
    it += (numeric_limits<ptrdiff_t>::max)();
    it -= -1; // integer overflow
}

void test_iter_subtract_neg_overflow() {
    repeat_view v(1);
    auto it = v.begin();
    it += (numeric_limits<ptrdiff_t>::min)();
    it -= 1; // integer overflow
}

void test_iter_add_zero() {
    repeat_view v(1, 1);
    auto it = v.begin();
    it += -1; // can't subtract bound past 0
}

void test_iter_add_pos_overflow() {
    repeat_view v(1);
    auto it = v.begin();
    it += (numeric_limits<ptrdiff_t>::max)();
    it += 1; // integer overflow
}

void test_iter_add_neg_overflow() {
    repeat_view v(1);
    auto it = v.begin();
    it += (numeric_limits<ptrdiff_t>::min)();
    it += -1; // integer overflow
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

    exec.add_death_tests({
        test_view_lvalue_negative,
        test_view_rvalue_negative,
        test_iter_decrement,
        test_iter_decrement_overflow,
        test_iter_increment,
        test_iter_subtract_zero,
        test_iter_subtract_pos_overflow,
        test_iter_subtract_neg_overflow,
        test_iter_add_zero,
        test_iter_add_pos_overflow,
        test_iter_add_neg_overflow,
    });
    return exec.run(argc, argv);
}
