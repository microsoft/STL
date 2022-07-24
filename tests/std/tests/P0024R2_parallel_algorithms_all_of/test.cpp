// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <execution>
#include <vector>

#include <parallel_algorithms_utilities.hpp>

using namespace std;
using namespace std::execution;

const auto read_char_as_bool = [](char c) { return c != 0; };

void test_case_all_of_parallel(const size_t testSize) {
    vector<char> c(testSize, true);
    assert(all_of(par, c.begin(), c.end(), read_char_as_bool));
    // testing every possible combo takes too long
#ifdef EXHAUSTIVE
    for (char& b : c) {
        b = false;
        assert(!all_of(par, c.begin(), c.end(), read_char_as_bool));
        b = true;
    }
#else // ^^^ EXHAUSTIVE ^^^ // vvv !EXHAUSTIVE vvv
    if (testSize != 0) {
        c[testSize / 2] = false;
        assert(!all_of(par, c.begin(), c.end(), read_char_as_bool));
    }
#endif // EXHAUSTIVE
}

void test_case_any_of_parallel(const size_t testSize) {
    vector<char> c(testSize, false);
    assert(!any_of(par, c.begin(), c.end(), read_char_as_bool));
    // less exhaustive testing here under the assumption similar machinery to
    // all_of is used
    if (testSize != 0) {
        c[testSize / 2] = true;
        assert(any_of(par, c.begin(), c.end(), read_char_as_bool));
    }
}

void test_case_none_of_parallel(const size_t testSize) {
    vector<char> c(testSize, false);
    assert(none_of(par, c.begin(), c.end(), read_char_as_bool));
    // less exhaustive testing here under the assumption similar machinery to
    // all_of is used
    if (testSize != 0) {
        c[testSize / 2] = true;
        assert(!none_of(par, c.begin(), c.end(), read_char_as_bool));
    }
}

int main() {
    parallel_test_case(test_case_all_of_parallel);
    parallel_test_case(test_case_any_of_parallel);
    parallel_test_case(test_case_none_of_parallel);
}
