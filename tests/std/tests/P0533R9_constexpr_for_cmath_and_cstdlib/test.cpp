// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// TRANSITION, MSVC frontend needs to intercept inclusions of <inttypes.h>:
#ifndef _M_CEE_PURE
#include <__msvc_inttypes.hpp>
#endif
// ^^^ workaround ^^^

#include <cassert>
#include <cinttypes>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <type_traits>
using namespace std;

template <class T>
constexpr void test_classification_functions_cxx23();

template <class T>
constexpr void test_comparison_functions_cxx23();

constexpr bool test_cmath_cxx23() {
#if defined(_MSVC_INTERNAL_TESTING) || !defined(_MSVC_LIBC_MATH) // TRANSITION, MSVC-PR-767184/772024 fixed LNK2005
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

    test_classification_functions_cxx23<float>();
    test_classification_functions_cxx23<double>();
    test_classification_functions_cxx23<long double>();
    test_classification_functions_cxx23<int>();

#ifndef _MSVC_INTERNAL_TESTING // TRANSITION, MSVC-PR-767404 fixed the comparison functions in constant evaluation
    if !consteval
#endif // ^^^ workaround ^^^
    {
        test_comparison_functions_cxx23<float>();
        test_comparison_functions_cxx23<double>();
        test_comparison_functions_cxx23<long double>();
        test_comparison_functions_cxx23<int>();
    }

    return true;
}

