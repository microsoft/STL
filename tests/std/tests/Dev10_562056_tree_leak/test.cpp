// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <utility>
#include <vector>

using namespace std;

int g_kittens = 0;

class Kitten {
public:
    Kitten() : m_n(0) {
        ++g_kittens;
    }

    explicit Kitten(const int n) : m_n(n) {
        ++g_kittens;
    }

    Kitten(const Kitten& other) : m_n(other.m_n) {
        ++g_kittens;
    }

    Kitten& operator=(const Kitten& other) {
        m_n = other.m_n;

        return *this;
    }

    ~Kitten() {
        --g_kittens;
    }

    bool operator<(const Kitten& other) const {
        return m_n < other.m_n;
    }

private:
    int m_n;
};

int main() {
    assert(g_kittens == 0);

    {
        vector<Kitten> v(3);

        assert(g_kittens == 3);
    }

    assert(g_kittens == 0);

    {
        deque<Kitten> d(3);

        assert(g_kittens == 3);
    }

    assert(g_kittens == 0);

    {
        list<Kitten> l(3);

        assert(g_kittens == 3);
    }

    assert(g_kittens == 0);

    {
        set<Kitten> s;

        s.insert(Kitten(11));
        s.insert(Kitten(22));
        s.insert(Kitten(33));

        assert(g_kittens == 3);
    }

    assert(g_kittens == 0);

    {
        multiset<Kitten> ms;

        ms.insert(Kitten(11));
        ms.insert(Kitten(22));
        ms.insert(Kitten(33));

        assert(g_kittens == 3);
    }

    assert(g_kittens == 0);

    {
        map<int, Kitten> m;

        m[1] = Kitten(11);
        m[2] = Kitten(22);
        m[3] = Kitten(33);

        assert(g_kittens == 3);
    }

    assert(g_kittens == 0);

    {
        multimap<int, Kitten> mm;

        mm.insert(make_pair(1, Kitten(11)));
        mm.insert(make_pair(2, Kitten(22)));
        mm.insert(make_pair(3, Kitten(33)));

        assert(g_kittens == 3);
    }

    assert(g_kittens == 0);
}
