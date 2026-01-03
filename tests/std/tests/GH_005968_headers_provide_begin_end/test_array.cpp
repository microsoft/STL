// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>

#include "shared_test.hpp"

void test_array() {
    std::array<int, 3> container{1, 2, 3};
    shared_test(container);
}