template <class T>
constexpr void test_classification_functions_cxx23() {
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
constexpr void test_comparison_functions_cxx23() {
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

constexpr bool test_cstdlib_cxx23() {
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

/* CONSTEXPR_CMATH23 */ constexpr bool test_cinttypes_cxx23() {
    // Test LWG-3834 "Missing constexpr for std::intmax_t math functions in <cinttypes>".

    assert(imaxabs(-5LL) == 5LL);

    assert(imaxdiv(1729LL, 100LL).quot == 17LL);
    assert(imaxdiv(1729LL, 100LL).rem == 29LL);

    return true;
}

constexpr bool test_cmath_cxx26() {
    // These tests use round() because they're checking for basic functionality, not precision.

    assert(round(acos(0.6f) * 1000.0f) == 927.0f);
    assert(round(acos(0.6) * 1000.0) == 927.0);
    assert(round(acos(0.6l) * 1000.0l) == 927.0l);
    assert(round(acosf(0.6f) * 1000.0f) == 927.0f);
    assert(round(acosl(0.6l) * 1000.0l) == 927.0l);
    assert(round(acos(-1) * 1000.0) == 3142.0);

    assert(round(asin(0.6f) * 1000.0f) == 644.0f);
    assert(round(asin(0.6) * 1000.0) == 644.0);
    assert(round(asin(0.6l) * 1000.0l) == 644.0l);
    assert(round(asinf(0.6f) * 1000.0f) == 644.0f);
    assert(round(asinl(0.6l) * 1000.0l) == 644.0l);
    assert(round(asin(1) * 1000.0) == 1571.0);

    assert(round(atan(0.6f) * 1000.0f) == 540.0f);
    assert(round(atan(0.6) * 1000.0) == 540.0);
    assert(round(atan(0.6l) * 1000.0l) == 540.0l);
    assert(round(atanf(0.6f) * 1000.0f) == 540.0f);
    assert(round(atanl(0.6l) * 1000.0l) == 540.0l);
    assert(round(atan(1) * 1000.0) == 785.0);

    assert(round(atan2(0.06f, 0.1f) * 1000.0f) == 540.0f);
    assert(round(atan2(0.06, 0.1) * 1000.0) == 540.0);
    assert(round(atan2(0.06l, 0.1l) * 1000.0l) == 540.0l);
    assert(round(atan2f(0.06f, 0.1f) * 1000.0f) == 540.0f);
    assert(round(atan2l(0.06l, 0.1l) * 1000.0l) == 540.0l);
    assert(round(atan2(17, 29) * 1000.0) == 530.0);

    assert(round(cos(0.6f) * 1000.0f) == 825.0f);
    assert(round(cos(0.6) * 1000.0) == 825.0);
    assert(round(cos(0.6l) * 1000.0l) == 825.0l);
    assert(round(cosf(0.6f) * 1000.0f) == 825.0f);
    assert(round(cosl(0.6l) * 1000.0l) == 825.0l);
    assert(round(cos(1729) * 1000.0) == 432.0);

    assert(round(sin(0.6f) * 1000.0f) == 565.0f);
    assert(round(sin(0.6) * 1000.0) == 565.0);
    assert(round(sin(0.6l) * 1000.0l) == 565.0l);
    assert(round(sinf(0.6f) * 1000.0f) == 565.0f);
    assert(round(sinl(0.6l) * 1000.0l) == 565.0l);
    assert(round(sin(1729) * 1000.0) == 902.0);

    assert(round(tan(0.6f) * 1000.0f) == 684.0f);
    assert(round(tan(0.6) * 1000.0) == 684.0);
    assert(round(tan(0.6l) * 1000.0l) == 684.0l);
    assert(round(tanf(0.6f) * 1000.0f) == 684.0f);
    assert(round(tanl(0.6l) * 1000.0l) == 684.0l);
    assert(round(tan(1729) * 1000.0) == 2087.0);

    if !consteval { // TRANSITION, GH-3789
        assert(round(acosh(3.3f) * 1000.0f) == 1863.0f);
        assert(round(acosh(3.3) * 1000.0) == 1863.0);
        assert(round(acosh(3.3l) * 1000.0l) == 1863.0l);
        assert(round(acoshf(3.3f) * 1000.0f) == 1863.0f);
        assert(round(acoshl(3.3l) * 1000.0l) == 1863.0l);
        assert(round(acosh(7) * 1000.0) == 2634.0);

        assert(round(asinh(3.3f) * 1000.0f) == 1909.0f);
        assert(round(asinh(3.3) * 1000.0) == 1909.0);
        assert(round(asinh(3.3l) * 1000.0l) == 1909.0l);
        assert(round(asinhf(3.3f) * 1000.0f) == 1909.0f);
        assert(round(asinhl(3.3l) * 1000.0l) == 1909.0l);
        assert(round(asinh(7) * 1000.0) == 2644.0);

        assert(round(atanh(0.6f) * 1000.0f) == 693.0f);
        assert(round(atanh(0.6) * 1000.0) == 693.0);
        assert(round(atanh(0.6l) * 1000.0l) == 693.0l);
        assert(round(atanhf(0.6f) * 1000.0f) == 693.0f);
        assert(round(atanhl(0.6l) * 1000.0l) == 693.0l);
        assert(round(atanh(0) * 1000.0) == 0.0);

        assert(round(cosh(0.6f) * 1000.0f) == 1185.0f);
        assert(round(cosh(0.6) * 1000.0) == 1185.0);
        assert(round(cosh(0.6l) * 1000.0l) == 1185.0l);
        assert(round(coshf(0.6f) * 1000.0f) == 1185.0f);
        assert(round(coshl(0.6l) * 1000.0l) == 1185.0l);
        assert(round(cosh(2) * 1000.0) == 3762.0);

        assert(round(sinh(0.6f) * 1000.0f) == 637.0f);
        assert(round(sinh(0.6) * 1000.0) == 637.0);
        assert(round(sinh(0.6l) * 1000.0l) == 637.0l);
        assert(round(sinhf(0.6f) * 1000.0f) == 637.0f);
        assert(round(sinhl(0.6l) * 1000.0l) == 637.0l);
        assert(round(sinh(2) * 1000.0) == 3627.0);

        assert(round(tanh(0.6f) * 1000.0f) == 537.0f);
        assert(round(tanh(0.6) * 1000.0) == 537.0);
        assert(round(tanh(0.6l) * 1000.0l) == 537.0l);
        assert(round(tanhf(0.6f) * 1000.0f) == 537.0f);
        assert(round(tanhl(0.6l) * 1000.0l) == 537.0l);
        assert(round(tanh(2) * 1000.0) == 964.0);
    }

    assert(round(exp(0.6f) * 1000.0f) == 1822.0f);
    assert(round(exp(0.6) * 1000.0) == 1822.0);
    assert(round(exp(0.6l) * 1000.0l) == 1822.0l);
    assert(round(expf(0.6f) * 1000.0f) == 1822.0f);
    assert(round(expl(0.6l) * 1000.0l) == 1822.0l);
    assert(round(exp(2) * 1000.0) == 7389.0);

    if !consteval { // TRANSITION, GH-3789
        assert(round(exp2(0.6f) * 1000.0f) == 1516.0f);
        assert(round(exp2(0.6) * 1000.0) == 1516.0);
        assert(round(exp2(0.6l) * 1000.0l) == 1516.0l);
        assert(round(exp2f(0.6f) * 1000.0f) == 1516.0f);
        assert(round(exp2l(0.6l) * 1000.0l) == 1516.0l);
        assert(round(exp2(-3) * 1000.0) == 125.0);
    }

    assert(round(expm1(0.6f) * 1000.0f) == 822.0f);
    assert(round(expm1(0.6) * 1000.0) == 822.0);
    assert(round(expm1(0.6l) * 1000.0l) == 822.0l);
    assert(round(expm1f(0.6f) * 1000.0f) == 822.0f);
    assert(round(expm1l(0.6l) * 1000.0l) == 822.0l);
    assert(round(expm1(2) * 1000.0) == 6389.0);

    assert(round(log(0.6f) * 1000.0f) == -511.0f);
    assert(round(log(0.6) * 1000.0) == -511.0);
    assert(round(log(0.6l) * 1000.0l) == -511.0l);
    assert(round(logf(0.6f) * 1000.0f) == -511.0f);
    assert(round(logl(0.6l) * 1000.0l) == -511.0l);
    assert(round(log(7) * 1000.0) == 1946.0);

    assert(round(log10(0.6f) * 1000.0f) == -222.0f);
    assert(round(log10(0.6) * 1000.0) == -222.0);
    assert(round(log10(0.6l) * 1000.0l) == -222.0l);
    assert(round(log10f(0.6f) * 1000.0f) == -222.0f);
    assert(round(log10l(0.6l) * 1000.0l) == -222.0l);
    assert(round(log10(7) * 1000.0) == 845.0);

    assert(round(log1p(0.6f) * 1000.0f) == 470.0f);
    assert(round(log1p(0.6) * 1000.0) == 470.0);
    assert(round(log1p(0.6l) * 1000.0l) == 470.0l);
    assert(round(log1pf(0.6f) * 1000.0f) == 470.0f);
    assert(round(log1pl(0.6l) * 1000.0l) == 470.0l);
    assert(round(log1p(7) * 1000.0) == 2079.0);

    assert(round(log2(0.6f) * 1000.0f) == -737.0f);
    assert(round(log2(0.6) * 1000.0) == -737.0);
    assert(round(log2(0.6l) * 1000.0l) == -737.0l);
    assert(round(log2f(0.6f) * 1000.0f) == -737.0f);
    assert(round(log2l(0.6l) * 1000.0l) == -737.0l);
    assert(round(log2(7) * 1000.0) == 2807.0);

    assert(round(cbrt(0.6f) * 1000.0f) == 843.0f);
    assert(round(cbrt(0.6) * 1000.0) == 843.0);
    assert(round(cbrt(0.6l) * 1000.0l) == 843.0l);
    assert(round(cbrtf(0.6f) * 1000.0f) == 843.0f);
    assert(round(cbrtl(0.6l) * 1000.0l) == 843.0l);
    assert(round(cbrt(7) * 1000.0) == 1913.0);

    assert(round(hypot(3.3f, 7.7f) * 1000.0f) == 8377.0f);
    assert(round(hypot(3.3, 7.7) * 1000.0) == 8377.0);
    assert(round(hypot(3.3l, 7.7l) * 1000.0l) == 8377.0l);
    assert(round(hypotf(3.3f, 7.7f) * 1000.0f) == 8377.0f);
    assert(round(hypotl(3.3l, 7.7l) * 1000.0l) == 8377.0l);
    assert(round(hypot(3, 7) * 1000.0) == 7616.0);

    if !consteval { // TRANSITION, GH-3789
        assert(round(hypot(2.2f, 3.3f, 4.4f) * 1000.0f) == 5924.0f);
        assert(round(hypot(2.2, 3.3, 4.4) * 1000.0) == 5924.0);
        assert(round(hypot(2.2l, 3.3l, 4.4l) * 1000.0l) == 5924.0l);
        assert(round(hypot(2, 3, 4) * 1000.0) == 5385.0);
        // No 3-arg overloads for hypotf() and hypotl()
    }

    assert(round(pow(3.3f, 0.6f) * 1000.0f) == 2047.0f);
    assert(round(pow(3.3, 0.6) * 1000.0) == 2047.0);
    assert(round(pow(3.3l, 0.6l) * 1000.0l) == 2047.0l);
    assert(round(powf(3.3f, 0.6f) * 1000.0f) == 2047.0f);
    assert(round(powl(3.3l, 0.6l) * 1000.0l) == 2047.0l);
    assert(round(pow(2, -3) * 1000.0) == 125.0);

    assert(round(sqrt(0.6f) * 1000.0f) == 775.0f);
    assert(round(sqrt(0.6) * 1000.0) == 775.0);
    assert(round(sqrt(0.6l) * 1000.0l) == 775.0l);
    assert(round(sqrtf(0.6f) * 1000.0f) == 775.0f);
    assert(round(sqrtl(0.6l) * 1000.0l) == 775.0l);
    assert(round(sqrt(7) * 1000.0) == 2646.0);

    if !consteval { // TRANSITION, GH-3789
        assert(round(erf(0.6f) * 1000.0f) == 604.0f);
        assert(round(erf(0.6) * 1000.0) == 604.0);
        assert(round(erf(0.6l) * 1000.0l) == 604.0l);
        assert(round(erff(0.6f) * 1000.0f) == 604.0f);
        assert(round(erfl(0.6l) * 1000.0l) == 604.0l);
        assert(round(erf(1) * 1000.0) == 843.0);

        assert(round(erfc(0.6f) * 1000.0f) == 396.0f);
        assert(round(erfc(0.6) * 1000.0) == 396.0);
        assert(round(erfc(0.6l) * 1000.0l) == 396.0l);
        assert(round(erfcf(0.6f) * 1000.0f) == 396.0f);
        assert(round(erfcl(0.6l) * 1000.0l) == 396.0l);
        assert(round(erfc(1) * 1000.0) == 157.0);

        assert(round(lgamma(0.6f) * 1000.0f) == 398.0f);
        assert(round(lgamma(0.6) * 1000.0) == 398.0);
        assert(round(lgamma(0.6l) * 1000.0l) == 398.0l);
        assert(round(lgammaf(0.6f) * 1000.0f) == 398.0f);
        assert(round(lgammal(0.6l) * 1000.0l) == 398.0l);
        assert(round(lgamma(5) * 1000.0) == 3178.0);

        assert(round(tgamma(0.6f) * 1000.0f) == 1489.0f);
        assert(round(tgamma(0.6) * 1000.0) == 1489.0);
        assert(round(tgamma(0.6l) * 1000.0l) == 1489.0l);
        assert(round(tgammaf(0.6f) * 1000.0f) == 1489.0f);
        assert(round(tgammal(0.6l) * 1000.0l) == 1489.0l);
        assert(round(tgamma(5) * 1000.0) == 24000.0);
    }

    return true;
}

#if defined(_MSVC_INTERNAL_TESTING) || !defined(_M_ARM64EC) // TRANSITION, MSVC-PR-767414/MSVC-PR-768260 fixed LNK2019
void test_cmath_runtime() {
    assert(nearbyint(3.14f) == 3.0f);
    assert(nearbyint(3.14) == 3.0);
    assert(nearbyint(3.14l) == 3.0l);
    assert(nearbyintf(3.14f) == 3.0f);
    assert(nearbyintl(3.14l) == 3.0l);
    assert(nearbyint(1729) == 1729.0);

    assert(rint(3.14f) == 3.0f);
    assert(rint(3.14) == 3.0);
    assert(rint(3.14l) == 3.0l);
    assert(rintf(3.14f) == 3.0f);
    assert(rintl(3.14l) == 3.0l);
    assert(rint(1729) == 1729.0);

    assert(lrint(3.14f) == 3L);
    assert(lrint(3.14) == 3L);
    assert(lrint(3.14l) == 3L);
    assert(lrintf(3.14f) == 3L);
    assert(lrintl(3.14l) == 3L);
    assert(lrint(1729) == 1729L);

    assert(llrint(0x1p60f) == 0x1000'0000'0000'0000LL);
    assert(llrint(0x1p60) == 0x1000'0000'0000'0000LL);
    assert(llrint(0x1p60l) == 0x1000'0000'0000'0000LL);
    assert(llrintf(0x1p60f) == 0x1000'0000'0000'0000LL);
    assert(llrintl(0x1p60l) == 0x1000'0000'0000'0000LL);
    assert(llrint(1729) == 1729LL);

    assert(isnan(nan("")));
    assert(isnan(nanf("")));
    assert(isnan(nanl("")));
}
#endif // ^^^ no workaround ^^^

int main() {
#if defined(_MSVC_INTERNAL_TESTING) || !defined(_M_ARM64EC) // TRANSITION, MSVC-PR-767414/MSVC-PR-768260 fixed LNK2019
    test_cmath_cxx23();
    test_cstdlib_cxx23();
    test_cinttypes_cxx23();
    test_cmath_cxx26();
    test_cmath_runtime();
#endif // ^^^ no workaround ^^^

#ifdef __cpp_lib_constexpr_cmath
    static_assert(test_cmath_cxx23());
    static_assert(test_cstdlib_cxx23());
    static_assert(test_cinttypes_cxx23());
#if _HAS_CXX26 && defined(_MSVC_LIBC_MATH) // TRANSITION, GH-3789, should be `__cpp_lib_constexpr_cmath >= 202306L`
    static_assert(test_cmath_cxx26());
#endif // ^^^ _HAS_CXX26 && defined(_MSVC_LIBC_MATH) ^^^
#endif // ^^^ defined(__cpp_lib_constexpr_cmath) ^^^
}
