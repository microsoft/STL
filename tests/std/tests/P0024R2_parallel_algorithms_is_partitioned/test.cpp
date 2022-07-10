// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <execution>
#include <forward_list>
#include <iterator>
#include <list>
#include <vector>

#include <parallel_algorithms_utilities.hpp>

using namespace std;
using namespace std::execution;

const auto read_char_as_bool = [](char c) { return c != 0; };

template <template <class...> class Container>
void test_case_is_partitioned_parallel(const size_t testSize) {
    Container<char> c(testSize, true);

    assert(is_partitioned(par, c.begin(), c.end(), read_char_as_bool));

    if (testSize == 0) {
        return;
    }

    // T ... T F T ... T with F positioned at each index
    auto remainingAttempts = quadratic_complexity_case_limit;
    auto first             = c.begin();
    for (size_t i = 0; i < testSize - 1; ++i, ++first) {
        *first = false;
        assert(!is_partitioned(par, c.begin(), c.end(), read_char_as_bool));
        *first = true;
        if (--remainingAttempts == 0) {
            std::advance(first, static_cast<ptrdiff_t>(testSize - 1 - i));
            break;
        }
    }

    *first = false;
    assert(is_partitioned(par, c.begin(), c.end(), read_char_as_bool));
    *first = true;

    // front to back change T to F, end up with all F
    remainingAttempts = quadratic_complexity_case_limit;
    first             = c.begin();
    for (size_t i = 0; i < testSize - 1; ++i, ++first) {
        *first = false;
        assert(!is_partitioned(par, c.begin(), c.end(), read_char_as_bool));
        if (--remainingAttempts == 0) {
            first = std::fill_n(first, static_cast<ptrdiff_t>(testSize - 1 - i), '\0');
            break;
        }
    }
    *first = false;

    // contains all F
    assert(is_partitioned(par, c.begin(), c.end(), read_char_as_bool));

    // test cases for sizes 1 and 2 are exhausted at this point
    if (testSize <= 2) {
        return;
    }

    // F ... F T F ... F with T positioned at each index
    first  = c.begin();
    *first = true;
    assert(is_partitioned(par, c.begin(), c.end(), read_char_as_bool));
    *first = false;
    ++first;

    remainingAttempts = quadratic_complexity_case_limit;
    for (size_t i = 1; i < testSize; ++i, ++first) {
        *first = true;
        assert(!is_partitioned(par, c.begin(), c.end(), read_char_as_bool));
        *first = false;
        if (--remainingAttempts == 0) {
            break;
        }
    }

    // front to back change F to T, end up with all T
    remainingAttempts = quadratic_complexity_case_limit;
    first             = c.begin();
    *first            = true;
    while (++first != c.end()) {
        *first = true;
        assert(is_partitioned(par, c.begin(), c.end(), read_char_as_bool));
        if (--remainingAttempts == 0) {
            std::fill(first, c.end(), '\x01');
            break;
        }
    }

    // testing with 2 partition points (T F T ... T F T ... T), where the F at index 1 is fixed and the second F is
    // tried at each index
    remainingAttempts = quadratic_complexity_case_limit;
    first             = next(c.begin());
    *first            = false;
    while (++first != c.end()) {
        *first = false;
        assert(!is_partitioned(par, c.begin(), c.end(), read_char_as_bool));
        *first = true;
        if (--remainingAttempts == 0) {
            break;
        }
    }

    fill(c.begin(), c.end(), false);

    // testing with 2 partition adjacent points (T...T F T F...F) where the adjacent partition points are tried at each
    // index
    remainingAttempts = quadratic_complexity_case_limit;
    first             = c.begin();
    auto second       = next(first, 2);
    for (; second != c.end(); ++first, ++second) {
        *first  = true;
        *second = true;
        assert(!is_partitioned(par, c.begin(), c.end(), read_char_as_bool));
        *second = false;
        if (--remainingAttempts == 0) {
            break;
        }
    }
}

int main() {
    parallel_test_case(test_case_is_partitioned_parallel<vector>);
    parallel_test_case(test_case_is_partitioned_parallel<forward_list>);
    parallel_test_case(test_case_is_partitioned_parallel<list>);
}
