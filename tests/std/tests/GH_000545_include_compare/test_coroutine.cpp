// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <coroutine>

// Testing LWG-3330 "Include <compare> from most library headers" by intentionally NOT including <compare>

static_assert(std::is_eq(std::partial_ordering::equivalent));

void test_coroutine() {}
