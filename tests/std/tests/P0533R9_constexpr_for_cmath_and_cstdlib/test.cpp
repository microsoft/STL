// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <type_traits>
using namespace std;

template <class T>
constexpr void test_classification_functions();

template <class T>
constexpr void test_comparison_functions();

constexpr bool test_cmath() {
#ifndef _MSVC_LIBC_MATH // TRANSITION, MSVC-PR-767184 fixing error LNK2005: frexp already defined in test.obj
    {
        int exponent = 0;
        assert(frexp(15.5f, &exponent) == 0.96875f);
        assert(exponent == 4);
        assert(frexp(17.5, &exponent) == 0.546875);
        assert(exponent == 5);
        assert(frexp(15.5l, &exponent) == 0.96875l);
        assert(exponent == 4);
        assert(frexpf(17.5f, &exponent) == 0.546875f);
        assert(exponent == 5);
        assert(frexpl(15.5l, &exponent) == 0.96875l);
        assert(exponent == 4);
        assert(frexp(1729, &exponent) == 0.84423828125);
        assert(exponent == 11);
    }
#endif // ^^^ no workaround ^^^

    assert(ilogb(0.1729f) == -3);
    assert(ilogb(0.1729) == -3);
    assert(ilogb(0.1729l) == -3);
    assert(ilogbf(0.1729f) == -3);
    assert(ilogbl(0.1729l) == -3);
    assert(ilogb(2097) == 11);

    assert(ldexp(1.09375f, 3) == 8.75f);
    assert(ldexp(1.09375, 3) == 8.75);
    assert(ldexp(1.09375l, 3) == 8.75l);
    assert(ldexpf(1.09375f, 3) == 8.75f);
    assert(ldexpl(1.09375l, 3) == 8.75l);
    assert(ldexp(1729, 3) == 13832.0);

    assert(logb(0.1729f) == -3.0f);
    assert(logb(0.1729) == -3.0);
    assert(logb(0.1729l) == -3.0l);
    assert(logbf(0.1729f) == -3.0f);
    assert(logbl(0.1729l) == -3.0l);
    assert(logb(2097) == 11.0);

    {
        float flt       = 0.0f;
        double dbl      = 0.0;
        long double lng = 0.0l;
        assert(modf(13.1875f, &flt) == 0.1875f);
        assert(flt == 13.0f);
        assert(modf(14.1875, &dbl) == 0.1875);
        assert(dbl == 14.0);
        assert(modf(15.1875l, &lng) == 0.1875l);
        assert(lng == 15.0l);
        assert(modff(16.1875f, &flt) == 0.1875f);
        assert(flt == 16.0f);
        assert(modfl(17.1875l, &lng) == 0.1875l);
        assert(lng == 17.0l);
        assert(modf(1729, &dbl) == 0.0);
        assert(dbl == 1729.0);
    }

    assert(scalbn(1.09375f, 3) == 8.75f);
    assert(scalbn(1.09375, 3) == 8.75);
    assert(scalbn(1.09375l, 3) == 8.75l);
    assert(scalbnf(1.09375f, 3) == 8.75f);
    assert(scalbnl(1.09375l, 3) == 8.75l);
    assert(scalbn(1729, 3) == 13832.0);

    assert(scalbln(1.09375f, 3L) == 8.75f);
    assert(scalbln(1.09375, 3L) == 8.75);
    assert(scalbln(1.09375l, 3L) == 8.75l);
    assert(scalblnf(1.09375f, 3L) == 8.75f);
    assert(scalblnl(1.09375l, 3L) == 8.75l);
    assert(scalbln(1729, 3L) == 13832.0);

    assert(fabs(-3.14f) == 3.14f);
    assert(fabs(-3.14) == 3.14);
    assert(fabs(-3.14l) == 3.14l);
    assert(fabsf(-3.14f) == 3.14f);
    assert(fabsl(-3.14l) == 3.14l);
    assert(fabs(-1729) == 1729.0);

    assert(ceil(3.14f) == 4.0f);
    assert(ceil(3.14) == 4.0);
    assert(ceil(3.14l) == 4.0l);
    assert(ceilf(3.14f) == 4.0f);
    assert(ceill(3.14l) == 4.0l);
    assert(ceil(1729) == 1729.0);

    assert(floor(7.89f) == 7.0f);
    assert(floor(7.89) == 7.0);
    assert(floor(7.89l) == 7.0l);
    assert(floorf(7.89f) == 7.0f);
    assert(floorl(7.89l) == 7.0l);
    assert(floor(1729) == 1729.0);

    assert(round(3.14f) == 3.0f);
    assert(round(3.14) == 3.0);
    assert(round(3.14l) == 3.0l);
    assert(roundf(3.14f) == 3.0f);
    assert(roundl(3.14l) == 3.0l);
    assert(round(1729) == 1729.0);

    assert(lround(3.14f) == 3L);
    assert(lround(3.14) == 3L);
    assert(lround(3.14l) == 3L);
    assert(lroundf(3.14f) == 3L);
    assert(lroundl(3.14l) == 3L);
    assert(lround(1729) == 1729L);

    assert(llround(0x1p60f) == 0x1000'0000'0000'0000LL);
    assert(llround(0x1p60) == 0x1000'0000'0000'0000LL);
    assert(llround(0x1p60l) == 0x1000'0000'0000'0000LL);
    assert(llroundf(0x1p60f) == 0x1000'0000'0000'0000LL);
    assert(llroundl(0x1p60l) == 0x1000'0000'0000'0000LL);
    assert(llround(1729) == 1729LL);

    assert(trunc(4.56f) == 4.0f);
    assert(trunc(4.56) == 4.0);
    assert(trunc(4.56l) == 4.0l);
    assert(truncf(4.56f) == 4.0f);
    assert(truncl(4.56l) == 4.0l);
    assert(trunc(1729) == 1729.0);

    assert(fmod(31.5f, 4.0f) == 3.5f);
    assert(fmod(31.5, 4.0) == 3.5);
    assert(fmod(31.5l, 4.0l) == 3.5l);
    assert(fmodf(31.5f, 4.0f) == 3.5f);
    assert(fmodl(31.5l, 4.0l) == 3.5l);
    assert(fmod(2097, 100) == 97.0);

    assert(remainder(31.5f, 4.0f) == -0.5f);
    assert(remainder(31.5, 4.0) == -0.5);
    assert(remainder(31.5l, 4.0l) == -0.5l);
    assert(remainderf(31.5f, 4.0f) == -0.5f);
    assert(remainderl(31.5l, 4.0l) == -0.5l);
    assert(remainder(2097, 100) == -3.0);

    {
        int quo = 0;
        assert(remquo(17.25f, 5.0f, &quo) == 2.25f);
        assert(quo == 3);
        assert(remquo(22.25, 5.0, &quo) == 2.25);
        assert(quo == 4);
        assert(remquo(17.25l, 5.0l, &quo) == 2.25l);
        assert(quo == 3);
        assert(remquof(22.25f, 5.0f, &quo) == 2.25f);
        assert(quo == 4);
        assert(remquol(17.25l, 5.0l, &quo) == 2.25l);
        assert(quo == 3);
        assert(remquo(1729, 400, &quo) == 129.0);
        assert(quo == 4);
    }

    assert(copysign(3.14f, -7.77f) == -3.14f);
    assert(copysign(3.14, -7.77) == -3.14);
    assert(copysign(3.14l, -7.77l) == -3.14l);
    assert(copysignf(3.14f, -7.77f) == -3.14f);
    assert(copysignl(3.14l, -7.77l) == -3.14l);
    assert(copysign(1729, -5) == -1729.0);

    assert(nextafter(3.0f, 0.0f) == 0x1.7ffffep+1f);
    assert(nextafter(3.0, 0.0) == 0x1.7ffffffffffffp+1);
    assert(nextafter(3.0l, 0.0l) == 0x1.7ffffffffffffp+1l);
    assert(nextafterf(3.0f, 0.0f) == 0x1.7ffffep+1f);
    assert(nextafterl(3.0l, 0.0l) == 0x1.7ffffffffffffp+1l);
    assert(nextafter(1729, 0) == 0x1.b03ffffffffffp+10);

    assert(nexttoward(3.0f, 0.0l) == 0x1.7ffffep+1f);
    assert(nexttoward(3.0, 0.0l) == 0x1.7ffffffffffffp+1);
    assert(nexttoward(3.0l, 0.0l) == 0x1.7ffffffffffffp+1l);
    assert(nexttowardf(3.0f, 0.0l) == 0x1.7ffffep+1f);
    assert(nexttowardl(3.0l, 0.0l) == 0x1.7ffffffffffffp+1l);
    assert(nexttoward(1729, 0.0l) == 0x1.b03ffffffffffp+10);

    assert(fdim(9.75f, 2.5f) == 7.25f);
    assert(fdim(9.75, 2.5) == 7.25);
    assert(fdim(9.75l, 2.5l) == 7.25l);
    assert(fdimf(9.75f, 2.5f) == 7.25f);
    assert(fdiml(9.75l, 2.5l) == 7.25l);
    assert(fdim(1729, 1700) == 29.0);

    assert(fmax(3.14f, 7.77f) == 7.77f);
    assert(fmax(3.14, 7.77) == 7.77);
    assert(fmax(3.14l, 7.77l) == 7.77l);
    assert(fmaxf(3.14f, 7.77f) == 7.77f);
    assert(fmaxl(3.14l, 7.77l) == 7.77l);
    assert(fmax(1729, 2097) == 2097.0);

    assert(fmin(3.14f, 7.77f) == 3.14f);
    assert(fmin(3.14, 7.77) == 3.14);
    assert(fmin(3.14l, 7.77l) == 3.14l);
    assert(fminf(3.14f, 7.77f) == 3.14f);
    assert(fminl(3.14l, 7.77l) == 3.14l);
    assert(fmin(1729, 2097) == 1729.0);

    assert(fma(5.125f, 3.0f, 2.5f) == 17.875f);
    assert(fma(5.125, 3.0, 2.5) == 17.875);
    assert(fma(5.125l, 3.0l, 2.5l) == 17.875l);
    assert(fmaf(5.125f, 3.0f, 2.5f) == 17.875f);
    assert(fmal(5.125l, 3.0l, 2.5l) == 17.875l);
    assert(fma(17, 100, 29) == 1729.0);

    test_classification_functions<float>();
    test_classification_functions<double>();
    test_classification_functions<long double>();
    test_classification_functions<int>();

#ifndef _MSVC_INTERNAL_TESTING // TRANSITION, MSVC-PR-767404 fixed the comparison functions in constant evaluation
    if !consteval
#endif // ^^^ workaround ^^^
    {
        test_comparison_functions<float>();
        test_comparison_functions<double>();
        test_comparison_functions<long double>();
        test_comparison_functions<int>();
    }

    return true;
}

template <class T>
constexpr void test_classification_functions() {
    {
        constexpr T zro{};
        constexpr T val = static_cast<T>(5);

        assert(fpclassify(zro) == FP_ZERO);
        assert(fpclassify(val) == FP_NORMAL);

        assert(isfinite(zro));
        assert(isfinite(val));

        assert(!isinf(zro));
        assert(!isinf(val));

        assert(!isnan(zro));
        assert(!isnan(val));

        assert(!isnormal(zro));
        assert(isnormal(val));
    }

    if constexpr (is_floating_point_v<T>) {
        constexpr T sub = numeric_limits<T>::denorm_min();
        constexpr T inf = numeric_limits<T>::infinity();
        constexpr T nan = numeric_limits<T>::quiet_NaN();

#ifndef _MSVC_INTERNAL_TESTING // TRANSITION, MSVC-PR-767404 fixed fpclassify for subnormal floats
        if constexpr (!is_same_v<T, float>)
#endif // ^^^ workaround ^^^
        {
            assert(fpclassify(sub) == FP_SUBNORMAL);
        }
        assert(fpclassify(inf) == FP_INFINITE);
        assert(fpclassify(nan) == FP_NAN);

        assert(isfinite(sub));
        assert(!isfinite(inf));
        assert(!isfinite(nan));

        assert(!isinf(sub));
        assert(isinf(inf));
        assert(!isinf(nan));

        assert(!isnan(sub));
        assert(!isnan(inf));
        assert(isnan(nan));

#ifndef _MSVC_INTERNAL_TESTING // TRANSITION, MSVC-PR-767404 fixed fpclassify for subnormal floats
        if constexpr (!is_same_v<T, float>)
#endif // ^^^ workaround ^^^
        {
            assert(!isnormal(sub));
        }
        assert(!isnormal(inf));
        assert(!isnormal(nan));
    }
}

template <class T>
constexpr void test_comparison_functions() {
    constexpr T lo = static_cast<T>(-3);
    constexpr T hi = static_cast<T>(4);

    assert(signbit(lo));
    assert(!signbit(hi));

    assert(!isgreater(lo, hi));
    assert(isgreater(hi, lo));
    assert(!isgreater(hi, hi));

    assert(!isgreaterequal(lo, hi));
    assert(isgreaterequal(hi, lo));
    assert(isgreaterequal(hi, hi));

    assert(isless(lo, hi));
    assert(!isless(hi, lo));
    assert(!isless(hi, hi));

    assert(islessequal(lo, hi));
    assert(!islessequal(hi, lo));
    assert(islessequal(hi, hi));

    assert(islessgreater(lo, hi));
    assert(islessgreater(hi, lo));
    assert(!islessgreater(hi, hi));

    assert(!isunordered(lo, hi));
    assert(!isunordered(hi, lo));
    assert(!isunordered(hi, hi));

    if constexpr (is_floating_point_v<T>) {
        constexpr T nan = numeric_limits<T>::quiet_NaN();

        assert(!isgreater(lo, nan));
        assert(!isgreater(nan, lo));
        assert(!isgreater(nan, nan));

        assert(!isgreaterequal(lo, nan));
        assert(!isgreaterequal(nan, lo));
        assert(!isgreaterequal(nan, nan));

        assert(!isless(lo, nan));
        assert(!isless(nan, lo));
        assert(!isless(nan, nan));

        assert(!islessequal(lo, nan));
        assert(!islessequal(nan, lo));
        assert(!islessequal(nan, nan));

        assert(!islessgreater(lo, nan));
        assert(!islessgreater(nan, lo));
        assert(!islessgreater(nan, nan));

        assert(isunordered(lo, nan));
        assert(isunordered(nan, lo));
        assert(isunordered(nan, nan));
    }
}

constexpr bool test_cstdlib() {
    assert(abs(-5) == 5);
    assert(abs(-5L) == 5L);
    assert(abs(-5LL) == 5LL);
    assert(abs(-5.0f) == 5.0f);
    assert(abs(-5.0) == 5.0);
    assert(abs(-5.0l) == 5.0l);

    assert(labs(-5L) == 5L);

    assert(llabs(-5LL) == 5LL);

    assert(div(1729, 100).quot == 17);
    assert(div(1729, 100).rem == 29);
    assert(div(1729L, 100L).quot == 17L);
    assert(div(1729L, 100L).rem == 29L);
    assert(div(1729LL, 100LL).quot == 17LL);
    assert(div(1729LL, 100LL).rem == 29LL);

    assert(ldiv(1729L, 100L).quot == 17L);
    assert(ldiv(1729L, 100L).rem == 29L);

    assert(lldiv(1729LL, 100LL).quot == 17LL);
    assert(lldiv(1729LL, 100LL).rem == 29LL);

    return true;
}

int main() {
#ifndef _M_ARM64EC // TRANSITION, reported many occurrences of error LNK2019: unresolved external symbol
    test_cmath();
    test_cstdlib();
#endif // ^^^ no workaround ^^^

#ifdef __cpp_lib_constexpr_cmath
    static_assert(test_cmath());
    static_assert(test_cstdlib());
#endif // ^^^ defined(__cpp_lib_constexpr_cmath) ^^^
}
