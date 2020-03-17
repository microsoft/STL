// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version> // TRANSITION, EDG support for concepts
#ifdef __cpp_lib_concepts // TRANSITION, EDG support for concepts

#include <ranges>

// Testing LWG-3330 "Include <compare> from most library headers" by intentionally NOT including <compare>

static_assert(std::is_eq(std::partial_ordering::equivalent));

#endif // TRANSITION, EDG support for concepts

void test_ranges() {}
