// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <vector>

using namespace std;

static const auto is_true  = [](bool b) { return b; };
static const auto is_false = [](bool b) { return !b; };

int main() {
    vector<bool> x(100, false);
    vector<bool> y(100, true);

    assert(all_of(x.begin(), x.end(), is_false));
    assert(all_of(y.begin(), y.end(), is_true));

    swap(x[12], y[34]);

    assert(all_of(x.begin(), x.begin() + 12, is_false));
    assert(x[12]);
    assert(all_of(x.begin() + 13, x.end(), is_false));

    assert(all_of(y.begin(), y.begin() + 34, is_true));
    assert(!y[34]);
    assert(all_of(y.begin() + 35, y.end(), is_true));
}
