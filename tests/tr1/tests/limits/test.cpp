// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <limits>
#define TEST_NAME "<limits>"

#include "tdefs.h"
#include <float.h>
#include <limits.h>
#include <limits>
#include <wchar.h>

struct Mytype { // wrap an int
    Mytype(int valarg = 0) : val(valarg) { // construct from integer
    }

    bool operator==(const Mytype& right) const { // compare with right
        return val == right.val;
    }

    int val;
};

void test_main() { // test basic workings of limits definitions
    static const STD float_denorm_style fds[] = {STD denorm_indeterminate, STD denorm_absent, STD denorm_present};
    static const STD float_round_style frs[]  = {STD round_indeterminate, STD round_toward_zero, STD round_to_nearest,
         STD round_toward_infinity, STD round_toward_neg_infinity};
    int i, j, ok;

    for (ok = 1, i = sizeof(fds) / sizeof(fds[0]); 0 < i;) {
        for (j = --i; 0 < j;) { // test an (i, j) pair
            int testno = i * 0x100 + j;
            ok         = CHECK0(ok, testno, fds[i] != fds[--j]);
        }
    }
    CHECK_MSG("float_denorm_style values are distinct", ok);

    for (ok = 1, i = sizeof(frs) / sizeof(frs[0]); 0 < i;) {
        for (j = --i; 0 < j;) { // test an (i, j) pair
            int testno = i * 0x100 + j;
            ok         = CHECK0(ok, testno, frs[i] != frs[--j]);
        }
    }
    CHECK_MSG("float_round_style values are distinct", ok);

    { // check limits template
        typedef STD numeric_limits<Mytype> Lim;
        CHECK(!Lim::is_specialized);
        CHECK(!Lim::is_signed);
        CHECK(!Lim::is_integer);
        CHECK(!Lim::is_exact);
        CHECK(!Lim::is_bounded);
        CHECK(!Lim::is_modulo);

        CHECK(Lim::min() == Mytype(0));
        CHECK(Lim::max() == Mytype(0));
        CHECK(Lim::lowest() == Mytype(0));

        CHECK_INT(Lim::digits, 0);
        CHECK_INT(Lim::digits10, 0);
        CHECK_INT(Lim::max_digits10, 0);
        CHECK_INT(Lim::radix, 0);

        CHECK(!Lim::has_infinity);
        CHECK(!Lim::has_quiet_NaN);
        CHECK(!Lim::has_signaling_NaN);
        CHECK(!Lim::has_denorm_loss);
        CHECK(!Lim::is_iec559);

        CHECK(Lim::epsilon() == Mytype(0));
        CHECK(Lim::round_error() == Mytype(0));
        CHECK(Lim::infinity() == Mytype(0));
        CHECK(Lim::quiet_NaN() == Mytype(0));
        CHECK(Lim::signaling_NaN() == Mytype(0));
        CHECK(Lim::denorm_min() == Mytype(0));

        CHECK_INT(Lim::min_exponent, 0);
        CHECK_INT(Lim::min_exponent10, 0);
        CHECK_INT(Lim::max_exponent, 0);
        CHECK_INT(Lim::max_exponent10, 0);

        CHECK_INT(Lim::has_denorm, STD denorm_absent);
        CHECK_INT(Lim::round_style, STD round_toward_zero);
    }

    { // check limits<bool>
        typedef STD numeric_limits<bool> Lim;
        CHECK(Lim::is_specialized);
        CHECK(!Lim::is_signed);
        CHECK(Lim::is_integer);
        CHECK(Lim::is_exact);
        CHECK(Lim::is_bounded);
        CHECK(!Lim::is_modulo);

        CHECK_INT(Lim::min(), false);
        CHECK_INT(Lim::max(), true);
        CHECK(Lim::lowest() == Lim::min());

        CHECK_INT(Lim::digits, 1);
        CHECK_INT(Lim::digits10, 0);
        CHECK_INT(Lim::max_digits10, 0);
        CHECK_INT(Lim::radix, 2);
    }

    { // check limits<char>
        typedef STD numeric_limits<char> Lim;
        CHECK(Lim::is_specialized);
        CHECK_INT(Lim::is_signed, (CHAR_MIN != 0));
        CHECK(Lim::is_integer);
        CHECK(Lim::is_exact);
        CHECK(Lim::is_bounded);
        CHECK_INT(Lim::is_modulo, (CHAR_MIN == 0));

        CHECK_INT(Lim::min(), CHAR_MIN);
        CHECK_INT(Lim::max(), CHAR_MAX);
        CHECK(Lim::lowest() == Lim::min());

        CHECK_INT(Lim::digits, CHAR_BIT * sizeof(char) - (CHAR_MIN != 0 ? 1 : 0));
        CHECK_INT(Lim::digits10, (CHAR_BIT * sizeof(char) - (CHAR_MIN != 0 ? 1 : 0)) * 301L / 1000);
        CHECK_INT(Lim::max_digits10, 0);
        CHECK_INT(Lim::radix, 2);
    }

    { // check limits<signed char>
        typedef STD numeric_limits<signed char> Lim;
        CHECK(Lim::is_specialized);
        CHECK(Lim::is_signed);
        CHECK(Lim::is_integer);
        CHECK(Lim::is_exact);
        CHECK(Lim::is_bounded);
        CHECK(!Lim::is_modulo);

        CHECK_INT(Lim::min(), SCHAR_MIN);
        CHECK_INT(Lim::max(), SCHAR_MAX);
        CHECK(Lim::lowest() == Lim::min());

        CHECK_INT(Lim::digits, CHAR_BIT * sizeof(char) - 1);
        CHECK_INT(Lim::digits10, (CHAR_BIT * sizeof(char) - 1) * 301L / 1000);
        CHECK_INT(Lim::max_digits10, 0);
        CHECK_INT(Lim::radix, 2);
    }

    { // check limits<unsigned char>
        typedef STD numeric_limits<unsigned char> Lim;
        CHECK(Lim::is_specialized);
        CHECK(!Lim::is_signed);
        CHECK(Lim::is_integer);
        CHECK(Lim::is_exact);
        CHECK(Lim::is_bounded);
        CHECK(Lim::is_modulo);

        CHECK_INT(Lim::min(), 0);
        CHECK_INT(Lim::max(), UCHAR_MAX);
        CHECK(Lim::lowest() == Lim::min());

        CHECK_INT(Lim::digits, CHAR_BIT * sizeof(char));
        CHECK_INT(Lim::digits10, CHAR_BIT * sizeof(char) * 301L / 1000);
        CHECK_INT(Lim::max_digits10, 0);
        CHECK_INT(Lim::radix, 2);
    }

    { // check limits<wchar_t>
        typedef STD numeric_limits<wchar_t> Lim;
        CHECK(Lim::is_specialized);
        CHECK_INT(Lim::is_signed, (WCHAR_MIN != 0));
        CHECK(Lim::is_integer);
        CHECK(Lim::is_exact);
        CHECK(Lim::is_bounded);
        CHECK(Lim::is_modulo);

        CHECK_INT(Lim::min(), WCHAR_MIN);
        CHECK_INT(Lim::max(), WCHAR_MAX);
        CHECK(Lim::lowest() == Lim::min());

        CHECK_INT(Lim::digits, CHAR_BIT * sizeof(wchar_t) - (WCHAR_MIN != 0 ? 1 : 0));
        CHECK_INT(Lim::digits10, (CHAR_BIT * sizeof(wchar_t) - (WCHAR_MIN != 0 ? 1 : 0)) * 301L / 1000);
        CHECK_INT(Lim::max_digits10, 0);
        CHECK_INT(Lim::radix, 2);
    }

    { // check limits<signed short>
        typedef STD numeric_limits<signed short> Lim;
        CHECK(Lim::is_specialized);
        CHECK(Lim::is_signed);
        CHECK(Lim::is_integer);
        CHECK(Lim::is_exact);
        CHECK(Lim::is_bounded);
        CHECK(!Lim::is_modulo);

        CHECK_INT(Lim::min(), SHRT_MIN);
        CHECK_INT(Lim::max(), SHRT_MAX);
        CHECK(Lim::lowest() == Lim::min());

        CHECK_INT(Lim::digits, CHAR_BIT * sizeof(short) - 1);
        CHECK_INT(Lim::digits10, (CHAR_BIT * sizeof(short) - 1) * 301L / 1000);
        CHECK_INT(Lim::max_digits10, 0);
        CHECK_INT(Lim::radix, 2);
    }

    { // check limits<unsigned short>
        typedef STD numeric_limits<unsigned short> Lim;
        CHECK(Lim::is_specialized);
        CHECK(!Lim::is_signed);
        CHECK(Lim::is_integer);
        CHECK(Lim::is_exact);
        CHECK(Lim::is_bounded);
        CHECK(Lim::is_modulo);

        CHECK_INT(Lim::min(), 0);
        CHECK_INT(Lim::max(), USHRT_MAX);
        CHECK(Lim::lowest() == Lim::min());

        CHECK_INT(Lim::digits, CHAR_BIT * sizeof(short));
        CHECK_INT(Lim::digits10, CHAR_BIT * sizeof(short) * 301L / 1000);
        CHECK_INT(Lim::max_digits10, 0);
        CHECK_INT(Lim::radix, 2);
    }

    { // check limits<signed int>
        typedef STD numeric_limits<signed int> Lim;
        CHECK(Lim::is_specialized);
        CHECK(Lim::is_signed);
        CHECK(Lim::is_integer);
        CHECK(Lim::is_exact);
        CHECK(Lim::is_bounded);
        CHECK(!Lim::is_modulo);

        CHECK_INT(Lim::min(), INT_MIN);
        CHECK_INT(Lim::max(), INT_MAX);
        CHECK(Lim::lowest() == Lim::min());

        CHECK_INT(Lim::digits, CHAR_BIT * sizeof(int) - 1);
        CHECK_INT(Lim::digits10, (CHAR_BIT * sizeof(int) - 1) * 301L / 1000);
        CHECK_INT(Lim::max_digits10, 0);
        CHECK_INT(Lim::radix, 2);
    }

    { // check limits<unsigned int>
        typedef STD numeric_limits<unsigned int> Lim;
        CHECK(Lim::is_specialized);
        CHECK(!Lim::is_signed);
        CHECK(Lim::is_integer);
        CHECK(Lim::is_exact);
        CHECK(Lim::is_bounded);
        CHECK(Lim::is_modulo);

        CHECK_INT(Lim::min(), 0);
        CHECK_INT(Lim::max(), UINT_MAX);
        CHECK(Lim::lowest() == Lim::min());

        CHECK_INT(Lim::digits, CHAR_BIT * sizeof(int));
        CHECK_INT(Lim::digits10, CHAR_BIT * sizeof(int) * 301L / 1000);
        CHECK_INT(Lim::max_digits10, 0);
        CHECK_INT(Lim::radix, 2);
    }

    { // check limits<signed long>
        typedef STD numeric_limits<signed long> Lim;
        CHECK(Lim::is_specialized);
        CHECK(Lim::is_signed);
        CHECK(Lim::is_integer);
        CHECK(Lim::is_exact);
        CHECK(Lim::is_bounded);
        CHECK(!Lim::is_modulo);

        CHECK(Lim::min() == LONG_MIN);
        CHECK(Lim::max() == LONG_MAX);
        CHECK(Lim::lowest() == Lim::min());

        CHECK_INT(Lim::digits, CHAR_BIT * sizeof(long) - 1);
        CHECK_INT(Lim::digits10, (CHAR_BIT * sizeof(long) - 1) * 301L / 1000);
        CHECK_INT(Lim::max_digits10, 0);
        CHECK_INT(Lim::radix, 2);
    }

    { // check limits<unsigned long>
        typedef STD numeric_limits<unsigned long> Lim;
        CHECK(Lim::is_specialized);
        CHECK(!Lim::is_signed);
        CHECK(Lim::is_integer);
        CHECK(Lim::is_exact);
        CHECK(Lim::is_bounded);
        CHECK(Lim::is_modulo);

        CHECK_INT(Lim::min(), 0);
        CHECK(Lim::max() == ULONG_MAX);
        CHECK(Lim::lowest() == Lim::min());

        CHECK_INT(Lim::digits, CHAR_BIT * sizeof(long));
        CHECK_INT(Lim::digits10, CHAR_BIT * sizeof(long) * 301L / 1000);
        CHECK_INT(Lim::max_digits10, 0);
        CHECK_INT(Lim::radix, 2);
    }

    { // check limits<signed long long>
        typedef STD numeric_limits<long long> Lim;
        CHECK(Lim::is_specialized);
        CHECK(Lim::is_signed);
        CHECK(Lim::is_integer);
        CHECK(Lim::is_exact);
        CHECK(Lim::is_bounded);
        CHECK(!Lim::is_modulo);

        CHECK(Lim::min() == LLONG_MIN);
        CHECK(Lim::max() == LLONG_MAX);
        CHECK(Lim::lowest() == Lim::min());

        CHECK_INT(Lim::digits, CHAR_BIT * sizeof(long long) - 1);
        CHECK_INT(Lim::digits10, (CHAR_BIT * sizeof(long long) - 1) * 301L / 1000);
        CHECK_INT(Lim::max_digits10, 0);
        CHECK_INT(Lim::radix, 2);
    }

    { // check limits<unsigned long long>
        typedef STD numeric_limits<unsigned long long> Lim;
        CHECK(Lim::is_specialized);
        CHECK(!Lim::is_signed);
        CHECK(Lim::is_integer);
        CHECK(Lim::is_exact);
        CHECK(Lim::is_bounded);
        CHECK(Lim::is_modulo);

        CHECK(Lim::min() == 0);
        CHECK(Lim::max() == ULLONG_MAX);
        CHECK(Lim::lowest() == Lim::min());

        CHECK_INT(Lim::digits, CHAR_BIT * sizeof(long long));
        CHECK_INT(Lim::digits10, CHAR_BIT * sizeof(long long) * 301L / 1000);
        CHECK_INT(Lim::max_digits10, 0);
        CHECK_INT(Lim::radix, 2);
    }

    { // check limits<float>
        typedef STD numeric_limits<float> Lim;
        CHECK(Lim::is_specialized);
        CHECK(Lim::is_signed);
        CHECK(!Lim::is_integer);
        CHECK(!Lim::is_exact);
        CHECK(Lim::is_bounded);
        CHECK(!Lim::is_modulo);

        CHECK(Lim::min() == FLT_MIN);
        CHECK(Lim::max() == FLT_MAX);
        CHECK(Lim::lowest() == -Lim::max());
        CHECK(Lim::epsilon() == FLT_EPSILON);

        CHECK_INT(Lim::digits, FLT_MANT_DIG);
        CHECK_INT(Lim::digits10, FLT_DIG);
        CHECK_INT(Lim::max_digits10, 2 + FLT_MANT_DIG * 301L / 1000);

        CHECK_INT(Lim::max_exponent, FLT_MAX_EXP);
        CHECK_INT(Lim::max_exponent10, FLT_MAX_10_EXP);
        CHECK_INT(Lim::min_exponent, FLT_MIN_EXP);
        CHECK_INT(Lim::min_exponent10, FLT_MIN_10_EXP);
    }

    { // check limits<double>
        typedef STD numeric_limits<double> Lim;
        CHECK(Lim::is_specialized);
        CHECK(Lim::is_signed);
        CHECK(!Lim::is_integer);
        CHECK(!Lim::is_exact);
        CHECK(Lim::is_bounded);
        CHECK(!Lim::is_modulo);

        CHECK(Lim::min() == DBL_MIN);
        CHECK(Lim::max() == DBL_MAX);
        CHECK(Lim::lowest() == -Lim::max());
        CHECK(Lim::epsilon() == DBL_EPSILON);

        CHECK_INT(Lim::digits, DBL_MANT_DIG);
        CHECK_INT(Lim::digits10, DBL_DIG);
        CHECK_INT(Lim::max_digits10, 2 + DBL_MANT_DIG * 301L / 1000);

        CHECK_INT(Lim::max_exponent, DBL_MAX_EXP);
        CHECK_INT(Lim::max_exponent10, DBL_MAX_10_EXP);
        CHECK_INT(Lim::min_exponent, DBL_MIN_EXP);
        CHECK_INT(Lim::min_exponent10, DBL_MIN_10_EXP);
    }

    { // check limits<long double>
        typedef STD numeric_limits<long double> Lim;
        CHECK(Lim::is_specialized);
        CHECK(Lim::is_signed);
        CHECK(!Lim::is_integer);
        CHECK(!Lim::is_exact);
        CHECK(Lim::is_bounded);
        CHECK(!Lim::is_modulo);

        CHECK(Lim::min() == LDBL_MIN);
        CHECK(Lim::max() == LDBL_MAX);
        CHECK(Lim::lowest() == -Lim::max());
        CHECK(Lim::epsilon() == LDBL_EPSILON);

        CHECK_INT(Lim::digits, LDBL_MANT_DIG);
        CHECK_INT(Lim::digits10, LDBL_DIG);
        CHECK_INT(Lim::max_digits10, 2 + LDBL_MANT_DIG * 301L / 1000);

        CHECK_INT(Lim::max_exponent, LDBL_MAX_EXP);
        CHECK_INT(Lim::max_exponent10, LDBL_MAX_10_EXP);
        CHECK_INT(Lim::min_exponent, LDBL_MIN_EXP);
        CHECK_INT(Lim::min_exponent10, LDBL_MIN_10_EXP);
    }
}
