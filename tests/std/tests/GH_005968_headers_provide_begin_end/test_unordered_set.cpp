// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <unordered_set>

#include "shared_test.hpp"

void test_unordered_set() {
    std::unordered_set<int> container{1,2,3};
    shared_test(container);
    
    std::unordered_multiset<int> container2{4,5,6};
    shared_test(container2);
}
