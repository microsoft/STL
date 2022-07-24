// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <type_traits>
#include <vector>

using namespace std;

const bool* test2_int_int();
const bool* test2_short_long();

int main() {
    const bool* const x1 = &is_same_v<int, int>;
    const bool* const x2 = test2_int_int();

    const bool* const y = &is_same_v<float, double>;
    const bool* const z = test2_short_long();

    vector<const bool*> v = {x1, x2, y, z};
    sort(v.begin(), v.end());
    v.erase(unique(v.begin(), v.end()), v.end());

    assert(x1 == x2);
    assert(v.size() == 3);
}
