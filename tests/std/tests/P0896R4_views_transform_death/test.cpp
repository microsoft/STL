// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

#include <cassert>
#include <cstddef>
#include <ranges>
#include <span>
#include <vector>

#include <test_death.hpp>
using namespace std;

static int some_ints[] = {0, 1, 2, 3};

[[maybe_unused]] constexpr auto lambda = [x = 42](int) { return x == 42; };
using TV                               = decltype(ranges::transform_view{span{some_ints}, lambda});

void test_constructor_wrong_range() {
    vector<int> vec0{0, 1, 2, 3};
    vector<int> vec1{4, 5, 6, 7};
    auto r0            = views::transform(vec0, lambda);
    using R            = decltype(r0);
    same_as<R> auto r1 = views::transform(vec1, lambda);
    ranges::iterator_t<R> i{r0, r1.begin().base()}; // vector iterators in range are from different containers
}

void test_operator_star_value_initialized_iterator() {
    ranges::iterator_t<TV> i{};
    (void) (*i); // cannot dereference value-initialized transform_view iterator
}

void test_operator_star_end_iterator() {
    TV r{some_ints, lambda};
    ranges::iterator_t<TV> i = ranges::next(r.begin(), r.end());
    (void) (*i); // cannot dereference end transform_view iterator
}

void test_operator_preincrement_value_initialized_iterator() {
    ranges::iterator_t<TV> i{};
    ++i; // cannot increment value-initialized transform_view iterator
}

void test_operator_preincrement_after_end() {
    TV r{some_ints, lambda};
    ranges::iterator_t<TV> i = ranges::next(r.begin(), r.end());
    ++i; // cannot increment transform_view iterator past end
}

void test_operator_postincrement_value_initialized_iterator() {
    ranges::iterator_t<TV> i{};
    i++; // cannot increment value-initialized transform_view iterator
}

void test_operator_postincrement_after_end() {
    TV r{some_ints, lambda};
    ranges::iterator_t<TV> i = ranges::next(r.begin(), r.end());
    i++; // cannot increment transform_view iterator past end
}

void test_operator_predecrement_value_initialized_iterator() {
    ranges::iterator_t<TV> i{};
    --i; // cannot decrement value-initialized transform_view iterator
}

void test_operator_predecrement_before_begin() {
    TV r{some_ints, lambda};
    ranges::iterator_t<TV> i = r.begin();
    --i; // cannot decrement transform_view iterator before begin
}

void test_operator_postdecrement_value_initialized_iterator() {
    ranges::iterator_t<TV> i{};
    i--; // cannot decrement value-initialized transform_view iterator
}

void test_operator_postdecrement_before_begin() {
    TV r{some_ints, lambda};
    ranges::iterator_t<TV> i = r.begin();
    i--; // cannot decrement transform_view iterator before begin
}

void test_operator_seek_forward_value_initialized_iterator() {
    ranges::iterator_t<TV> i{};
    i += 42; // cannot seek value-initialized transform_view iterator
}

void test_operator_seek_forward_after_end() {
    TV r{some_ints, lambda};
    ranges::iterator_t<TV> i = r.begin() + ranges::distance(some_ints) / 2;
    i += ranges::distance(some_ints); // cannot seek transform_view iterator past end
}

void test_operator_seek_backward_value_initialized_iterator() {
    ranges::iterator_t<TV> i{};
    i -= 42; // cannot seek value-initialized transform_view iterator
}

void test_operator_seek_backward_before_begin() {
    TV r{some_ints, lambda};
    ranges::iterator_t<TV> i = r.begin() + ranges::distance(some_ints) / 2;
    i -= ranges::distance(some_ints); // cannot seek transform_view iterator before begin
}

void test_operator_bracket_value_initialized_iterator() {
    ranges::iterator_t<TV> i{};
    (void) i[42]; // cannot seek value-initialized transform_view iterator
}

void test_operator_bracket_before_begin() {
    TV r{some_ints, lambda};
    ranges::iterator_t<TV> i = r.begin() + ranges::distance(some_ints) / 2;
    (void) i[-ranges::distance(some_ints)]; // cannot seek transform_view iterator before begin
}

void test_operator_bracket_after_end() {
    TV r{some_ints, lambda};
    ranges::iterator_t<TV> i = r.begin() + ranges::distance(some_ints) / 2;
    (void) i[ranges::distance(some_ints)]; // cannot seek transform_view iterator after end
}

void test_operator_equal_incompatible_different() {
    TV r0{some_ints, lambda};
    ranges::iterator_t<TV> i0 = r0.begin();
    TV r1{some_ints, lambda};
    ranges::iterator_t<TV> i1 = r1.begin();
    (void) (i0 == i1); // cannot compare incompatible transform_view iterators
}

void test_operator_equal_incompatible_value_initialized() {
    TV r{some_ints, lambda};
    ranges::iterator_t<TV> i = r.begin();
    (void) (i == ranges::iterator_t<TV>{}); // cannot compare incompatible transform_view iterators
}

