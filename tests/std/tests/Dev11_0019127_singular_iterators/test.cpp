// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// C++03 24.1 [lib.iterator.requirements]/5:
// "Results of most expressions are undefined for singular values; the only
// exception is an assignment of a non-singular value to an iterator that holds
// a singular value."

// C++0x N3126 24.2.1 [iterator.requirements.general]/5:
// "Results of most expressions are undefined for singular values; the only
// exceptions are destroying an iterator that holds a singular value, the
// assignment of a non-singular value to an iterator that holds a singular value,
// and, for iterators that satisfy the DefaultConstructible requirements, using
// a value-initialized iterator as the source of a copy or move operation."

#include <array>
#include <cassert>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

template <typename C>
void test_sequence() {
    C c;
    c.push_back(11);
    c.push_back(22);
    c.push_back(33);

    typename C::iterator i1 = c.begin(), i2 = c.begin(), i3 = c.begin(), i4 = c.begin();
    typename C::iterator x1, x2, x3, x4, x5, x6;

    x1 = i1;
    assert(*x1 == 11);

    x2 = move(i2);
    assert(*x2 == 11);

    i3 = x3;

    i4 = move(x4);

    auto x7(x5);
    auto x8(move(x6));

    (void) x7;
    (void) x8;
}

template <typename M>
void test_map() {
    M m;
    m.insert(make_pair(11, 123));
    m.insert(make_pair(22, 456));
    m.insert(make_pair(33, 789));

    typename M::iterator i1 = m.find(22), i2 = m.find(22), i3 = m.find(22), i4 = m.find(22);
    typename M::iterator x1, x2, x3, x4, x5, x6;

    x1 = i1;
    assert(x1->second == 456);

    x2 = move(i2);
    assert(x2->second == 456);

    i3 = x3;

    i4 = move(x4);

    auto x7(x5);
    auto x8(move(x6));

    (void) x7;
    (void) x8;
}

template <typename S>
void test_set() {
    S s;
    s.insert(11);
    s.insert(22);
    s.insert(33);

    typename S::iterator i1 = s.find(22), i2 = s.find(22), i3 = s.find(22), i4 = s.find(22);
    typename S::iterator x1, x2, x3, x4, x5, x6;

    x1 = i1;
    assert(*x1 == 22);

    x2 = move(i2);
    assert(*x2 == 22);

    i3 = x3;

    i4 = move(x4);

    auto x7(x5);
    auto x8(move(x6));

    (void) x7;
    (void) x8;
}

int main() {
    test_sequence<vector<int>>();
    test_sequence<deque<int>>();
    test_sequence<list<int>>();

    test_map<map<int, int>>();
    test_map<multimap<int, int>>();
    test_set<set<int>>();
    test_set<multiset<int>>();

    test_map<unordered_map<int, int>>();
    test_map<unordered_multimap<int, int>>();
    test_set<unordered_set<int>>();
    test_set<unordered_multiset<int>>();

    {
        forward_list<int> f;
        f.push_front(33);
        f.push_front(22);
        f.push_front(11);

        forward_list<int>::iterator i1 = f.begin(), i2 = f.begin(), i3 = f.begin(), i4 = f.begin();
        forward_list<int>::iterator x1, x2, x3, x4, x5, x6;

        x1 = i1;
        assert(*x1 == 11);

        x2 = move(i2);
        assert(*x2 == 11);

        i3 = x3;

        i4 = move(x4);

        auto x7(x5);
        auto x8(move(x6));

        (void) x7;
        (void) x8;
    }

    {
        array<int, 3> a = {{11, 22, 33}};

        array<int, 3>::iterator i1 = a.begin(), i2 = a.begin(), i3 = a.begin(), i4 = a.begin();
        array<int, 3>::iterator x1, x2, x3, x4, x5, x6;

        x1 = i1;
        assert(*x1 == 11);

        x2 = move(i2);
        assert(*x2 == 11);

        i3 = x3;

        i4 = move(x4);

        auto x7(x5);
        auto x8(move(x6));

        (void) x7;
        (void) x8;
    }

    {
        string s("meow");

        string::iterator i1 = s.begin(), i2 = s.begin(), i3 = s.begin(), i4 = s.begin();
        string::iterator x1, x2, x3, x4, x5, x6;

        x1 = i1;
        assert(*x1 == 'm');

        x2 = move(i2);
        assert(*x2 == 'm');

        i3 = x3;

        i4 = move(x4);

        auto x7(x5);
        auto x8(move(x6));

        (void) x7;
        (void) x8;
    }

    {
        const string s("cute fluffy kittens");
        const regex r("\\w+");

        sregex_iterator i1(s.begin(), s.end(), r);
        sregex_iterator i2(i1), i3(i1), i4(i1);
        sregex_iterator x1, x2, x3, x4, x5, x6;

        x1 = i1;
        assert((*x1)[0] == "cute");

        x2 = move(i2);
        assert((*x2)[0] == "cute");

        i3 = x3;

        i4 = move(x4);

        auto x7(x5);
        auto x8(move(x6));

        (void) x7;
        (void) x8;
    }

    {
        const string s("cute fluffy kittens");
        const regex r("\\w+");

        sregex_token_iterator i1(s.begin(), s.end(), r);
        sregex_token_iterator i2(i1), i3(i1), i4(i1);
        sregex_token_iterator x1, x2, x3, x4, x5, x6;

        x1 = i1;
        assert(*x1 == "cute");

        x2 = move(i2);
        assert(*x2 == "cute");

        i3 = x3;

        i4 = move(x4);

        auto x7(x5);
        auto x8(move(x6));

        (void) x7;
        (void) x8;
    }

    {
        vector<int> v;
        v.push_back(11);
        v.push_back(22);
        v.push_back(33);

        vector<int>::reverse_iterator i1 = v.rbegin(), i2 = v.rbegin(), i3 = v.rbegin(), i4 = v.rbegin();
        vector<int>::reverse_iterator x1, x2, x3, x4, x5, x6;

        x1 = i1;
        assert(*x1 == 33);

        x2 = move(i2);
        assert(*x2 == 33);

        i3 = x3;

        i4 = move(x4);

        auto x7(x5);
        auto x8(move(x6));

        (void) x7;
        (void) x8;
    }
}
