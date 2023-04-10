// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <deque>
#include <vector>

using namespace std;

void test_391805();

int main() {
    deque<int> d;

    for (int n = 0; n < 1000; ++n) {
        d.push_back(n);

        if (d.size() < 26) {
            continue;
        }

        vector<deque<int>::iterator> v;

        for (deque<int>::iterator i = d.begin() + 1; i != d.end(); ++i) {
            v.push_back(i);
        }

        d.pop_front();

        for (deque<int>::size_type i = 0; i < d.size(); ++i) {
            assert(d[i] == *v[i]);
        }
    }

    test_391805();
}

// Also test Dev10-391805 "STL: Prefast error in deque".

void test_391805() {
    deque<int> d;

    d.push_back(10);
    d.push_back(20);
    d.push_front(30);
    d.push_front(40);

    assert(d.size() == 4 && d[0] == 40 && d[1] == 30 && d[2] == 10 && d[3] == 20);
}
