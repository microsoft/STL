// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// GH-6533: <math.h> under /Zc:cmath should provide the abs() overloads that <cmath> declares

#ifdef STDLIB_H_BEFORE
#include <stdlib.h>
#endif // ^^^ defined(STDLIB_H_BEFORE) ^^^

#include <math.h>

#if defined(STDLIB_H_AFTER)
#include <stdlib.h>
#elif defined(CMATH_AFTER)
#include <cmath>
#endif // ^^^ defined(CMATH_AFTER) ^^^

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <class T, class U>
constexpr bool is_same_v = false;
template <class T>
constexpr bool is_same_v<T, T> = true;

// The UCRT's <math.h> declares these, so they're available in every mode.
STATIC_ASSERT(is_same_v<decltype(abs(0)), int>);
STATIC_ASSERT(is_same_v<decltype(labs(0L)), long>);
STATIC_ASSERT(is_same_v<decltype(llabs(0LL)), long long>);

#ifdef _MSVC_LIBC_MATH
STATIC_ASSERT(is_same_v<decltype(abs(0L)), long>);
STATIC_ASSERT(is_same_v<decltype(abs(0LL)), long long>);
STATIC_ASSERT(is_same_v<decltype(abs(0.0F)), float>);
STATIC_ASSERT(is_same_v<decltype(abs(0.0)), double>);
STATIC_ASSERT(is_same_v<decltype(abs(0.0L)), long double>);

#if _HAS_CXX23
STATIC_ASSERT(abs(-1) == 1);
STATIC_ASSERT(abs(-2L) == 2L);
STATIC_ASSERT(abs(-3'000'000'000LL) == 3'000'000'000LL);
STATIC_ASSERT(abs(-1.5F) == 1.5F);
STATIC_ASSERT(abs(-2.5) == 2.5);
STATIC_ASSERT(abs(-3.5L) == 3.5L);
STATIC_ASSERT(labs(-4L) == 4L);
STATIC_ASSERT(llabs(-3'000'000'000LL) == 3'000'000'000LL);
#endif // ^^^ _HAS_CXX23 ^^^
#endif // ^^^ defined(_MSVC_LIBC_MATH) ^^^
