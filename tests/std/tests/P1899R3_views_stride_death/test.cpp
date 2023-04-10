// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

#include <cstddef>
#include <limits>
#include <ranges>

#include <test_death.hpp>

using namespace std;

static constexpr int some_ints[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

void test_view_negative_stride() {
    [[maybe_unused]] auto v = ranges::stride_view(some_ints, -1); // stride must be greater than 0
}

void test_iterator_dereference_at_end() {
    auto v  = ranges::stride_view(some_ints, 3);
    auto it = v.end();
    (void) *it; // cannot dereference stride_view end iterator
}

void test_iterator_preincrement_past_end() {
    auto v  = ranges::stride_view(some_ints, 3);
    auto it = v.end();
    ++it; // cannot increment stride_view end iterator
}

void test_iterator_postincrement_past_end() {
    auto v  = ranges::stride_view(some_ints, 3);
    auto it = v.end();
    it++; // cannot increment stride_view end iterator
}

void test_iterator_advance_past_end() {
    auto v  = ranges::stride_view(some_ints, 3);
    auto it = v.begin();
    it += 5; // cannot advance stride_view iterator past end
}

void test_iterator_advance_past_end_with_integer_overflow() {
    auto v  = ranges::stride_view(some_ints, 3);
    auto it = v.begin();
    it += (numeric_limits<ptrdiff_t>::max)() / 2; // cannot advance stride_view iterator past end (integer overflow)
}

void test_iterator_advance_negative_min() {
    auto v  = ranges::stride_view(some_ints, 3);
    auto it = v.begin();
    it -= (numeric_limits<ptrdiff_t>::min)(); // cannot advance stride_view iterator past end (integer overflow)
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({
        test_view_negative_stride,
        test_iterator_dereference_at_end,
        test_iterator_preincrement_past_end,
        test_iterator_postincrement_past_end,
        test_iterator_advance_past_end,
        test_iterator_advance_past_end_with_integer_overflow,
        test_iterator_advance_negative_min,
    });
#else // ^^^ test everything / test only _CONTAINER_DEBUG_LEVEL case vvv
    exec.add_death_tests({
        test_view_negative_stride,
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0

    return exec.run(argc, argv);
}
