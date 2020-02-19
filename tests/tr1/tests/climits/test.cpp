// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <climits>
#define TEST_NAME "<climits>"

#include "tdefs.h"
#include <climits>

void test_cpp() { // test C++ header
    static const char char_val[]         = {CHAR_MAX, CHAR_MIN};
    static const signed char schar_val[] = {SCHAR_MAX, SCHAR_MIN};
    static const short shrt_val[]        = {SHRT_MAX, SHRT_MIN};
    static const int int_val[]           = {INT_MAX, INT_MIN};
    static const long long_val[]         = {LONG_MAX, LONG_MIN};

    static const unsigned char uchar_val[]  = {UCHAR_MAX};
    static const unsigned short ushrt_val[] = {USHRT_MAX};
    static const unsigned int uint_val[]    = {UINT_MAX};
    static const unsigned long ulong_val[]  = {ULONG_MAX};

    CHECK_INT(char_val[0], CHAR_MAX);
    CHECK_INT(char_val[1], CHAR_MIN);
    CHECK_INT(schar_val[0], SCHAR_MAX);
    CHECK_INT(schar_val[1], SCHAR_MIN);
    CHECK_INT(shrt_val[0], SHRT_MAX);
    CHECK_INT(shrt_val[1], SHRT_MIN);
    CHECK_INT(int_val[0], INT_MAX);
    CHECK_INT(int_val[1], INT_MIN);
    CHECK(long_val[0] == LONG_MAX);
    CHECK(long_val[1] == LONG_MIN);

    CHECK(uchar_val[0] == UCHAR_MAX);
    CHECK(ushrt_val[0] == USHRT_MAX);
    CHECK(uint_val[0] == UINT_MAX);
    CHECK(ulong_val[0] == ULONG_MAX);

    static const long long long_long_val[]           = {LLONG_MAX, LLONG_MIN}; // C99 only
    static const unsigned long long ulong_long_val[] = {ULLONG_MAX};

    CHECK(long_long_val[0] == LLONG_MAX);
    CHECK(long_long_val[1] == LLONG_MIN);
    CHECK(ulong_long_val[0] == ULLONG_MAX);
}

void test_main() { // test basic workings of climits definitions
    test_cpp();
}
