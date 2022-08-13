// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <execution>
#include <functional>
#include <numeric>
#include <random>
#include <vector>

#include <parallel_algorithms_utilities.hpp>

using namespace std;
using namespace std::execution;

void test_case_random(const size_t testSize, mt19937& gen) {
    vector<unsigned int> s(testSize);
    generate(s.begin(), s.end(), ref(gen));
    vector<unsigned int> expected(testSize);
    adjacent_difference(s.begin(), s.end(), expected.begin()); // prepare expected values with serial algorithm
    {
        // also test seq because algorithm is different
        vector<unsigned int> actual(testSize);
        assert(adjacent_difference(seq, s.begin(), s.end(), actual.begin()) == actual.end());
        assert(expected == actual);
    }
    {
        vector<unsigned int> actual(testSize);
        assert(adjacent_difference(par, s.begin(), s.end(), actual.begin()) == actual.end());
        assert(expected == actual);
    }
}

void test_case_iota(const size_t testSize) {
    vector<unsigned int> s(testSize);
    iota(s.begin(), s.end(), 1U); // note start at 1 so that all the actual values are 1
    {
        vector<unsigned int> actual(testSize);
        assert(adjacent_difference(seq, s.begin(), s.end(), actual.begin()) == actual.end());
        assert(count(actual.begin(), actual.end(), 1U) == static_cast<ptrdiff_t>(testSize));
    }

    {
        vector<unsigned int> actual(testSize);
        assert(adjacent_difference(par, s.begin(), s.end(), actual.begin()) == actual.end());
        assert(count(actual.begin(), actual.end(), 1U) == static_cast<ptrdiff_t>(testSize));
    }
}

void test_case_plus(const size_t testSize) {
    vector<unsigned int> s(testSize);
    iota(s.begin(), s.end(), 0U);
    vector<unsigned int> expected(testSize);
    for (unsigned int result = 1; result < testSize; ++result) {
        expected[result] = result * 2 - 1;
    }

    {
        vector<unsigned int> actual(testSize);
        assert(adjacent_difference(seq, s.begin(), s.end(), actual.begin(), plus<>{}) == actual.end());
        assert(expected == actual);
    }

    {
        vector<unsigned int> actual(testSize);
        assert(adjacent_difference(par, s.begin(), s.end(), actual.begin(), plus<>{}) == actual.end());
        assert(expected == actual);
    }
}

int main() {
    mt19937 gen(1729);

    parallel_test_case(test_case_random, gen);
    parallel_test_case(test_case_iota);
    parallel_test_case(test_case_plus);
}
