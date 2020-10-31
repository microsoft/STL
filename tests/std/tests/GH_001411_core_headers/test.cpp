// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CORE_HEADERS_ONLY

#include <__msvc_all_public_headers.hpp>

#ifdef _YVALS
#error Core headers should not include <yvals.h>.
#endif

#include <chrono>

#ifndef _YVALS
#error Non-core headers like <chrono> should include <yvals.h>.
#endif

int main() {} // COMPILE-ONLY
