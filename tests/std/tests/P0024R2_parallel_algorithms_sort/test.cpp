// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <execution>
#include <numeric>
#include <random>
#include <vector>

#include <parallel_algorithms_utilities.hpp>

using namespace std;
using namespace std::execution;

void test_case_sort_parallel_special_cases() {
    vector<int> testData;
    sort(par, testData.begin(), testData.end()); // empty range
    testData.push_back(1);
    sort(par, testData.begin(), testData.end()); // 1 element
    assert(testData == vector<int>({1}));
    testData.push_back(0);
    sort(par, testData.begin(), testData.end());
    assert(testData == vector<int>({0, 1}));
}

void test_case_sort_parallel(const size_t testSize, mt19937& gen) {
    vector<size_t> c(testSize + testSize / 2);
    uniform_int_distribution<size_t> dist(static_cast<size_t>(0), testSize);
    const auto seqEnd = c.begin() + static_cast<ptrdiff_t>(testSize);
    iota(c.begin(), seqEnd, static_cast<size_t>(0));
    generate(seqEnd, c.end(), [&] { return dist(gen); }); // add some duplicate elements
    shuffle(c.begin(), c.end(), gen);
    sort(par, c.begin(), c.end());
    assert(is_sorted(c.begin(), c.end()));
}

int main() {
    mt19937 gen(1729);

    test_case_sort_parallel_special_cases();
    parallel_test_case(test_case_sort_parallel, gen);
}
