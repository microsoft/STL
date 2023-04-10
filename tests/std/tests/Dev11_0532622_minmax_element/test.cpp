// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// N3485 25.4.7 [alg.min.max]

// /21, min_element "Returns: The first iterator i in the range [first,last) such that for any iterator j
// in the range [first,last) the following corresponding conditions hold: !(*j < *i) or comp(*j, *i) == false.
// Returns last if first == last."

// /23, max_element "Returns: The first iterator i in the range [first,last) such that for any iterator j
// in the range [first,last) the following corresponding conditions hold: !(*i < *j) or comp(*i, *j) == false.
// Returns last if first == last."

// /25, minmax_element "Returns: make_pair(first, first) if [first,last) is empty, otherwise make_pair(m, M),
// where m is the first iterator in [first,last) such that no iterator in the range refers to a smaller element, and
// where M is the last iterator in [first,last) such that no iterator in the range refers to a larger element."

#include <algorithm>
#include <cassert>
#include <iterator>
#include <utility>
#include <vector>

using namespace std;

void test_all_permutations(vector<int>& v) {
    assert(is_sorted(v.begin(), v.end()));

    do {
        const auto first_smallest = find(v.begin(), v.end(), 11);
        const auto first_largest  = find(v.begin(), v.end(), 33);
        const auto last_largest   = find(v.rbegin(), v.rend(), 33).base() - 1;

        assert(min_element(v.begin(), v.end()) == first_smallest);
        assert(max_element(v.begin(), v.end()) == first_largest);
        assert(minmax_element(v.begin(), v.end()) == make_pair(first_smallest, last_largest));

    } while (next_permutation(v.begin(), v.end()));
}

int main() {
    vector<int> v;

    // Test 0 elements.
    assert(min_element(v.begin(), v.end()) == v.begin());
    assert(max_element(v.begin(), v.end()) == v.begin());
    assert(minmax_element(v.begin(), v.end()) == make_pair(v.begin(), v.begin()));

    // Test 1 element.
    v.assign(1, 1729);
    assert(min_element(v.begin(), v.end()) == v.begin());
    assert(max_element(v.begin(), v.end()) == v.begin());
    assert(minmax_element(v.begin(), v.end()) == make_pair(v.begin(), v.begin()));

    // Test 2 elements: 11, 11
    v.assign(2, 11);
    assert(min_element(v.begin(), v.end()) == v.begin());
    assert(max_element(v.begin(), v.end()) == v.begin());
    assert(minmax_element(v.begin(), v.end()) == make_pair(v.begin(), v.begin() + 1));

    // Test 2 elements: 22, 33
    v[0] = 22;
    v[1] = 33;
    assert(min_element(v.begin(), v.end()) == v.begin());
    assert(max_element(v.begin(), v.end()) == v.begin() + 1);
    assert(minmax_element(v.begin(), v.end()) == make_pair(v.begin(), v.begin() + 1));

    // Test 2 elements: 55, 44
    v[0] = 55;
    v[1] = 44;
    assert(min_element(v.begin(), v.end()) == v.begin() + 1);
    assert(max_element(v.begin(), v.end()) == v.begin());
    assert(minmax_element(v.begin(), v.end()) == make_pair(v.begin() + 1, v.begin()));

    // Test 9 elements.
    v.clear();
    v.insert(v.end(), 3, 11);
    v.insert(v.end(), 3, 22);
    v.insert(v.end(), 3, 33);
    test_all_permutations(v);

    // Test 10 elements.
    v.clear();
    v.insert(v.end(), 3, 11);
    v.insert(v.end(), 4, 22);
    v.insert(v.end(), 3, 33);
    test_all_permutations(v);

    { // Original repro.
        const int data[] = {1, 2, 3, 9, 3, 3, 3, 4, 5, 0, 0, 6, 7, 8, 9, 9, 9};

        assert(min_element(begin(data), end(data)) == begin(data) + 9);
        assert(max_element(begin(data), end(data)) == begin(data) + 3);
        assert(minmax_element(begin(data), end(data)) == make_pair(begin(data) + 9, begin(data) + 16));
    }
}
