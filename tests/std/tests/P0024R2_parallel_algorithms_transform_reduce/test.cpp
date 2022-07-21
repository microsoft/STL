// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma warning(disable : 4242 4244 4365) // test_case_incorrect_special_case_reasoning tests narrowing on purpose
#include <algorithm>
#include <cassert>
#include <execution>
#include <functional>
#include <iterator>
#include <memory>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

#include <parallel_algorithms_utilities.hpp>

using namespace std;
using namespace std::execution;

const auto add_a_different_way      = [](unsigned int a, unsigned int b) { return a + b; };
const auto multiply_a_different_way = [](unsigned int a, unsigned int b) { return a * b; };

void test_case_transform_reduce_binary(const size_t testSize, mt19937& gen) {
    vector<unsigned int> c(testSize * 2);
    const auto inputBegin   = c.begin();
    const auto inputEnd     = c.begin() + static_cast<ptrdiff_t>(testSize);
    const auto resultsBegin = inputEnd;
    generate(inputBegin, inputEnd, [&] { return gen(); });
    auto correct = inner_product(inputBegin, inputEnd, resultsBegin, 0U);
    assert(correct == transform_reduce(inputBegin, inputEnd, resultsBegin, 0U));
    assert(correct == transform_reduce(seq, inputBegin, inputEnd, resultsBegin, 0U));
    assert(correct == transform_reduce(par, inputBegin, inputEnd, resultsBegin, 0U));

    correct += 42U;

    assert(correct == transform_reduce(inputBegin, inputEnd, resultsBegin, 42U));
    assert(correct == transform_reduce(seq, inputBegin, inputEnd, resultsBegin, 42U));
    assert(correct == transform_reduce(par, inputBegin, inputEnd, resultsBegin, 42U));

    assert(correct
           == transform_reduce(inputBegin, inputEnd, resultsBegin, 42U, add_a_different_way, multiply_a_different_way));
    assert(correct
           == transform_reduce(
               seq, inputBegin, inputEnd, resultsBegin, 42U, add_a_different_way, multiply_a_different_way));
    assert(correct
           == transform_reduce(
               par, inputBegin, inputEnd, resultsBegin, 42U, add_a_different_way, multiply_a_different_way));
}

const auto times_ten = [](unsigned int a) { return a * 10; };

void test_case_transform_reduce(const size_t testSize, mt19937& gen) {
    vector<unsigned int> c(testSize);
    const auto b = c.begin();
    const auto e = c.end();
    generate(b, e, [&] { return gen(); });
    auto correct =
        accumulate(b, e, 0U, [](unsigned int accumulator, unsigned int elem) { return accumulator + elem * 10; });

    assert(correct == transform_reduce(b, e, 0U, plus<>{}, times_ten));
    assert(correct == transform_reduce(seq, b, e, 0U, plus<>{}, times_ten));
    assert(correct == transform_reduce(par, b, e, 0U, plus<>{}, times_ten));

    correct += 42U;

    assert(correct == transform_reduce(b, e, 42U, plus<>{}, times_ten));
    assert(correct == transform_reduce(seq, b, e, 42U, plus<>{}, times_ten));
    assert(correct == transform_reduce(par, b, e, 42U, plus<>{}, times_ten));
}

vector<unique_ptr<vector<unsigned int>>> get_move_only_test_data(const size_t testSize) {
    vector<unique_ptr<vector<unsigned int>>> testData;
    testData.reserve(testSize);
    for (size_t idx = 0; idx < testSize; ++idx) {
        testData.emplace_back(make_unique<vector<unsigned int>>(1, static_cast<unsigned int>(idx)));
    }

    return testData;
}

// one could argue that the move-only examples mutate the input which is presently disallowed by the
// parallel algorithms, but the standard is unclear here and if this isn't allowed, the standard
// is bad and should feel bad

template <class ExPo>
void test_case_move_only_binary(ExPo&& exec, const size_t testSize) {
    auto testData1                          = get_move_only_test_data(testSize);
    auto testData2                          = get_move_only_test_data(testSize);
    unique_ptr<vector<unsigned int>> result = transform_reduce(
        forward<ExPo>(exec), make_move_iterator(testData1.begin()), make_move_iterator(testData1.end()),
        make_move_iterator(testData2.begin()), make_unique<vector<unsigned int>>(),
        [](unique_ptr<vector<unsigned int>> lhs, unique_ptr<vector<unsigned int>> rhs) {
            lhs->insert(lhs->end(), rhs->begin(), rhs->end());
            return lhs;
        },
        [](unique_ptr<vector<unsigned int>> lhs, unique_ptr<vector<unsigned int>> rhs) {
            assert(lhs->size() == 1); // should only be called directly on the input sequence
            assert(rhs->size() == 1);
            unsigned int lhsInt = (*lhs)[0];
            unsigned int rhsInt = (*rhs)[0];
            assert(lhsInt == rhsInt);
            (*lhs)[0] = lhsInt * rhsInt;
            return lhs;
        });

    sort(result->begin(), result->end());
    for (size_t idx = 0; idx < testSize; ++idx) {
        assert((*result)[idx] == idx * idx);
    }
}

template <class ExPo>
void test_case_move_only(ExPo&& exec, const size_t testSize) {
    auto testData                           = get_move_only_test_data(testSize);
    unique_ptr<vector<unsigned int>> result = transform_reduce(
        forward<ExPo>(exec), make_move_iterator(testData.begin()), make_move_iterator(testData.end()),
        make_unique<vector<unsigned int>>(),
        [](unique_ptr<vector<unsigned int>> lhs, unique_ptr<vector<unsigned int>> rhs) {
            lhs->insert(lhs->end(), rhs->begin(), rhs->end());
            return lhs;
        },
        [](unique_ptr<vector<unsigned int>> target) {
            assert(target->size() == 1); // should only be called directly on the input sequence
            target->back() *= 10;
            return target;
        });

    sort(result->begin(), result->end());
    for (size_t idx = 0; idx < testSize; ++idx) {
        assert((*result)[idx] == idx * 10);
    }
}

void test_case_incorrect_special_case_reasoning() {
    unsigned char a[] = {64, 1};
    unsigned char b[] = {64, 1};
    // (64 + 64) * (1 + 1) mod 256 == 0, Usual Arithmetic Conversions would say 256
    assert(transform_reduce(begin(a), end(a), b, 0, multiplies<unsigned char>{}, plus<>{}) == 0);

    unsigned char c[] = {128, 10};
    unsigned char d[] = {128, 10};
    // (128 + 128 mod 256 == 0) * (10 + 10 mod 256) == 0, Usual Arithmetic Conversions would say 5120
    assert(transform_reduce(begin(c), end(c), d, 0, multiplies<>{}, plus<unsigned char>{}) == 0);
}

int main() {
    mt19937 gen(1729);
    parallel_test_case(test_case_transform_reduce_binary, gen);
    parallel_test_case(test_case_transform_reduce, gen);
    parallel_test_case([](const size_t testSize) { test_case_move_only_binary(seq, testSize); });
    parallel_test_case([](const size_t testSize) { test_case_move_only_binary(par, testSize); });
    parallel_test_case([](const size_t testSize) { test_case_move_only(seq, testSize); });
    parallel_test_case([](const size_t testSize) { test_case_move_only(par, testSize); });
    test_case_incorrect_special_case_reasoning();
}
