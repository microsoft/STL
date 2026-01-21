// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <vector>

#include "shared_test.hpp"

void test_vector() {
    std::vector<int> container{1, 2, 3};
    shared_test(container);
}
