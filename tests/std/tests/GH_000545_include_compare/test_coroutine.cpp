// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version> // TRANSITION, Clang 14 coroutine support
#ifdef __cpp_lib_coroutine // TRANSITION, Clang 14 coroutine support

#include <coroutine>

// Testing LWG-3330 "Include <compare> from most library headers" by intentionally NOT including <compare>

static_assert(std::is_eq(std::partial_ordering::equivalent));

#endif // TRANSITION, Clang 14 coroutine support

void test_coroutine() {}
