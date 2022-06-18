// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <any>
#include <cassert>
using namespace std;

int main() {
    // GH-990 <any>: std::any doesn't link when exceptions are disabled
    assert(any_cast<int>(any{1729}) == 1729);
}
