// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <format>
#include <string>
using namespace std;

int main() {
    string expected = "[1.e-37]";
    string actual   = format("[{:#6.0g}]", 1.234e-37);
    assert(expected == actual);
    return 0;
}
