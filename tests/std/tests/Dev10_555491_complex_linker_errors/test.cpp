// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <complex>
#include <stdint.h>
#include <string.h>

using namespace std;

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
}
