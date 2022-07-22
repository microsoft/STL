// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <string>
#include <tuple>

using namespace std;

int main() {
    tuple<int, string, int> t(1, "x", 2);

    int a    = 4;
    string b = "y";
    int c    = 7;

    assert(get<0>(t) == 1);
    assert(get<1>(t) == "x");
    assert(get<2>(t) == 2);

    assert(a == 4);
    assert(b == "y");
    assert(c == 7);

    tie(a, b, c) = t;

    assert(get<0>(t) == 1);
    assert(get<1>(t) == "x");
    assert(get<2>(t) == 2);

    assert(a == 1);
    assert(b == "x");
    assert(c == 2);
}
