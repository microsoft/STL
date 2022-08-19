// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <tuple>
#include <utility>

using namespace std;

int main() {
    int a = 0;
    int b = 0;

    pair<int, int> p(1, 2);

    tie(a, b) = p;

    assert(a == 1 && b == 2);

    const pair<int, int> q(3, 4);

    tie(a, b) = q;

    assert(a == 3 && b == 4);
}
