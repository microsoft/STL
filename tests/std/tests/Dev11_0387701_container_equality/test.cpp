// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <deque>
#include <forward_list>
#include <functional>
#include <list>
#include <map>
#include <random>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

struct Meow {
    int m_x;

    Meow() : m_x(0) {}

    bool operator==(const Meow& other) const {
        return m_x == other.m_x;
    }

    bool operator<(const Meow& other) const {
        return m_x < other.m_x;
    }
};

template <>
struct std::hash<Meow> {
    typedef size_t result_type;
    typedef Meow argument_type;

    size_t operator()(const Meow& m) const {
        return hash<int>()(m.m_x);
    }
};

template <typename C>
void assert_equal(const C& c1, const C& c2) {
    assert(c1 == c2);
    assert(!(c1 != c2));
}

template <typename C>
void assert_different(const C& c1, const C& c2) {
    assert(!(c1 == c2));
    assert(c1 != c2);
}

template <typename C>
void test() {
    assert_equal(C(), C());
}

int main() {
    test<array<Meow, 1>>();
    test<deque<Meow>>();
    test<forward_list<Meow>>();
    test<list<Meow>>();
    test<vector<Meow>>();
    test<map<Meow, Meow>>();
    test<multimap<Meow, Meow>>();
    test<set<Meow>>();
    test<multiset<Meow>>();
    test<unordered_map<Meow, Meow>>();
    test<unordered_multimap<Meow, Meow>>();
    test<unordered_set<Meow>>();
    test<unordered_multiset<Meow>>();

    mt19937 prng(1729);
    uniform_int_distribution<unsigned int> dist100(0, 99);
    uniform_int_distribution<unsigned int> dist500(10000, 10499);

    {
        set<unsigned int> s;

        while (s.size() < 1000) {
            s.insert(prng());
        }

        vector<unsigned int> v(s.begin(), s.end());

        shuffle(v.begin(), v.end(), prng);
        unordered_set<unsigned int> us1(v.begin(), v.end());

        // same sizes, same elements, same orders => should be equal
        assert_equal(us1, us1);

        shuffle(v.begin(), v.end(), prng);
        unordered_set<unsigned int> us2(v.begin(), v.end());

        // same sizes, same elements, different orders => should be equal
        assert_equal(us1, us2);
    }

    {
        set<unsigned int> s;

        while (s.size() < 1000) {
            const unsigned int n = prng();

            if (n != 0) {
                s.insert(n);
            }
        }

        vector<unsigned int> v(s.begin(), s.end());

        shuffle(v.begin(), v.end(), prng);
        unordered_set<unsigned int> us1(v.begin(), v.end());

        v[0] = 0;

        shuffle(v.begin(), v.end(), prng);
        unordered_set<unsigned int> us2(v.begin(), v.end());

        // same sizes, different elements => should be non-equal
        assert_different(us1, us2);
    }

    {
        set<unsigned int> s;

        while (s.size() < 1000) {
            s.insert(prng());
        }

        vector<unsigned int> v(s.begin(), s.end());

        shuffle(v.begin(), v.end(), prng);
        unordered_set<unsigned int> us1(v.begin(), v.end());

        while (s.size() < 1001) {
            s.insert(prng());
        }

        v.assign(s.begin(), s.end());

        shuffle(v.begin(), v.end(), prng);
        unordered_set<unsigned int> us2(v.begin(), v.end());

        // different sizes => should be non-equal
        assert_different(us1, us2);
    }


    {
        vector<unsigned int> v;

        for (int i = 0; i < 1000; ++i) {
            v.push_back(dist100(prng));
            v.push_back(dist500(prng));
            v.push_back(prng());
        }

        shuffle(v.begin(), v.end(), prng);
        unordered_multiset<unsigned int> ums1(v.begin(), v.end());

        // same sizes, same elements, same orders => should be equal
        assert_equal(ums1, ums1);

        shuffle(v.begin(), v.end(), prng);
        unordered_multiset<unsigned int> ums2(v.begin(), v.end());

        // same sizes, same elements, different orders => should be equal
        assert_equal(ums1, ums2);
    }

    {
        vector<unsigned int> v;

        for (int i = 0; i < 1000; ++i) {
            v.push_back(dist100(prng));
            v.push_back(dist500(prng));
            v.push_back(prng());
        }

        vector<unsigned int> v1 = v;
        v1.push_back(11);
        v1.push_back(11);
        v1.push_back(11);
        v1.push_back(22);
        v1.push_back(22);

        vector<unsigned int> v2 = v;
        v2.push_back(11);
        v2.push_back(11);
        v2.push_back(22);
        v2.push_back(22);
        v2.push_back(22);

        shuffle(v1.begin(), v1.end(), prng);
        unordered_multiset<unsigned int> ums1(v1.begin(), v1.end());

        shuffle(v2.begin(), v2.end(), prng);
        unordered_multiset<unsigned int> ums2(v2.begin(), v2.end());

        // same sizes, different element counts => should be non-equal
        assert_different(ums1, ums2);
    }

    {
        vector<unsigned int> v;

        for (int i = 0; i < 1000; ++i) {
            v.push_back(dist100(prng));
            v.push_back(dist500(prng));
            v.push_back(prng());
        }

        shuffle(v.begin(), v.end(), prng);
        unordered_multiset<unsigned int> ums1(v.begin(), v.end());

        ++v[0];

        shuffle(v.begin(), v.end(), prng);
        unordered_multiset<unsigned int> ums2(v.begin(), v.end());

        // same sizes, different elements => should be non-equal
        assert_different(ums1, ums2);
    }

    {
        vector<unsigned int> v;

        for (int i = 0; i < 1000; ++i) {
            v.push_back(dist100(prng));
            v.push_back(dist500(prng));
            v.push_back(prng());
        }

        shuffle(v.begin(), v.end(), prng);
        unordered_multiset<unsigned int> ums1(v.begin(), v.end());

        v.push_back(prng());

        shuffle(v.begin(), v.end(), prng);
        unordered_multiset<unsigned int> ums2(v.begin(), v.end());

        // different sizes => should be non-equal
        assert_different(ums1, ums2);
    }


    {
        map<unsigned int, unsigned int> m;

        while (m.size() < 1000) {
            m[prng()] = prng();
        }

        vector<pair<unsigned int, unsigned int>> v(m.begin(), m.end());

        shuffle(v.begin(), v.end(), prng);
        unordered_map<unsigned int, unsigned int> um1(v.begin(), v.end());

        // same sizes, same elements, same orders => should be equal
        assert_equal(um1, um1);

        shuffle(v.begin(), v.end(), prng);
        unordered_map<unsigned int, unsigned int> um2(v.begin(), v.end());

        // same sizes, same elements, different orders => should be equal
        assert_equal(um1, um2);
    }

    {
        map<unsigned int, unsigned int> m;

        while (m.size() < 1000) {
            const unsigned int key = prng();
            const unsigned int val = prng();

            if (key != 0 && val != 0) {
                m[key] = val;
            }
        }

        vector<pair<unsigned int, unsigned int>> v(m.begin(), m.end());

        shuffle(v.begin(), v.end(), prng);
        unordered_map<unsigned int, unsigned int> um1(v.begin(), v.end());

        v[0].second = 0;

        shuffle(v.begin(), v.end(), prng);
        unordered_map<unsigned int, unsigned int> um2(v.begin(), v.end());

        // same sizes, same keys, different values => should be non-equal
        assert_different(um1, um2);

        v[0].first = 0;

        shuffle(v.begin(), v.end(), prng);
        unordered_map<unsigned int, unsigned int> um3(v.begin(), v.end());

        // same sizes, different keys, same values => should be non-equal
        assert_different(um2, um3);
    }

    {
        map<unsigned int, unsigned int> m;

        while (m.size() < 1001) {
            m[prng()] = prng();
        }

        vector<pair<unsigned int, unsigned int>> v(m.begin(), m.end());
        v.pop_back();

        shuffle(v.begin(), v.end(), prng);
        unordered_map<unsigned int, unsigned int> um1(v.begin(), v.end());

        v.assign(m.begin(), m.end());

        shuffle(v.begin(), v.end(), prng);
        unordered_map<unsigned int, unsigned int> um2(v.begin(), v.end());

        // different sizes => should be non-equal
        assert_different(um1, um2);
    }


    {
        vector<pair<unsigned int, unsigned int>> v;

        for (int i = 0; i < 1000; ++i) {
            v.push_back(make_pair(dist100(prng), dist100(prng)));
            v.push_back(make_pair(dist500(prng), dist500(prng)));
            v.push_back(make_pair(prng(), prng()));
        }

        shuffle(v.begin(), v.end(), prng);
        unordered_multimap<unsigned int, unsigned int> umm1(v.begin(), v.end());

        // same sizes, same elements, same orders => should be equal
        assert_equal(umm1, umm1);

        shuffle(v.begin(), v.end(), prng);
        unordered_multimap<unsigned int, unsigned int> umm2(v.begin(), v.end());

        // same sizes, same elements, different orders => should be equal
        assert_equal(umm1, umm2);
    }

    {
        vector<pair<unsigned int, unsigned int>> v;

        for (int i = 0; i < 1000; ++i) {
            v.push_back(make_pair(dist100(prng), dist100(prng)));
            v.push_back(make_pair(dist500(prng), dist500(prng)));
            v.push_back(make_pair(prng(), prng()));
        }

        vector<pair<unsigned int, unsigned int>> v1 = v;
        v1.push_back(make_pair(11u, 33u));
        v1.push_back(make_pair(11u, 33u));
        v1.push_back(make_pair(11u, 33u));
        v1.push_back(make_pair(22u, 44u));
        v1.push_back(make_pair(22u, 44u));

        vector<pair<unsigned int, unsigned int>> v2 = v;
        v2.push_back(make_pair(11u, 33u));
        v2.push_back(make_pair(11u, 33u));
        v2.push_back(make_pair(22u, 44u));
        v2.push_back(make_pair(22u, 44u));
        v2.push_back(make_pair(22u, 44u));

        shuffle(v1.begin(), v1.end(), prng);
        unordered_multimap<unsigned int, unsigned int> umm1(v1.begin(), v1.end());

        shuffle(v2.begin(), v2.end(), prng);
        unordered_multimap<unsigned int, unsigned int> umm2(v2.begin(), v2.end());

        // same sizes, different element counts => should be non-equal
        assert_different(umm1, umm2);
    }

    {
        vector<pair<unsigned int, unsigned int>> v;

        for (int i = 0; i < 1000; ++i) {
            v.push_back(make_pair(dist100(prng), dist100(prng)));
            v.push_back(make_pair(dist500(prng), dist500(prng)));
            v.push_back(make_pair(prng(), prng()));
        }

        shuffle(v.begin(), v.end(), prng);
        unordered_multimap<unsigned int, unsigned int> umm1(v.begin(), v.end());

        ++v[0].second;

        shuffle(v.begin(), v.end(), prng);
        unordered_multimap<unsigned int, unsigned int> umm2(v.begin(), v.end());

        // same sizes, same keys, different values => should be non-equal
        assert_different(umm1, umm2);

        ++v[0].first;

        shuffle(v.begin(), v.end(), prng);
        unordered_multimap<unsigned int, unsigned int> umm3(v.begin(), v.end());

        // same sizes, different keys, same values => should be non-equal
        assert_different(umm2, umm3);
    }

    {
        vector<pair<unsigned int, unsigned int>> v;

        for (int i = 0; i < 1000; ++i) {
            v.push_back(make_pair(dist100(prng), dist100(prng)));
            v.push_back(make_pair(dist500(prng), dist500(prng)));
            v.push_back(make_pair(prng(), prng()));
        }

        shuffle(v.begin(), v.end(), prng);
        unordered_multimap<unsigned int, unsigned int> umm1(v.begin(), v.end());

        v.push_back(make_pair(dist100(prng), dist100(prng)));

        shuffle(v.begin(), v.end(), prng);
        unordered_multimap<unsigned int, unsigned int> umm2(v.begin(), v.end());

        // different sizes => should be non-equal
        assert_different(umm1, umm2);
    }
}
