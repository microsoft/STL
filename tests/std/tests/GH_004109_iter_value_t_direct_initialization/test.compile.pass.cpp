// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

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

struct Val;

struct RRef {
    RRef(const Val&);
};

struct Val {
    explicit Val(const RRef&);
    Val& operator=(const RRef&);
    auto operator<=>(const Val&) const = default;
};

struct I {
    using value_type      = Val;
    using difference_type = int;
    Val& operator*() const;
    Val& operator[](int) const;
    I& operator++();
    I operator++(int);
    I& operator--();
    I operator--(int);
    I& operator+=(int);
    I& operator-=(int);
    friend auto operator<=>(I, I) = default;
    friend int operator-(I, I);
    friend I operator+(I, int);
    friend I operator-(I, int);
    friend I operator+(int, I);
    friend RRef iter_move(const I&);
};

static_assert(random_access_iterator<I>);
static_assert(sortable<I>);

bool always_true(Val) {
    return true;
}

bool always_false(Val) {
    return false;
}

extern Val arr[3];
extern Val res[3];
extern Val val;
const int zero = 0;
// GH-4109: <algorithm>: iter_value_t<I> should always use direct-initialization
void test_gh_4109() {
    I nil{};

    bool (*pred)(Val)       = nullptr;
    bool (*pred2)(Val, Val) = nullptr;
    bool (*comp)(Val, Val)  = nullptr;
    Val (*gen)()            = nullptr;
    Val (*unop)(Val)        = nullptr;
    Val (*binop)(Val, Val)  = nullptr;


    (void) all_of(nil, nil, pred);
    (void) any_of(nil, nil, pred);
    (void) none_of(nil, nil, pred);
    for_each(nil, nil, pred);
    for_each_n(nil, zero, pred);
    (void) find(nil, nil, val);
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
    (void) count(nil, nil, val);
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
    (void) search_n(nil, nil, zero, val);
    (void) search_n(nil, nil, 5, val);
    (void) search_n(nil, nil, zero, val, pred2);
    (void) search_n(nil, nil, 5, val, pred2);
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
    remove_copy(begin(arr), end(arr), nil, val);
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
    sample(nil, nil, nil, zero, mt19937(1729));
    shuffle(nil, nil, mt19937(1729));
    shift_left(nil, nil, 1729);
    shift_right(nil, nil, 1729);
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
    (void) lower_bound(nil, nil, val);
    (void) lower_bound(nil, nil, val, comp);
    (void) upper_bound(nil, nil, val);
    (void) upper_bound(nil, nil, val, comp);
    (void) equal_range(nil, nil, val);
    (void) equal_range(nil, nil, val, comp);
    (void) binary_search(nil, nil, val);
    (void) binary_search(nil, nil, val, comp);
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
    (void) accumulate(nil, nil, val, binop);
    (void) reduce(nil, nil, val, binop);
    (void) inner_product(nil, nil, nil, val, binop, binop);
    (void) transform_reduce(nil, nil, nil, val, binop, binop);
    (void) transform_reduce(nil, nil, val, binop, unop);
    partial_sum(nil, nil, nil, binop);
    exclusive_scan(nil, nil, nil, val, binop);
    inclusive_scan(nil, nil, nil, binop);
    inclusive_scan(nil, nil, nil, binop, val);
    transform_exclusive_scan(nil, nil, nil, val, binop, unop);
    transform_inclusive_scan(nil, nil, nil, binop, unop);
    transform_inclusive_scan(nil, nil, nil, binop, unop, val);
    adjacent_difference(nil, nil, nil, binop);
    uninitialized_copy(nil, nil, nil);
    uninitialized_copy_n(nil, zero, nil);
    uninitialized_fill(nil, nil, val);
    uninitialized_fill_n(nil, zero, val);

#ifdef __cpp_lib_execution
    using namespace std::execution;
    (void) all_of(par, nil, nil, pred);
    (void) any_of(par, nil, nil, pred);
    (void) none_of(par, nil, nil, pred);
    for_each(par, nil, nil, pred);
    for_each_n(par, nil, zero, pred);
    (void) find(par, nil, nil, val);
    (void) find_if(par, nil, nil, pred);
    (void) find_if_not(par, nil, nil, pred);
    (void) find_end(par, nil, nil, nil, nil);
    (void) find_end(par, nil, nil, nil, nil, comp);
    (void) find_first_of(par, nil, nil, nil, nil);
    (void) find_first_of(par, nil, nil, nil, nil, comp);
    (void) adjacent_find(par, nil, nil);
    (void) adjacent_find(par, nil, nil, pred2);
    (void) count(par, nil, nil, val);
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
    (void) search_n(par, nil, nil, zero, val);
    (void) search_n(par, nil, nil, 5, val);
    (void) search_n(par, nil, nil, zero, val, pred2);
    (void) search_n(par, nil, nil, 5, val, pred2);

    copy(par, nil, nil, nil);
    copy_if(par, nil, nil, nil, pred);
    copy_n(par, nil, zero, nil);
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

    shift_left(par, nil, nil, 1729);
    shift_right(par, nil, nil, 1729);

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

    (void) reduce(par, nil, nil, val, binop);

    (void) transform_reduce(par, nil, nil, nil, val, binop, binop);
    (void) transform_reduce(par, nil, nil, val, binop, unop);

    exclusive_scan(par, nil, nil, nil, val, binop);
    inclusive_scan(par, nil, nil, nil, binop, val);
    transform_exclusive_scan(par, nil, nil, nil, val, binop, unop);
    transform_inclusive_scan(par, nil, nil, nil, binop, unop, val);

    adjacent_difference(par, nil, nil, nil, binop);
#endif // __cpp_lib_execution
}
