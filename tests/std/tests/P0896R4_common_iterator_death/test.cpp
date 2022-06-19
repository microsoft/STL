// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

#include <algorithm>
#include <cassert>
#include <concepts>
#include <iterator>

#include <test_death.hpp>
using namespace std;

struct simple_input_iter {
    using value_type      = int;
    using difference_type = int;

    value_type operator*() const {
        return 0;
    }
    value_type operator->() const {
        return 0;
    }
    simple_input_iter& operator++() {
        return *this;
    }
    simple_input_iter operator++(int) {
        return *this;
    }

    bool operator==(const simple_input_iter&) const = default;
    bool operator==(const default_sentinel_t&) const {
        return true;
    }

    difference_type operator-(const simple_input_iter&) const {
        return 42;
    }
    friend difference_type operator-(const simple_input_iter&, const default_sentinel_t&) {
        return 42;
    }
    friend difference_type operator-(const default_sentinel_t&, const simple_input_iter&) {
        return 42;
    }

    friend void iter_swap(const simple_input_iter&, const simple_input_iter&) {}
};

using CIT = common_iterator<simple_input_iter, default_sentinel_t>;

void test_case_operator_dereference_sentinel() {
    CIT cit{default_sentinel};
    (void) (*cit); // common_iterator can only be dereferenced if it holds an iterator
}

void test_case_operator_dereference_valueless() {
    CIT cit{_Variantish_empty_tag{}};
    (void) (*cit); // common_iterator can only be dereferenced if it holds an iterator
}

void test_case_operator_dereference_const_sentinel() {
    const CIT cit{default_sentinel};
    (void) (*cit); // common_iterator can only be dereferenced if it holds an iterator
}

void test_case_operator_dereference_const_valueless() {
    const CIT cit{_Variantish_empty_tag{}};
    (void) (*cit); // common_iterator can only be dereferenced if it holds an iterator
}

void test_case_operator_arrow_sentinel() {
    CIT cit{default_sentinel};
    (void) (cit.operator->()); // common_iterator can only be dereferenced if it holds an iterator
}
void test_case_operator_arrow_valueless() {
    CIT cit{_Variantish_empty_tag{}};
    (void) (cit.operator->()); // common_iterator can only be dereferenced if it holds an iterator
}

void test_case_operator_preincrement_sentinel() {
    CIT cit{default_sentinel};
    ++cit; // common_iterator can only be preincremented if it holds an iterator
}

void test_case_operator_preincrement_valueless() {
    CIT cit{_Variantish_empty_tag{}};
    ++cit; // common_iterator can only be preincremented if it holds an iterator
}

void test_case_operator_postincrement_sentinel() {
    CIT cit{default_sentinel};
    cit++; // common_iterator can only be postincremented if it holds an iterator
}

void test_case_operator_postincrement_valueless() {
    CIT cit{_Variantish_empty_tag{}};
    cit++; // common_iterator can only be postincremented if it holds an iterator
}

void test_case_equality_left_valueless() {
    CIT cit1{_Variantish_empty_tag{}};
    CIT cit2{};
    (void) (cit1 == cit2); // common_iterator can only be compared if it holds a value
}

void test_case_equality_right_valueless() {
    CIT cit1{};
    CIT cit2{_Variantish_empty_tag{}};
    (void) (cit1 == cit2); // common_iterator can only be compared if it holds a value
}

void test_case_difference_left_valueless() {
    CIT cit1{_Variantish_empty_tag{}};
    CIT cit2{};
    (void) (cit1 - cit2); // common_iterator can only be subtracted if it holds a value
}

void test_case_difference_right_valueless() {
    CIT cit1{};
    CIT cit2{_Variantish_empty_tag{}};
    (void) (cit1 - cit2); // common_iterator can only be subtracted if it holds a value
}

void test_case_iter_move_sentinel() {
    CIT cit{default_sentinel};
    (void) ranges::iter_move(cit); // can only iter_move from common_iterator if it holds an iterator
}

void test_case_iter_move_valueless() {
    CIT cit{_Variantish_empty_tag{}};
    (void) ranges::iter_move(cit); // can only iter_move from common_iterator if it holds an iterator
}

void test_case_iter_swap_sentinel_left_sentinel() {
    CIT cit1{default_sentinel};
    CIT cit2{};
    (void) ranges::iter_swap(cit1, cit2); // can only iter_swap common_iterators if both hold iterators
}

void test_case_iter_swap_sentinel_left_valueless() {
    CIT cit1{_Variantish_empty_tag{}};
    CIT cit2{};
    (void) ranges::iter_swap(cit1, cit2); // can only iter_swap common_iterators if both hold iterators
}

void test_case_iter_swap_sentinel_right_sentinel() {
    CIT cit1{};
    CIT cit2{default_sentinel};
    (void) ranges::iter_swap(cit1, cit2); // can only iter_swap common_iterators if both hold iterators
}

void test_case_iter_swap_sentinel_right_valueless() {
    CIT cit1{};
    CIT cit2{_Variantish_empty_tag{}};
    (void) ranges::iter_swap(cit1, cit2); // can only iter_swap common_iterators if both hold iterators
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({
        test_case_operator_dereference_sentinel,
        test_case_operator_dereference_valueless,
        test_case_operator_dereference_const_sentinel,
        test_case_operator_dereference_const_valueless,
        test_case_operator_arrow_sentinel,
        test_case_operator_arrow_valueless,
        test_case_operator_preincrement_sentinel,
        test_case_operator_preincrement_valueless,
        test_case_operator_postincrement_sentinel,
        test_case_operator_postincrement_valueless,
        test_case_equality_left_valueless,
        test_case_equality_right_valueless,
        test_case_difference_left_valueless,
        test_case_difference_right_valueless,
        test_case_iter_move_sentinel,
        test_case_iter_move_valueless,
        test_case_iter_swap_sentinel_left_sentinel,
        test_case_iter_swap_sentinel_left_valueless,
        test_case_iter_swap_sentinel_right_sentinel,
        test_case_iter_swap_sentinel_right_valueless,
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0

    return exec.run(argc, argv);
}
