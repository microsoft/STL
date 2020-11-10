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
using FV                               = decltype(ranges::filter_view{some_ints, lambda});

void test_view_predicate() {
    FV r;
    (void) r.pred(); // value-initialized filter_view has no predicate
}

void test_view_begin() {
    FV r;
    (void) r.begin(); // N4861 [range.filter.view]/3 forbids calling begin on a filter_view that holds no predicate
}

void test_constructor_wrong_range() {
    vector<int> vec0{0, 1, 2, 3};
    vector<int> vec1{4, 5, 6, 7};
    auto r0            = views::filter(vec0, lambda);
    using R            = decltype(r0);
    same_as<R> auto r1 = views::filter(vec1, lambda);
    ranges::iterator_t<R> i{r0, r1.begin().base()}; // vector iterators in range are from different containers
}

void test_operator_star_value_initialized_iterator() {
    ranges::iterator_t<FV> i{};
    (void) (*i); // cannot dereference value-initialized filter_view iterator
}

void test_operator_star_end_iterator() {
    FV r{some_ints, lambda};
    ranges::iterator_t<FV> i = ranges::next(r.begin(), r.end());
    (void) (*i); // cannot dereference end filter_view iterator
}

void test_operator_arrow_value_initialized_iterator() {
    ranges::iterator_t<FV> i{};
    (void) (i.operator->()); // cannot dereference value-initialized filter_view iterator
}

void test_operator_arrow_end_iterator() {
    FV r{some_ints, lambda};
    ranges::iterator_t<FV> i = ranges::next(r.begin(), r.end());
    (void) (i.operator->()); // cannot dereference end filter_view iterator
}

void test_operator_preincrement_value_initialized_iterator() {
    ranges::iterator_t<FV> i{};
    ++i; // cannot increment value-initialized filter_view iterator
}

void test_operator_preincrement_after_end() {
    FV r{some_ints, lambda};
    ranges::iterator_t<FV> i = ranges::next(r.begin(), r.end());
    ++i; // cannot increment filter_view iterator past end
}

void test_operator_postincrement_value_initialized_iterator() {
    ranges::iterator_t<FV> i{};
    i++; // cannot increment value-initialized filter_view iterator
}

void test_operator_postincrement_after_end() {
    FV r{some_ints, lambda};
    ranges::iterator_t<FV> i = ranges::next(r.begin(), r.end());
    i++; // cannot increment filter_view iterator past end
}

void test_operator_predecrement_before_begin() {
    FV r{some_ints, lambda};
    ranges::iterator_t<FV> i = r.begin();
    --i; // cannot decrement filter_view iterator before begin
}

void test_operator_postdecrement_before_begin() {
    FV r{some_ints, lambda};
    ranges::iterator_t<FV> i = r.begin();
    i--; // cannot decrement filter_view iterator before begin
}

void test_operator_equal_incompatible_different() {
    FV r0{some_ints, lambda};
    ranges::iterator_t<FV> i0 = r0.begin();
    FV r1{some_ints, lambda};
    ranges::iterator_t<FV> i1 = r1.begin();
    (void) (i0 == i1); // cannot compare incompatible filter_view iterators for equality
}

void test_operator_equal_incompatible_value_initialized() {
    FV r{some_ints, lambda};
    ranges::iterator_t<FV> i = r.begin();
    (void) (i == ranges::iterator_t<FV>{}); // cannot compare incompatible filter_view iterators for equality
}

void test_iter_move_value_initialized_iterator() {
    ranges::iterator_t<FV> i{};
    (void) ranges::iter_move(i); // cannot dereference value-initialized filter_view iterator
}

void test_iter_swap_value_initialized_iterators() {
    ranges::iterator_t<FV> i0{};
    ranges::iterator_t<FV> i1{};
    (void) ranges::iter_swap(i0, i1); // cannot dereference value-initialized filter_view iterator
}

void test_iter_swap_value_initialized_iterator_left() {
    ranges::iterator_t<FV> i0{};
    FV r{some_ints, lambda};
    ranges::iterator_t<FV> i1 = r.begin();
    (void) ranges::iter_swap(i0, i1); // cannot dereference value-initialized filter_view iterator
}

void test_iter_swap_value_initialized_iterator_right() {
    FV r{some_ints, lambda};
    ranges::iterator_t<FV> i0 = r.begin();
    ranges::iterator_t<FV> i1{};
    (void) ranges::iter_swap(i0, i1); // cannot dereference value-initialized filter_view iterator
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({
        test_view_predicate,
        test_view_begin,

        test_constructor_wrong_range,
        test_operator_star_value_initialized_iterator,
        test_operator_star_end_iterator,
        test_operator_arrow_value_initialized_iterator,
        test_operator_arrow_end_iterator,
        test_operator_preincrement_value_initialized_iterator,
        test_operator_preincrement_after_end,
        test_operator_postincrement_value_initialized_iterator,
        test_operator_postincrement_after_end,
        test_operator_predecrement_before_begin,
        test_operator_postdecrement_before_begin,
        test_operator_equal_incompatible_different,
        test_operator_equal_incompatible_value_initialized,
        test_iter_move_value_initialized_iterator,
        test_iter_swap_value_initialized_iterators,
        test_iter_swap_value_initialized_iterator_left,
        test_iter_swap_value_initialized_iterator_right,
    });
#else // ^^^ test everything / test only _CONTAINER_DEBUG_LEVEL cases vvv
    exec.add_death_tests({
        test_view_predicate,
        test_view_begin,
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0

    return exec.run(argc, argv);
}
