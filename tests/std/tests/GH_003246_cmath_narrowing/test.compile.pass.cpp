// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cmath>

// Ensure the compiler doesn't warn about narrowing long double to double in <cmath> GENERIC_MATH2 templates
#define TEST(meow)                           \
    long double test_##meow(long double x) { \
        return std::meow(x, 1);              \
    }

TEST(atan2)
TEST(hypot)
TEST(pow)
TEST(fmod)
TEST(remainder)
TEST(copysign)
TEST(nextafter)
TEST(fdim)
TEST(fmax)
TEST(fmin)
