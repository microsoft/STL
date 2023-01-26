// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <limits>
#include <numeric>

using std::gcd;
using std::lcm;

constexpr auto int_min = std::numeric_limits<int>::min();
constexpr auto int_max = std::numeric_limits<int>::max();

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
STATIC_ASSERT(gcd(int_max, int_max) == int_max);
STATIC_ASSERT(gcd(int_min, int_max) == 1);
// gcd(int_min, int_min) -> undefined behavior
STATIC_ASSERT(gcd(int_min + 1, int_min + 1) == int_max);

STATIC_ASSERT(gcd(4294967296ULL, 65536L) == 65536ULL);
STATIC_ASSERT(gcd(4294967296ULL, 65536UL) == 65536ULL);

STATIC_ASSERT(lcm(0, 0) == 0);
STATIC_ASSERT(lcm(0, 1) == 0);
STATIC_ASSERT(lcm(1, 0) == 0);
STATIC_ASSERT(lcm(1073741824, 536870912) == 1073741824);
STATIC_ASSERT(lcm(1073741824, -536870912) == 1073741824);
STATIC_ASSERT(lcm(-1073741824, 536870912) == 1073741824);
