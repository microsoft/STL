// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <memory>
#include <utility>

using namespace std;

void test() {
    pair<int, unique_ptr<int>> p{42, {}};
    (void) p;
}

int main() {} // COMPILE-ONLY
