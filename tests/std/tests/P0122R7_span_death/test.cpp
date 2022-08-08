// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

#include <algorithm>
#include <cstddef>
#include <span>
#include <vector>

#include <test_death.hpp>
using namespace std;

int globalArray[5]{10, 20, 30, 40, 50};
int otherArray[5]{10, 20, 30, 40, 50};

void test_case_operator_dereference_value_initialized_iterator() {
    span<int>::iterator it; // note: for IDL to work correctly, default-init and value-init are equivalent
    (void) *it; // cannot dereference value-initialized span iterator
}

void test_case_operator_dereference_end_iterator() {
    span<int> sp(globalArray);
    span<int>::iterator it = sp.end();
    (void) *it; // cannot dereference end span iterator
}

void test_case_operator_arrow_value_initialized_iterator() {
    span<int>::iterator it;
    (void) it.operator->(); // cannot dereference value-initialized span iterator
}

void test_case_operator_arrow_end_iterator() {
    span<int> sp(globalArray);
    span<int>::iterator it = sp.end();
    (void) it.operator->(); // cannot dereference end span iterator
}

void test_case_operator_preincrement_value_initialized_iterator() {
    span<int>::iterator it;
    ++it; // cannot increment value-initialized span iterator
}

void test_case_operator_preincrement_after_end() {
    span<int> sp(globalArray);
    span<int>::iterator it = sp.end();
    ++it; // cannot increment span iterator past end
}

void test_case_operator_predecrement_value_initialized_iterator() {
    span<int>::iterator it;
    --it; // cannot decrement value-initialized span iterator
}

void test_case_operator_predecrement_before_begin() {
    span<int> sp(globalArray);
    span<int>::iterator it = sp.begin();
    --it; // cannot decrement span iterator before begin
}

void test_case_operator_advance_value_initialized_iterator() {
    span<int>::iterator it;
    it += 1; // cannot seek value-initialized span iterator
}

void test_case_operator_advance_value_initialized_iterator_zero() {
    span<int>::iterator it;
    it += 0; // OK
}

void test_case_operator_advance_before_begin() {
    span<int> sp(globalArray);
    span<int>::iterator it = sp.begin();
    it += -1; // cannot seek span iterator before begin
}

void test_case_operator_advance_after_end() {
    span<int> sp(globalArray);
    span<int>::iterator it = sp.end();
    it += 1; // cannot seek span iterator after end
}

void test_case_operator_retreat_value_initialized_iterator() {
    span<int>::iterator it;
    it -= 1; // cannot seek value-initialized span iterator
}

void test_case_operator_retreat_value_initialized_iterator_zero() {
    span<int>::iterator it;
    it -= 0; // OK
}

void test_case_operator_retreat_before_begin() {
    span<int> sp(globalArray);
    span<int>::iterator it = sp.begin();
    it -= 1; // cannot seek span iterator before begin
}

void test_case_operator_retreat_after_end() {
    span<int> sp(globalArray);
    span<int>::iterator it = sp.end();
    it -= -1; // cannot seek span iterator after end
}

void test_case_operator_subtract_incompatible_different_data() {
    span<int> sp1(globalArray);
    span<int> sp2(otherArray);
    (void) (sp1.begin() - sp2.begin()); // cannot subtract incompatible span iterators
}

void test_case_operator_subtract_incompatible_different_size() {
    span<int> sp1(globalArray, 3);
    span<int> sp2(globalArray, 4);
    (void) (sp1.begin() - sp2.begin()); // cannot subtract incompatible span iterators
}

void test_case_operator_subtract_incompatible_value_initialized() {
    span<int> sp(globalArray);
    (void) (sp.begin() - span<int>::iterator{}); // cannot subtract incompatible span iterators
}

void test_case_operator_equal_incompatible_different_data() {
    span<int> sp1(globalArray);
    span<int> sp2(otherArray);
    (void) (sp1.begin() == sp2.begin()); // cannot compare incompatible span iterators for equality
}

void test_case_operator_equal_incompatible_different_size() {
    span<int> sp1(globalArray, 3);
    span<int> sp2(globalArray, 4);
    (void) (sp1.begin() == sp2.begin()); // cannot compare incompatible span iterators for equality
}

void test_case_operator_equal_incompatible_value_initialized() {
    span<int> sp(globalArray);
    (void) (sp.begin() == span<int>::iterator{}); // cannot compare incompatible span iterators for equality
}

