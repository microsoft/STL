// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <iterator>
#include <memory>
#include <numeric>
#include <random>

#ifdef __cpp_lib_execution
#include <execution>
using std::execution::par;
#endif // __cpp_lib_execution

using namespace std;

struct Val;

struct RRef {
    RRef(const Val&);
};

struct Val {
    using difference_type = int;
    explicit Val(const RRef&);
    Val& operator=(const RRef&);
    bool operator==(const Val&) const;
#if _HAS_CXX20
    strong_ordering operator<=>(const Val&) const;
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
    bool operator!=(const Val&) const;
    bool operator<(const Val&) const;
    bool operator<=(const Val&) const;
    bool operator>(const Val&) const;
    bool operator>=(const Val&) const;
#endif // ^^^ !_HAS_CXX20 ^^^
    Val& operator++();
    Val operator++(int);
};

struct I {
    using iterator_category = random_access_iterator_tag;
    using value_type        = Val;
    using difference_type   = int;
    using pointer           = void;
    using reference         = Val&;
    reference operator*() const;
    reference operator[](int) const;
    I& operator++();
    I operator++(int);
    I& operator--();
    I operator--(int);
    I& operator+=(int);
    I& operator-=(int);
    friend bool operator==(const I&, const I&);
    friend bool operator!=(const I&, const I&);
    friend bool operator<(const I&, const I&);
    friend bool operator<=(const I&, const I&);
    friend bool operator>(const I&, const I&);
    friend bool operator>=(const I&, const I&);
    friend difference_type operator-(I, I);
    friend I operator+(I, difference_type);
    friend I operator-(I, difference_type);
    friend I operator+(difference_type, I);
    friend RRef iter_move(const I&);

#if !defined(__clang__) && !defined(__EDG__) // TRANSITION, VSO-1941943
    int dummy{0};
#endif // ^^^ workaround ^^^
};

#if _HAS_CXX20
static_assert(random_access_iterator<I>);
static_assert(sortable<I>);
#endif // _HAS_CXX20

