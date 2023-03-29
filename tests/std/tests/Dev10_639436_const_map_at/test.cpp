// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS

#include <cassert>
#include <cstdlib>
#include <hash_map>
#include <map>
#include <unordered_map>

using namespace std;

template <typename T>
void add_map_elements(T& m) {
    m.emplace(5, 25);
    m.emplace(6, 36);
    m.emplace_hint(m.begin(), 7, 49);
}

template <typename T>
void test_non_const_at() {
    T m;

    add_map_elements(m);

    const T c = m;

    assert(m.at(6) == 36);
    assert(c.at(6) == 36);
}

int main() {
    test_non_const_at<map<int, int>>();
    test_non_const_at<hash_map<int, int>>();
    test_non_const_at<unordered_map<int, int>>();
}
