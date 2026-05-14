// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version>
#if defined(__cpp_lib_optional_range_support)
static_assert(false, "When this feature is implemented, update this to a Standard mode check.");

#include <optional>

#include "shared_test.hpp"

void test_optional() {
    std::optional<int> container{1};
    shared_test(container);
}

#else // ^^^ defined(__cpp_lib_optional_range_support) / !defined(__cpp_lib_optional_range_support) vvv

void test_optional() {}

#endif // ^^^ !defined(__cpp_lib_optional_range_support) ^^^
