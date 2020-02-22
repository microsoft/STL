// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// This header is included after one of <cmath>, <cstdlib>, or both, by different TUs
#pragma once

#include <type_traits>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

STATIC_ASSERT(std::is_same_v<decltype(abs(0)), int>);
STATIC_ASSERT(std::is_same_v<decltype(abs(0L)), long>);
STATIC_ASSERT(std::is_same_v<decltype(abs(0LL)), long long>);
STATIC_ASSERT(std::is_same_v<decltype(abs(0.0F)), float>);
STATIC_ASSERT(std::is_same_v<decltype(abs(0.0)), double>);
STATIC_ASSERT(std::is_same_v<decltype(abs(0.0L)), long double>);

#undef STATIC_ASSERT
