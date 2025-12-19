// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version>
#if __has_include(<hive>)

#include <hive>

#include "shared_test.hpp"

void test_hive() {
    std::hive<int> container{1, 2, 3};
    shared_test(container);
}

#else // ^^^ __has_include(<hive>) / !__has_include(<hive>) vvv

void test_hive() {}

#endif // !__has_include(<hive>)