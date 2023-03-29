// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <list>
#include <vector>

using namespace std;

int main() {
    {
        vector<int> v(1000, 1729);

        assert(v.size() == 1000);
        assert(v.capacity() == 1000);

        v.resize(1003);

        assert(v.size() == 1003);
        assert(v.capacity() == 1500);
    }

    {
        vector<int> v(1000, 1729);

        assert(v.size() == 1000);
        assert(v.capacity() == 1000);

        v.resize(8000);

        assert(v.size() == 8000);
        assert(v.capacity() == 8000);
    }

    {
        vector<int> v(1000, 1729);

        assert(v.size() == 1000);
        assert(v.capacity() == 1000);

        v.push_back(47);

        assert(v.size() == 1001);
        assert(v.capacity() == 1500);
    }

    {
        vector<int> v(1000, 1729);

        assert(v.size() == 1000);
        assert(v.capacity() == 1000);

        list<int> l(3, 47);

        v.insert(v.end(), l.begin(), l.end());

        assert(v.size() == 1003);
        assert(v.capacity() == 1500);
    }

    {
        vector<int> v(1000, 1729);

        assert(v.size() == 1000);
        assert(v.capacity() == 1000);

        list<int> l(7000, 47);

        v.insert(v.end(), l.begin(), l.end());

        assert(v.size() == 8000);
        assert(v.capacity() == 8000);
    }

    {
        vector<int> v(1000, 1729);

        assert(v.size() == 1000);
        assert(v.capacity() == 1000);

        v.insert(v.end(), 3, 47);

        assert(v.size() == 1003);
        assert(v.capacity() == 1500);
    }

    {
        vector<int> v(1000, 1729);

        assert(v.size() == 1000);
        assert(v.capacity() == 1000);

        v.insert(v.end(), 7000, 47);

        assert(v.size() == 8000);
        assert(v.capacity() == 8000);
    }
}
