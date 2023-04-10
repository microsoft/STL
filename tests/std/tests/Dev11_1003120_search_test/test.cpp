// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <forward_list>
#include <iterator>
#include <list>
#include <vector>

using namespace std;

template <typename C1, typename C2>
void test(const char* s1, const char* s2, int expected) {
    C1 c1(s1, s1 + strlen(s1));
    C2 c2(s2, s2 + strlen(s2));

    auto iter = search(c1.begin(), c1.end(), c2.begin(), c2.end(), [](char ch1, char ch2) {
        // This is almost redundant but it will help ensure we don't dereference an invalid iterator:
        assert(ch1 >= 'a' && ch1 <= 'f');
        assert(ch2 >= 'a' && ch2 <= 'f');
        return ch1 == ch2;
    });

    if (iter == c1.end()) {
        assert(-1 == expected);
    } else {
        assert(distance(c1.begin(), iter) == expected);
    }
}

template <typename C1, typename C2>
void test_type() {
    // s1 is longer than s2
    // positive tests
    test<C1, C2>("abc", "ab", 0);
    test<C1, C2>("cabc", "ab", 1);
    test<C1, C2>("cab", "ab", 1);
    test<C1, C2>("abc", "c", 2);
    test<C1, C2>("ccab", "abc", -1);
    test<C1, C2>("a", "", 0);
    // negative tests
    test<C1, C2>("ab", "c", -1);
    test<C1, C2>("abc", "de", -1);
    test<C1, C2>("abc", "ad", -1);
    test<C1, C2>("abcd", "bce", -1);
    test<C1, C2>("abcd", "cde", -1);

    // s1 is the same length as s2
    // positive tests
    test<C1, C2>("ab", "ab", 0);
    test<C1, C2>("a", "a", 0);
    // negative tests
    test<C1, C2>("", "", -1);
    test<C1, C2>("a", "b", -1);
    test<C1, C2>("ab", "cd", -1);
    test<C1, C2>("ab", "ac", -1);

    // s1 is shorter than s2
    // positive tests - none
    // negative tests
    test<C1, C2>("ab", "cde", -1);
    test<C1, C2>("a", "ab", -1);
    test<C1, C2>("ab", "abc", -1);
    test<C1, C2>("", "a", -1);
}

int main() {

    test_type<vector<char>, vector<char>>();
    test_type<vector<char>, list<char>>();
    test_type<vector<char>, forward_list<char>>();

    test_type<list<char>, vector<char>>();
    test_type<list<char>, list<char>>();
    test_type<list<char>, forward_list<char>>();

    test_type<forward_list<char>, vector<char>>();
    test_type<forward_list<char>, list<char>>();
    test_type<forward_list<char>, forward_list<char>>();
}
