// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <functional>
#include <vector>

using namespace std;

int main() {
    {
        vector<int> v;

        for (int i = 0; i < 6; ++i) {
            v.push_back(i * 5);
        }

        for (int i = 0; i < 3; ++i) {
            v.push_back(5);
        }

        for (int i = 0; i < 6; ++i) {
            v.push_back(i * 5);
        }

        for (int i = 0; i < 3; ++i) {
            v.push_back(5);
        }

        assert(search_n(v.begin(), v.end(), 5, 7) == v.end() && "Found 5 occurrences of 7. This is BAD.");
        assert(search_n(v.begin(), v.end(), 4, 5) == v.end() && "Found 4 occurrences of 5. This is BAD.");
        assert(
            search_n(v.begin(), v.end(), 5, 7, equal_to<int>()) == v.end() && "Found 5 occurrences of 7. This is BAD.");
        assert(
            search_n(v.begin(), v.end(), 4, 5, equal_to<int>()) == v.end() && "Found 4 occurrences of 5. This is BAD.");
    }

    {
        vector<int> v;

        v.push_back(11);
        v.push_back(22);
        v.push_back(33);
        v.push_back(44);
        v.push_back(55);

        assert(search_n(v.rbegin(), v.rend(), 4, 8) == v.rend());
        assert(search_n(v.rbegin(), v.rend(), 4, 8, equal_to<int>()) == v.rend());
    }
}
