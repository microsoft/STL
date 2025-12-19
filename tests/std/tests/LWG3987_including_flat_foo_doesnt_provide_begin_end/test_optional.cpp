// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version>
#if _HAS_CXX17

#include <optional>

#include "shared_test.hpp"

void test_optional() {
#ifdef __cpp_lib_optional_range_support
    std::optional<int> container{1};
    shared_test(container);
#endif // defined(__cpp_lib_optional_range_support)
}

#else // ^^^ _HAS_CXX17 / !_HAS_CXX17 vvv

void test_optional() {}

#endif // !_HAS_CXX17