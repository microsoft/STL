// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <deque>

#include "shared_test.hpp"

void test_deque() {
    std::deque<int> container{1, 2, 3};
    shared_test(container);
}
