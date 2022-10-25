// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <set>
#include <vector>

using namespace std;

int main() {
    vector<set<int>> v;

    v.push_back(set<int>());

    v[0].insert(55);

    v.push_back(v[0]);

    assert(v[1].size() == 1);
}
