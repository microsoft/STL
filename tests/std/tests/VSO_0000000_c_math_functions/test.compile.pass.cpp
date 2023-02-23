// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#if defined(BOTH)
#include <cmath>
#include <cstdlib>
#elif defined(CMATH)
#include <cmath>
#elif defined(CSTDLIB)
#include <cstdlib>
#else
#error This test requires one of BOTH, CMATH, or CSTDLIB to be defined.
#endif

#include <type_traits>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

STATIC_ASSERT(std::is_same_v<decltype(abs(0)), int>);
STATIC_ASSERT(std::is_same_v<decltype(abs(0L)), long>);
STATIC_ASSERT(std::is_same_v<decltype(abs(0LL)), long long>);
STATIC_ASSERT(std::is_same_v<decltype(abs(0.0F)), float>);
STATIC_ASSERT(std::is_same_v<decltype(abs(0.0)), double>);
STATIC_ASSERT(std::is_same_v<decltype(abs(0.0L)), long double>);

#undef STATIC_ASSERT
