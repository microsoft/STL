// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <map>

#include "shared_test.hpp"

void test_map() {
    std::map<int, int> container{
        {1, 2},
        {3, 4}
    };
    shared_test(container);

    std::multimap<int, int> container2{
        {5, 2},
        {6, 4}
    };
    shared_test(container2);
}
