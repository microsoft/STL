// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <functional>

using namespace std;

int main() {
    int x  = 100;
    int* p = &x;

    reference_wrapper<int*> r(p);

    *r /= 5;

    assert(x == 20);
}