void test_case_operator_less_incompatible_different_data() {
    span<int> sp1(globalArray);
    span<int> sp2(otherArray);
    (void) (sp1.begin() < sp2.begin()); // cannot compare incompatible span iterators
}

void test_case_operator_less_incompatible_different_size() {
    span<int> sp1(globalArray, 3);
    span<int> sp2(globalArray, 4);
    (void) (sp1.begin() < sp2.begin()); // cannot compare incompatible span iterators
}

void test_case_operator_less_incompatible_value_initialized() {
    span<int> sp(globalArray);
    (void) (sp.begin() < span<int>::iterator{}); // cannot compare incompatible span iterators
}

void test_case_algorithm_incompatible_different_data() {
    span<int> sp1(globalArray);
    span<int> sp2(otherArray);
    (void) find(sp1.begin(), sp2.begin(), -1); // span iterators from different views do not form a range
}

void test_case_algorithm_incompatible_different_size() {
    span<int> sp1(globalArray, 3);
    span<int> sp2(globalArray, 4);
    (void) find(sp1.begin(), sp2.begin(), -1); // span iterators from different views do not form a range
}

void test_case_algorithm_incompatible_value_initialized() {
    span<int> sp(globalArray);
    (void) find(sp.begin(), span<int>::iterator{}, -1); // span iterators from different views do not form a range
}

void test_case_algorithm_incompatible_transposed() {
    span<int> sp(globalArray);
    (void) find(sp.end(), sp.begin(), -1); // span iterator range transposed
}

void test_case_constructor_first_count_incompatible_extent() {
    span<int, 3> sp(begin(globalArray),
        size(globalArray)); // Cannot construct span with static extent from range [first, first + count) as count !=
                            // extent
    (void) sp;
}

void test_case_constructor_first_last_incompatible_extent() {
    span<int, 3> sp(begin(globalArray), end(globalArray)); // Cannot construct span with static extent from range
                                                           // [first, last) as last - first != extent
    (void) sp;
}

void test_case_constructor_range_incompatible_extent() {
    vector<int> v(begin(globalArray), end(globalArray));
    span<int, 3> sp(v); // Cannot construct span with static extent from range r as std::ranges::size(r) != extent
    (void) sp;
}

void test_case_constructor_span_incompatible_extent() {
    span<int> sp(begin(globalArray), end(globalArray));
    span<int, 3> sp2(sp); // Cannot construct span with static extent from other span as other.size() != extent
    (void) sp2;
}

void test_case_first_excessive_compiletime_count() {
    span<int> sp(globalArray);
    (void) sp.first<6>(); // Count out of range in span::first()
}

void test_case_first_excessive_runtime_count_dynamic_extent() {
    span<int> sp(globalArray);
    (void) sp.first(6); // Count out of range in span::first(count)
}

void test_case_first_excessive_runtime_count_static_extent() {
    span<int, 5> sp(globalArray);
    (void) sp.first(6); // Count out of range in span::first(count)
}

void test_case_last_excessive_compiletime_count() {
    span<int> sp(globalArray);
    (void) sp.last<6>(); // Count out of range in span::last()
}

void test_case_last_excessive_runtime_count_dynamic_extent() {
    span<int> sp(globalArray);
    (void) sp.last(6); // Count out of range in span::last(count)
}

void test_case_last_excessive_runtime_count_static_extent() {
    span<int, 5> sp(globalArray);
    (void) sp.last(6); // Count out of range in span::last(count)
}

void test_case_subspan_excessive_compiletime_offset() {
    span<int> sp(globalArray);
    (void) sp.subspan<6>(); // Offset out of range in span::subspan()
}

void test_case_subspan_excessive_compiletime_count() {
    span<int> sp(globalArray);
    (void) sp.subspan<2, 4>(); // Count out of range in span::subspan()
}

void test_case_subspan_excessive_runtime_offset_dynamic_extent() {
    span<int> sp(globalArray);
    (void) sp.subspan(6); // Offset out of range in span::subspan(offset, count)
}

void test_case_subspan_excessive_runtime_count_dynamic_extent() {
    span<int> sp(globalArray);
    (void) sp.subspan(2, 4); // Count out of range in span::subspan(offset, count)
}

