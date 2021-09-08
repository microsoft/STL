// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _HAS_EXCEPTIONS 0

#include <any>

using namespace std;

int test() {
    return any_cast<int>(any{});
}

int main() {} // COMPILE-ONLY
