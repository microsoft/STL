// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version>
#if _HAS_CXX23

#include <stacktrace>

#include "shared_test.hpp"

void test_stacktrace() {
    std::stacktrace container;
    shared_test(container);
}

#else // ^^^ _HAS_CXX23 / !_HAS_CXX23 vvv

void test_stacktrace() {}

#endif // ^^^ !_HAS_CXX23 ^^^
