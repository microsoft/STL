// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version> // TRANSITION, P0912R5 Library Support For Coroutines
#ifdef __cpp_lib_coroutine // TRANSITION, P0912R5 Library Support For Coroutines

#include <coroutine>

// Testing LWG-3330 "Include <compare> from most library headers" by intentionally NOT including <compare>

static_assert(std::is_eq(std::partial_ordering::equivalent));

#endif // TRANSITION, P0912R5 Library Support For Coroutines

void test_coroutine() {}
