// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <assert.h>
#include <execution>
#include <forward_list>
#include <iterator>
#include <list>
#include <parallel_algorithms_utilities.hpp>
#include <vector>

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
    auto first = c.begin();
    for (size_t i = 0; i < testSize - 1; ++i, ++first) {
        *first = false;
        assert(!is_partitioned(par, c.begin(), c.end(), read_char_as_bool));
        *first = true;
    }

    *first = false;
    assert(is_partitioned(par, c.begin(), c.end(), read_char_as_bool));
    *first = true;

    // front to back change T to F, end up with all F
    first = c.begin();
    for (size_t i = 0; i < testSize - 1; ++i, ++first) {
        *first = false;
        assert(!is_partitioned(par, c.begin(), c.end(), read_char_as_bool));
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

    for (size_t i = 1; i < testSize; ++i, ++first) {
        *first = true;
        assert(!is_partitioned(par, c.begin(), c.end(), read_char_as_bool));
        *first = false;
    }

    // front to back change F to T, end up with all T
    first  = c.begin();
    *first = true;
    while (++first != c.end()) {
        *first = true;
        assert(is_partitioned(par, c.begin(), c.end(), read_char_as_bool));
    }

    // testing with 2 partition points (T F T ... T F T ... T), where the F at index 1 is fixed and the second F is
    // tried at each index
    first  = std::next(c.begin());
    *first = false;
    while (++first != c.end()) {
        *first = false;
        assert(!is_partitioned(par, c.begin(), c.end(), read_char_as_bool));
        *first = true;
    }

    fill(c.begin(), c.end(), false);

    // testing with 2 partition adjacent points (T...T F T F...F) where the adjacent partition points are tried at each
    // index
    first       = c.begin();
    auto second = std::next(first, 2);
    for (; second != c.end(); ++first, ++second) {
        *first  = true;
        *second = true;
        assert(!is_partitioned(par, c.begin(), c.end(), read_char_as_bool));
        *second = false;
    }
}

int main() {
    parallel_test_case(test_case_is_partitioned_parallel<vector>);
    parallel_test_case(test_case_is_partitioned_parallel<forward_list>);
    parallel_test_case(test_case_is_partitioned_parallel<list>);
}
