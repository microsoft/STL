// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <cfloat>
#define TEST_NAME "<cfloat>"

#include "tdefs.h"
#include <cfloat>

void test_cpp() { // test C++ header
    static int radix = FLT_RADIX;

    CHECK_INT(radix, FLT_RADIX);
    CHECK(-1 <= FLT_ROUNDS && FLT_ROUNDS <= 3);

    CHECK(10 <= DBL_DIG);
    CHECK(DBL_EPSILON <= 1e-9);
    CHECK(0 < DBL_MANT_DIG);
    CHECK(0 < DBL_DIG);
    CHECK(1e37 < DBL_MAX);
    CHECK(37 <= DBL_MAX_10_EXP);
    CHECK(DBL_MIN <= 1e-37);
    CHECK(DBL_MIN_10_EXP <= -37);

    CHECK(6 <= FLT_DIG);
    CHECK(FLT_EPSILON <= 1e-5);
    CHECK(0 < FLT_MANT_DIG);
    CHECK(0 < FLT_DIG);
    CHECK(1e37 < FLT_MAX);
    CHECK(37 <= FLT_MAX_10_EXP);
    CHECK(FLT_MIN <= 1e-37);
    CHECK(FLT_MIN_10_EXP <= -37);

    CHECK(10 <= LDBL_DIG);
    CHECK(LDBL_EPSILON <= 1e-9);
    CHECK(0 < LDBL_MANT_DIG);
    CHECK(0 < LDBL_DIG);
    CHECK(1e37 < LDBL_MAX);
    CHECK(37 <= LDBL_MAX_10_EXP);
    CHECK(LDBL_MIN <= 1e-37);
    CHECK(LDBL_MIN_10_EXP <= -37);

    CHECK(10 <= DECIMAL_DIG); // C99 only
    CHECK(-1 <= FLT_EVAL_METHOD && FLT_EVAL_METHOD <= 2);
}

void test_main() { // test basic workings of cfloat definitions
    test_cpp();
}
