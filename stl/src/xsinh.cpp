// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cerrno>
#include <cmath>
#include <xutility>
#include <ymath.h>

namespace {
    double _Poly(double x, const double* tab, int n) noexcept { // compute polynomial
        double y;

        for (y = *tab; 0 <= --n;) {
            y = y * x + *++tab;
        }

        return y;
    }
} // unnamed namespace

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE double __CLRCALL_PURE_OR_CDECL _Sinh(double x, double y) noexcept {
    // compute y * sinh(x), |y| <= 1

    // coefficients
    static constexpr double p[] = {0.0000000001632881, 0.0000000250483893, 0.0000027557344615, 0.0001984126975233,
        0.0083333333334816, 0.1666666666666574, 1.0000000000000001};
    constexpr size_t NP         = std::size(p) - 1;

    short neg;

    switch (_STD fpclassify(x)) { // test for special codes
    case FP_NAN:
        return x;
    case FP_INFINITE:
        return y != 0.0 ? x : _STD signbit(x) ? -y : y;
    case FP_ZERO:
        return x * y;
    default: // finite
        if (y == 0.0) {
            return x < 0.0 ? -y : y;
        }

        if (x < 0.0) {
            x   = -x;
            neg = 1;
        } else {
            neg = 0;
        }

        constexpr double rteps = 0x1p-27;
        constexpr double _Xbig = ((48 + _DOFF) + 2) * 0.347;
        if (x < rteps) {
            x *= y; // x tiny
        } else if (x < 1.0) {
            double w = x * x;

            x += x * w * _Poly(w, p, NP - 1);
            x *= y;
        } else if (x < _Xbig) { // worth adding in exp(-x)
            _Exp(&x, 1.0, -1);
            x = y * (x - 0.25 / x);
        } else {
            switch (_Exp(&x, y, -1)) {
            case FP_ZERO: // report underflow
            case FP_INFINITE: // report overflow
                errno = ERANGE;
                break;
            }
        }

        return neg ? -x : x;
    }
}

_CRTIMP2_PURE long double __CLRCALL_PURE_OR_CDECL _LSinh(long double x, long double y) noexcept {
    return _Sinh(static_cast<double>(x), static_cast<double>(y));
}

_CRTIMP2_PURE float __CLRCALL_PURE_OR_CDECL _FSinh(float x, float y) noexcept {
    // compute y * sinh(x), |y| <= 1

    // coefficients
    static constexpr float p[] = {0.00020400F, 0.00832983F, 0.16666737F, 0.99999998F};

    short neg;

    switch (_STD fpclassify(x)) { // test for special codes
    case FP_NAN:
        return x;
    case FP_INFINITE:
        return y != 0.0F ? x : _STD signbit(x) ? -y : y;
    case FP_ZERO:
        return x * y;
    default: // finite
        if (y == 0.0F) {
            return x < 0.0F ? -y : y;
        }

        if (x < 0.0F) {
            x   = -x;
            neg = 1;
        } else {
            neg = 0;
        }

        constexpr float rteps  = 0x1p-12f;
        constexpr float _FXbig = ((16 + _FOFF) + 2) * 0.347f;
        if (x < rteps) {
            x *= y; // x tiny
        } else if (x < 1.0F) {
            float w = x * x;

            x += ((p[0] * w + p[1]) * w + p[2]) * w * x;
            x *= y;
        } else if (x < _FXbig) { // worth adding in exp(-x)
            _FExp(&x, 1.0F, -1);
            x = y * (x - 0.25F / x);
        } else {
            switch (_FExp(&x, y, -1)) {
            case FP_ZERO: // report underflow
            case FP_INFINITE: // report overflow
                errno = ERANGE;
                break;
            }
        }

        return neg ? -x : x;
    }
}

_END_EXTERN_C_UNLESS_PURE
