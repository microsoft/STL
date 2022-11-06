// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <execution>
#include <random>
#include <vector>

#include <parallel_algorithms_utilities.hpp>

using namespace std;
using namespace std::execution;

void test_case_replace_parallel(const size_t testSize, mt19937& gen) {
    vector<char> actual(testSize);
    uniform_int_distribution<int> dist('a', 'z');
    generate(actual.begin(), actual.end(), [&]() { return static_cast<char>(dist(gen)); });

    vector<char> expected(actual);
    replace(expected.begin(), expected.end(), 'a', 'b');
    replace(par, actual.begin(), actual.end(), 'a', 'b');
    assert(expected == actual);
}

void test_case_replace_if_parallel(const size_t testSize, mt19937& gen) {
    vector<char> actual(testSize);
    uniform_int_distribution<int> dist('a', 'z');
    generate(actual.begin(), actual.end(), [&]() { return static_cast<char>(dist(gen)); });

    auto pred = [](char c) { return c == 'a'; };

    vector<char> expected(actual);
    replace_if(expected.begin(), expected.end(), pred, 'b');
    replace_if(par, actual.begin(), actual.end(), pred, 'b');
    assert(expected == actual);
}

int main() {
#ifndef _M_CEE // TRANSITION, VSO-1659695
    mt19937 gen(1729);

    parallel_test_case(test_case_replace_parallel, gen);
    parallel_test_case(test_case_replace_if_parallel, gen);
#endif // _M_CEE
}
