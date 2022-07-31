// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <tuple>

using namespace std;

bool size_less(const string& l, const string& r) {
    return make_tuple(l.size(), ref(l)) < make_tuple(r.size(), ref(r));
}

struct Kl {
    typedef void is_transparent;
    bool operator()(const string& l, const string& r) const {
        return size_less(l, r);
    }
    bool operator()(const string& l, const int n) const {
        return l.size() < static_cast<size_t>(n);
    }
};

struct Ku {
    typedef void is_transparent;
    bool operator()(const string& l, const string& r) const {
        return size_less(l, r);
    }
    bool operator()(const int n, const string& r) const {
        return static_cast<size_t>(n) < r.size();
    }
};

struct Ke {
    typedef void is_transparent;
    bool operator()(const string& l, const string& r) const {
        return size_less(l, r);
    }
    bool operator()(const string& l, const int n) const {
        return l.size() < static_cast<size_t>(n);
    }
    bool operator()(const int n, const string& r) const {
        return static_cast<size_t>(n) < r.size();
    }
};

template class std::multiset<string>;
template class std::multiset<string, Kl>;
template class std::multiset<string, Ku>;
template class std::multiset<string, Ke>;
template class std::multiset<string, less<>>;
template class std::set<string, Ke>;
template class std::multimap<string, int, Ke>;
template class std::map<string, int, Ke>;

