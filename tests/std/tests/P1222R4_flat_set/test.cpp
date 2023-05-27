// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <deque>
#include <flat_set>
#include <vector>

using namespace std;

int main() {
    flat_set<int> s{1, 2, 2, 2, 3};
    assert(s.size() == 3);
    s.insert(43);
    assert(s.size() == 4);

    int myInts[] = {1, 2, 3, 4, 55};
    s.insert_range(myInts);

    flat_multiset<int, less<int>, deque<int>> d;
}
