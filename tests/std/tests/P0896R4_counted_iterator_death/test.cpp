// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

#include <cassert>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <vector>

#include <test_death.hpp>
using namespace std;

int globalArray[5]{10, 20, 30, 40, 50};
int otherArray[5]{10, 20, 30, 40, 50};
vector<int> checkedArray{10, 20, 30, 40, 50};
using vit = vector<int>::iterator;

struct simple_input_iter {
    using value_type      = int;
    using difference_type = ptrdiff_t;

    value_type operator*() const {
        return *_ptr;
    }
    simple_input_iter& operator++() {
        ++_ptr;
        return *this;
    }
    simple_input_iter operator++(int) {
        simple_input_iter _tmp = *this;
        ++_ptr;
        return _tmp;
    }

    bool operator==(simple_input_iter const&) const = default;

    int* _ptr = nullptr;
};

void test_case_construction_negative_length() {
    counted_iterator<int*> cit{globalArray, -1}; // counted_iterator requires non-negative length n
    (void) cit;
}

void test_case_operator_dereference_value_initialized_iterator() {
    counted_iterator<int*> cit{}; // note: for IDL to work correctly, default-init and value-init are equivalent
    (void) (*cit); // cannot dereference value-initialized counted_iterator
}

void test_case_operator_dereference_end_iterator() {
    counted_iterator<int*> cit{globalArray, 0};
    (void) (*cit); // cannot dereference end counted_iterator
}

void test_case_operator_preincrement_value_initialized_iterator() {
    counted_iterator<int*> cit{};
    ++cit; // cannot increment value-initialized counted_iterator
}

void test_case_operator_preincrement_value_initialized_input_iterator() {
    counted_iterator<simple_input_iter> cit{};
    ++cit; // cannot increment value-initialized counted_iterator
}

void test_case_operator_preincrement_after_end() {
    counted_iterator<int*> cit{globalArray, 0};
    ++cit; // cannot increment counted_iterator past end
}

void test_case_operator_preincrement_after_end_input_iterator() {
    counted_iterator<simple_input_iter> cit{simple_input_iter{globalArray}, 0};
    ++cit; // cannot increment counted_iterator past end
}

void test_case_operator_postincrement_value_initialized_iterator() {
    counted_iterator<int*> cit{};
    cit++; // cannot increment value-initialized counted_iterator
}

void test_case_operator_postincrement_value_initialized_input_iterator() {
    counted_iterator<simple_input_iter> cit{};
    cit++; // cannot increment value-initialized counted_iterator
}

void test_case_operator_postincrement_after_end() {
    counted_iterator<int*> cit{globalArray, 0};
    cit++; // cannot increment counted_iterator past end
}

void test_case_operator_postincrement_after_end_input_iterator() {
    counted_iterator<simple_input_iter> cit{simple_input_iter{globalArray}, 0};
    cit++; // cannot increment value-initialized counted_iterator
}

void test_case_operator_predecrement_before_begin() {
    counted_iterator<vit> cit{begin(checkedArray), 5};
    --cit; // cannot decrement counted_iterator before begin
}

void test_case_operator_decrement_before_begin() {
    counted_iterator<vit> cit{begin(checkedArray), 5};
    cit--; // cannot decrement counted_iterator before begin
}

void test_case_operator_advance_value_initialized_iterator() {
    counted_iterator<int*> cit{};
    cit += 1; // cannot seek value-initialized counted_iterator
}

void test_case_operator_advance_after_end() {
    counted_iterator<int*> cit{globalArray, 2};
    cit += 3; // cannot seek counted_iterator after end
}

void test_case_operator_advance_copy_value_initialized_iterator() {
    counted_iterator<int*> cit{};
    (void) (cit + 1); // cannot seek value-initialized counted_iterator
}

void test_case_operator_advance_copy_after_end() {
    counted_iterator<int*> cit{globalArray, 2};
    (void) (cit + 3); // cannot seek counted_iterator after end
}

void test_case_operator_advance_copy_2_value_initialized_iterator() {
    counted_iterator<int*> cit{};
    (void) (1 + cit); // cannot seek value-initialized counted_iterator
}

void test_case_operator_advance_copy_2_after_end() {
    counted_iterator<int*> cit{globalArray, 2};
    (void) (3 + cit); // cannot seek counted_iterator after end
}

void test_case_operator_retreat_before_begin() {
    counted_iterator<vit> cit{begin(checkedArray), 5};
    cit -= 1; // cannot seek counted_iterator before begin
}

void test_case_operator_retreat_negative_after_end() {
    counted_iterator<int*> cit{globalArray, 0};
    cit -= -1; // cannot seek counted_iterator after end
}

