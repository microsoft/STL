// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version> // TRANSITION, GH-395
#ifdef __cpp_lib_concepts // TRANSITION, GH-395

#include <ranges>

// Testing LWG-3330 "Include <compare> from most library headers" by intentionally NOT including <compare>

static_assert(std::is_eq(std::partial_ordering::equivalent));

#endif // TRANSITION, GH-395

void test_ranges() {}
