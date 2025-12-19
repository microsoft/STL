// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version>
#if _HAS_CXX20
#include <span>

#include "shared_test.hpp"

void test_span() {
    int array[]{1, 2, 3};
    std::span<int> container(array);
    shared_test(container);
}

#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv

void test_span() {}

#endif // !_HAS_CXX20
