// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version>
#if defined(__cpp_lib_hive)

#include <hive>

#include "shared_test.hpp"

void test_hive() {
    std::hive<int> container{1, 2, 3};
    shared_test(container);
}

#else // ^^^ defined(__cpp_lib_hive) / feature not available vvv

void test_hive() {}

#endif // feature not available
