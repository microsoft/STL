// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cmath>
#include <complex>
#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <class T, class U>
T pseudo_bit_cast(const U& source) {
    static_assert(sizeof(T) == sizeof(U), "");
    T result;
    memcpy(&result, &source, sizeof(result));
    return result;
}

constexpr uint32_t allowedFloatUlp  = 2;
constexpr uint64_t allowedDoubleUlp = 1;

bool nearly_equal(const float actual, const float expected) {
    // the subtraction produces [-N ULP == large positive value, 0, +N ULP == N]
    const auto difference = pseudo_bit_cast<uint32_t>(actual) - pseudo_bit_cast<uint32_t>(expected);
    // now [-N ULP == 0, 0 == N, +N ULP == 2N]
    const auto differencePN = difference + allowedFloatUlp;
    return differencePN <= 2 * allowedFloatUlp;
}

bool nearly_equal(const double actual, const double expected) {
    const auto difference   = pseudo_bit_cast<uint64_t>(actual) - pseudo_bit_cast<uint64_t>(expected);
    const auto differencePN = difference + allowedDoubleUlp;
    return differencePN <= 2 * allowedDoubleUlp;
}

bool nearly_equal(const long double actual, const long double expected) {
    return nearly_equal(static_cast<double>(actual), static_cast<double>(expected));
}

template <class T>
bool nearly_equal_partwise(const complex<T> actual, const complex<T> expected) {
    return nearly_equal(actual.real(), expected.real()) && nearly_equal(actual.imag(), expected.imag());
}

// Also test GH-2728 <complex>: Implementation divergence for division by zero
void test_gh_2728() {
    const complex<double> zero{0, 0};
    const complex<double> minus_zero{-0.0, 0.0};
    constexpr double inf = numeric_limits<double>::infinity();
    constexpr double nan = numeric_limits<double>::quiet_NaN();
    {
        const complex<double> test_one_one = complex<double>{1, 1} / zero;
        assert(isinf(test_one_one.real()));
        assert(test_one_one.real() > 0);
        assert(isinf(test_one_one.imag()));
        assert(test_one_one.imag() > 0);
    }
    {
        const complex<double> test_one_one = complex<double>{1, 1} / minus_zero;
        assert(isinf(test_one_one.real()));
        assert(test_one_one.real() < 0);
        assert(isinf(test_one_one.imag()));
        assert(test_one_one.imag() < 0);
    }
    {
        const complex<double> test_one_zero = complex<double>{1, 0} / zero;
        assert(isinf(test_one_zero.real()));
        assert(test_one_zero.real() > 0);
        assert(isnan(test_one_zero.imag()));
    }
    {
        const complex<double> test_one_minus_one = complex<double>{1, -1} / zero;
        assert(isinf(test_one_minus_one.real()));
        assert(test_one_minus_one.real() > 0);
        assert(isinf(test_one_minus_one.imag()));
        assert(test_one_minus_one.imag() < 0);
    }
    {
        const complex<double> test_zero_one = complex<double>{0, 1} / zero;
        assert(isnan(test_zero_one.real()));
        assert(isinf(test_zero_one.imag()));
        assert(test_zero_one.imag() > 0);
    }
    {
        const complex<double> test_zero_zero = complex<double>{0, 0} / zero;
        assert(isnan(test_zero_zero.real()));
        assert(isnan(test_zero_zero.imag()));
    }
    {
        const complex<double> test_zero_zero = complex<double>{0, 0} / minus_zero;
        assert(isnan(test_zero_zero.real()));
        assert(isnan(test_zero_zero.imag()));
    }
    {
        const complex<double> test_zero_minus_one = complex<double>{0, -1} / zero;
        assert(isnan(test_zero_minus_one.real()));
        assert(isinf(test_zero_minus_one.imag()));
        assert(test_zero_minus_one.imag() < 0);
    }
    {
        const complex<double> test_minus_one_one = complex<double>{-1, 1} / zero;
        assert(isinf(test_minus_one_one.real()));
        assert(test_minus_one_one.real() < 0);
        assert(isinf(test_minus_one_one.imag()));
        assert(test_minus_one_one.imag() > 0);
    }
    {
        const complex<double> test_minus_one_zero = complex<double>{-1, 0} / zero;
        assert(isinf(test_minus_one_zero.real()));
        assert(test_minus_one_zero.real() < 0);
        assert(isnan(test_minus_one_zero.imag()));
    }
    {
        const complex<double> test_minus_one_minus_one = complex<double>{-1, -1} / zero;
        assert(isinf(test_minus_one_minus_one.real()));
        assert(test_minus_one_minus_one.real() < 0);
        assert(isinf(test_minus_one_minus_one.imag()));
        assert(test_minus_one_minus_one.imag() < 0);
    }
    {
        const complex<double> test_minus_one_minus_one = complex<double>{-1, -1} / minus_zero;
        assert(isinf(test_minus_one_minus_one.real()));
        assert(test_minus_one_minus_one.real() > 0);
        assert(isinf(test_minus_one_minus_one.imag()));
        assert(test_minus_one_minus_one.imag() > 0);
    }
    {
        const complex<double> test_inf_inf = complex<double>{inf, inf} / zero;
        assert(isinf(test_inf_inf.real()));
        assert(test_inf_inf.real() > 0);
        assert(isinf(test_inf_inf.imag()));
        assert(test_inf_inf.imag() > 0);
    }
    {
        const complex<double> test_inf_inf = complex<double>{inf, inf} / minus_zero;
        assert(isinf(test_inf_inf.real()));
        assert(test_inf_inf.real() < 0);
        assert(isinf(test_inf_inf.imag()));
        assert(test_inf_inf.imag() < 0);
    }
    {
        const complex<double> test_inf_minus_inf = complex<double>{inf, -inf} / zero;
        assert(isinf(test_inf_minus_inf.real()));
        assert(test_inf_minus_inf.real() > 0);
        assert(isinf(test_inf_minus_inf.imag()));
        assert(test_inf_minus_inf.imag() < 0);
    }
    {
        const complex<double> test_minus_inf_minus_inf = complex<double>{-inf, -inf} / zero;
        assert(isinf(test_minus_inf_minus_inf.real()));
        assert(test_minus_inf_minus_inf.real() < 0);
        assert(isinf(test_minus_inf_minus_inf.imag()));
        assert(test_minus_inf_minus_inf.imag() < 0);
    }
    {
        const complex<double> test_minus_inf_minus_inf = complex<double>{-inf, -inf} / minus_zero;
        assert(isinf(test_minus_inf_minus_inf.real()));
        assert(test_minus_inf_minus_inf.real() > 0);
        assert(isinf(test_minus_inf_minus_inf.imag()));
        assert(test_minus_inf_minus_inf.imag() > 0);
    }
    {
        const complex<double> test_one_nan = complex<double>{1, nan} / zero;
        assert(isinf(test_one_nan.real()));
        assert(test_one_nan.real() > 0);
        assert(isnan(test_one_nan.imag()));
    }
    {
        const complex<double> test_nan_one = complex<double>{nan, 1} / zero;
        assert(isnan(test_nan_one.real()));
        assert(isinf(test_nan_one.imag()));
        assert(test_nan_one.imag() > 0);
    }
    {
        const complex<double> test_nan_nan = complex<double>{nan, nan} / zero;
        assert(isnan(test_nan_nan.real()));
        assert(isnan(test_nan_nan.imag()));
    }
    {
        const complex<double> test_nan_nan = complex<double>{nan, nan} / minus_zero;
        assert(isnan(test_nan_nan.real()));
        assert(isnan(test_nan_nan.imag()));
    }
}


