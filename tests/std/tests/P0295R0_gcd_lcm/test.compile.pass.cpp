// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <limits.h>
#include <numeric>

using std::gcd;
using std::lcm;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

int main() {} // COMPILE-ONLY

STATIC_ASSERT(gcd(0, 0) == 0);
STATIC_ASSERT(gcd(0, 1) == 1);
STATIC_ASSERT(gcd(1, 0) == 1);
STATIC_ASSERT(gcd(0, 1) == 1);
STATIC_ASSERT(gcd(60, 24) == 12);
STATIC_ASSERT(gcd(196, 42) == 14);
STATIC_ASSERT(gcd(54, 24) == 6);

STATIC_ASSERT(gcd(0, -1) == 1);
STATIC_ASSERT(gcd(-1, 0) == 1);
STATIC_ASSERT(gcd(0, -1) == 1);
STATIC_ASSERT(gcd(-60, -24) == 12);
STATIC_ASSERT(gcd(-196, -42) == 14);
STATIC_ASSERT(gcd(-54, -24) == 6);

STATIC_ASSERT(gcd(0, 1) == 1);
STATIC_ASSERT(gcd(-1, 0) == 1);
STATIC_ASSERT(gcd(0, 1) == 1);
STATIC_ASSERT(gcd(-60, 24) == 12);
STATIC_ASSERT(gcd(-196, 42) == 14);
STATIC_ASSERT(gcd(-54, 24) == 6);

STATIC_ASSERT(gcd(1073741824, 536870912) == 536870912);
STATIC_ASSERT(gcd(1073741824, -536870912) == 536870912);
STATIC_ASSERT(gcd(-1073741824, 536870912) == 536870912);
STATIC_ASSERT(gcd(INT_MAX, INT_MAX) == INT_MAX);
STATIC_ASSERT(gcd(INT_MIN, INT_MAX) == 1);
// gcd(INT_MIN, INT_MIN) -> undefined behavior
STATIC_ASSERT(gcd(INT_MIN + 1, INT_MIN + 1) == INT_MAX);

STATIC_ASSERT(gcd(4294967296ULL, 65536L) == 65536ULL);
STATIC_ASSERT(gcd(4294967296ULL, 65536UL) == 65536ULL);

STATIC_ASSERT(lcm(0, 0) == 0);
STATIC_ASSERT(lcm(0, 1) == 0);
STATIC_ASSERT(lcm(1, 0) == 0);
STATIC_ASSERT(lcm(1073741824, 536870912) == 1073741824);
STATIC_ASSERT(lcm(1073741824, -536870912) == 1073741824);
STATIC_ASSERT(lcm(-1073741824, 536870912) == 1073741824);