extern Val val;
extern mt19937 urbg; // UniformRandomBitGenerator
const int zero = 0;
// GH-4109: <algorithm>: iter_value_t<I> should always use direct-initialization
void test_gh_4109() {
    I nil{};

    bool (*pred)(Val)       = nullptr;
    bool (*pred2)(Val, Val) = nullptr;
    bool (*comp)(Val, Val)  = nullptr;
    Val (*gen)()            = nullptr;
#if _HAS_AUTO_PTR_ETC
    int (*rng)(int) = nullptr;
#endif // _HAS_AUTO_PTR_ETC
    Val (*unop)(Val)       = nullptr;
    Val (*binop)(Val, Val) = nullptr;
#if _HAS_CXX20
    strong_ordering (*comp_three_way)(Val, Val) = nullptr;
#endif // _HAS_CXX20

    (void) all_of(nil, nil, pred);
    (void) any_of(nil, nil, pred);
    (void) none_of(nil, nil, pred);
    for_each(nil, nil, pred);
#if _HAS_CXX17
    for_each_n(nil, zero, pred);
#endif // _HAS_CXX17
    (void) find(nil, nil, val);
    (void) find_if(nil, nil, pred);
    (void) find_if_not(nil, nil, pred);
    (void) find_end(nil, nil, nil, nil);
    (void) find_end(nil, nil, nil, nil, pred2);
    (void) find_first_of(nil, nil, nil, nil);
    (void) find_first_of(nil, nil, nil, nil, pred2);
    (void) adjacent_find(nil, nil);
    (void) adjacent_find(nil, nil, pred2);
    (void) count(nil, nil, val);
    (void) count_if(nil, nil, pred);
    (void) mismatch(nil, nil, nil);
    (void) mismatch(nil, nil, nil, pred2);
    (void) mismatch(nil, nil, nil, nil);
    (void) mismatch(nil, nil, nil, nil, pred2);
    (void) equal(nil, nil, nil);
    (void) equal(nil, nil, nil, pred2);
    (void) equal(nil, nil, nil, nil);
    (void) equal(nil, nil, nil, nil, pred2);
    (void) is_permutation(nil, nil, nil);
    (void) is_permutation(nil, nil, nil, pred2);
    (void) is_permutation(nil, nil, nil, nil);
    (void) is_permutation(nil, nil, nil, nil, pred2);
    (void) search(nil, nil, nil, nil);
    (void) search(nil, nil, nil, nil, pred2);
    (void) search_n(nil, nil, zero, val);
    (void) search_n(nil, nil, zero, val, pred2);
    copy(nil, nil, nil);
    copy_n(nil, zero, nil);
    copy_if(nil, nil, nil, pred);
    copy_backward(nil, nil, nil);
    move(nil, nil, nil);
    move_backward(nil, nil, nil);
    swap_ranges(nil, nil, nil);
    transform(nil, nil, nil, unop);
    transform(nil, nil, nil, nil, binop);
    replace(nil, nil, val, val);
    replace_if(nil, nil, pred, val);
    replace_copy(nil, nil, nil, val, val);
    replace_copy_if(nil, nil, nil, pred, val);
    fill(nil, nil, val);
    fill_n(nil, zero, val);
    generate(nil, nil, gen);
    generate_n(nil, zero, gen);
    (void) remove(nil, nil, val);
    (void) remove_if(nil, nil, pred);
    remove_copy(nil, nil, nil, val);
    remove_copy_if(nil, nil, nil, pred);
    (void) unique(nil, nil);
    (void) unique(nil, nil, pred2);
    unique_copy(nil, nil, nil);
    unique_copy(nil, nil, nil, pred2);
    reverse(nil, nil);
    reverse_copy(nil, nil, nil);
    rotate(nil, nil, nil);
    rotate_copy(nil, nil, nil, nil);
#if _HAS_AUTO_PTR_ETC
    random_shuffle(nil, nil);
    random_shuffle(nil, nil, rng);
#endif // _HAS_AUTO_PTR_ETC
#if _HAS_CXX17
    sample(nil, nil, nil, zero, urbg);
#endif // _HAS_CXX17
    shuffle(nil, nil, urbg);
#if _HAS_CXX20
    shift_left(nil, nil, 1729);
    shift_right(nil, nil, 1729);
#endif // _HAS_CXX20
    sort(nil, nil);
    sort(nil, nil, comp);
    stable_sort(nil, nil);
    stable_sort(nil, nil, comp);
    partial_sort(nil, nil, nil);
    partial_sort(nil, nil, nil, comp);
    partial_sort_copy(nil, nil, nil, nil);
    partial_sort_copy(nil, nil, nil, nil, comp);
    (void) is_sorted(nil, nil);
    (void) is_sorted(nil, nil, comp);
    (void) is_sorted_until(nil, nil);
    (void) is_sorted_until(nil, nil, comp);
    nth_element(nil, nil, nil);
    nth_element(nil, nil, nil, comp);
    (void) lower_bound(nil, nil, val);
    (void) lower_bound(nil, nil, val, comp);
    (void) upper_bound(nil, nil, val);
    (void) upper_bound(nil, nil, val, comp);
    (void) equal_range(nil, nil, val);
    (void) equal_range(nil, nil, val, comp);
    (void) binary_search(nil, nil, val);
    (void) binary_search(nil, nil, val, comp);
    (void) is_partitioned(nil, nil, pred);
    partition(nil, nil, pred);
    stable_partition(nil, nil, pred);
    partition_copy(nil, nil, nil, nil, pred);
    (void) partition_point(nil, nil, pred);
    merge(nil, nil, nil, nil, nil);
    merge(nil, nil, nil, nil, nil, comp);
    inplace_merge(nil, nil, nil);
    inplace_merge(nil, nil, nil, comp);
    (void) includes(nil, nil, nil, nil);
    (void) includes(nil, nil, nil, nil, comp);
    set_union(nil, nil, nil, nil, nil);
    set_union(nil, nil, nil, nil, nil, comp);
    set_intersection(nil, nil, nil, nil, nil);
    set_intersection(nil, nil, nil, nil, nil, comp);
    set_difference(nil, nil, nil, nil, nil);
    set_difference(nil, nil, nil, nil, nil, comp);
    set_symmetric_difference(nil, nil, nil, nil, nil);
    set_symmetric_difference(nil, nil, nil, nil, nil, comp);
    push_heap(nil, nil);
    push_heap(nil, nil, comp);
    pop_heap(nil, nil);
    pop_heap(nil, nil, comp);
    make_heap(nil, nil);
    make_heap(nil, nil, comp);
    sort_heap(nil, nil);
    sort_heap(nil, nil, comp);
    (void) is_heap(nil, nil);
    (void) is_heap(nil, nil, comp);
    (void) is_heap_until(nil, nil);
    (void) is_heap_until(nil, nil, comp);
    (void) min_element(nil, nil);
    (void) min_element(nil, nil, comp);
    (void) max_element(nil, nil);
    (void) max_element(nil, nil, comp);
    (void) minmax_element(nil, nil);
    (void) minmax_element(nil, nil, comp);
    (void) lexicographical_compare(nil, nil, nil, nil);
    (void) lexicographical_compare(nil, nil, nil, nil, comp);
#if _HAS_CXX20
    (void) lexicographical_compare_three_way(nil, nil, nil, nil);
    (void) lexicographical_compare_three_way(nil, nil, nil, nil, comp_three_way);
#endif // _HAS_CXX20
    next_permutation(nil, nil);
    next_permutation(nil, nil, comp);
    prev_permutation(nil, nil);
    prev_permutation(nil, nil, comp);

    (void) accumulate(nil, nil, val, binop);
#if _HAS_CXX17
    (void) reduce(nil, nil, val, binop);
#endif // _HAS_CXX17
    (void) inner_product(nil, nil, nil, val, binop, binop);
#if _HAS_CXX17
    (void) transform_reduce(nil, nil, nil, val, binop, binop);
    (void) transform_reduce(nil, nil, val, binop, unop);
#endif // _HAS_CXX17
    partial_sum(nil, nil, nil, binop);
#if _HAS_CXX17
    exclusive_scan(nil, nil, nil, val, binop);
    inclusive_scan(nil, nil, nil, binop);
    inclusive_scan(nil, nil, nil, binop, val);
    transform_exclusive_scan(nil, nil, nil, val, binop, unop);
    transform_inclusive_scan(nil, nil, nil, binop, unop);
    transform_inclusive_scan(nil, nil, nil, binop, unop, val);
#endif // _HAS_CXX17
    adjacent_difference(nil, nil, nil, binop);
    iota(nil, nil, val);

    uninitialized_copy(nil, nil, nil);
    uninitialized_copy_n(nil, zero, nil);
#if _HAS_CXX17
    uninitialized_move(nil, nil, nil);
    uninitialized_move_n(nil, zero, nil);
#endif // _HAS_CXX17
    uninitialized_fill(nil, nil, val);
    uninitialized_fill_n(nil, zero, val);

#ifdef __cpp_lib_execution
    (void) all_of(par, nil, nil, pred);
    (void) any_of(par, nil, nil, pred);
    (void) none_of(par, nil, nil, pred);
    for_each(par, nil, nil, pred);
    for_each_n(par, nil, zero, pred);
    (void) find(par, nil, nil, val);
    (void) find_if(par, nil, nil, pred);
    (void) find_if_not(par, nil, nil, pred);
#ifndef _M_CEE // TRANSITION, VSO-1946395
    (void) find_end(par, nil, nil, nil, nil);
    (void) find_end(par, nil, nil, nil, nil, pred2);
#endif // ^^^ no workaround ^^^
    (void) find_first_of(par, nil, nil, nil, nil);
    (void) find_first_of(par, nil, nil, nil, nil, pred2);
    (void) adjacent_find(par, nil, nil);
    (void) adjacent_find(par, nil, nil, pred2);
    (void) count(par, nil, nil, val);
    (void) count_if(par, nil, nil, pred);
    (void) mismatch(par, nil, nil, nil);
    (void) mismatch(par, nil, nil, nil, pred2);
    (void) mismatch(par, nil, nil, nil, nil);
    (void) mismatch(par, nil, nil, nil, nil, pred2);
    (void) equal(par, nil, nil, nil);
    (void) equal(par, nil, nil, nil, pred2);
    (void) equal(par, nil, nil, nil, nil);
    (void) equal(par, nil, nil, nil, nil, pred2);
    (void) search(par, nil, nil, nil, nil);
    (void) search(par, nil, nil, nil, nil, pred2);
    (void) search_n(par, nil, nil, zero, val);
    (void) search_n(par, nil, nil, zero, val, pred2);
    copy(par, nil, nil, nil);
    copy_n(par, nil, zero, nil);
    copy_if(par, nil, nil, nil, pred);
    move(par, nil, nil, nil);
    swap_ranges(par, nil, nil, nil);
    transform(par, nil, nil, nil, unop);
    transform(par, nil, nil, nil, nil, binop);
    replace(par, nil, nil, val, val);
    replace_if(par, nil, nil, pred, val);
    replace_copy(par, nil, nil, nil, val, val);
    replace_copy_if(par, nil, nil, nil, pred, val);
    fill(par, nil, nil, val);
    fill_n(par, nil, zero, val);
    generate(par, nil, nil, gen);
    generate_n(par, nil, zero, gen);
    (void) remove(par, nil, nil, val);
    (void) remove_if(par, nil, nil, pred);
    remove_copy(par, nil, nil, nil, val);
    remove_copy_if(par, nil, nil, nil, pred);
    (void) unique(par, nil, nil);
    (void) unique(par, nil, nil, pred2);
    unique_copy(par, nil, nil, nil);
    unique_copy(par, nil, nil, nil, pred2);
    reverse(par, nil, nil);
    reverse_copy(par, nil, nil, nil);
    rotate(par, nil, nil, nil);
    rotate_copy(par, nil, nil, nil, nil);
#if _HAS_CXX20
    shift_left(par, nil, nil, 1729);
    shift_right(par, nil, nil, 1729);
#endif // _HAS_CXX20
    sort(par, nil, nil);
    sort(par, nil, nil, comp);
    stable_sort(par, nil, nil);
    stable_sort(par, nil, nil, comp);
    partial_sort(par, nil, nil, nil);
    partial_sort(par, nil, nil, nil, comp);
    partial_sort_copy(par, nil, nil, nil, nil);
    partial_sort_copy(par, nil, nil, nil, nil, comp);
    (void) is_sorted(par, nil, nil);
    (void) is_sorted(par, nil, nil, comp);
    (void) is_sorted_until(par, nil, nil);
    (void) is_sorted_until(par, nil, nil, comp);
    nth_element(par, nil, nil, nil);
    nth_element(par, nil, nil, nil, comp);
    (void) is_partitioned(par, nil, nil, pred);
    partition(par, nil, nil, pred);
    stable_partition(par, nil, nil, pred);
    partition_copy(par, nil, nil, nil, nil, pred);
    merge(par, nil, nil, nil, nil, nil);
    merge(par, nil, nil, nil, nil, nil, comp);
    inplace_merge(par, nil, nil, nil);
    inplace_merge(par, nil, nil, nil, comp);
    (void) includes(par, nil, nil, nil, nil);
    (void) includes(par, nil, nil, nil, nil, comp);
    set_union(par, nil, nil, nil, nil, nil);
    set_union(par, nil, nil, nil, nil, nil, comp);
    set_intersection(par, nil, nil, nil, nil, nil);
    set_intersection(par, nil, nil, nil, nil, nil, comp);
    set_difference(par, nil, nil, nil, nil, nil);
    set_difference(par, nil, nil, nil, nil, nil, comp);
    set_symmetric_difference(par, nil, nil, nil, nil, nil);
    set_symmetric_difference(par, nil, nil, nil, nil, nil, comp);
    (void) is_heap(par, nil, nil);
    (void) is_heap(par, nil, nil, comp);
    (void) is_heap_until(par, nil, nil);
    (void) is_heap_until(par, nil, nil, comp);
    (void) min_element(par, nil, nil);
    (void) min_element(par, nil, nil, comp);
    (void) max_element(par, nil, nil);
    (void) max_element(par, nil, nil, comp);
    (void) minmax_element(par, nil, nil);
    (void) minmax_element(par, nil, nil, comp);
    (void) lexicographical_compare(par, nil, nil, nil, nil);
    (void) lexicographical_compare(par, nil, nil, nil, nil, comp);

    (void) reduce(par, nil, nil, val, binop);
    (void) transform_reduce(par, nil, nil, nil, val, binop, binop);
    (void) transform_reduce(par, nil, nil, val, binop, unop);
    exclusive_scan(par, nil, nil, nil, val, binop);
    inclusive_scan(par, nil, nil, nil, binop);
    inclusive_scan(par, nil, nil, nil, binop, val);
    transform_exclusive_scan(par, nil, nil, nil, val, binop, unop);
    transform_inclusive_scan(par, nil, nil, nil, binop, unop);
    transform_inclusive_scan(par, nil, nil, nil, binop, unop, val);
    adjacent_difference(par, nil, nil, nil, binop);

    uninitialized_copy(par, nil, nil, nil);
    uninitialized_copy_n(par, nil, zero, nil);
    uninitialized_move(par, nil, nil, nil);
    uninitialized_move_n(par, nil, zero, nil);
    uninitialized_fill(par, nil, nil, val);
    uninitialized_fill_n(par, nil, zero, val);
#endif // __cpp_lib_execution

#if _HAS_CXX20
    // Non-modifying sequence operations
    (void) ranges::all_of(nil, nil, pred);
    (void) ranges::any_of(nil, nil, pred);
    (void) ranges::none_of(nil, nil, pred);
#if _HAS_CXX23
    (void) ranges::contains(nil, nil, val);
    (void) ranges::contains_subrange(nil, nil, nil, nil);
#endif // _HAS_CXX23
    ranges::for_each(nil, nil, pred);
    ranges::for_each_n(nil, zero, pred);
    (void) ranges::find(nil, nil, val);
    (void) ranges::find_if(nil, nil, pred);
    (void) ranges::find_if_not(nil, nil, pred);
#if _HAS_CXX23
    (void) ranges::find_last(nil, nil, val);
    (void) ranges::find_last_if(nil, nil, pred);
    (void) ranges::find_last_if_not(nil, nil, pred);
#endif // _HAS_CXX23
    (void) ranges::find_end(nil, nil, nil, nil);
    (void) ranges::find_end(nil, nil, nil, nil, pred2);
    (void) ranges::find_first_of(nil, nil, nil, nil);
    (void) ranges::find_first_of(nil, nil, nil, nil, pred2);
    (void) ranges::adjacent_find(nil, nil);
    (void) ranges::adjacent_find(nil, nil, pred2);
    (void) ranges::count(nil, nil, val);
    (void) ranges::count_if(nil, nil, pred);
    (void) ranges::mismatch(nil, nil, nil, nil);
    (void) ranges::mismatch(nil, nil, nil, nil, pred2);
    (void) ranges::equal(nil, nil, nil, nil);
    (void) ranges::equal(nil, nil, nil, nil, pred2);
    (void) ranges::is_permutation(nil, nil, nil, nil);
    (void) ranges::is_permutation(nil, nil, nil, nil, pred2);
    (void) ranges::search(nil, nil, nil, nil);
    (void) ranges::search(nil, nil, nil, nil, pred2);
    (void) ranges::search_n(nil, nil, zero, val);
    (void) ranges::search_n(nil, nil, zero, val, pred2);
#if _HAS_CXX23
    (void) ranges::starts_with(nil, nil, nil, nil);
    (void) ranges::starts_with(nil, nil, nil, nil, pred2);
    (void) ranges::ends_with(nil, nil, nil, nil);
    (void) ranges::ends_with(nil, nil, nil, nil, pred2);
    (void) ranges::fold_left(nil, nil, val, binop);
    (void) ranges::fold_left_first(nil, nil, binop);
    (void) ranges::fold_right(nil, nil, val, binop);
    (void) ranges::fold_right_last(nil, nil, binop);
    (void) ranges::fold_left_with_iter(nil, nil, val, binop);
    (void) ranges::fold_left_first_with_iter(nil, nil, binop);
#endif // _HAS_CXX23
    ranges::copy(nil, nil, nil);
    ranges::copy_n(nil, zero, nil);
    ranges::copy_if(nil, nil, nil, pred);
    ranges::copy_backward(nil, nil, nil);
    ranges::move(nil, nil, nil);
    ranges::move_backward(nil, nil, nil);
    ranges::swap_ranges(nil, nil, nil, nil);
    ranges::transform(nil, nil, nil, unop);
    ranges::transform(nil, nil, nil, nil, nil, binop);
    ranges::replace(nil, nil, val, val);
    ranges::replace_if(nil, nil, pred, val);
    ranges::replace_copy(nil, nil, nil, val, val);
    ranges::replace_copy_if(nil, nil, nil, pred, val);
    ranges::fill(nil, nil, val);
    ranges::fill_n(nil, zero, val);
    ranges::generate(nil, nil, gen);
    ranges::generate_n(nil, zero, gen);
    (void) ranges::remove(nil, nil, val);
    (void) ranges::remove_if(nil, nil, pred);
    ranges::remove_copy(nil, nil, nil, val);
    ranges::remove_copy_if(nil, nil, nil, pred);
    (void) ranges::unique(nil, nil);
    (void) ranges::unique(nil, nil, pred2);
    ranges::unique_copy(nil, nil, nil);
    ranges::unique_copy(nil, nil, nil, pred2);
    ranges::reverse(nil, nil);
    ranges::reverse_copy(nil, nil, nil);
    ranges::rotate(nil, nil, nil);
    ranges::rotate_copy(nil, nil, nil, nil);
    ranges::sample(nil, nil, nil, zero, urbg);
    ranges::shuffle(nil, nil, urbg);
#if _HAS_CXX23
    ranges::shift_left(nil, nil, 1729);
    ranges::shift_right(nil, nil, 1729);
#endif // _HAS_CXX23
    ranges::sort(nil, nil);
    ranges::sort(nil, nil, comp);
    ranges::stable_sort(nil, nil);
    ranges::stable_sort(nil, nil, comp);
    ranges::partial_sort(nil, nil, nil);
    ranges::partial_sort(nil, nil, nil, comp);
    ranges::partial_sort_copy(nil, nil, nil, nil);
    ranges::partial_sort_copy(nil, nil, nil, nil, comp);
    (void) ranges::is_sorted(nil, nil);
    (void) ranges::is_sorted(nil, nil, comp);
    (void) ranges::is_sorted_until(nil, nil);
    (void) ranges::is_sorted_until(nil, nil, comp);
    ranges::nth_element(nil, nil, nil);
    ranges::nth_element(nil, nil, nil, comp);
    (void) ranges::lower_bound(nil, nil, val);
    (void) ranges::lower_bound(nil, nil, val, comp);
    (void) ranges::upper_bound(nil, nil, val);
    (void) ranges::upper_bound(nil, nil, val, comp);
    (void) ranges::equal_range(nil, nil, val);
    (void) ranges::equal_range(nil, nil, val, comp);
    (void) ranges::binary_search(nil, nil, val);
    (void) ranges::binary_search(nil, nil, val, comp);
    (void) ranges::is_partitioned(nil, nil, pred);
    ranges::partition(nil, nil, pred);
    ranges::stable_partition(nil, nil, pred);
    ranges::partition_copy(nil, nil, nil, nil, pred);
    (void) ranges::partition_point(nil, nil, pred);
    ranges::merge(nil, nil, nil, nil, nil);
    ranges::merge(nil, nil, nil, nil, nil, comp);
    ranges::inplace_merge(nil, nil, nil);
    ranges::inplace_merge(nil, nil, nil, comp);
    (void) ranges::includes(nil, nil, nil, nil);
    (void) ranges::includes(nil, nil, nil, nil, comp);
    ranges::set_union(nil, nil, nil, nil, nil);
    ranges::set_union(nil, nil, nil, nil, nil, comp);
    ranges::set_intersection(nil, nil, nil, nil, nil);
    ranges::set_intersection(nil, nil, nil, nil, nil, comp);
    ranges::set_difference(nil, nil, nil, nil, nil);
    ranges::set_difference(nil, nil, nil, nil, nil, comp);
    ranges::set_symmetric_difference(nil, nil, nil, nil, nil);
    ranges::set_symmetric_difference(nil, nil, nil, nil, nil, comp);
    ranges::push_heap(nil, nil);
    ranges::push_heap(nil, nil, comp);
    ranges::pop_heap(nil, nil);
    ranges::pop_heap(nil, nil, comp);
    ranges::make_heap(nil, nil);
    ranges::make_heap(nil, nil, comp);
    ranges::sort_heap(nil, nil);
    ranges::sort_heap(nil, nil, comp);
    (void) ranges::is_heap(nil, nil);
    (void) ranges::is_heap(nil, nil, comp);
    (void) ranges::is_heap_until(nil, nil);
    (void) ranges::is_heap_until(nil, nil, comp);
    (void) ranges::min_element(nil, nil);
    (void) ranges::min_element(nil, nil, comp);
    (void) ranges::max_element(nil, nil);
    (void) ranges::max_element(nil, nil, comp);
    (void) ranges::minmax_element(nil, nil);
    (void) ranges::minmax_element(nil, nil, comp);
    (void) ranges::lexicographical_compare(nil, nil, nil, nil);
    (void) ranges::lexicographical_compare(nil, nil, nil, nil, comp);
    ranges::next_permutation(nil, nil);
    ranges::next_permutation(nil, nil, comp);
    ranges::prev_permutation(nil, nil);
    ranges::prev_permutation(nil, nil, comp);

    // Constrained numeric operations
#if _HAS_CXX23
    ranges::iota(nil, nil, val);
#endif // _HAS_CXX23

    // Constrained uninitialized memory algorithms
    ranges::uninitialized_copy(nil, nil, nil, nil);
    ranges::uninitialized_copy_n(nil, zero, nil, nil);
    ranges::uninitialized_move(nil, nil, nil, nil);
    ranges::uninitialized_move_n(nil, zero, nil, nil);
    ranges::uninitialized_fill(nil, nil, val);
    ranges::uninitialized_fill_n(nil, zero, val);
#endif // _HAS_CXX20
}