void test_case_subspan_excessive_runtime_offset_static_extent() {
    span<int, 5> sp(globalArray);
    (void) sp.subspan(6); // Offset out of range in span::subspan(offset, count)
}

void test_case_subspan_excessive_runtime_count_static_extent() {
    span<int, 5> sp(globalArray);
    (void) sp.subspan(2, 4); // Count out of range in span::subspan(offset, count)
}

void test_case_size_bytes_overflow() {
    span<int> sp(begin(globalArray), static_cast<size_t>(-2)); // undefined behavior, not detected here
    (void) sp.size_bytes(); // size of span in bytes exceeds std::numeric_limits<size_t>::max()
}

void test_case_operator_subscript_out_of_range_dynamic_extent() {
    span<int> sp(globalArray);
    (void) sp[5]; // span index out of range
}

void test_case_operator_subscript_out_of_range_static_extent() {
    span<int, 5> sp(globalArray);
    (void) sp[5]; // span index out of range
}

void test_case_front_empty_dynamic_extent() {
    span<int> sp;
    (void) sp.front(); // front of empty span
}

void test_case_back_empty_dynamic_extent() {
    span<int> sp;
    (void) sp.back(); // back of empty span
}

void test_case_front_empty_static_extent() {
    span<int, 0> sp;
    (void) sp.front(); // front of empty span
}

void test_case_back_empty_static_extent() {
    span<int, 0> sp;
    (void) sp.back(); // back of empty span
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec([] {
        test_case_operator_advance_value_initialized_iterator_zero();
        test_case_operator_retreat_value_initialized_iterator_zero();
    });

#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({
        test_case_operator_dereference_value_initialized_iterator,
        test_case_operator_dereference_end_iterator,
        test_case_operator_arrow_value_initialized_iterator,
        test_case_operator_arrow_end_iterator,
        test_case_operator_preincrement_value_initialized_iterator,
        test_case_operator_preincrement_after_end,
        test_case_operator_predecrement_value_initialized_iterator,
        test_case_operator_predecrement_before_begin,
        test_case_operator_advance_value_initialized_iterator,
        test_case_operator_advance_before_begin,
        test_case_operator_advance_after_end,
        test_case_operator_retreat_value_initialized_iterator,
        test_case_operator_retreat_before_begin,
        test_case_operator_retreat_after_end,
        test_case_operator_subtract_incompatible_different_data,
        test_case_operator_subtract_incompatible_different_size,
        test_case_operator_subtract_incompatible_value_initialized,
        test_case_operator_equal_incompatible_different_data,
        test_case_operator_equal_incompatible_different_size,
        test_case_operator_equal_incompatible_value_initialized,
        test_case_operator_less_incompatible_different_data,
        test_case_operator_less_incompatible_different_size,
        test_case_operator_less_incompatible_value_initialized,
        test_case_algorithm_incompatible_different_data,
        test_case_algorithm_incompatible_different_size,
        test_case_algorithm_incompatible_value_initialized,
        test_case_algorithm_incompatible_transposed,
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0

    // _CONTAINER_DEBUG_LEVEL tests
    exec.add_death_tests({
        test_case_constructor_first_count_incompatible_extent,
        test_case_constructor_first_last_incompatible_extent,
        test_case_constructor_range_incompatible_extent,
        test_case_constructor_span_incompatible_extent,
        test_case_first_excessive_compiletime_count,
        test_case_first_excessive_runtime_count_dynamic_extent,
        test_case_first_excessive_runtime_count_static_extent,
        test_case_last_excessive_compiletime_count,
        test_case_last_excessive_runtime_count_dynamic_extent,
        test_case_last_excessive_runtime_count_static_extent,
        test_case_subspan_excessive_compiletime_offset,
        test_case_subspan_excessive_compiletime_count,
        test_case_subspan_excessive_runtime_offset_dynamic_extent,
        test_case_subspan_excessive_runtime_count_dynamic_extent,
        test_case_subspan_excessive_runtime_offset_static_extent,
        test_case_subspan_excessive_runtime_count_static_extent,
        test_case_size_bytes_overflow,
        test_case_operator_subscript_out_of_range_dynamic_extent,
        test_case_operator_subscript_out_of_range_static_extent,
        test_case_front_empty_dynamic_extent,
        test_case_back_empty_dynamic_extent,
        test_case_front_empty_static_extent,
        test_case_back_empty_static_extent,
    });

    return exec.run(argc, argv);
}
