// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <limits>
#include <numeric>

using std::gcd;
using std::lcm;

constexpr auto int_min = std::numeric_limits<int>::min();
constexpr auto int_max = std::numeric_limits<int>::max();

static_assert(gcd(0, 0) == 0);
static_assert(gcd(0, 1) == 1);
static_assert(gcd(1, 0) == 1);
static_assert(gcd(0, 1) == 1);
static_assert(gcd(60, 24) == 12);
static_assert(gcd(196, 42) == 14);
static_assert(gcd(54, 24) == 6);

static_assert(gcd(0, -1) == 1);
static_assert(gcd(-1, 0) == 1);
static_assert(gcd(0, -1) == 1);
static_assert(gcd(-60, -24) == 12);
static_assert(gcd(-196, -42) == 14);
static_assert(gcd(-54, -24) == 6);

static_assert(gcd(0, 1) == 1);
static_assert(gcd(-1, 0) == 1);
static_assert(gcd(0, 1) == 1);
static_assert(gcd(-60, 24) == 12);
static_assert(gcd(-196, 42) == 14);
static_assert(gcd(-54, 24) == 6);

static_assert(gcd(1073741824, 536870912) == 536870912);
static_assert(gcd(1073741824, -536870912) == 536870912);
static_assert(gcd(-1073741824, 536870912) == 536870912);
static_assert(gcd(int_max, int_max) == int_max);
static_assert(gcd(int_min, int_max) == 1);
// gcd(int_min, int_min) -> undefined behavior
static_assert(gcd(int_min + 1, int_min + 1) == int_max);

static_assert(gcd(4294967296ULL, 65536L) == 65536ULL);
static_assert(gcd(4294967296ULL, 65536UL) == 65536ULL);

static_assert(lcm(0, 0) == 0);
static_assert(lcm(0, 1) == 0);
static_assert(lcm(1, 0) == 0);
static_assert(lcm(1073741824, 536870912) == 1073741824);
static_assert(lcm(1073741824, -536870912) == 1073741824);
static_assert(lcm(-1073741824, 536870912) == 1073741824);
