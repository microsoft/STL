// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Defend against regression of VSO-938757 "Headers are unfriendly to clang-cl with SAL annotation re-use"
//
// Clang emits an error when it sees a non-standard attribute (__declspec()) before a standard attribute ([[nodiscard]])
// in a declaration. To support reuse of the STL's SAL annotations in Clang we order our various attribute declarations
// appropriately. This test guards against regression by compiling all headers with clang-cl after defining _PREFAST_ so
// as to trigger the aforementioned errors.

#include <__msvc_all_public_headers.hpp>
