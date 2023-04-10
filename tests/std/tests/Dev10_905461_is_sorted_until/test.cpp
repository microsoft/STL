// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <functional>
#include <vector>

using namespace std;

int main() {
    vector<int> v;

    assert(is_sorted(v.begin(), v.end()));
    assert(is_sorted(v.begin(), v.end(), greater<int>()));
    assert(is_sorted_until(v.begin(), v.end()) == v.end());
    assert(is_sorted_until(v.begin(), v.end(), greater<int>()) == v.end());

    v.push_back(11);

    assert(is_sorted(v.begin(), v.end()));
    assert(is_sorted(v.begin(), v.end(), greater<int>()));
    assert(is_sorted_until(v.begin(), v.end()) == v.end());
    assert(is_sorted_until(v.begin(), v.end(), greater<int>()) == v.end());

    v.push_back(22);

    assert(is_sorted(v.begin(), v.end()));
    assert(!is_sorted(v.begin(), v.end(), greater<int>()));
    assert(is_sorted_until(v.begin(), v.end()) == v.end());
    assert(is_sorted_until(v.begin(), v.end(), greater<int>()) == v.begin() + 1);

    v.clear();

    v.push_back(11);
    v.push_back(22);
    v.push_back(33);
    v.push_back(44);
    v.push_back(55);

    assert(is_sorted(v.begin(), v.end()));
    assert(is_sorted_until(v.begin(), v.end()) == v.end());

    v[3] = 0;

    assert(!is_sorted(v.begin(), v.end()));
    assert(is_sorted_until(v.begin(), v.end()) == v.begin() + 3);

    v.clear();

    v.push_back(55);
    v.push_back(44);
    v.push_back(33);
    v.push_back(22);
    v.push_back(11);

    assert(is_sorted(v.begin(), v.end(), greater<int>()));
    assert(is_sorted_until(v.begin(), v.end(), greater<int>()) == v.end());

    v[3] = 99;

    assert(!is_sorted(v.begin(), v.end(), greater<int>()));
    assert(is_sorted_until(v.begin(), v.end(), greater<int>()) == v.begin() + 3);
}
