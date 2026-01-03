// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <list>

#include "shared_test.hpp"

void test_list() {
    std::list<int> container{1, 2, 3};
    shared_test(container);
}
