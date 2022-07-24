// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <cstdlib>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

template <typename Container>
void test() {
    Container src;
    assert(src.begin() == src.end());

    Container dest = move(src);
    assert(src.begin() == src.end());
    assert(dest.begin() == dest.end());
}

int main() {
    test<vector<int>>();
    test<deque<int>>();
    test<list<int>>();

    test<set<int>>();
    test<multiset<int>>();
    test<map<int, int>>();
    test<multimap<int, int>>();

    test<string>();

    test<vector<bool>>();

    test<array<int, 0>>();

    test<unordered_set<int>>();
    test<unordered_multiset<int>>();
    test<unordered_map<int, int>>();
    test<unordered_multimap<int, int>>();

    test<forward_list<int>>();
}
