// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <iterator>
#include <list>
#include <vector>

using namespace std;

int main() {
    vector<int> v;

    for (int i = 0; i < 10; ++i) {
        v.push_back(i);
    }

    {
        vector<bool> b;

        transform(v.begin(), v.end(), back_inserter(b), [](int n) { return n < 5; });

        insert_iterator<vector<bool>> init(b, b.begin() + 9);

        *init++ = true;

        vector<bool> correct{true, true, true, true, true, false, false, false, false, true, false};

        assert(b == correct);
    }

    {
        list<int> l;

        transform(v.begin(), v.end(), back_inserter(l), [](int n) { return n * 10; });
        transform(v.begin(), v.end(), front_inserter(l), [](int n) { return n * 100; });
        transform(v.begin(), v.end(), inserter(l, next(l.begin(), 5)), [](int n) { return n * 1000; });

        const int c[] = {900, 800, 700, 600, 500, 0, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 400, 300,
            200, 100, 0, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90};

        const list<int> correct(begin(c), end(c));

        assert(l == correct);
    }
}
