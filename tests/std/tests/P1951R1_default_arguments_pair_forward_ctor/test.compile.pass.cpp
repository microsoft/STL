// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <memory>
#include <utility>

using namespace std;

void test() {
    pair<int, unique_ptr<int>> p1{42, {}};
    pair<unique_ptr<int>, int> p2{{}, 42};
    pair<unique_ptr<int>, unique_ptr<int>> p3{{}, {}};
    (void) p1;
    (void) p2;
    (void) p3;
}