int main() {
    complex<float> f(1, 2);

    (void) pow(f, 2);
    (void) log(f);
    (void) sinh(f);
    (void) cosh(f);
    (void) exp(f);


    complex<double> d(1, 2);

    (void) pow(d, 2);
    (void) log(d);
    (void) sinh(d);
    (void) cosh(d);
    (void) exp(d);


    complex<long double> l(1, 2);

    (void) pow(l, 2);
    (void) log(l);
    (void) sinh(l);
    (void) cosh(l);
    (void) exp(l);

    // Also test GH-379 "asinh(-1e+307 + 2e+307i) is wrong"

    assert(nearly_equal_partwise(acos(-2e+307 + 1e+307i), 2.677945044588987 - 708.3914896859491i));
    assert(nearly_equal_partwise(acos(-4e+37f + 2e+37if), 2.6779451f - 87.386663if));
    assert(nearly_equal_partwise(acos(-2e+307L + 1e+307il), 2.677945044588987L - 708.3914896859491il));

    assert(nearly_equal_partwise(acos(2e+307 + 1e+307i), 0.4636476090008061 - 708.3914896859491i));
    assert(nearly_equal_partwise(acos(4e+37f + 2e+37if), 0.4636476f - 87.386663if));
    assert(nearly_equal_partwise(acos(2e+307L + 1e+307il), 0.4636476090008061L - 708.3914896859491il));

    assert(nearly_equal_partwise(acos(2e+307 - 1e+307i), 0.4636476090008061 + 708.3914896859491i));
    assert(nearly_equal_partwise(acos(4e+37f - 2e+37if), 0.4636476f + 87.386663if));
    assert(nearly_equal_partwise(acos(2e+307L - 1e+307il), 0.4636476090008061L + 708.3914896859491il));

    assert(nearly_equal_partwise(acosh(-1e+307 + 2e+307i), 708.3914896859491 + 2.0344439357957027i));
    assert(nearly_equal_partwise(acosh(-2e+37f + 4e+37if), 87.386663f + 2.0344439if));
    assert(nearly_equal_partwise(acosh(-1e+307L + 2e+307il), 708.3914896859491L + 2.0344439357957027il));

    assert(nearly_equal_partwise(asinh(-1e+307 + 2e+307i), -708.3914896859491 + 1.1071487177940904i));
    assert(nearly_equal_partwise(asinh(-2e+37f + 4e+37if), -87.386663f + 1.1071488if));
    assert(nearly_equal_partwise(asinh(-1e+307L + 2e+307il), -708.3914896859491L + 1.1071487177940904il));

    // Also test GH-785 "<complex>: std::arg does not work for negative real values"

    STATIC_ASSERT(is_same_v<decltype(arg(1729)), double>);
    STATIC_ASSERT(is_same_v<decltype(imag(1729)), double>);
    STATIC_ASSERT(is_same_v<decltype(real(1729)), double>);
    STATIC_ASSERT(is_same_v<decltype(norm(1729)), double>);
    STATIC_ASSERT(is_same_v<decltype(conj(1729)), complex<double>>);
    STATIC_ASSERT(is_same_v<decltype(proj(1729)), complex<double>>);

    STATIC_ASSERT(is_same_v<decltype(arg(1729.0f)), float>);
    STATIC_ASSERT(is_same_v<decltype(imag(1729.0f)), float>);
    STATIC_ASSERT(is_same_v<decltype(real(1729.0f)), float>);
    STATIC_ASSERT(is_same_v<decltype(norm(1729.0f)), float>);
    STATIC_ASSERT(is_same_v<decltype(conj(1729.0f)), complex<float>>);
    STATIC_ASSERT(is_same_v<decltype(proj(1729.0f)), complex<float>>);

    STATIC_ASSERT(is_same_v<decltype(arg(1729.0)), double>);
    STATIC_ASSERT(is_same_v<decltype(imag(1729.0)), double>);
    STATIC_ASSERT(is_same_v<decltype(real(1729.0)), double>);
    STATIC_ASSERT(is_same_v<decltype(norm(1729.0)), double>);
    STATIC_ASSERT(is_same_v<decltype(conj(1729.0)), complex<double>>);
    STATIC_ASSERT(is_same_v<decltype(proj(1729.0)), complex<double>>);

    STATIC_ASSERT(is_same_v<decltype(arg(1729.0L)), long double>);
    STATIC_ASSERT(is_same_v<decltype(imag(1729.0L)), long double>);
    STATIC_ASSERT(is_same_v<decltype(real(1729.0L)), long double>);
    STATIC_ASSERT(is_same_v<decltype(norm(1729.0L)), long double>);
    STATIC_ASSERT(is_same_v<decltype(conj(1729.0L)), complex<long double>>);
    STATIC_ASSERT(is_same_v<decltype(proj(1729.0L)), complex<long double>>);

    assert((arg(-1.0) == arg(complex<double>{-1.0, 0.0})));
    assert((arg(-1) == arg(complex<double>{-1.0, 0.0})));

    assert(imag(1729.0) == 0.0);
    assert(imag(1729) == 0.0);

    assert(real(1729.0) == 1729.0);
    assert(real(1729) == 1729.0);

    assert(norm(0x1p63) == 0x1p126);
    assert(norm(0x8000'0000'0000'0000ULL) == 0x1p126);

    assert((conj(1729.0) == complex<double>{1729.0, -0.0}));
    assert((conj(1729) == complex<double>{1729.0, -0.0}));
    assert(signbit(conj(1729.0).imag()));
    assert(signbit(conj(1729).imag()));

    assert((proj(1729.0) == complex<double>{1729.0, 0.0}));
    assert((proj(1729) == complex<double>{1729.0, 0.0}));
    constexpr double inf = numeric_limits<double>::infinity();
    assert((proj(inf) == complex<double>{inf, 0.0}));
    assert((proj(-inf) == complex<double>{inf, 0.0}));

    test_gh_2728();

    // Also test N4928 [complex.numbers.general]/2:
    // "Specializations of complex for cv-unqualified floating-point types are trivially-copyable literal types"
    STATIC_ASSERT(is_trivially_copyable_v<complex<float>>);
    STATIC_ASSERT(is_trivially_copyable_v<complex<double>>);
    STATIC_ASSERT(is_trivially_copyable_v<complex<long double>>);
}