void test_case_operator_retreat_copy_before_begin() {
    counted_iterator<vit> cit{begin(checkedArray), 5};
    (void) (cit - 1); // cannot seek counted_iterator before begin
}

void test_case_operator_retreat_copy_after_end() {
    counted_iterator<int*> cit{globalArray, 0};
    (void) (cit - -1); // cannot seek counted_iterator after end
}

void test_case_operator_subtract_incompatible_different_data() {
    counted_iterator<int*> cit1(globalArray, 2);
    counted_iterator<int*> cit2(otherArray, 2);
    (void) (cit1 - cit2); // cannot subtract incompatible counted_iterators
}

void test_case_operator_subtract_incompatible_different_size() {
    counted_iterator<int*> cit1(globalArray, 3);
    counted_iterator<int*> cit2(globalArray, 4);
    (void) (cit1 - cit2); // cannot subtract incompatible counted_iterators
}

void test_case_operator_subtract_incompatible_value_initialized() {
    counted_iterator<int*> cit{globalArray, 0};
    (void) (cit - counted_iterator<int*>{}); // cannot subtract incompatible counted_iterators
}

void test_case_operator_equal_incompatible_different_data() {
    counted_iterator<int*> cit1(globalArray, 2);
    counted_iterator<int*> cit2(otherArray, 2);
    (void) (cit1 == cit2); // cannot compare incompatible counted_iterators for equality
}

void test_case_operator_equal_incompatible_different_size() {
    counted_iterator<int*> cit1(globalArray, 3);
    counted_iterator<int*> cit2(globalArray, 4);
    (void) (cit1 == cit2); // cannot compare incompatible counted_iterators for equality
}

void test_case_operator_equal_incompatible_value_initialized() {
    counted_iterator<int*> cit{globalArray, 0};
    (void) (cit == counted_iterator<int*>{}); // cannot compare incompatible counted_iterators for equality
}

void test_case_operator_spaceship_incompatible_different_data() {
    counted_iterator<int*> cit1(globalArray, 3);
    counted_iterator<int*> cit2(otherArray, 3);
    (void) (cit1 <=> cit2); // cannot compare incompatible counted_iterators
}

void test_case_operator_spaceship_incompatible_different_size() {
    counted_iterator<int*> cit1(globalArray, 3);
    counted_iterator<int*> cit2(globalArray, 4);
    (void) (cit1 <=> cit2); // cannot compare incompatible counted_iterators
}

void test_case_operator_spaceship_incompatible_value_initialized() {
    counted_iterator<int*> cit{globalArray, 0};
    (void) (cit <=> counted_iterator<int*>{}); // cannot compare incompatible counted_iterators
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({
        test_case_construction_negative_length,
        test_case_operator_dereference_value_initialized_iterator,
        test_case_operator_dereference_end_iterator,
        test_case_operator_preincrement_value_initialized_iterator,
        test_case_operator_preincrement_value_initialized_input_iterator,
        test_case_operator_preincrement_after_end,
        test_case_operator_preincrement_after_end_input_iterator,
        test_case_operator_postincrement_value_initialized_iterator,
        test_case_operator_postincrement_value_initialized_input_iterator,
        test_case_operator_postincrement_after_end,
        test_case_operator_postincrement_after_end_input_iterator,
        test_case_operator_predecrement_before_begin,
        test_case_operator_decrement_before_begin,
        test_case_operator_advance_value_initialized_iterator,
        test_case_operator_advance_after_end,
        test_case_operator_advance_copy_value_initialized_iterator,
        test_case_operator_advance_copy_after_end,
        test_case_operator_advance_copy_2_value_initialized_iterator,
        test_case_operator_advance_copy_2_after_end,
        test_case_operator_retreat_before_begin,
        test_case_operator_retreat_negative_after_end,
        test_case_operator_retreat_copy_before_begin,
        test_case_operator_retreat_copy_after_end,
#ifndef _M_CEE // TRANSITION, VSO-1665606
        test_case_operator_subtract_incompatible_different_data,
        test_case_operator_subtract_incompatible_different_size,
        test_case_operator_subtract_incompatible_value_initialized,
        test_case_operator_equal_incompatible_different_data,
        test_case_operator_equal_incompatible_different_size,
        test_case_operator_equal_incompatible_value_initialized,
        test_case_operator_spaceship_incompatible_different_data,
        test_case_operator_spaceship_incompatible_different_size,
        test_case_operator_spaceship_incompatible_value_initialized,
#endif // _M_CEE
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0

    return exec.run(argc, argv);
}