void test_operator_not_equal_incompatible_different() {
    TV r0{some_ints, lambda};
    ranges::iterator_t<TV> i0 = r0.begin();
    TV r1{some_ints, lambda};
    ranges::iterator_t<TV> i1 = r1.begin();
    (void) (i0 != i1); // cannot compare incompatible transform_view iterators
}

void test_operator_not_equal_incompatible_value_initialized() {
    TV r{some_ints, lambda};
    ranges::iterator_t<TV> i = r.begin();
    (void) (i != ranges::iterator_t<TV>{}); // cannot compare incompatible transform_view iterators
}

void test_operator_less_incompatible_different() {
    TV r0{some_ints, lambda};
    ranges::iterator_t<TV> i0 = r0.begin();
    TV r1{some_ints, lambda};
    ranges::iterator_t<TV> i1 = r1.begin();
    (void) (i0 < i1); // cannot compare incompatible transform_view iterators
}

void test_operator_less_incompatible_value_initialized() {
    TV r{some_ints, lambda};
    ranges::iterator_t<TV> i = r.begin();
    (void) (i < ranges::iterator_t<TV>{}); // cannot compare incompatible transform_view iterators
}

void test_operator_greater_incompatible_different() {
    TV r0{some_ints, lambda};
    ranges::iterator_t<TV> i0 = r0.begin();
    TV r1{some_ints, lambda};
    ranges::iterator_t<TV> i1 = r1.begin();
    (void) (i0 > i1); // cannot compare incompatible transform_view iterators
}

void test_operator_greater_incompatible_value_initialized() {
    TV r{some_ints, lambda};
    ranges::iterator_t<TV> i = r.begin();
    (void) (i > ranges::iterator_t<TV>{}); // cannot compare incompatible transform_view iterators
}

void test_operator_less_equal_incompatible_different() {
    TV r0{some_ints, lambda};
    ranges::iterator_t<TV> i0 = r0.begin();
    TV r1{some_ints, lambda};
    ranges::iterator_t<TV> i1 = r1.begin();
    (void) (i0 <= i1); // cannot compare incompatible transform_view iterators
}

void test_operator_less_equal_incompatible_value_initialized() {
    TV r{some_ints, lambda};
    ranges::iterator_t<TV> i = r.begin();
    (void) (i <= ranges::iterator_t<TV>{}); // cannot compare incompatible transform_view iterators
}

void test_operator_greater_equal_incompatible_different() {
    TV r0{some_ints, lambda};
    ranges::iterator_t<TV> i0 = r0.begin();
    TV r1{some_ints, lambda};
    ranges::iterator_t<TV> i1 = r1.begin();
    (void) (i0 >= i1); // cannot compare incompatible transform_view iterators
}

void test_operator_greater_equal_incompatible_value_initialized() {
    TV r{some_ints, lambda};
    ranges::iterator_t<TV> i = r.begin();
    (void) (i >= ranges::iterator_t<TV>{}); // cannot compare incompatible transform_view iterators
}

void test_operator_spaceship_incompatible_different() {
    TV r0{some_ints, lambda};
    ranges::iterator_t<TV> i0 = r0.begin();
    TV r1{some_ints, lambda};
    ranges::iterator_t<TV> i1 = r1.begin();
    (void) (i0 <=> i1); // cannot compare incompatible transform_view iterators
}

void test_operator_spaceship_incompatible_value_initialized() {
    TV r{some_ints, lambda};
    ranges::iterator_t<TV> i = r.begin();
    (void) (i <=> ranges::iterator_t<TV>{}); // cannot compare incompatible transform_view iterators
}

void test_operator_plus_value_initialized() {
    ranges::iterator_t<TV> i{};
    (void) (i + 42); // cannot seek value-initialized transform_view iterator
}

void test_operator_plus_before_begin() {
    TV r{some_ints, lambda};
    ranges::iterator_t<TV> i = r.begin() + ranges::distance(some_ints) / 2;
    (void) (i + -ranges::distance(some_ints)); // cannot seek transform_view iterator before begin
}

void test_operator_plus_after_end() {
    TV r{some_ints, lambda};
    ranges::iterator_t<TV> i = r.begin() + ranges::distance(some_ints) / 2;
    (void) (i + ranges::distance(some_ints)); // cannot seek transform_view iterator after end
}

void test_flipped_operator_plus_value_initialized() {
    ranges::iterator_t<TV> i{};
    (void) (42 + i); // cannot seek value-initialized transform_view iterator
}

void test_flipped_operator_plus_before_begin() {
    TV r{some_ints, lambda};
    ranges::iterator_t<TV> i = r.begin() + ranges::distance(some_ints) / 2;
    (void) (-ranges::distance(some_ints) + i); // cannot seek transform_view iterator before begin
}

void test_flipped_operator_plus_after_end() {
    TV r{some_ints, lambda};
    ranges::iterator_t<TV> i = r.begin() + ranges::distance(some_ints) / 2;
    (void) (ranges::distance(some_ints) + i); // cannot seek transform_view iterator after end
}

