// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

#include <cstddef>
#include <string_view>

#include <test_death.hpp>

using namespace std;

void test_case_operator_dereference_value_initialized_iterator() {
    string_view::iterator it; // note: for IDL to work correctly, default init and value init are equivalent
    (void) *it; // cannot dereference value-initialized string_view iterator
}

void test_case_operator_dereference_end_iterator() {
    string_view sv("text");
    string_view::iterator it = sv.end();
    (void) *it; // cannot dereference end string_view iterator
}

void test_case_operator_arrow_value_initialized_iterator() {
    string_view::iterator it;
    (void) it.operator->(); // cannot dereference value-initialized string_view iterator
}

void test_case_operator_arrow_end_iterator() {
    string_view sv("text");
    string_view::iterator it = sv.end();
    (void) it.operator->(); // cannot dereference end string_view iterator
}

void test_case_operator_preincrement_value_initialized_iterator() {
    string_view::iterator it;
    ++it; // cannot increment value-initialized string_view iterator
}

void test_case_operator_preincrement_off_end() {
    string_view sv("text");
    string_view::iterator it = sv.begin();
    for (size_t idx = 0; idx < 5; ++idx) {
        ++it; // cannot increment string_view iterator past end
    }
}

void test_case_operator_predecrement_value_initialized_iterator() {
    string_view::iterator it;
    --it; // cannot decrement value-initialized string_view iterator
}

void test_case_operator_predecrement_before_begin() {
    string_view sv("text");
    string_view::iterator it = sv.begin();
    --it; // cannot decrement string_view iterator before begin
}

void test_case_operator_advance_value_initialized_iterator() {
    string_view::iterator it;
    it += 1; // cannot seek value-initialized string_view iterator
}

void test_case_operator_advance_value_initialized_iterator_zero() {
    string_view::iterator it;
    it += 0; // OK
}

void test_case_operator_advance_before_begin() {
    string_view sv("text");
    string_view::iterator it = sv.begin();
    it += -1; // cannot seek string_view iterator before begin
}

void test_case_operator_advance_after_end() {
    string_view sv("text");
    string_view::iterator it = sv.begin();
    it += 5; // cannot seek string_view iterator after end
}

void test_case_operator_retreat_value_initialized_iterator() {
    string_view::iterator it;
    it -= 1; // cannot seek value-initialized string_view iterator
}

void test_case_operator_retreat_value_initialized_iterator_zero() {
    string_view::iterator it;
    it -= 0; // OK
}

void test_case_operator_retreat_before_begin() {
    string_view sv("text");
    string_view::iterator it = sv.begin();
    it -= 1; // cannot seek string_view iterator before begin
}

void test_case_operator_retreat_after_end() {
    string_view sv("text");
    string_view::iterator it = sv.begin();
    it -= -5; // cannot seek string_view iterator after end
}

void test_case_operator_subtract_incompatible_different_views() {
    string_view sv1("text");
    string_view sv2("text2");
    (void) (sv1.begin() - sv2.begin()); // cannot subtract incompatible string_view iterators
}

void test_case_operator_subtract_incompatible_value_initialized() {
    string_view sv1("text");
    (void) (sv1.begin() - string_view::iterator{}); // cannot subtract incompatible string_view iterators
}

void test_case_operator_equal_incompatible_different_views() {
    string_view sv1("text");
    string_view sv2("text2");
    (void) (sv1.begin() == sv2.begin()); // cannot compare incompatible string_view iterators for equality
}

void test_case_operator_equal_incompatible_value_initialized() {
    string_view sv1("text");
    (void) (sv1.begin() == string_view::iterator{}); // cannot compare incompatible string_view iterators for equality
}

void test_case_operator_less_incompatible_different_views() {
    string_view sv1("text");
    string_view sv2("text2");
    (void) (sv1.begin() < sv2.begin()); // cannot compare incompatible string_view iterators
}

void test_case_operator_less_incompatible_value_initialized() {
    string_view sv1("text");
    (void) (sv1.begin() < string_view::iterator{}); // cannot compare incompatible string_view iterators
}

void test_case_operator_subscript_out_of_range() {
    string_view sv("text");
    (void) sv[4]; // string_view subscript out of range
}

void test_case_front_empty() {
    string_view sv;
    (void) sv.front(); // cannot call front on empty string_view
}

void test_case_back_empty() {
    string_view sv;
    (void) sv.back(); // cannot call back on empty string_view
}

void test_case_remove_prefix_too_large() {
    string_view sv("text");
    sv.remove_prefix(5); // cannot remove prefix longer than total size
}

void test_case_remove_prefix_zero() {
    string_view sv;
    sv.remove_prefix(0); // OK
}

void test_case_remove_suffix_too_large() {
    string_view sv("text");
    sv.remove_suffix(5); // cannot remove suffix longer than total size
}

void test_case_remove_suffix_zero() {
    string_view sv;
    sv.remove_suffix(0); // OK
}

void test_case_remove_prefix_incompatible() {
    string_view sv("text");
    auto old_range = sv.begin();
    sv.remove_prefix(1);
    auto new_range = sv.begin();
    (void) (old_range == new_range); // cannot compare incompatible string_view iterators for equality
}

void test_case_remove_suffix_incompatible() {
    string_view sv("text");
    auto old_range = sv.begin();
    sv.remove_suffix(1);
    auto new_range = sv.begin();
    (void) (old_range == new_range); // cannot compare incompatible string_view iterators for equality
}

void test_case_Copy_s() {
    string_view sv("text");
    char buffer[2];
#pragma warning(suppress : 28020) // yay PREfast catches this mistake at compile time!
    sv._Copy_s(buffer, 2, 4); // CRT invalid parameter handler (memcpy_s failed)
}

void test_case_null_constructor() {
#pragma warning(suppress : 6387) // yay PREfast catches this mistake at compile time!
    string_view sv(nullptr, 1); // non-zero size null string_view
    (void) sv;
}

void test_case_null_constructor_zero() {
    string_view sv(nullptr, 0); // OK
    (void) sv;
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec([] {
        test_case_operator_advance_value_initialized_iterator_zero();
        test_case_operator_retreat_value_initialized_iterator_zero();
        test_case_remove_prefix_zero();
        test_case_remove_suffix_zero();
        test_case_null_constructor_zero();
    });

#if _ITERATOR_DEBUG_LEVEL != 0
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
        test_case_operator_subtract_incompatible_different_views,
        test_case_operator_subtract_incompatible_value_initialized,
        test_case_operator_equal_incompatible_different_views,
        test_case_operator_equal_incompatible_value_initialized,
        test_case_operator_less_incompatible_different_views,
        test_case_operator_less_incompatible_value_initialized,
        test_case_remove_prefix_incompatible,
        test_case_remove_suffix_incompatible,
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0

    exec.add_death_tests({
        // These tests are turned on for _ITERATOR_DEBUG_LEVEL == 0 as part of
        // VSO-830211 "Macro to enable runtime bounds checking for subscript operator for STL containers"
        test_case_operator_subscript_out_of_range,
        test_case_front_empty,
        test_case_back_empty,
        test_case_remove_prefix_too_large,
        test_case_remove_suffix_too_large,
        test_case_Copy_s,
        test_case_null_constructor,
    });

    return exec.run(argc, argv);
}
