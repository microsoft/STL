// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <iostream>

using namespace std;

int main() {
    assert(clog.tie() == nullptr);
    assert(wclog.tie() == nullptr);
}
