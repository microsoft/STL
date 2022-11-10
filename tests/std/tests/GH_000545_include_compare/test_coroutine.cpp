// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef _M_CEE // TRANSITION, VSO-1663233
#include <coroutine>

// Testing LWG-3330 "Include <compare> from most library headers" by intentionally NOT including <compare>

static_assert(std::is_eq(std::partial_ordering::equivalent));
#endif // _M_CEE

void test_coroutine() {}
