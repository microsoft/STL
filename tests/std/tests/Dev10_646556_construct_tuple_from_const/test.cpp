// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <string>
#include <tuple>
#include <utility>

using namespace std;

int main() {
    int a          = 10;
    string b       = "twenty";
    const int c    = 30;
    const string d = "forty";

    tuple<int, string, int, string> t(a, b, c, d);

    pair<int, int> p(a, c);
    pair<string, string> q(b, d);

    assert(get<0>(t) == 10);
    assert(get<1>(t) == "twenty");
    assert(get<2>(t) == 30);
    assert(get<3>(t) == "forty");
    assert(p.first == 10);
    assert(p.second == 30);
    assert(q.first == "twenty");
    assert(q.second == "forty");
}