int main() {
    static const char* const arr[] = {
        "ape", // 0
        "bat", // 1
        "cat", // 2
        "bear", // 3
        "wolf", // 4
        "raven", // 5
        "giraffe", // 6
        "panther", // 7
        "unicorn", // 8
        "direwolf", // 9
        "elephant" // 10
    };

    {
        multiset<string, Kl> l(begin(arr), end(arr));

        assert(end(arr) - begin(arr) == distance(l.begin(), l.end()));
        assert(equal(begin(arr), end(arr), l.begin()));

        assert(l.lower_bound(2) == next(l.begin(), 0));
        assert(l.lower_bound(3) == next(l.begin(), 0));
        assert(l.lower_bound(4) == next(l.begin(), 3));
        assert(l.lower_bound(5) == next(l.begin(), 5));
        assert(l.lower_bound(6) == next(l.begin(), 6));
        assert(l.lower_bound(7) == next(l.begin(), 6));
        assert(l.lower_bound(8) == next(l.begin(), 9));
        assert(l.lower_bound(9) == next(l.begin(), 11));

        const auto& cl = l;

        assert(cl.lower_bound(2) == next(cl.begin(), 0));
        assert(cl.lower_bound(3) == next(cl.begin(), 0));
        assert(cl.lower_bound(4) == next(cl.begin(), 3));
        assert(cl.lower_bound(5) == next(cl.begin(), 5));
        assert(cl.lower_bound(6) == next(cl.begin(), 6));
        assert(cl.lower_bound(7) == next(cl.begin(), 6));
        assert(cl.lower_bound(8) == next(cl.begin(), 9));
        assert(cl.lower_bound(9) == next(cl.begin(), 11));
    }

    {
        multiset<string, Ku> u(begin(arr), end(arr));

        assert(end(arr) - begin(arr) == distance(u.begin(), u.end()));
        assert(equal(begin(arr), end(arr), u.begin()));

        assert(u.upper_bound(2) == next(u.begin(), 0));
        assert(u.upper_bound(3) == next(u.begin(), 3));
        assert(u.upper_bound(4) == next(u.begin(), 5));
        assert(u.upper_bound(5) == next(u.begin(), 6));
        assert(u.upper_bound(6) == next(u.begin(), 6));
        assert(u.upper_bound(7) == next(u.begin(), 9));
        assert(u.upper_bound(8) == next(u.begin(), 11));
        assert(u.upper_bound(9) == next(u.begin(), 11));

        const auto& cu = u;

        assert(cu.upper_bound(2) == next(cu.begin(), 0));
        assert(cu.upper_bound(3) == next(cu.begin(), 3));
        assert(cu.upper_bound(4) == next(cu.begin(), 5));
        assert(cu.upper_bound(5) == next(cu.begin(), 6));
        assert(cu.upper_bound(6) == next(cu.begin(), 6));
        assert(cu.upper_bound(7) == next(cu.begin(), 9));
        assert(cu.upper_bound(8) == next(cu.begin(), 11));
        assert(cu.upper_bound(9) == next(cu.begin(), 11));
    }

    {
        multiset<string, Ke> e(begin(arr), end(arr));
        set<string, Ke> f(begin(arr), end(arr));
        multimap<string, int, Ke> g{{"ape", 0}, {"bat", 1}, {"cat", 2}, {"bear", 3}, {"wolf", 4}, {"raven", 5},
            {"giraffe", 6}, {"panther", 7}, {"unicorn", 8}, {"direwolf", 9}, {"elephant", 10}};
        map<string, int, Ke> h{{"ape", 0}, {"bat", 1}, {"cat", 2}, {"bear", 3}, {"wolf", 4}, {"raven", 5},
            {"giraffe", 6}, {"panther", 7}, {"unicorn", 8}, {"direwolf", 9}, {"elephant", 10}};

        assert(end(arr) - begin(arr) == distance(e.begin(), e.end()));
        assert(equal(begin(arr), end(arr), e.begin()));

        assert(e.find(2) == e.end());
        assert(e.find(3)->size() == 3);
        assert(e.find(4)->size() == 4);
        assert(e.find(5) == next(e.begin(), 5));
        assert(e.find(6) == e.end());
        assert(e.find(7)->size() == 7);
        assert(e.find(8)->size() == 8);
        assert(e.find(9) == e.end());

#if _HAS_CXX20
        assert(!e.contains(2));
        assert(e.contains(3));
        assert(e.contains(4));
        assert(e.contains(5));
        assert(!e.contains(6));
        assert(e.contains(7));
        assert(e.contains(8));
        assert(!e.contains(9));
        assert(!f.contains(2));
        assert(f.contains(3));
        assert(f.contains(4));
        assert(f.contains(5));
        assert(!f.contains(6));
        assert(f.contains(7));
        assert(f.contains(8));
        assert(!f.contains(9));
        assert(!g.contains(2));
        assert(g.contains(3));
        assert(g.contains(4));
        assert(g.contains(5));
        assert(!g.contains(6));
        assert(g.contains(7));
        assert(g.contains(8));
        assert(!g.contains(9));
        assert(!h.contains(2));
        assert(h.contains(3));
        assert(h.contains(4));
        assert(h.contains(5));
        assert(!h.contains(6));
        assert(h.contains(7));
        assert(h.contains(8));
        assert(!h.contains(9));
#endif // _HAS_CXX20

        assert(e.count(2) == 0);
        assert(e.count(3) == 3);
        assert(e.count(4) == 2);
        assert(e.count(5) == 1);
        assert(e.count(6) == 0);
        assert(e.count(7) == 3);
        assert(e.count(8) == 2);
        assert(e.count(9) == 0);

        assert(e.equal_range(2) == make_pair(next(e.begin(), 0), next(e.begin(), 0)));
        assert(e.equal_range(3) == make_pair(next(e.begin(), 0), next(e.begin(), 3)));
        assert(e.equal_range(4) == make_pair(next(e.begin(), 3), next(e.begin(), 5)));
        assert(e.equal_range(5) == make_pair(next(e.begin(), 5), next(e.begin(), 6)));
        assert(e.equal_range(6) == make_pair(next(e.begin(), 6), next(e.begin(), 6)));
        assert(e.equal_range(7) == make_pair(next(e.begin(), 6), next(e.begin(), 9)));
        assert(e.equal_range(8) == make_pair(next(e.begin(), 9), next(e.begin(), 11)));
        assert(e.equal_range(9) == make_pair(next(e.begin(), 11), next(e.begin(), 11)));

        const auto& ce = e;

        assert(ce.find(2) == ce.end());
        assert(ce.find(3)->size() == 3);
        assert(ce.find(4)->size() == 4);
        assert(ce.find(5) == next(ce.begin(), 5));
        assert(ce.find(6) == ce.end());
        assert(ce.find(7)->size() == 7);
        assert(ce.find(8)->size() == 8);
        assert(ce.find(9) == ce.end());

#if _HAS_CXX20
        const auto& cf = f;
        const auto& cg = g;
        const auto& ch = h;

        assert(!ce.contains(2));
        assert(ce.contains(3));
        assert(ce.contains(4));
        assert(ce.contains(5));
        assert(!ce.contains(6));
        assert(ce.contains(7));
        assert(ce.contains(8));
        assert(!ce.contains(9));
        assert(!cf.contains(2));
        assert(cf.contains(3));
        assert(cf.contains(4));
        assert(cf.contains(5));
        assert(!cf.contains(6));
        assert(cf.contains(7));
        assert(cf.contains(8));
        assert(!cf.contains(9));
        assert(!cg.contains(2));
        assert(cg.contains(3));
        assert(cg.contains(4));
        assert(cg.contains(5));
        assert(!cg.contains(6));
        assert(cg.contains(7));
        assert(cg.contains(8));
        assert(!cg.contains(9));
        assert(!ch.contains(2));
        assert(ch.contains(3));
        assert(ch.contains(4));
        assert(ch.contains(5));
        assert(!ch.contains(6));
        assert(ch.contains(7));
        assert(ch.contains(8));
        assert(!ch.contains(9));
#endif // _HAS_CXX20

        assert(ce.count(2) == 0);
        assert(ce.count(3) == 3);
        assert(ce.count(4) == 2);
        assert(ce.count(5) == 1);
        assert(ce.count(6) == 0);
        assert(ce.count(7) == 3);
        assert(ce.count(8) == 2);
        assert(ce.count(9) == 0);

        assert(ce.equal_range(2) == make_pair(next(ce.begin(), 0), next(ce.begin(), 0)));
        assert(ce.equal_range(3) == make_pair(next(ce.begin(), 0), next(ce.begin(), 3)));
        assert(ce.equal_range(4) == make_pair(next(ce.begin(), 3), next(ce.begin(), 5)));
        assert(ce.equal_range(5) == make_pair(next(ce.begin(), 5), next(ce.begin(), 6)));
        assert(ce.equal_range(6) == make_pair(next(ce.begin(), 6), next(ce.begin(), 6)));
        assert(ce.equal_range(7) == make_pair(next(ce.begin(), 6), next(ce.begin(), 9)));
        assert(ce.equal_range(8) == make_pair(next(ce.begin(), 9), next(ce.begin(), 11)));
        assert(ce.equal_range(9) == make_pair(next(ce.begin(), 11), next(ce.begin(), 11)));
    }

#if _HAS_CXX23
    {
        multiset<string, Ke> e(begin(arr), end(arr));
        set<string, Ke> f(begin(arr), end(arr));
        multimap<string, int, Ke> g{{"ape", 0}, {"bat", 1}, {"cat", 2}, {"bear", 3}, {"wolf", 4}, {"raven", 5},
            {"giraffe", 6}, {"panther", 7}, {"unicorn", 8}, {"direwolf", 9}, {"elephant", 10}};
        map<string, int, Ke> h{{"ape", 0}, {"bat", 1}, {"cat", 2}, {"bear", 3}, {"wolf", 4}, {"raven", 5},
            {"giraffe", 6}, {"panther", 7}, {"unicorn", 8}, {"direwolf", 9}, {"elephant", 10}};

        assert(e.erase(2) == 0);
        assert(e.erase(3) == 3);
        assert(e.erase(4) == 2);
        assert(e.erase(5) == 1);
        assert(e.erase(6) == 0);
        assert(e.erase(7) == 3);
        assert(e.erase(8) == 2);
        assert(e.erase(9) == 0);
        assert(e.empty());

        assert(f.erase(2) == 0);
        assert(f.erase(3) == 3);
        assert(f.erase(4) == 2);
        assert(f.erase(5) == 1);
        assert(f.erase(6) == 0);
        assert(f.erase(7) == 3);
        assert(f.erase(8) == 2);
        assert(f.erase(9) == 0);
        assert(f.empty());

        assert(g.erase(2) == 0);
        assert(g.erase(3) == 3);
        assert(g.erase(4) == 2);
        assert(g.erase(5) == 1);
        assert(g.erase(6) == 0);
        assert(g.erase(7) == 3);
        assert(g.erase(8) == 2);
        assert(g.erase(9) == 0);
        assert(g.empty());

        assert(h.erase(2) == 0);
        assert(h.erase(3) == 3);
        assert(h.erase(4) == 2);
        assert(h.erase(5) == 1);
        assert(h.erase(6) == 0);
        assert(h.erase(7) == 3);
        assert(h.erase(8) == 2);
        assert(h.erase(9) == 0);
        assert(h.empty());
    }

    {
        multiset<string, Ke> i(begin(arr), end(arr));
        set<string, Ke> j(begin(arr), end(arr));
        multimap<string, int, Ke> k{{"ape", 0}, {"bat", 1}, {"cat", 2}, {"bear", 3}, {"wolf", 4}, {"raven", 5},
            {"giraffe", 6}, {"panther", 7}, {"unicorn", 8}, {"direwolf", 9}, {"elephant", 10}};
        map<string, int, Ke> l{{"ape", 0}, {"bat", 1}, {"cat", 2}, {"bear", 3}, {"wolf", 4}, {"raven", 5},
            {"giraffe", 6}, {"panther", 7}, {"unicorn", 8}, {"direwolf", 9}, {"elephant", 10}};

        auto test_extract = []<typename C>(C& c) {
            auto n2 = c.extract(2);
            auto n3 = c.extract(3);
            auto n4 = c.extract(4);
            auto n5 = c.extract(5);
            auto n6 = c.extract(6);
            auto n7 = c.extract(7);
            auto n8 = c.extract(8);
            auto n9 = c.extract(9);

            assert(!n2);
            assert(n3);
            assert(n4);
            assert(n5);
            assert(!n6);
            assert(n7);
            assert(n8);
            assert(!n9);

            C d;

            d.insert(move(n2));
            d.insert(move(n3));
            d.insert(move(n4));
            d.insert(move(n5));
            d.insert(move(n6));
            d.insert(move(n7));
            d.insert(move(n8));
            d.insert(move(n9));

            assert(c.size() == 6);
            assert(d.size() == 5);
        };
        test_extract(i);
        test_extract(j);
        test_extract(k);
        test_extract(l);
    }
#endif // _HAS_CXX23
}
