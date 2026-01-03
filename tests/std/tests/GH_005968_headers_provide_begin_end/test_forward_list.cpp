// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <forward_list>

#include "shared_test.hpp"

void test_forward_list() {
    std::forward_list<int> container{1, 2, 3};
    shared_test(container);
}
