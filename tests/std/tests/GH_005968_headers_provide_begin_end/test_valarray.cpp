// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <valarray>

#include "shared_test.hpp"

void test_valarray() {
    std::valarray<int> container{1, 2, 3};
    shared_test(container);
}
