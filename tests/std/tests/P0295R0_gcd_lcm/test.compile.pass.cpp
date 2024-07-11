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
// gcd(int_min, int_max) -> undefined behavior
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

template <class T>
constexpr bool test_nonbool_integral_type() {
    static_assert(gcd(T{60}, T{24}) == T{12});
    static_assert(lcm(T{60}, T{24}) == T{120});
    return true;
}

static_assert(test_nonbool_integral_type<char>());
static_assert(test_nonbool_integral_type<wchar_t>());
#ifdef __cpp_char8_t
static_assert(test_nonbool_integral_type<char8_t>());
#endif // __cpp_char8_t
static_assert(test_nonbool_integral_type<char16_t>());
static_assert(test_nonbool_integral_type<char32_t>());
static_assert(test_nonbool_integral_type<signed char>());
static_assert(test_nonbool_integral_type<short>());
static_assert(test_nonbool_integral_type<int>());
static_assert(test_nonbool_integral_type<long>());
static_assert(test_nonbool_integral_type<long long>());
static_assert(test_nonbool_integral_type<unsigned char>());
static_assert(test_nonbool_integral_type<unsigned short>());
static_assert(test_nonbool_integral_type<unsigned int>());
static_assert(test_nonbool_integral_type<unsigned long>());
static_assert(test_nonbool_integral_type<unsigned long long>());
