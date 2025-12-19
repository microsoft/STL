// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version>
#if _HAS_CXX17

#include <string_view>

#include "shared_test.hpp"

void test_string_view() {
    char text[]                = "hello";
    std::string_view container = text;
    shared_test(container);
}
#else // ^^^ _HAS_CXX17 / !_HAS_CXX17 vvv

void test_string_view() {}

#endif // !_HAS_CXX17
