// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cerrno>
#include <cmath>
#include <ymath.h>

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE double __CLRCALL_PURE_OR_CDECL _Cosh(double x, double y) noexcept { // compute y * cosh(x), |y| <= 1
    switch (_STD fpclassify(x)) { // test for special codes
    case FP_NAN:
    case FP_INFINITE:
        return x;
    case FP_ZERO:
        return y;
    default: // finite
        if (y == 0.0) {
            return y;
        }

        if (x < 0.0) {
            x = -x;
        }

        constexpr double _Xbig = ((48 + _DOFF) + 2) * 0.347;
        if (x < _Xbig) { // worth adding in exp(-x)
            _Exp(&x, 1.0, -1);
            return y * (x + 0.25 / x);
        }
        switch (_Exp(&x, y, -1)) {
        case FP_ZERO: // report underflow
        case FP_INFINITE: // report overflow
            errno = ERANGE;
            break;
        }
        return x;
    }
}

_CRTIMP2_PURE long double __CLRCALL_PURE_OR_CDECL _LCosh(long double x, long double y) noexcept {
    return _Cosh(static_cast<double>(x), static_cast<double>(y));
}

_CRTIMP2_PURE float __CLRCALL_PURE_OR_CDECL _FCosh(float x, float y) noexcept { // compute y * cosh(x), |y| <= 1
    switch (_STD fpclassify(x)) { // test for special codes
    case FP_NAN:
    case FP_INFINITE:
        return x;
    case FP_ZERO:
        return y;
    default: // finite
        if (y == 0.0) {
            return y;
        }

        if (x < 0.0) {
            x = -x;
        }

        constexpr float _FXbig = ((16 + _FOFF) + 2) * 0.347f;
        if (x < _FXbig) { // worth adding in exp(-x)
            _FExp(&x, 1.0F, -1);
            return y * (x + 0.25F / x);
        }
        switch (_FExp(&x, y, -1)) {
        case FP_ZERO: // report underflow
        case FP_INFINITE: // report overflow
            errno = ERANGE;
            break;
        }
        return x;
    }
}

_END_EXTERN_C_UNLESS_PURE
