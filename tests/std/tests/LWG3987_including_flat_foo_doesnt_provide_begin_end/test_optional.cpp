// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version>
#if defined(__cpp_lib_optional_range_support)

#include <optional>

#include "shared_test.hpp"

void test_optional() {
    std::optional<int> container{1};
    shared_test(container);
}

#else // ^^^ defined(__cpp_lib_optional_range_support) / feature not available vvv

void test_optional() {}

#endif // feature not available