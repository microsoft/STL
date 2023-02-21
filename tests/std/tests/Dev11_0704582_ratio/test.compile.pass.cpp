// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <ratio>
#include <type_traits>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

STATIC_ASSERT(is_same_v<ratio_add<ratio<30, 8>, ratio<42, 27>>, ratio<191, 36>>);
STATIC_ASSERT(is_same_v<ratio_subtract<ratio<30, 8>, ratio<42, 27>>, ratio<79, 36>>);
STATIC_ASSERT(is_same_v<ratio_multiply<ratio<30, 8>, ratio<42, 27>>, ratio<35, 6>>);
STATIC_ASSERT(is_same_v<ratio_divide<ratio<30, 8>, ratio<27, 42>>, ratio<35, 6>>);

STATIC_ASSERT(ratio_equal_v<ratio<6, 10>, ratio<3, 5>>);
STATIC_ASSERT(!ratio_not_equal_v<ratio<6, 10>, ratio<3, 5>>);

const long long Two30 = 1LL << 30;
const long long Two32 = 1LL << 32;
typedef ratio<Two30, Two32> Quarter;
typedef ratio<Two32, Two30> Four;

STATIC_ASSERT(ratio_less_v<Quarter, Four>);
STATIC_ASSERT(ratio_less_equal_v<Quarter, Four>);
STATIC_ASSERT(ratio_greater_v<Four, Quarter>);
STATIC_ASSERT(ratio_greater_equal_v<Four, Quarter>);

STATIC_ASSERT(is_same_v<ratio_add<ratio<0, 5>, ratio<0, 5>>, ratio<0, 1>>);
STATIC_ASSERT(is_same_v<ratio_subtract<ratio<0, 5>, ratio<0, 5>>, ratio<0, 1>>);
STATIC_ASSERT(is_same_v<ratio_multiply<ratio<0, 5>, ratio<0, 5>>, ratio<0, 1>>);
STATIC_ASSERT(is_same_v<ratio_divide<ratio<0, 5>, ratio<7, 7>>, ratio<0, 1>>);
STATIC_ASSERT(ratio_equal_v<ratio<0, 5>, ratio<0, 5>>);
STATIC_ASSERT(!ratio_not_equal_v<ratio<0, 5>, ratio<0, 5>>);
STATIC_ASSERT(!ratio_less_v<ratio<0, 5>, ratio<0, 5>>);
STATIC_ASSERT(ratio_less_equal_v<ratio<0, 5>, ratio<0, 5>>);
STATIC_ASSERT(!ratio_greater_v<ratio<0, 5>, ratio<0, 5>>);
STATIC_ASSERT(ratio_greater_equal_v<ratio<0, 5>, ratio<0, 5>>);

// ratio_add must use typename ratio<>::type internally.
// This is forbidden from returning ratio<68, 30>.
STATIC_ASSERT(is_same_v<ratio_add<ratio<7, 6>, ratio<11, 10>>, ratio<34, 15>>);
