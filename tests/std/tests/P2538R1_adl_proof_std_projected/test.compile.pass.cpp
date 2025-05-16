// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef _M_CEE // TRANSITION, VSO-1659496
#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>
#include <ranges>
#include <utility>

template <class Tag>
struct tagged_truth {
    template <class T>
    constexpr bool operator()(T&&) const noexcept {
        return true;
    }
};

template <class Tag>
struct tagged_zeroer {
    template <class T>
    constexpr int operator()(T&&) const noexcept {
        return 0;
    }
};

template <class Tag>
struct tagged_true_comparator {
    template <class T, class U>
    constexpr bool operator()(T&&, U&&) const noexcept {
        return true;
    }
};

template <class Tag>
struct tagged_equal {
    template <class T, class U>
    constexpr auto operator()(T&& t, U&& u) const -> decltype(std::forward<T>(t) == std::forward<U>(u)) {
        return std::forward<T>(t) == std::forward<U>(u);
    }
};

template <class Tag>
struct tagged_less {
    template <class T, class U>
    constexpr auto operator()(T&& t, U&& u) const -> decltype(std::forward<T>(t) < std::forward<U>(u)) {
        return std::forward<T>(t) < std::forward<U>(u);
    }
};

template <class Tag>
struct tagged_identity {
    template <class T>
    constexpr T&& operator()(T&& t) const noexcept {
        return std::forward<T>(t);
    }
};

template <class Tag>
struct tagged_left_selector {
    template <class T>
    constexpr T operator()(T lhs, T) const noexcept {
        return lhs;
    }
};

template <class Tag>
struct tagged_zero_equality {
    template <class T>
    constexpr auto operator()(T&& t) const -> decltype(std::forward<T>(t) == std::remove_cvref_t<T>{}) {
        return std::forward<T>(t) == std::remove_cvref_t<T>{};
    }
};

template <class Tag>
struct tagged_base {};

template <class Tag>
struct tagged_derived : tagged_base<Tag> {};

template <class T>
struct holder {
    T t;
};

struct incomplete;

using simple_truth         = tagged_truth<void>;
using simple_identity      = tagged_identity<void>;
using simple_left_selector = tagged_left_selector<void>;
using simple_zero_equality = tagged_zero_equality<void>;

using validator                  = holder<incomplete>*;
using validating_truth           = tagged_truth<holder<incomplete>>;
using validating_zeroer          = tagged_zeroer<holder<incomplete>>;
using validating_true_comparator = tagged_true_comparator<holder<incomplete>>;
using validating_equal           = tagged_equal<holder<incomplete>>;
using validating_less            = tagged_less<holder<incomplete>>;
using validating_identity        = tagged_identity<holder<incomplete>>;
using validating_left_selector   = tagged_left_selector<holder<incomplete>>;
using validating_zero_equality   = tagged_zero_equality<holder<incomplete>>;
using validating_derived         = tagged_derived<holder<incomplete>>;
using validating_base            = tagged_base<holder<incomplete>>;

static_assert(std::sentinel_for<validating_base*, validating_derived*>);
#ifndef __EDG__ // TRANSITION, DevCom-10581519
static_assert(!std::sized_sentinel_for<validating_base*, validating_derived*>);
#endif // ^^^ no workaround ^^^

template <class T>
bool less_function(T lhs, T rhs) {
    return lhs < rhs;
}

