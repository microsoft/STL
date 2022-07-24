// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma warning(disable : 4242 4244 4365) // test_case_incorrect_special_case_reasoning tests narrowing on purpose
#include <algorithm>
#include <cassert>
#include <execution>
#include <iterator>
#include <memory>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

#include <parallel_algorithms_utilities.hpp>

using namespace std;
using namespace std::execution;

void test_case_reduce(const size_t testSize, mt19937& gen) {
    vector<unsigned int> c(testSize);
    const auto b = c.begin();
    const auto e = c.end();
    generate(b, e, [&] { return gen(); });
    auto correct = accumulate(b, e, 0U);
    assert(correct == reduce(b, e));
    assert(correct == reduce(seq, b, e));
    assert(correct == reduce(par, b, e));

    correct += 42U;

    assert(correct == reduce(b, e, 42U));
    assert(correct == reduce(seq, b, e, 42U));
    assert(correct == reduce(par, b, e, 42U));

    auto add_a_different_way = [](unsigned int a, unsigned int b) { return a + b; };
    assert(correct == reduce(b, e, 42U, add_a_different_way));
    assert(correct == reduce(seq, b, e, 42U, add_a_different_way));
    assert(correct == reduce(par, b, e, 42U, add_a_different_way));
}

vector<unique_ptr<vector<unsigned int>>> get_move_only_test_data(const size_t testSize) {
    vector<unique_ptr<vector<unsigned int>>> testData;
    testData.reserve(testSize);
    for (size_t idx = 0; idx < testSize; ++idx) {
        testData.emplace_back(make_unique<vector<unsigned int>>(1, static_cast<unsigned int>(idx)));
    }

    return testData;
}

template <class ExPo>
void test_case_move_only(ExPo&& exec, const size_t testSize) {
    // one could argue that this mutates the input which is presently disallowed by the parallel
    // algorithms, but the standard is unclear here and if this isn't allowed, the standard
    // is bad and should feel bad
    auto testData                           = get_move_only_test_data(testSize);
    unique_ptr<vector<unsigned int>> result = reduce(forward<ExPo>(exec), make_move_iterator(testData.begin()),
        make_move_iterator(testData.end()), make_unique<vector<unsigned int>>(),
        [](unique_ptr<vector<unsigned int>> lhs, unique_ptr<vector<unsigned int>> rhs) {
            lhs->insert(lhs->end(), rhs->begin(), rhs->end());
            return lhs;
        });

    sort(result->begin(), result->end());
    for (size_t idx = 0; idx < testSize; ++idx) {
        assert((*result)[idx] == idx);
    }
}

void test_case_incorrect_special_case_reasoning() {
    unsigned char a[] = {128, 128};
    // 128 + 128 mod 256 == 0, but Usual Arithmetic Conversions would say 256
    assert(reduce(begin(a), end(a), 0U, plus<unsigned char>{}) == 0);
}

int main() {
    mt19937 gen(1729);
    parallel_test_case(test_case_reduce, gen);
    parallel_test_case([](const size_t testSize) { test_case_move_only(seq, testSize); });
    parallel_test_case([](const size_t testSize) { test_case_move_only(par, testSize); });
    test_case_incorrect_special_case_reasoning();
}
