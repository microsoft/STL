// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <sstream>

int main() {
    unsigned long long s = 10000000000000000000ULL;
    unsigned long long t = 0;
    unsigned long u      = 1000000000UL;
    unsigned long v      = 0;
    std::stringstream str;
    std::stringstream str2;

    str << s;
    str >> t;

    assert(str);
    assert(s == t);

    str2 << u;
    str2 >> v;

    assert(str2);
    assert(u == v);
}