void test_ranges_algorithms() {
    using namespace std::ranges;

    int iarr[1]{};
    validator varr[1]{};

    validating_derived darr[1]{};
    validating_derived* const dptr = darr;
    validating_base* const bptr    = darr; // behaves as a non-sized sentinel

    (void) all_of(varr, varr, simple_truth{});
    (void) all_of(varr, simple_truth{});
    (void) all_of(iarr, iarr, validating_truth{});
    (void) all_of(iarr, validating_truth{});

    (void) any_of(varr, varr, simple_truth{});
    (void) any_of(varr, simple_truth{});
    (void) any_of(iarr, iarr, validating_truth{});
    (void) any_of(iarr, validating_truth{});

    (void) none_of(varr, varr, simple_truth{});
    (void) none_of(varr, simple_truth{});
    (void) none_of(iarr, iarr, validating_truth{});
    (void) none_of(iarr, validating_truth{});

#if _HAS_CXX23
    (void) contains(varr, varr, validator{});
    (void) contains(varr, validator{});
    (void) contains(iarr, iarr, 0, validating_identity{});
    (void) contains(iarr, 0, validating_identity{});

    (void) contains_subrange(varr, varr, varr, varr);
    (void) contains_subrange(varr, varr);
    (void) contains_subrange(iarr, iarr, iarr, iarr, validating_equal{});
    // (void) contains_subrange(iarr, iarr, validating_equal{}); // needs to check ADL-found operator*
    (void) contains_subrange(iarr, iarr, {}, validating_identity{});
#endif // _HAS_CXX23

    (void) for_each(varr, varr, simple_truth{});
    (void) for_each(varr, simple_truth{});
    (void) for_each(varr, varr, validating_truth{});
    (void) for_each(varr, validating_truth{});
    (void) for_each(iarr, iarr, validating_truth{});
    (void) for_each(iarr, validating_truth{});

    (void) for_each_n(varr, 0, simple_truth{});
    (void) for_each_n(varr, 0, validating_truth{});
    (void) for_each_n(iarr, 0, validating_truth{});

    (void) find(varr, varr, validator{});
    (void) find(varr, validator{});

    (void) find_if(varr, varr, simple_truth{});
    (void) find_if(varr, simple_truth{});
    (void) find_if(iarr, iarr, validating_truth{});
    (void) find_if(iarr, validating_truth{});

    (void) find_if_not(varr, varr, simple_truth{});
    (void) find_if_not(varr, simple_truth{});
    (void) find_if_not(iarr, iarr, validating_truth{});
    (void) find_if_not(iarr, validating_truth{});

#if _HAS_CXX23
    (void) find_last(varr, varr, validator{});
    (void) find_last(varr, validator{});

    (void) find_last_if(varr, varr, simple_truth{});
    (void) find_last_if(varr, simple_truth{});
    (void) find_last_if(iarr, iarr, validating_truth{});
    (void) find_last_if(iarr, validating_truth{});

    (void) find_last_if_not(varr, varr, simple_truth{});
    (void) find_last_if_not(varr, simple_truth{});
    (void) find_last_if_not(iarr, iarr, validating_truth{});
    (void) find_last_if_not(iarr, validating_truth{});
#endif // _HAS_CXX23

    (void) find_end(varr, varr, varr, varr);
    (void) find_end(varr, varr);
    (void) find_end(varr, varr, varr, varr, validating_equal{});
    (void) find_end(varr, varr, validating_equal{});
    (void) find_end(iarr, iarr, iarr, iarr, validating_equal{});
    (void) find_end(iarr, iarr, validating_equal{});
    (void) find_end(dptr, bptr, dptr, bptr, validating_true_comparator{});

    (void) find_first_of(varr, varr, varr, varr);
    (void) find_first_of(varr, varr);
    (void) find_first_of(varr, varr, varr, varr, validating_equal{});
    (void) find_first_of(varr, varr, validating_equal{});
    (void) find_first_of(iarr, iarr, iarr, iarr, validating_equal{});
    (void) find_first_of(iarr, iarr, validating_equal{});

    (void) adjacent_find(varr, varr);
    (void) adjacent_find(varr);
    (void) adjacent_find(iarr, iarr, validating_equal{});
    (void) adjacent_find(iarr, iarr, {}, validating_identity{});
    // (void) adjacent_find(iarr, validating_equal{}); // needs to check ADL-found swap
    (void) adjacent_find(iarr, {}, validating_identity{});
    (void) adjacent_find(dptr, bptr, validating_true_comparator{});

    (void) count(varr, varr, validator{});
    (void) count(varr, validator{});
    (void) count(iarr, iarr, 0, validating_identity{});
    (void) count(iarr, 0, validating_identity{});

    (void) count_if(varr, varr, simple_truth{});
    (void) count_if(varr, simple_truth{});
    (void) count_if(iarr, iarr, validating_truth{});
    (void) count_if(iarr, validating_truth{});

    (void) mismatch(varr, varr, varr, varr);
    (void) mismatch(varr, varr);
    (void) mismatch(iarr, iarr, iarr, iarr, validating_equal{});
    (void) mismatch(iarr, iarr, validating_equal{});
    (void) mismatch(dptr, bptr, dptr, bptr, validating_true_comparator{});

    (void) equal(varr, varr, varr, varr);
    (void) equal(varr, varr);
    (void) equal(iarr, iarr, iarr, iarr, validating_equal{});
    (void) equal(iarr, iarr, validating_equal{});
    (void) equal(dptr, bptr, dptr, bptr, validating_true_comparator{});

    (void) is_permutation(varr, varr, varr, varr);
    (void) is_permutation(varr, varr);
    (void) is_permutation(varr, varr, varr, varr, validating_equal{});
    (void) is_permutation(varr, varr, validating_equal{});
    (void) is_permutation(iarr, iarr, iarr, iarr, validating_equal{});
    (void) is_permutation(iarr, iarr, validating_equal{});
    (void) is_permutation(iarr, iarr, iarr, iarr, {}, validating_identity{});
    (void) is_permutation(iarr, iarr, {}, validating_identity{});
    (void) is_permutation(dptr, bptr, dptr, bptr, validating_true_comparator{});

    (void) search(varr, varr, varr, varr);
    (void) search(varr, varr);
    (void) search(iarr, iarr, iarr, iarr, validating_equal{});
    (void) search(iarr, iarr, validating_equal{});
    (void) search(dptr, bptr, dptr, bptr, validating_true_comparator{});

    (void) search_n(varr, varr, 0, validator{});
    (void) search_n(varr, 0, validator{});
    (void) search_n(iarr, iarr, 0, 0, validating_equal{});
    (void) search_n(iarr, 0, 0, validating_equal{});
    (void) search_n(dptr, bptr, 0, validating_derived{}, validating_true_comparator{});

#if _HAS_CXX23
    (void) starts_with(varr, varr, varr, varr);
    (void) starts_with(varr, varr);
    (void) starts_with(iarr, iarr, iarr, iarr, validating_equal{});
    (void) starts_with(iarr, iarr, validating_equal{});
    (void) starts_with(dptr, bptr, dptr, bptr, validating_true_comparator{});

    (void) ends_with(varr, varr, varr, varr);
    (void) ends_with(varr, varr);
    (void) ends_with(iarr, iarr, iarr, iarr, validating_equal{});
    (void) ends_with(iarr, iarr, validating_equal{});
    (void) ends_with(dptr, bptr, dptr, bptr, validating_true_comparator{});
#endif // _HAS_CXX23

    int iarr2[1]{};
    validator varr2[1]{};

    (void) copy_if(varr, varr + 1, varr2, simple_truth{});
    (void) copy_if(varr, varr2, simple_truth{});
    (void) copy_if(iarr, iarr + 1, iarr2, validating_truth{});
    (void) copy_if(iarr, iarr2, validating_truth{});

    (void) transform(varr, varr, varr2, std::identity{});
    (void) transform(varr, varr2, std::identity{});
    (void) transform(varr, varr, varr, varr, varr2, simple_left_selector{});
    (void) transform(varr, varr, varr2, simple_left_selector{});
    (void) transform(iarr, iarr, iarr2, validating_identity{});
    (void) transform(iarr, iarr, iarr, iarr, iarr2, validating_left_selector{});
    (void) transform(iarr, iarr, iarr2, validating_left_selector{});

    (void) replace(varr, varr, validator{}, validator{});
    (void) replace(varr, validator{}, validator{});
    (void) replace(iarr, iarr, 0, 0, validating_identity{});
    (void) replace(iarr, 0, 0, validating_identity{});

    (void) replace_if(varr, varr, simple_truth{}, validator{});
    (void) replace_if(varr, simple_truth{}, validator{});
    (void) replace_if(iarr, iarr, validating_truth{}, 0);
    (void) replace_if(iarr, validating_truth{}, 0);

    (void) replace_copy(varr, varr, varr2, validator{}, validator{});
    (void) replace_copy(varr, varr2, validator{}, validator{});
    (void) replace_copy(iarr, iarr, iarr2, 0, 0, validating_identity{});
    (void) replace_copy(iarr, iarr2, 0, 0, validating_identity{});

    (void) replace_copy_if(varr, varr, varr2, simple_truth{}, validator{});
    (void) replace_copy_if(varr, varr2, simple_truth{}, validator{});
    (void) replace_copy_if(iarr, iarr, iarr2, validating_truth{}, 0);
    (void) replace_copy_if(iarr, iarr2, validating_truth{}, 0);

    using std::ranges::remove; // avoid ambiguity
    (void) remove(varr, varr, validator{});
    (void) remove(varr, validator{});

    (void) remove_if(varr, varr, simple_truth{});
    (void) remove_if(varr, simple_truth{});
    (void) remove_if(iarr, iarr, validating_truth{});
    (void) remove_if(iarr, validating_truth{});

    (void) remove_copy(varr, varr, varr2, validator{});
    (void) remove_copy(varr, varr2, validator{});

    (void) remove_copy_if(varr, varr, varr2, simple_truth{});
    (void) remove_copy_if(varr, varr2, simple_truth{});
    (void) remove_copy_if(iarr, iarr, iarr2, validating_truth{});
    (void) remove_copy_if(iarr, iarr2, validating_truth{});

    (void) unique(varr, varr);
    (void) unique(varr);
    (void) unique(iarr, iarr, validating_equal{});
    // (void) unique(iarr, validating_equal{}); // needs to check ADL-found swap
    (void) unique(iarr, iarr, {}, validating_identity{});
    (void) unique(iarr, {}, validating_identity{});

    (void) unique_copy(varr, varr, varr2);
    (void) unique_copy(varr, varr2);
    (void) unique_copy(iarr, iarr, iarr2, validating_equal{});
    // (void) unique_copy(iarr, iarr2, validating_equal{}); // needs to check ADL-found swap
    (void) unique_copy(iarr, iarr, iarr2, {}, validating_identity{});
    (void) unique_copy(iarr, iarr2, {}, validating_identity{});

    sort(varr, varr);
    sort(varr);
    sort(varr, varr, less{});
    // sort(varr, less{}); // need to check ADL-found operator==
    sort(varr, &less_function<validator>);
    sort(iarr, iarr, validating_less{});
    // sort(iarr, validating_less{}); // need to check ADL-found swap
    sort(iarr, iarr, {}, validating_identity{});

    stable_sort(varr, varr);
    stable_sort(varr);
    stable_sort(varr, varr, less{});
    // stable_sort(varr, less{}); // need to check ADL-found operator==
    stable_sort(varr, &less_function<validator>);
    stable_sort(iarr, iarr, validating_less{});
    // stable_sort(iarr, validating_less{}); // need to check ADL-found swap
    stable_sort(iarr, iarr, {}, validating_identity{});

    partial_sort(varr, varr, varr);
    partial_sort(varr, varr, static_cast<holder<incomplete>* const*>(varr)); // non-common
    partial_sort(varr, varr);
    partial_sort(subrange{varr, static_cast<holder<incomplete>* const*>(varr)}, varr); // non-common
    partial_sort(varr, varr, varr, less{});
    // partial_sort(varr, varr, less{}); // need to check ADL-found operator==
    partial_sort(varr, varr, &less_function<validator>);
    partial_sort(iarr, iarr, iarr, validating_less{});
    partial_sort(iarr, iarr, iarr, {}, validating_identity{});
    partial_sort(iarr, iarr, static_cast<const int*>(iarr), validating_less{}, validating_identity{}); // non-common
    partial_sort(iarr, iarr, static_cast<const int*>(iarr), {}, validating_identity{}); // non-common
    // partial_sort(iarr, iarr, validating_less{}); // need to check ADL-found swap
    partial_sort(iarr, iarr, {}, validating_identity{});
    partial_sort(subrange{iarr, static_cast<const int*>(iarr)}, iarr, validating_less{}); // non-common
    partial_sort(subrange{iarr, static_cast<const int*>(iarr)}, iarr, {}, validating_identity{}); // non-common

    void* vparr[2]{};

    (void) partial_sort_copy(varr, varr, varr2, varr2);
    (void) partial_sort_copy(varr, varr2);
    (void) partial_sort_copy(varr, varr, vparr, vparr);
    (void) partial_sort_copy(varr, vparr);
    (void) partial_sort_copy(iarr, iarr, iarr2, iarr2, validating_less{});
    (void) partial_sort_copy(iarr, iarr2, validating_less{});

    (void) is_sorted(varr, varr);
    (void) is_sorted(varr);
    (void) is_sorted(iarr, iarr, validating_less{});
    // (void) is_sorted(iarr, validating_less{}); // need to check ADL-found swap
    (void) is_sorted(iarr, {}, validating_identity{});

    (void) is_sorted_until(varr, varr);
    (void) is_sorted_until(varr);
    (void) is_sorted_until(iarr, iarr, validating_less{});
    // (void) is_sorted_until(iarr, validating_less{}); // need to check ADL-found swap
    (void) is_sorted_until(iarr, {}, validating_identity{});

    nth_element(varr, varr, varr);
    nth_element(varr, varr);
    nth_element(iarr, iarr, iarr, validating_less{});
    // nth_element(iarr, iarr, validating_less{}); // need to check ADL-found swap
    nth_element(iarr, iarr, {}, validating_identity{});

    (void) lower_bound(varr, varr, validator{});
    (void) lower_bound(varr, validator{});
    (void) lower_bound(iarr, iarr, 0, validating_less{});
    (void) lower_bound(iarr, 0, validating_less{});

    (void) upper_bound(varr, varr, validator{});
    (void) upper_bound(varr, validator{});
    (void) upper_bound(iarr, iarr, 0, validating_less{});
    (void) upper_bound(iarr, 0, validating_less{});

    (void) equal_range(varr, varr, validator{});
    (void) equal_range(varr, validator{});
    (void) equal_range(iarr, iarr, 0, validating_less{});
    (void) equal_range(iarr, 0, validating_less{});

    (void) binary_search(varr, varr, validator{});
    (void) binary_search(varr, validator{});
    (void) binary_search(iarr, iarr, 0, validating_less{});
    (void) binary_search(iarr, 0, validating_less{});

    (void) is_partitioned(varr, varr, simple_zero_equality{});
    (void) is_partitioned(varr, simple_zero_equality{});
    (void) is_partitioned(iarr, iarr, validating_zero_equality{});
    (void) is_partitioned(iarr, validating_zero_equality{});

    (void) partition(varr, varr, simple_zero_equality{});
    (void) partition(varr, simple_zero_equality{});
    (void) partition(iarr, iarr, validating_zero_equality{});
    (void) partition(iarr, validating_zero_equality{});

    (void) stable_partition(varr, varr, simple_zero_equality{});
    (void) stable_partition(varr, simple_zero_equality{});
    (void) stable_partition(iarr, iarr, validating_zero_equality{});
    (void) stable_partition(iarr, validating_zero_equality{});

    int iarr3[2]{};
    validator varr3[2]{};

    (void) partition_copy(varr, varr, varr2, varr3, simple_zero_equality{});
    (void) partition_copy(varr, varr2, varr3, simple_zero_equality{});
    (void) partition_copy(iarr, iarr, iarr2, iarr3, validating_zero_equality{});
    (void) partition_copy(iarr, iarr2, iarr3, validating_zero_equality{});

    (void) partition_point(varr, varr, simple_zero_equality{});
    (void) partition_point(varr, simple_zero_equality{});
    (void) partition_point(iarr, iarr, validating_zero_equality{});
    (void) partition_point(iarr, validating_zero_equality{});
    (void) partition_point(dptr, bptr, validating_zero_equality{}, validating_zeroer{});

    (void) merge(varr, varr, varr2, varr2, varr3);
    (void) merge(varr, varr2, varr3);
    (void) merge(iarr, iarr, iarr2, iarr2, iarr3, validating_less{});
    (void) merge(iarr, iarr2, iarr3, validating_less{});

    (void) inplace_merge(varr, varr, varr);
    (void) inplace_merge(varr, varr);
    (void) inplace_merge(iarr, iarr, iarr, validating_less{});
    (void) inplace_merge(iarr, iarr, {}, validating_identity{});

    (void) includes(varr, varr, varr, varr);
    (void) includes(varr, varr);
    (void) includes(iarr, iarr, iarr, iarr, validating_less{});
    (void) includes(iarr, iarr, validating_less{});

    (void) set_union(varr, varr, varr, varr, varr3);
    (void) set_union(varr, varr, varr3);
    (void) set_union(iarr, iarr, iarr, iarr, iarr3, validating_less{});
    (void) set_union(iarr, iarr, iarr3, validating_less{});

    (void) set_intersection(varr, varr, varr, varr, varr3);
    (void) set_intersection(varr, varr, varr3);
    (void) set_intersection(iarr, iarr, iarr, iarr, iarr3, validating_less{});
    (void) set_intersection(iarr, iarr, iarr3, validating_less{});

    (void) set_difference(varr, varr, varr, varr, varr3);
    (void) set_difference(varr, varr, varr3);
    (void) set_difference(iarr, iarr, iarr, iarr, iarr3, validating_less{});
    (void) set_difference(iarr, iarr, iarr3, validating_less{});

    (void) set_symmetric_difference(varr, varr, varr, varr, varr3);
    (void) set_symmetric_difference(varr, varr, varr3);
    (void) set_symmetric_difference(iarr, iarr, iarr, iarr, iarr3, validating_less{});
    (void) set_symmetric_difference(iarr, iarr, iarr3, validating_less{});

    (void) push_heap(varr3, varr3 + 1);
    (void) push_heap(varr3);
    (void) push_heap(iarr3, iarr3 + 1, validating_less{});
    (void) push_heap(iarr3, {}, validating_identity{});

    (void) pop_heap(varr3, varr3 + 1);
    (void) pop_heap(varr3);
    (void) pop_heap(iarr3, iarr3 + 1, validating_less{});
    (void) pop_heap(iarr3, {}, validating_identity{});

    (void) make_heap(varr3, varr3 + 1);
    (void) make_heap(varr3);
    (void) make_heap(iarr3, iarr3 + 1, validating_less{});
    (void) make_heap(iarr3, {}, validating_identity{});

    (void) sort_heap(varr3, varr3 + 1);
    (void) sort_heap(varr3);
    (void) sort_heap(iarr3, iarr3 + 1, validating_less{});
    (void) sort_heap(iarr3, {}, validating_identity{});

    (void) is_heap(varr3, varr3 + 1);
    (void) is_heap(varr3);
    (void) is_heap(iarr3, iarr3 + 1, validating_less{});
    (void) is_heap(iarr3, {}, validating_identity{});

    (void) is_heap_until(varr3, varr3 + 1);
    (void) is_heap_until(varr3);
    (void) is_heap_until(iarr3, iarr3 + 1, validating_less{});
    (void) is_heap_until(iarr3, {}, validating_identity{});

    (void) min(+varr, +varr);
    (void) min({+varr, +varr});
    (void) min(varr);
    (void) min(+iarr, +iarr, validating_less{});
    (void) min({+iarr, +iarr}, {}, validating_identity{});
    (void) min(iarr, validating_less{});

    (void) max(+varr, +varr);
    (void) max({+varr, +varr});
    (void) max(varr);
    (void) max(+iarr, +iarr, validating_less{});
    (void) max({+iarr, +iarr}, {}, validating_identity{});
    (void) max(iarr, validating_less{});

    (void) minmax(+varr, +varr);
    (void) minmax({+varr, +varr});
    (void) minmax(varr);
    (void) minmax(+iarr, +iarr, validating_less{});
    (void) minmax({+iarr, +iarr}, {}, validating_identity{});
    (void) minmax(iarr, validating_less{});

    (void) min_element(varr, varr + 1);
    (void) min_element(varr);
    (void) min_element(iarr, iarr + 1, validating_less{});
    (void) min_element(iarr, {}, validating_identity{});

    (void) max_element(varr, varr + 1);
    (void) max_element(varr);
    (void) max_element(iarr, iarr + 1, validating_less{});
    (void) max_element(iarr, {}, validating_identity{});

    (void) minmax_element(varr, varr + 1);
    (void) minmax_element(varr);
    (void) minmax_element(iarr, iarr + 1, validating_less{});
    (void) minmax_element(iarr, {}, validating_identity{});

    (void) clamp(+varr, +varr, +varr);
    (void) clamp(+iarr, +iarr, +iarr, validating_less{});

    (void) lexicographical_compare(varr, varr, varr, varr);
    (void) lexicographical_compare(varr, varr);
    (void) lexicographical_compare(iarr, iarr, iarr, iarr, validating_less{});
    (void) lexicographical_compare(iarr, iarr, validating_less{});

    (void) next_permutation(varr, varr);
    (void) next_permutation(varr);
    (void) next_permutation(iarr, iarr, validating_less{});
    (void) next_permutation(iarr, {}, validating_identity{});

    (void) prev_permutation(varr, varr);
    (void) prev_permutation(varr);
    (void) prev_permutation(iarr, iarr, validating_less{});
    (void) prev_permutation(iarr, {}, validating_identity{});
}

// Separated test for ranges::count and equality
static_assert(std::equality_comparable<validator>);
static_assert(std::indirectly_comparable<validator*, validator*, std::equal_to<>>);
static_assert(std::sortable<validator*>);

constexpr bool test_ranges_count() {
    using namespace std::ranges;

    validator a[10]{};
    assert(count(a, a + 10, nullptr) == 10);
    assert(count(a, nullptr) == 10);
    return true;
}
static_assert(test_ranges_count());
#endif // ^^^ no workaround ^^^
