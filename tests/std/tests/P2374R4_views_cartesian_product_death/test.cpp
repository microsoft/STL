// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

#include <array>
#include <cassert>
#include <limits>
#include <ranges>
#include <span>
#include <utility>

#include <test_death.hpp>

using namespace std;

constexpr auto much_ints    = views::iota(0ull, (numeric_limits<unsigned long long>::max)());
constexpr array little_ints = {1, 2, 3, 4};
void test_view_size() {
    auto v = views::cartesian_product(much_ints, much_ints, much_ints);
    // Size of cartesian product cannot be represented by _Size_type<false> (N4928 [range.cartesian.view]/10).
    (void) v.size();
}

void test_view_const_size() {
    auto v = views::cartesian_product(much_ints, much_ints, much_ints);
    // Size of cartesian product cannot be represented by _Size_type<true> (N4928 [range.cartesian.view]/10).
    (void) as_const(v).size();
}

void test_iterator_advance_past_end_with_small_offset() {
    // This preconditions check works only when all ranges model ranges::sized_range
    auto v = views::cartesian_product(little_ints, little_ints, little_ints);
    auto i = v.begin();
    // Cannot advance cartesian_product_view iterator past end (N4928 [range.cartesian.iterator]/19).
    i += 65;
}

void test_iterator_advance_past_end_with_big_offset() {
    // This preconditions check works only when all ranges model ranges::sized_range
    auto v = views::cartesian_product(little_ints, little_ints, little_ints);
    auto i = v.begin();
    // Cannot advance cartesian_product_view iterator past end (N4928 [range.cartesian.iterator]/19).
    i += 1000;
}

void test_iterator_advance_before_begin() {
    auto v = views::cartesian_product(little_ints, little_ints, little_ints);
    auto i = v.end();
    // Cannot advance cartesian_product_view iterator before begin (N4928 [range.cartesian.iterator]/19).
    i += -65;
}

void test_iterator_differencing() {
    auto v  = views::cartesian_product(much_ints, much_ints, much_ints);
    auto i1 = v.begin();
    auto i2 = v.end();
    // Scaled-sum cannot be represented by _Difference_type<false> (N4928 [range.cartesian.iterator]/8).
    (void) (i2 - i1);
}

void test_iterator_and_default_sentinel_differencing() {
    auto v = views::cartesian_product(much_ints, much_ints, much_ints);
    auto i = v.begin();
    // Scaled-sum cannot be represented by _Difference_type<false> (N4928 [range.cartesian.iterator]/8).
    (void) (default_sentinel - i);
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({
        test_view_size,
        test_view_const_size,
        test_iterator_advance_past_end_with_small_offset,
        test_iterator_advance_past_end_with_big_offset,
        test_iterator_advance_before_begin,
        test_iterator_differencing,
        test_iterator_and_default_sentinel_differencing,
    });
#else // ^^^ test everything / test only _CONTAINER_DEBUG_LEVEL cases vvv
    exec.add_death_tests({
        test_view_size,
        test_view_const_size,
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0

    return exec.run(argc, argv);
}
