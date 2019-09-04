// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Sinh function

#include "xmath.h"

_EXTERN_C_UNLESS_PURE

// coefficients
static const double p[] = {0.0000000001632881, 0.0000000250483893, 0.0000027557344615, 0.0001984126975233,
    0.0083333333334816, 0.1666666666666574, 1.0000000000000001};

static constexpr size_t NP = sizeof(p) / sizeof(p[0]) - 1;

_CRTIMP2_PURE double __CLRCALL_PURE_OR_CDECL _Sinh(double x, double y) { // compute y * sinh(x), |y| <= 1
    short neg;

    switch (_Dtest(&x)) { // test for special codes
    case _NANCODE:
        return x;
    case _INFCODE:
        return y != 0.0 ? x : DSIGN(x) ? -y : y;
    case 0:
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

        if (x < _Rteps._Double) {
            x *= y; // x tiny
        } else if (x < 1.0) {
            double w = x * x;

            x += x * w * _Poly(w, p, NP - 1);
            x *= y;
        } else if (x < _Xbig) { // worth adding in exp(-x)
            _Exp(&x, 1.0, -1);
            x = y * (x - 0.25 / x);
        } else {
            switch (_Exp(&x, y, -1)) { // report over/underflow
            case 0:
                _Feraise(_FE_UNDERFLOW);
                break;
            case _INFCODE:
                _Feraise(_FE_OVERFLOW);
            }
        }

        return neg ? -x : x;
    }
}

_END_EXTERN_C_UNLESS_PURE
