// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <execution>
#include <list>
#include <random>
#include <vector>

#include <parallel_algorithms_utilities.hpp>

using namespace std;
using namespace std::execution;

const auto cmpTens = [](auto a, auto b) { return a / 10 < b / 10; };

template <template <class...> class Container>
void test_case_stable_sort_parallel_special_cases() {
    Container<int> testData;
    stable_sort(par, testData.begin(), testData.end()); // empty range
    testData.push_back(1);
    stable_sort(par, testData.begin(), testData.end()); // 1 element
    assert(testData == Container<int>({1}));
    testData.push_back(0);
    stable_sort(par, testData.begin(), testData.end());
    assert(testData == Container<int>({0, 1}));
    testData.clear();
    testData.push_back(11);
    testData.push_back(25);
    testData.push_back(10);
    stable_sort(par, testData.begin(), testData.end(), cmpTens);
    assert(testData == Container<int>({11, 10, 25}));
}

vector<size_t> get_test_case_vector(const size_t testSize, mt19937& gen) {
    vector<size_t> c;
    for (size_t idx = 0; idx < testSize; ++idx) {
        c.push_back(idx * 10 + 5);
    }

    shuffle(c.begin(), c.end(), gen);

    // add some duplicate elements that, to be stable, must come after
    uniform_int_distribution<size_t> dist(static_cast<size_t>(0), testSize - 1);
    for (size_t idx = 0; idx < testSize / 2; ++idx) {
        c.push_back(dist(gen) * 10);
    }

    return c;
}

template <class FwdIt>
void assert_stable_sort_cmpTens_test_case(FwdIt first, FwdIt last) {
    size_t highest = 0;
    while (first != last) {
        auto target = *first;
        assert(target > highest);
        highest = target;
        assert(target % 10 == 5);
        target -= 5;
        do {
            ++first;
        } while (first != last && *first == target);
    }
}

void test_case_stable_sort_parallel_list(const size_t testSize, mt19937& gen) {
    list<size_t> c;
    {
        const auto cVec = get_test_case_vector(testSize, gen);
        c.assign(cVec.begin(), cVec.end());
    }

    stable_sort(par, c.begin(), c.end(), cmpTens);
    assert_stable_sort_cmpTens_test_case(c.begin(), c.end());
}

void test_case_stable_sort_parallel_vector(const size_t testSize, mt19937& gen) {
    auto c = get_test_case_vector(testSize, gen);
    stable_sort(par, c.begin(), c.end(), cmpTens);
    assert_stable_sort_cmpTens_test_case(c.begin(), c.end());
}

int main() {
    mt19937 gen(1729);

    test_case_stable_sort_parallel_special_cases<list>();
    test_case_stable_sort_parallel_special_cases<vector>();
    parallel_test_case(test_case_stable_sort_parallel_list, gen);
    parallel_test_case(test_case_stable_sort_parallel_vector, gen);
}
