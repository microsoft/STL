// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CONTAINER_DEBUG_LEVEL 1

#include <algorithm>
#include <cassert>
#include <concepts>
#include <iterator>

#include <test_death.hpp>
using namespace std;

template <class T = int>
struct throwingIter {
    using value_type      = T;
    using difference_type = T;

    value_type operator*() const {
        return _val;
    }
    value_type operator->() const {
        return _val;
    }
    throwingIter& operator++() {
        ++_val;
    }
    throwingIter operator++(int) {
        ++_val;
    }

    bool operator==(throwingIter const&) const = default;
    bool operator==(default_sentinel_t const&) const {
        return true;
    }

    template <class U = int>
    friend void iter_swap(throwingIter const& x, throwingIter<U> const& y) {
        x._val = exchange(x._val, y._val);
    }

    throwingIter() = default;
    throwingIter(int val) : _val(val) {}
    throwingIter(const throwingIter& other) {
        if (other._val == -1) {
            throw;
        }
        _val = other._val;
    }
    throwingIter(throwingIter&& other) {
        if (other._val == -1) {
            throw;
        }
        _val = other._val;
    }
    throwingIter& operator=(const throwingIter& other) {
        if (_val == -1) {
            throw;
        }
        _val = other._val;
        return *this;
    }
    throwingIter& operator=(throwingIter&& other) {
        if (_val == -1) {
            throw;
        }
        _val = other._val;
        return *this;
    }
    T _val = 0;
};

using CIT = common_iterator<throwingIter<int>, default_sentinel_t>;

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

void test_case_operator_increment() {
    CIT cit{default_sentinel};
    cit++; // cannot increment common_iterator that holds a sentinel
}

void test_case_iter_move() {
    CIT cit{default_sentinel};
    (void) ranges::iter_move(cit); // cannot iter_move common_iterator that holds a sentinel
}

void test_case_iter_swap() {
    CIT cit1{default_sentinel};
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
        test_case_operator_increment,
        test_case_iter_move,
        test_case_iter_swap,
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0

    return exec.run(argc, argv);
}
