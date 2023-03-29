// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <deque>
#include <forward_list>
#include <functional>
#include <iterator>
#include <list>
#include <memory>
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

struct UniqueHash {
    size_t operator()(const unique_ptr<int>& p) const {
        return hash<int*>()(p.get());
    }
};

template <typename C>
int product(const C& c) {
    int n = 1;

    for (auto i = c.begin(); i != c.end(); ++i) {
        n *= **i;
    }

    return n;
}

template <typename C>
void test(C& src) {
    C x = move(src);

    assert(product(x) == 7986);

    C y;

    y = move(x);

    assert(product(y) == 7986);

    typedef typename C::allocator_type A;

    C z(move(y), A());

    assert(product(z) == 7986);
}

int main() {
    {
        vector<unique_ptr<int>> v;
        v.push_back(unique_ptr<int>(new int(11)));
        v.push_back(unique_ptr<int>(new int(22)));
        v.push_back(unique_ptr<int>(new int(33)));
        test(v);

        deque<unique_ptr<int>> d;
        d.push_back(unique_ptr<int>(new int(11)));
        d.push_back(unique_ptr<int>(new int(22)));
        d.push_back(unique_ptr<int>(new int(33)));
        test(d);

        list<unique_ptr<int>> l;
        l.push_back(unique_ptr<int>(new int(11)));
        l.push_back(unique_ptr<int>(new int(22)));
        l.push_back(unique_ptr<int>(new int(33)));
        test(l);

        set<unique_ptr<int>> s;
        s.insert(unique_ptr<int>(new int(11)));
        s.insert(unique_ptr<int>(new int(22)));
        s.insert(unique_ptr<int>(new int(33)));
        test(s);

        unordered_set<unique_ptr<int>, UniqueHash> us;
        us.insert(unique_ptr<int>(new int(11)));
        us.insert(unique_ptr<int>(new int(22)));
        us.insert(unique_ptr<int>(new int(33)));
        test(us);

        forward_list<unique_ptr<int>> fl;
        fl.push_front(unique_ptr<int>(new int(11)));
        fl.push_front(unique_ptr<int>(new int(22)));
        fl.push_front(unique_ptr<int>(new int(33)));
        test(fl);
    }


    {
        vector<unique_ptr<int>> x;
        x.push_back(unique_ptr<int>(new int(11)));
        x.push_back(unique_ptr<int>(new int(22)));
        x.push_back(unique_ptr<int>(new int(33)));

        vector<unique_ptr<int>> y;
        y.push_back(unique_ptr<int>(new int(44)));
        y.push_back(unique_ptr<int>(new int(55)));

        x.swap(y);

        assert(x.size() == 2 && y.size() == 3 && *x.back() == 55 && *y.back() == 33);
    }

    {
        deque<unique_ptr<int>> x;
        x.push_back(unique_ptr<int>(new int(11)));
        x.push_back(unique_ptr<int>(new int(22)));
        x.push_back(unique_ptr<int>(new int(33)));

        deque<unique_ptr<int>> y;
        y.push_back(unique_ptr<int>(new int(44)));
        y.push_back(unique_ptr<int>(new int(55)));

        x.swap(y);

        assert(x.size() == 2 && y.size() == 3 && *x.back() == 55 && *y.back() == 33);
    }

    {
        list<unique_ptr<int>> x;
        x.push_back(unique_ptr<int>(new int(11)));
        x.push_back(unique_ptr<int>(new int(22)));
        x.push_back(unique_ptr<int>(new int(33)));

        list<unique_ptr<int>> y;
        y.push_back(unique_ptr<int>(new int(44)));
        y.push_back(unique_ptr<int>(new int(55)));

        x.swap(y);

        assert(x.size() == 2 && y.size() == 3 && *x.back() == 55 && *y.back() == 33);
    }

    {
        set<unique_ptr<int>> x;
        x.insert(unique_ptr<int>(new int(11)));
        x.insert(unique_ptr<int>(new int(22)));
        x.insert(unique_ptr<int>(new int(33)));

        set<unique_ptr<int>> y;
        y.insert(unique_ptr<int>(new int(44)));
        y.insert(unique_ptr<int>(new int(55)));

        x.swap(y);

        assert(x.size() == 2 && y.size() == 3 && product(x) == 2420 && product(y) == 7986);
    }

    {
        unordered_set<unique_ptr<int>, UniqueHash> x;
        x.insert(unique_ptr<int>(new int(11)));
        x.insert(unique_ptr<int>(new int(22)));
        x.insert(unique_ptr<int>(new int(33)));

        unordered_set<unique_ptr<int>, UniqueHash> y;
        y.insert(unique_ptr<int>(new int(44)));
        y.insert(unique_ptr<int>(new int(55)));

        x.swap(y);

        assert(x.size() == 2 && y.size() == 3 && product(x) == 2420 && product(y) == 7986);
    }

    {
        forward_list<unique_ptr<int>> x;
        x.push_front(unique_ptr<int>(new int(11)));
        x.push_front(unique_ptr<int>(new int(22)));
        x.push_front(unique_ptr<int>(new int(33)));

        forward_list<unique_ptr<int>> y;
        y.push_front(unique_ptr<int>(new int(44)));
        y.push_front(unique_ptr<int>(new int(55)));

        x.swap(y);

        assert(distance(x.begin(), x.end()) == 2 && distance(y.begin(), y.end()) == 3 && *x.front() == 55
               && *y.front() == 33);
    }
}
