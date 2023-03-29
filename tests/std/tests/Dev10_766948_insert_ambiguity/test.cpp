// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS

#include <algorithm>
#include <cassert>
#include <hash_map>
#include <hash_set>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

// Dev10-766948 "STL: insert() ambiguity in all associative containers except map and set"
// LWG-2005 "unordered_map::insert(T&&) protection should apply to map too"
// LWG-2354 "Unnecessary copying when inserting into maps with braced-init syntax"

template <typename C>
void test_m() {
    C src;
    src[1] = 10;
    src[2] = 20;
    src[3] = 30;

    C dest;
    dest.insert(src.cbegin(), src.cend());

    auto first = src.cbegin();
    auto last  = src.cend();
    dest.insert(first, last);

    const pair<const int, int> p(4, 40);
    assert(dest.insert(p).first->first == 4);
    assert(dest.insert({5, 50}).first->first == 5);
    assert(dest.insert(make_pair(6, 60)).first->first == 6);
    const pair<const int, int> q(7, 70);
    assert(dest.insert(dest.cbegin(), q)->first == 7);
    assert(dest.insert(dest.cbegin(), {8, 80})->first == 8);
    assert(dest.insert(dest.cbegin(), make_pair(9, 90))->first == 9);
    dest.insert({{11, 111}, {22, 222}, {33, 333}});

    const vector<pair<int, int>> actual(dest.cbegin(), dest.cend());
    const vector<pair<int, int>> correct{{1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}, {6, 60}, {7, 70}, {8, 80},
        {9, 90}, {11, 111}, {22, 222}, {33, 333}};
    assert(is_permutation(actual.begin(), actual.end(), correct.begin(), correct.end()));
}

template <typename C>
void test_s() {
    C src;
    src.insert(1);
    src.insert(2);
    src.insert(3);

    C dest;
    dest.insert(src.cbegin(), src.cend());

    auto first = src.cbegin();
    auto last  = src.cend();
    dest.insert(first, last);

    const int x = 4;
    assert(*dest.insert(x).first == 4);
    assert(*dest.insert(5).first == 5);
    const int y = 6;
    assert(*dest.insert(dest.cbegin(), y) == 6);
    assert(*dest.insert(dest.cbegin(), 7) == 7);
    dest.insert({8, 9, 10});

    const vector<int> correct{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    assert(is_permutation(dest.cbegin(), dest.cend(), correct.begin(), correct.end()));
}

template <typename C>
void test_mm() {
    C src;
    src.insert(make_pair(1, 10));
    src.insert(make_pair(1, -10));
    src.insert(make_pair(2, 20));
    src.insert(make_pair(2, -20));
    src.insert(make_pair(3, 30));
    src.insert(make_pair(3, -30));

    C dest;
    dest.insert(src.cbegin(), src.cend());

    auto first = src.cbegin();
    auto last  = src.cend();
    dest.insert(first, last);

    const pair<const int, int> p(4, 40);
    assert(dest.insert(p)->first == 4);
    assert(dest.insert({5, 50})->first == 5);
    assert(dest.insert(make_pair(6, 60))->first == 6);
    const pair<const int, int> q(7, 70);
    assert(dest.insert(dest.cbegin(), q)->first == 7);
    assert(dest.insert(dest.cbegin(), {8, 80})->first == 8);
    assert(dest.insert(dest.cbegin(), make_pair(9, 90))->first == 9);
    dest.insert({{11, 111}, {22, 222}, {33, 333}});

    const vector<pair<int, int>> actual(dest.cbegin(), dest.cend());
    const vector<pair<int, int>> correct{{1, 10}, {1, 10}, {1, -10}, {1, -10}, {2, 20}, {2, 20}, {2, -20}, {2, -20},
        {3, 30}, {3, 30}, {3, -30}, {3, -30}, {4, 40}, {5, 50}, {6, 60}, {7, 70}, {8, 80}, {9, 90}, {11, 111},
        {22, 222}, {33, 333}};
    assert(is_permutation(actual.begin(), actual.end(), correct.begin(), correct.end()));
}

template <typename C>
void test_ms() {
    C src;
    src.insert(1);
    src.insert(1);
    src.insert(2);
    src.insert(2);
    src.insert(3);
    src.insert(3);

    C dest;
    dest.insert(src.cbegin(), src.cend());

    auto first = src.cbegin();
    auto last  = src.cend();
    dest.insert(first, last);

    const int x = 4;
    assert(*dest.insert(x) == 4);
    assert(*dest.insert(5) == 5);
    const int y = 6;
    assert(*dest.insert(dest.cbegin(), y) == 6);
    assert(*dest.insert(dest.cbegin(), 7) == 7);
    dest.insert({8, 9, 10});

    const vector<int> correct{1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 5, 6, 7, 8, 9, 10};
    assert(is_permutation(dest.cbegin(), dest.cend(), correct.begin(), correct.end()));
}

int main() {
    test_m<map<int, int>>();
    test_s<set<int>>();
    test_mm<multimap<int, int>>();
    test_ms<multiset<int>>();

    test_m<hash_map<int, int>>();
    test_s<hash_set<int>>();
    test_mm<hash_multimap<int, int>>();
    test_ms<hash_multiset<int>>();

    test_m<unordered_map<int, int>>();
    test_s<unordered_set<int>>();
    test_mm<unordered_multimap<int, int>>();
    test_ms<unordered_multiset<int>>();
}
