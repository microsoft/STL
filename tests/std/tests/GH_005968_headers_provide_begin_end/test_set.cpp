// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <set>

#include "shared_test.hpp"

void test_set() {
    std::set<int> container{1,2,3};
    shared_test(container);

    std::multiset<int> container2{1,2,3};
    shared_test(container2);
}
