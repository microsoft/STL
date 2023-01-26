// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// DevDiv-253803 "<algorithm>: merge() asserts when given null src/dest"

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <numeric>
#include <random>

#ifdef __cpp_lib_execution
#include <execution>
#endif // __cpp_lib_execution

using namespace std;

bool always_true(int) {
    return true;
}

bool always_false(int) {
    return false;
}

const int zero = 0;

int main() {
    int* nil                = nullptr;
    bool (*pred)(int)       = nullptr;
    bool (*pred2)(int, int) = nullptr;
    bool (*comp)(int, int)  = nullptr;
    int (*gen)()            = nullptr;
#if _HAS_AUTO_PTR_ETC
    ptrdiff_t (*rng)(ptrdiff_t) = nullptr;
#endif // _HAS_AUTO_PTR_ETC
    int (*unop)(int)       = nullptr;
    int (*binop)(int, int) = nullptr;

    int arr[3] = {55, 55, 55};
    int res[3] = {};

    (void) all_of(nil, nil, pred);
    (void) any_of(nil, nil, pred);
    (void) none_of(nil, nil, pred);
    for_each(nil, nil, pred);
#if _HAS_CXX17
    for_each_n(nil, zero, pred);
#endif // _HAS_CXX17
    (void) find(nil, nil, 1729);
    (void) find_if(nil, nil, pred);
    (void) find_if_not(nil, nil, pred);
    (void) find_end(nil, nil, nil, nil);
    (void) find_end(nil, nil, nil, nil, pred2);
    (void) find_end(begin(arr), end(arr), nil, nil);
    (void) find_end(begin(arr), end(arr), nil, nil, pred2);
    (void) find_end(nil, nil, begin(arr), end(arr));
    (void) find_end(nil, nil, begin(arr), end(arr), pred2);
    (void) find_first_of(nil, nil, nil, nil);
    (void) find_first_of(nil, nil, nil, nil, pred2);
    (void) find_first_of(begin(arr), end(arr), nil, nil);
    (void) find_first_of(begin(arr), end(arr), nil, nil, pred2);
    (void) find_first_of(nil, nil, begin(arr), end(arr));
    (void) find_first_of(nil, nil, begin(arr), end(arr), pred2);
    (void) adjacent_find(nil, nil);
    (void) adjacent_find(nil, nil, pred2);
    (void) adjacent_find(begin(arr), begin(arr) + 1);
    (void) adjacent_find(begin(arr), begin(arr) + 1, pred2);
    (void) count(nil, nil, 1729);
    (void) count_if(nil, nil, pred);
    (void) mismatch(nil, nil, nil);
    (void) mismatch(nil, nil, nil, pred2);
    (void) mismatch(nil, nil, nil, nil);
    (void) mismatch(nil, nil, nil, nil, pred2);
    (void) mismatch(begin(arr), end(arr), nil, nil);
    (void) mismatch(begin(arr), end(arr), nil, nil, pred2);
    (void) mismatch(nil, nil, begin(arr), end(arr));
    (void) mismatch(nil, nil, begin(arr), end(arr), pred2);
    (void) equal(nil, nil, nil);
    (void) equal(nil, nil, nil, pred2);
    (void) equal(nil, nil, nil, nil);
    (void) equal(nil, nil, nil, nil, pred2);
    (void) equal(begin(arr), end(arr), nil, nil);
    (void) equal(begin(arr), end(arr), nil, nil, pred2);
    (void) equal(nil, nil, begin(arr), end(arr));
    (void) equal(nil, nil, begin(arr), end(arr), pred2);
    (void) is_permutation(nil, nil, nil);
    (void) is_permutation(nil, nil, nil, pred2);
    (void) is_permutation(nil, nil, nil, nil);
    (void) is_permutation(nil, nil, nil, nil, pred2);
    (void) is_permutation(begin(arr), end(arr), nil, nil);
    (void) is_permutation(begin(arr), end(arr), nil, nil, pred2);
    (void) is_permutation(nil, nil, begin(arr), end(arr));
    (void) is_permutation(nil, nil, begin(arr), end(arr), pred2);
    (void) search(nil, nil, nil, nil);
    (void) search(nil, nil, nil, nil, pred2);
    (void) search(begin(arr), end(arr), nil, nil);
    (void) search(begin(arr), end(arr), nil, nil, pred2);
    (void) search(nil, nil, begin(arr), end(arr));
    (void) search(nil, nil, begin(arr), end(arr), pred2);
    (void) search_n(nil, nil, zero, 1729);
    (void) search_n(nil, nil, 5, 1729);
    (void) search_n(nil, nil, zero, 1729, pred2);
    (void) search_n(nil, nil, 5, 1729, pred2);
    copy(nil, nil, nil);
    copy_n(nil, zero, nil);
    copy_if(nil, nil, nil, pred);
    copy_if(begin(arr), end(arr), nil, &always_false);
    copy_backward(nil, nil, nil);
    move(nil, nil, nil);
    move_backward(nil, nil, nil);
    swap_ranges(nil, nil, nil);
    transform(nil, nil, nil, unop);
    transform(nil, nil, nil, nil, binop);
    replace(nil, nil, -1, 1729);
    replace_if(nil, nil, pred, 1729);
    replace_copy(nil, nil, nil, -1, 1729);
    replace_copy_if(nil, nil, nil, pred, 1729);
    fill(nil, nil, 1729);
    fill_n(nil, zero, 1729);
    generate(nil, nil, gen);
    generate_n(nil, zero, gen);
    (void) remove(nil, nil, 1729);
    (void) remove_if(nil, nil, pred);
    remove_copy(nil, nil, nil, 1729);
    remove_copy(begin(arr), end(arr), nil, 55);
    remove_copy_if(nil, nil, nil, pred);
    remove_copy_if(begin(arr), end(arr), nil, &always_true);
    (void) unique(nil, nil);
    (void) unique(nil, nil, pred2);
    (void) unique(begin(arr), begin(arr) + 1);
    (void) unique(begin(arr), begin(arr) + 1, pred2);
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
    sample(nil, nil, nil, zero, mt19937(1729));
#endif // _HAS_CXX17
    shuffle(nil, nil, mt19937(1729));
#if _HAS_CXX20
    shift_left(nil, nil, 1729);
    shift_right(nil, nil, 1729);
#endif // _HAS_CXX20
    (void) is_partitioned(nil, nil, pred);
    partition(nil, nil, pred);
    stable_partition(nil, nil, pred);
    partition_copy(nil, nil, nil, nil, pred);
    partition_copy(begin(arr), end(arr), begin(res), nil, &always_true);
    partition_copy(begin(arr), end(arr), nil, begin(res), &always_false);
    (void) partition_point(nil, nil, pred);
    sort(nil, nil);
    sort(nil, nil, comp);
    sort(begin(arr), begin(arr) + 1);
    sort(begin(arr), begin(arr) + 1, comp);
    stable_sort(nil, nil);
    stable_sort(nil, nil, comp);
    stable_sort(begin(arr), begin(arr) + 1);
    stable_sort(begin(arr), begin(arr) + 1, comp);
    partial_sort(nil, nil, nil);
    partial_sort(nil, nil, nil, comp);
    partial_sort_copy(nil, nil, nil, nil);
    partial_sort_copy(nil, nil, nil, nil, comp);
    (void) is_sorted(nil, nil);
    (void) is_sorted(nil, nil, comp);
    (void) is_sorted(begin(arr), begin(arr) + 1);
    (void) is_sorted(begin(arr), begin(arr) + 1, comp);
    (void) is_sorted_until(nil, nil);
    (void) is_sorted_until(nil, nil, comp);
    (void) is_sorted_until(begin(arr), begin(arr) + 1);
    (void) is_sorted_until(begin(arr), begin(arr) + 1, comp);
    nth_element(nil, nil, nil);
    nth_element(nil, nil, nil, comp);
    (void) lower_bound(nil, nil, 1729);
    (void) lower_bound(nil, nil, 1729, comp);
    (void) upper_bound(nil, nil, 1729);
    (void) upper_bound(nil, nil, 1729, comp);
    (void) equal_range(nil, nil, 1729);
    (void) equal_range(nil, nil, 1729, comp);
    (void) binary_search(nil, nil, 1729);
    (void) binary_search(nil, nil, 1729, comp);
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
    make_heap(nil, nil);
    make_heap(nil, nil, comp);
    sort_heap(nil, nil);
    sort_heap(nil, nil, comp);
    (void) is_heap(nil, nil);
    (void) is_heap(nil, nil, comp);
    (void) is_heap_until(nil, nil);
    (void) is_heap_until(nil, nil, comp);

    initializer_list<int> il = {11};
    (void) min(il, comp);
    (void) max(il, comp);
    (void) minmax(il, comp);

    (void) min_element(nil, nil);
    (void) min_element(nil, nil, comp);
    (void) min_element(begin(arr), begin(arr) + 1);
    (void) min_element(begin(arr), begin(arr) + 1, comp);
    (void) max_element(nil, nil);
    (void) max_element(nil, nil, comp);
    (void) max_element(begin(arr), begin(arr) + 1);
    (void) max_element(begin(arr), begin(arr) + 1, comp);
    (void) minmax_element(nil, nil);
    (void) minmax_element(nil, nil, comp);
    (void) minmax_element(begin(arr), begin(arr) + 1);
    (void) minmax_element(begin(arr), begin(arr) + 1, comp);
    (void) lexicographical_compare(nil, nil, nil, nil);
    (void) lexicographical_compare(begin(arr), end(arr), nil, nil);
    (void) lexicographical_compare(nil, nil, begin(arr), end(arr));
    (void) lexicographical_compare(nil, nil, nil, nil, comp);
    (void) lexicographical_compare(begin(arr), end(arr), nil, nil, comp);
    (void) lexicographical_compare(nil, nil, begin(arr), end(arr), comp);
    next_permutation(nil, nil);
    next_permutation(nil, nil, comp);
    prev_permutation(nil, nil);
    prev_permutation(nil, nil, comp);
    (void) accumulate(nil, nil, 1729);
    (void) accumulate(nil, nil, 1729, binop);
#if _HAS_CXX17
    (void) reduce(nil, nil);
    (void) reduce(nil, nil, 1729);
    (void) reduce(nil, nil, 1729, binop);
#endif // _HAS_CXX17
    (void) inner_product(nil, nil, nil, 1729);
    (void) inner_product(nil, nil, nil, 1729, binop, binop);
#if _HAS_CXX17
    (void) transform_reduce(nil, nil, nil, 1729);
    (void) transform_reduce(nil, nil, nil, 1729, binop, binop);
    (void) transform_reduce(nil, nil, 1729, binop, unop);
#endif // _HAS_CXX17
    partial_sum(nil, nil, nil);
    partial_sum(nil, nil, nil, binop);
#if _HAS_CXX17
    exclusive_scan(nil, nil, nil, 1729);
    exclusive_scan(nil, nil, nil, 1729, binop);
    inclusive_scan(nil, nil, nil);
    inclusive_scan(nil, nil, nil, binop);
    inclusive_scan(nil, nil, nil, binop, 1729);
    transform_exclusive_scan(nil, nil, nil, 1729, binop, unop);
    transform_inclusive_scan(nil, nil, nil, binop, unop);
    transform_inclusive_scan(nil, nil, nil, binop, unop, 1729);
#endif // _HAS_CXX17
    adjacent_difference(nil, nil, nil);
    adjacent_difference(nil, nil, nil, binop);
    iota(nil, nil, 1729);
    uninitialized_copy(nil, nil, nil);
    uninitialized_copy_n(nil, zero, nil);
    uninitialized_fill(nil, nil, 1729);
    uninitialized_fill_n(nil, zero, 1729);

#ifdef __cpp_lib_execution
    using namespace std::execution;
    (void) all_of(par, nil, nil, pred);
    (void) any_of(par, nil, nil, pred);
    (void) none_of(par, nil, nil, pred);
    for_each(par, nil, nil, pred);
    for_each_n(par, nil, zero, pred);
    (void) find(par, nil, nil, 1729);
    (void) find_if(par, nil, nil, pred);
    (void) find_if_not(par, nil, nil, pred);
    (void) find_end(par, nil, nil, nil, nil);
    (void) find_end(par, nil, nil, nil, nil, comp);
    (void) find_first_of(par, nil, nil, nil, nil);
    (void) find_first_of(par, nil, nil, nil, nil, comp);
    (void) adjacent_find(par, nil, nil);
    (void) adjacent_find(par, nil, nil, pred2);
    (void) count(par, nil, nil, 1729);
    (void) count_if(par, nil, nil, pred);
    (void) mismatch(par, nil, nil, nil);
    (void) mismatch(par, nil, nil, nil, pred2);
    (void) mismatch(par, nil, nil, nil, nil);
    (void) mismatch(par, nil, nil, nil, nil, pred2);
    (void) mismatch(par, begin(arr), end(arr), nil, nil);
    (void) mismatch(par, begin(arr), end(arr), nil, nil, pred2);
    (void) mismatch(par, nil, nil, begin(arr), end(arr));
    (void) mismatch(par, nil, nil, begin(arr), end(arr), pred2);
    (void) equal(par, nil, nil, nil);
    (void) equal(par, nil, nil, nil, pred2);
    (void) equal(par, nil, nil, nil, nil);
    (void) equal(par, nil, nil, nil, nil, pred2);
    (void) equal(par, begin(arr), end(arr), nil, nil);
    (void) equal(par, begin(arr), end(arr), nil, nil, pred2);
    (void) equal(par, nil, nil, begin(arr), end(arr));
    (void) equal(par, nil, nil, begin(arr), end(arr), pred2);
    (void) search(par, nil, nil, nil, nil);
    (void) search(par, nil, nil, nil, nil, pred2);
    (void) search_n(par, nil, nil, zero, 1729);
    (void) search_n(par, nil, nil, 5, 1729);
    (void) search_n(par, nil, nil, zero, 1729, pred2);
    (void) search_n(par, nil, nil, 5, 1729, pred2);

    copy(par, nil, nil, nil);
    copy_if(par, nil, nil, nil, pred);
    copy_n(par, nil, zero, nil);
    move(par, nil, nil, nil);
    swap_ranges(par, nil, nil, nil);
    transform(par, nil, nil, nil, unop);
    transform(par, nil, nil, nil, nil, binop);
    replace(par, nil, nil, 1, 1);
    replace_if(par, nil, nil, pred, 1);
    replace_copy(par, nil, nil, nil, 1729, 1729);
    replace_copy_if(par, nil, nil, nil, pred, 1729);
    fill(par, nil, nil, 1729);
    fill_n(par, nil, zero, 1729);
    generate(par, nil, nil, gen);
    generate_n(par, nil, zero, gen);
    (void) remove(par, nil, nil, 1729);
    (void) remove_if(par, nil, nil, pred);
    remove_copy(par, nil, nil, nil, 1729);
    remove_copy_if(par, nil, nil, nil, pred);
    (void) unique(par, nil, nil);
    (void) unique(par, nil, nil, comp);
    unique_copy(par, nil, nil, nil);
    unique_copy(par, nil, nil, nil, comp);
    reverse(par, nil, nil);
    reverse_copy(par, nil, nil, nil);
    rotate(par, nil, nil, nil);
    rotate_copy(par, nil, nil, nil, nil);

    sort(par, begin(arr), begin(arr) + 1);
    sort(par, begin(arr), begin(arr) + 1, comp);
    sort(par, nil, nil);
    sort(par, nil, nil, comp);
    stable_sort(par, nil, nil);
    stable_sort(par, nil, nil, comp);
    stable_sort(par, begin(arr), begin(arr) + 1);
    stable_sort(par, begin(arr), begin(arr) + 1, comp);
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

#if _HAS_CXX20
    shift_left(par, nil, nil, 1729);
    shift_right(par, nil, nil, 1729);
#endif // _HAS_CXX20

    partition(par, nil, nil, pred);
    partition_copy(par, nil, nil, nil, nil, pred);
    stable_partition(par, nil, nil, pred);
    (void) is_partitioned(par, nil, nil, pred);

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

    (void) reduce(par, nil, nil);
    (void) reduce(par, nil, nil, 1729);
    (void) reduce(par, nil, nil, 1729, binop);

    (void) transform_reduce(par, nil, nil, nil, 1729);
    (void) transform_reduce(par, nil, nil, nil, 1729, binop, binop);
    (void) transform_reduce(par, nil, nil, 1729, binop, unop);

    exclusive_scan(par, nil, nil, nil, 1729);
    exclusive_scan(par, nil, nil, nil, 1729, binop);
    inclusive_scan(par, nil, nil, nil);
    inclusive_scan(par, nil, nil, nil, binop);
    inclusive_scan(par, nil, nil, nil, binop, 1729);
    transform_exclusive_scan(par, nil, nil, nil, 1729, binop, unop);
    transform_inclusive_scan(par, nil, nil, nil, binop, unop);
    transform_inclusive_scan(par, nil, nil, nil, binop, unop, 1729);

    adjacent_difference(par, nil, nil, nil);
    adjacent_difference(par, nil, nil, nil, binop);
#endif // __cpp_lib_execution
}
