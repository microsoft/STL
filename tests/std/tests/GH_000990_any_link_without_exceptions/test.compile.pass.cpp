// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <any>

using namespace std;

int test() {
    // GH-990 <any>: std::any doesn't link when exceptions are disabled
    return any_cast<int>(any{});
}

int main() {} // COMPILE-ONLY
