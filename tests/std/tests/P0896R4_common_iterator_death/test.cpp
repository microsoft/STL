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

    friend void iter_swap(const simple_input_iter&, const simple_input_iter&) {}
};

using CIT = common_iterator<simple_input_iter, default_sentinel_t>;

void test_case_operator_dereference() {
    CIT cit{default_sentinel};
    (void) (*cit); // cannot dereference common_iterator that holds a sentinel
}

void test_case_operator_dereference_const() {
    const CIT cit{default_sentinel};
    (void) (*cit); // cannot dereference common_iterator that holds a sentinel
}

void test_case_operator_arrow() {
    CIT cit{default_sentinel};
    (void) (cit.operator->()); // cannot dereference common_iterator that holds a sentinel
}

void test_case_operator_preincrement() {
    CIT cit{default_sentinel};
    ++cit; // cannot pre increment common_iterator that holds a sentinel
}

void test_case_operator_postincrement() {
    CIT cit{default_sentinel};
    cit++; // cannot increment common_iterator that holds a sentinel
}

void test_case_iter_move() {
    CIT cit{default_sentinel};
    (void) ranges::iter_move(cit); // cannot iter_move common_iterator that holds a sentinel
}

void test_case_iter_swap_sentinel_left() {
    CIT cit1{default_sentinel};
    CIT cit2{};
    (void) ranges::iter_swap(cit1, cit2); // cannot iter_swap common_iterators that hold a sentinel
}

void test_case_iter_swap_sentinel_right() {
    CIT cit1{};
    CIT cit2{default_sentinel};
    (void) ranges::iter_swap(cit1, cit2); // cannot iter_swap common_iterators that hold a sentinel
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec([] {});

#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({
        test_case_operator_dereference,
        test_case_operator_dereference_const,
        test_case_operator_arrow,
        test_case_operator_preincrement,
        test_case_operator_postincrement,
        test_case_iter_move,
        test_case_iter_swap_sentinel_left,
        test_case_iter_swap_sentinel_right,
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0

    return exec.run(argc, argv);
}
