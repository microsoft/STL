// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <functional>
#include <utility>

using namespace std;

int main() {
    function<int()> f = []() { return 1729; };

    function<int()> g = move(f);

    assert(!f);
    assert(g);
    assert(g() == 1729);
}
