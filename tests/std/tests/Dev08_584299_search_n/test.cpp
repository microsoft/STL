// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <list>

// This test makes sure that the search_n algorithm behaves correctly.
// In VS2005, we were incorrectly identifying a shorter sequence as the
// correct result. E.g., in the sequence:

// 8,7,0,0,1,1,8,0,8,2,1,1,1,0,1,0,2,0,8,0,8,2,37,40

// search_n identified the first two zeroes as a sequence of 3 zeroes.

int main() {
    {
        // use random access iterators
        const int asize = 24;
        int a[asize]    = {8, 7, 0, 0, 1, 1, 8, 0, 8, 2, 1, 1, 1, 0, 1, 0, 2, 0, 8, 0, 8, 2, 37, 40};

        const auto res  = std::search_n(a, a + asize, 3, 0);
        const auto diff = res - a;
        assert(diff == 24);
    }

    {
        // use bidirectional iterators
        const int asize = 24;
        int a[asize]    = {8, 7, 0, 0, 1, 1, 8, 0, 8, 2, 1, 1, 1, 0, 1, 0, 2, 0, 8, 0, 8, 2, 37, 40};
        std::list<int> l;

        for (int i = 0; i < asize; ++i) {
            l.push_back(a[i]);
        }
        const auto res  = std::search_n(l.begin(), l.end(), 3, 0);
        const auto diff = std::distance(l.begin(), res);
        assert(diff == 24);
    }
}
