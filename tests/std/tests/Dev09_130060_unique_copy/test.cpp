// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

string join(const vector<int>& source) {
    string result;
    if (!source.empty()) {
        result.append(to_string(source[0]));
        for (size_t idx = 1; idx < source.size(); ++idx) {
            result.push_back(' ');
            result.append(to_string(source[idx]));
        }
    }

    return result;
}

void test_case_forward_source(const vector<int>& source, const vector<int>& expected) {
    vector<int> actual;
    unique_copy(source.cbegin(), source.cend(), back_inserter(actual));
    assert(actual == expected);
}

void test_case_forward_dest_same_t(const vector<int>& source, const vector<int>& expected) {
    stringstream ss(join(source));
    vector<int> actual(expected.size(), 0);
    assert(unique_copy(istream_iterator<int>(ss), istream_iterator<int>(), actual.begin()) == actual.end());

    assert(actual == expected);
}

void test_case_forward_dest_different_t(const vector<int>& source, const vector<int>& expected) {
    stringstream ss(join(source));
    vector<long> actual(expected.size(), 0);
    assert(unique_copy(istream_iterator<int>(ss), istream_iterator<int>(), actual.begin()) == actual.end());

    assert(equal(actual.cbegin(), actual.cend(), expected.cbegin(), expected.cend()));
}

void test_case_no_forwards(const vector<int>& source, const vector<int>& expected) {
    stringstream ss(join(source));
    vector<int> actual;
    unique_copy(istream_iterator<int>(ss), istream_iterator<int>(), back_inserter(actual));

    assert(actual == expected);
}

void test_case(const vector<int>& source, const vector<int>& expected) {
    test_case_forward_source(source, expected);
    test_case_forward_dest_same_t(source, expected);
    test_case_forward_dest_different_t(source, expected);
    test_case_no_forwards(source, expected);
}

int main() {
    test_case({}, {});
    test_case({1}, {1});
    test_case({1, 1}, {1});
    test_case({1, 2, 2, 3, 4, 4, 5}, {1, 2, 3, 4, 5});
}