void test_operator_minus_value_initialized() {
    ranges::iterator_t<TV> i{};
    (void) (i - 42); // cannot seek value-initialized transform_view iterator
}

void test_operator_minus_before_begin() {
    TV r{some_ints, lambda};
    ranges::iterator_t<TV> i = r.begin() + ranges::distance(some_ints) / 2;
    (void) (i - ranges::distance(some_ints)); // cannot seek transform_view iterator before begin
}

void test_operator_minus_after_end() {
    TV r{some_ints, lambda};
    ranges::iterator_t<TV> i = r.begin() + ranges::distance(some_ints) / 2;
    (void) (i - -ranges::distance(some_ints)); // cannot seek transform_view iterator after end
}

void test_operator_minus_incompatible_different() {
    TV r0{some_ints, lambda};
    ranges::iterator_t<TV> i0 = r0.begin();
    TV r1{some_ints, lambda};
    ranges::iterator_t<TV> i1 = r1.begin();
    (void) (i0 - i1); // cannot compare incompatible transform_view iterators
}

void test_operator_minus_incompatible_value_initialized() {
    TV r{some_ints, lambda};
    ranges::iterator_t<TV> i = r.begin();
    (void) (i - ranges::iterator_t<TV>{}); // cannot compare incompatible transform_view iterators
}

void test_iter_move_value_initialized_iterator() {
    ranges::iterator_t<TV> i{};
    (void) ranges::iter_move(i); // cannot dereference value-initialized transform_view iterator
}

void test_sentinel_compare_value_initialized() {
    auto r = ranges::subrange{counted_iterator{some_ints, ranges::distance(some_ints)}, default_sentinel}
           | views::transform(lambda);
    using R = decltype(r);
    static_assert(!ranges::common_range<R>);
    (void) (ranges::iterator_t<R>{} == r.end());
}

void test_sentinel_difference_value_initialized() {
    auto r = ranges::subrange{counted_iterator{some_ints, ranges::distance(some_ints)}, default_sentinel}
           | views::transform(lambda);
    using R = decltype(r);
    static_assert(!ranges::common_range<R>);
    static_assert(sized_sentinel_for<ranges::sentinel_t<R>, ranges::iterator_t<R>>);
    (void) (ranges::iterator_t<R>{} - r.end());
}

void test_flipped_sentinel_difference_value_initialized() {
    auto r = ranges::subrange{counted_iterator{some_ints, ranges::distance(some_ints)}, default_sentinel}
           | views::transform(lambda);
    using R = decltype(r);
    static_assert(!ranges::common_range<R>);
    static_assert(sized_sentinel_for<ranges::sentinel_t<R>, ranges::iterator_t<R>>);
    (void) (r.end() - ranges::iterator_t<R>{});
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({
        test_constructor_wrong_range,
        test_operator_star_value_initialized_iterator,
        test_operator_star_end_iterator,
        test_operator_preincrement_value_initialized_iterator,
        test_operator_preincrement_after_end,
        test_operator_postincrement_value_initialized_iterator,
        test_operator_postincrement_after_end,
        test_operator_predecrement_value_initialized_iterator,
        test_operator_predecrement_before_begin,
        test_operator_postdecrement_value_initialized_iterator,
        test_operator_postdecrement_before_begin,
        test_operator_seek_forward_value_initialized_iterator,
        test_operator_seek_forward_after_end,
        test_operator_seek_backward_value_initialized_iterator,
        test_operator_seek_backward_before_begin,
        test_operator_bracket_value_initialized_iterator,
        test_operator_bracket_before_begin,
        test_operator_bracket_after_end,
        test_operator_equal_incompatible_different,
        test_operator_equal_incompatible_value_initialized,
        test_operator_not_equal_incompatible_different,
        test_operator_not_equal_incompatible_value_initialized,
        test_operator_less_incompatible_different,
        test_operator_less_incompatible_value_initialized,
        test_operator_greater_incompatible_different,
        test_operator_greater_incompatible_value_initialized,
        test_operator_less_equal_incompatible_different,
        test_operator_less_equal_incompatible_value_initialized,
        test_operator_greater_equal_incompatible_different,
        test_operator_greater_equal_incompatible_value_initialized,
        test_operator_spaceship_incompatible_different,
        test_operator_spaceship_incompatible_value_initialized,
        test_operator_plus_value_initialized,
        test_operator_plus_before_begin,
        test_operator_plus_after_end,
        test_flipped_operator_plus_value_initialized,
        test_flipped_operator_plus_before_begin,
        test_flipped_operator_plus_after_end,
        test_operator_minus_value_initialized,
        test_operator_minus_before_begin,
        test_operator_minus_after_end,
        test_operator_minus_incompatible_different,
        test_operator_minus_incompatible_value_initialized,
        test_iter_move_value_initialized_iterator,
        test_sentinel_compare_value_initialized,
        test_sentinel_difference_value_initialized,
        test_flipped_sentinel_difference_value_initialized,
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0

    return exec.run(argc, argv);
}
