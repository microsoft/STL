// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstddef>
#include <iterator>

#include <test_death.hpp>

using namespace stdext;

void test_case_construction_out_of_range() {
    checked_array_iterator<int*> it(nullptr, 1, 0); // checked_array_iterator construction index out of range
    (void) it;
}

void test_case_operator_dereference_value_initialized_iterator() {
    checked_array_iterator<int*> it; // note: for IDL to work correctly, default init and value init are equivalent
    (void) *it; // cannot dereference value-initialized or null checked_array_iterator
}

void test_case_operator_dereference_end_iterator() {
    int arr[] = {1, 2};
    checked_array_iterator<int*> it(arr, 2, 2);
    (void) *it; // cannot dereference end checked_array_iterator
}

void test_case_operator_arrow_value_initialized_iterator() {
    checked_array_iterator<int*> it;
    (void) it.operator->(); // cannot dereference value-initialized or null checked_array_iterator
}

void test_case_operator_arrow_end_iterator() {
    int arr[] = {1, 2};
    checked_array_iterator<int*> it(arr, 2, 2);
    (void) it.operator->(); // cannot dereference end checked_array_iterator
}

void test_case_operator_preincrement_value_initialized_iterator() {
    checked_array_iterator<int*> it;
    ++it; // cannot increment value-initialized or null checked_array_iterator
}

void test_case_operator_preincrement_off_end() {
    int arr[] = {1, 2};
    checked_array_iterator<int*> it(arr, 2);
    for (size_t idx = 0; idx < 3; ++idx) {
        ++it; // cannot increment checked_array_iterator past end
    }
}

void test_case_operator_predecrement_value_initialized_iterator() {
    checked_array_iterator<int*> it;
    --it; // cannot decrement value-initialized or null checked_array_iterator
}

void test_case_operator_predecrement_before_begin() {
    int arr[] = {1, 2};
    checked_array_iterator<int*> it(arr, 2);
    --it; // cannot decrement checked_array_iterator before begin
}

void test_case_operator_advance_value_initialized_iterator() {
    checked_array_iterator<int*> it;
    it += 1; // cannot seek value-initialized or null checked_array_iterator
}

void test_case_operator_advance_value_initialized_iterator_zero() {
    checked_array_iterator<int*> it;
    it += 0; // OK
}

void test_case_operator_advance_before_begin() {
    int arr[] = {1, 2};
    checked_array_iterator<int*> it(arr, 2);
    it += -1; // cannot seek checked_array_iterator before begin
}

void test_case_operator_advance_after_end() {
    int arr[] = {1, 2};
    checked_array_iterator<int*> it(arr, 2);
    it += 3; // cannot seek checked_array_iterator after end
}

void test_case_operator_retreat_value_initialized_iterator() {
    checked_array_iterator<int*> it;
    it -= 1; // cannot seek value-initialized or null checked_array_iterator
}

void test_case_operator_retreat_value_initialized_iterator_zero() {
    checked_array_iterator<int*> it;
    it -= 0; // OK
}

void test_case_operator_retreat_before_begin() {
    int arr[] = {1, 2};
    checked_array_iterator<int*> it(arr, 2);
    it -= 1; // cannot seek checked_array_iterator before begin
}

void test_case_operator_retreat_after_end() {
    int arr[] = {1, 2};
    checked_array_iterator<int*> it(arr, 2);
    it -= -3; // cannot seek checked_array_iterator after end
}

void test_case_operator_subtract_incompatible_different_arrays() {
    int arr1[] = {1, 2};
    checked_array_iterator<int*> it1(arr1, 2);
    int arr2[] = {3, 4};
    checked_array_iterator<int*> it2(arr2, 2);
    (void) (it1 - it2); // cannot subtract incompatible checked_array_iterators
}

void test_case_operator_subtract_incompatible_different_sizes() {
    int arr[] = {1, 2, 3};
    checked_array_iterator<int*> it1(arr, 2);
    checked_array_iterator<int*> it2(arr, 3);
    (void) (it1 - it2); // cannot subtract incompatible checked_array_iterators
}

void test_case_operator_subtract_incompatible_value_initialized() {
    int arr[] = {1, 2};
    checked_array_iterator<int*> it(arr, 2);
    (void) (it - checked_array_iterator<int*>{}); // cannot subtract incompatible checked_array_iterators
}

void test_case_operator_equal_incompatible_different_arrays() {
    int arr1[] = {1, 2};
    checked_array_iterator<int*> it1(arr1, 2);
    int arr2[] = {3, 4};
    checked_array_iterator<int*> it2(arr2, 2);
    (void) (it1 == it2); // cannot compare incompatible checked_array_iterators for equality
}

void test_case_operator_equal_incompatible_different_sizes() {
    int arr[] = {1, 2, 3};
    checked_array_iterator<int*> it1(arr, 2);
    checked_array_iterator<int*> it2(arr, 3);
    (void) (it1 == it2); // cannot compare incompatible checked_array_iterators for equality
}

void test_case_operator_equal_incompatible_value_initialized() {
    int arr[] = {1, 2};
    checked_array_iterator<int*> it(arr, 2);
    (void) (it == checked_array_iterator<int*>{}); // cannot compare incompatible checked_array_iterators for equality
}

void test_case_operator_less_incompatible_different_arrays() {
    int arr1[] = {1, 2};
    checked_array_iterator<int*> it1(arr1, 2);
    int arr2[] = {3, 4};
    checked_array_iterator<int*> it2(arr2, 2);
    (void) (it1 < it2); // cannot compare incompatible checked_array_iterators
}

void test_case_operator_less_incompatible_different_sizes() {
    int arr[] = {1, 2, 3};
    checked_array_iterator<int*> it1(arr, 2);
    checked_array_iterator<int*> it2(arr, 3);
    (void) (it1 < it2); // cannot compare incompatible checked_array_iterators
}

void test_case_operator_less_incompatible_value_initialized() {
    int arr[] = {1, 2};
    checked_array_iterator<int*> it(arr, 2);
    (void) (it < checked_array_iterator<int*>{}); // cannot compare incompatible checked_array_iterators
}

// Note: checked_array_iterator checks are enabled even for _ITERATOR_DEBUG_LEVEL==0
int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec([] {
        test_case_operator_advance_value_initialized_iterator_zero();
        test_case_operator_retreat_value_initialized_iterator_zero();
    });

    exec.add_death_tests({
        test_case_operator_dereference_value_initialized_iterator,
        test_case_operator_dereference_end_iterator,
        test_case_operator_arrow_value_initialized_iterator,
        test_case_operator_arrow_end_iterator,
        test_case_operator_preincrement_value_initialized_iterator,
        test_case_operator_preincrement_off_end,
        test_case_operator_predecrement_value_initialized_iterator,
        test_case_operator_predecrement_before_begin,
        test_case_operator_advance_value_initialized_iterator,
        test_case_operator_advance_before_begin,
        test_case_operator_advance_after_end,
        test_case_operator_retreat_value_initialized_iterator,
        test_case_operator_retreat_before_begin,
        test_case_operator_retreat_after_end,
        test_case_operator_subtract_incompatible_different_arrays,
        test_case_operator_subtract_incompatible_different_sizes,
        test_case_operator_subtract_incompatible_value_initialized,
        test_case_operator_equal_incompatible_different_arrays,
        test_case_operator_equal_incompatible_different_sizes,
        test_case_operator_equal_incompatible_value_initialized,
        test_case_operator_less_incompatible_different_arrays,
        test_case_operator_less_incompatible_different_sizes,
        test_case_operator_less_incompatible_value_initialized,
    });

    return exec.run(argc, argv);
}
