// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Included pretty much everywhere, and fails due to "std::nothrow_t", "std::align_val_t"
#include <vcruntime_new.h>
// Suppress other known failed headers
#define _INNER_STD_NAMESPACE_TEST

namespace X {
    namespace std {}
} // namespace X
using namespace X;

#include <__msvc_all_public_headers.hpp>

int main() {} // COMPILE-ONLY
