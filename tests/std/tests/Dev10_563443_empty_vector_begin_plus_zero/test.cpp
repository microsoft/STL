// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <cstdlib>
#include <deque>
#include <string>
#include <vector>

using namespace std;

template <typename C>
void test() {
    C c;

    const typename C::const_iterator i = c.begin();
    const typename C::const_iterator j = c.begin() + 0;
    const typename C::const_iterator k = c.end();
    const typename C::const_iterator l = c.end() + 0;

    assert(i == j);
    assert(i == k);
    assert(i == l);
}

int main() {
    test<vector<int>>();
    test<deque<int>>();
    test<string>();
    test<array<int, 0>>();
    test<vector<bool>>();
}
