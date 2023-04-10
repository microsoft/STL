// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <execution>
#include <functional>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

#include <parallel_algorithms_utilities.hpp>

using namespace std;
using namespace std::execution;

template <class T>
void assert_message_vector(const bool b, const char* const msg, const T seedValue) {
    if (!b) {
        cerr << msg << " failed for seed value: " << seedValue << "\n";
        cerr << "This is a randomized test.\n";
        cerr << "DO NOT IGNORE/RERUN THIS FAILURE.\n";
        cerr << "You must report it to the STL maintainers.\n";
        abort();
    }
}

void test_case_set_difference_parallel(const size_t testSize) {
    vector<size_t> longList(testSize, 1UL);
    vector<size_t> shortList(testSize / 2, 1UL);
    vector<size_t> result(testSize);

    const int shortListSize = static_cast<int>(shortList.size());
    const int oddLength     = static_cast<int>(testSize % 2);

    const auto lb = longList.begin();
    const auto le = longList.end();
    const auto sb = shortList.begin();
    const auto se = shortList.end();
    const auto rb = result.begin();

    // === Ranges of duplicates ===
    // First range size > second range size (should be some leftover elements after operation)
    auto compare_result = set_difference(par, lb, le, sb, se, rb);
    assert(equal(rb, compare_result, lb + shortListSize, le));
    compare_result = set_difference(par, lb, le, sb, se, rb, greater());
    assert(equal(rb, compare_result, lb + shortListSize, le));

    if (testSize == 0) {
        return;
    }

    // Second range size > first range size (all elements from first range should be eliminated)
    compare_result = set_difference(par, sb, se, lb, le, rb);
    assert(rb == compare_result);
    compare_result = set_difference(par, sb, se, lb, le, rb, greater());
    assert(rb == compare_result);

    // First range size == second range size (all elements from first range should be eliminated)
    const auto longListMidpoint = lb + shortListSize;
    compare_result              = set_difference(par, lb, longListMidpoint, sb, se, rb);
    assert(rb == compare_result);
    compare_result = set_difference(par, lb, longListMidpoint, sb, se, rb, greater());
    assert(rb == compare_result);

    // === First range and second range are completely disjoint ===
    fill(sb, se, 2UL);
    // First range size > second range size
    compare_result = set_difference(par, lb, le, sb, se, rb);
    assert(equal(rb, compare_result, lb, le));
    compare_result = set_difference(par, lb, le, sb, se, rb, greater());
    assert(equal(rb, compare_result, lb, le));

    // Second range size > first range size
    compare_result = set_difference(par, sb, se, lb, le, rb);
    assert(equal(rb, compare_result, sb, se));
    compare_result = set_difference(par, sb, se, lb, le, rb, greater());
    assert(equal(rb, compare_result, sb, se));

    if (testSize == 1) {
        return;
    }

    // === Increasing lists ===
    // Increasing lists starting at 1, with shortList a subset of longList
    iota(lb, le, 1UL);
    iota(sb, se, 1UL);
    compare_result = set_difference(par, lb, le, sb, se, rb);
    assert(equal(rb, compare_result, longListMidpoint, le));
    compare_result = set_difference(par, sb, se, lb, le, rb);
    assert(rb == compare_result);
    compare_result = set_difference(par, lb, longListMidpoint, sb, se, rb);
    assert(rb == compare_result);

    // === Overlapping but not equal lists, no direct containment ===
    auto remainingAttempts    = quadratic_complexity_case_limit;
    auto overlappingListBegin = lb;
    auto overlappingListEnd   = overlappingListBegin + shortListSize;
    for (auto overlappingPoint = sb; overlappingPoint < se; ++overlappingPoint) {
        compare_result = set_difference(par, sb, se, overlappingListBegin, overlappingListEnd, rb);
        assert(equal(rb, compare_result, sb, overlappingPoint));
        ++overlappingListBegin;
        ++overlappingListEnd;
        if (--remainingAttempts == 0) {
            break;
        }
    }

    // shortList is a subset of longList, containing every other element of longList
    size_t curr = 2;
    for (auto& elem : shortList) {
        elem = curr;
        curr += 2;
    }
    vector<size_t> expected(longList.size() - shortList.size());
    curr = 1;
    for (auto& elem : expected) {
        elem = curr;
        curr += 2;
    }
    compare_result = set_difference(par, lb, le, sb, se, rb);
    assert(equal(rb, compare_result, expected.begin(), expected.end()));
    compare_result = set_difference(par, sb, se, lb, le, rb);
    assert(rb == compare_result);

    // === Decreasing lists ===
    // Decreasing lists starting at the size of list, with shortList a subset of longList
    curr = shortList.size();
    for (auto& elem : shortList) {
        elem = curr--;
    }
    curr = longList.size();
    for (auto& elem : longList) {
        elem = curr--;
    }

    compare_result = set_difference(par, lb, le, sb, se, rb, greater());
    assert(equal(rb, compare_result, lb, lb + shortListSize + oddLength));
    compare_result = set_difference(par, sb, se, lb, le, rb, greater());
    assert(rb == compare_result);
    compare_result = set_difference(par, longListMidpoint + oddLength, le, sb, se, rb, greater());
    assert(rb == compare_result);

    // === Overlapping but not equal lists, no direct containment ===
    remainingAttempts    = quadratic_complexity_case_limit;
    overlappingListBegin = lb + oddLength;
    overlappingListEnd   = overlappingListBegin + shortListSize;
    for (int overlappingPoint = 0; overlappingPoint < shortListSize; ++overlappingPoint) {
        compare_result = set_difference(par, sb, se, overlappingListBegin, overlappingListEnd, rb, greater());
        assert(equal(rb, compare_result, sb + overlappingPoint, se));
        ++overlappingListBegin;
        ++overlappingListEnd;
        if (--remainingAttempts == 0) {
            break;
        }
    }

    // test randomized input ranges
    const auto seedValue = random_device{}();
    mt19937 gen(seedValue);
    uniform_int_distribution<> dis(0, static_cast<int>(testSize));

    vector<int> list1(testSize);
    vector<int> list2(testSize);
    vector<int> seqRes(testSize);
    vector<int> parRes(testSize);

    for (auto& elem : list1) {
        elem = dis(gen);
    }
    for (auto& elem : list2) {
        elem = dis(gen);
    }

    sort(list1.begin(), list1.end());
    sort(list2.begin(), list2.end());
    auto seqComp = set_difference(list1.begin(), list1.end(), list2.begin(), list2.end(), seqRes.begin());
    auto parComp = set_difference(par, list1.begin(), list1.end(), list2.begin(), list2.end(), parRes.begin());
    assert_message_vector(
        equal(seqRes.begin(), seqComp, parRes.begin(), parComp), "Randomized vectors, set difference 1", seedValue);

    seqComp = set_difference(list2.begin(), list2.end(), list1.begin(), list1.end(), seqRes.begin());
    parComp = set_difference(par, list2.begin(), list2.end(), list1.begin(), list1.end(), parRes.begin());
    assert_message_vector(
        equal(seqRes.begin(), seqComp, parRes.begin(), parComp), "Randomized vectors, set difference 2", seedValue);
}

int main() {
    parallel_test_case(test_case_set_difference_parallel);
}
