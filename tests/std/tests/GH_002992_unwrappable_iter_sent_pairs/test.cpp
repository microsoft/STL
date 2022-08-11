// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <xutility>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

using namespace std;

template <class _Rng>
void test_range() {
    STATIC_ASSERT(same_as<ranges::_Unwrapped_iterator_t<_Rng>, _Unwrapped_t<iterator_t<_Rng>>>);
    STATIC_ASSERT(same_as<ranges::_Unwrapped_sentinel_t<_Rng>, _Unwrapped_t<sentinel_t<_Rng>>>);
}

int main() {
    test_range<string>();
    test_range<wstring>();

    test_range<array<int, 32>>();
    test_range<forward_list<int>>();
    test_range<list<int>>();
    test_range<set<int>>();
    test_range<map<int, int>>();
    test_range<unordered_set<int>>();
    test_range<unordered_map<int, int>>();
    test_range<vector<int>>();
}
