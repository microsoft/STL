// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Cosh function

#include "xmath.h"

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE double __CLRCALL_PURE_OR_CDECL _Cosh(double x, double y) { // compute y * cosh(x), |y| <= 1
    switch (_Dtest(&x)) { // test for special codes
    case _NANCODE:
    case _INFCODE:
        return x;
    case 0:
        return y;
    default: // finite
        if (y == 0.0) {
            return y;
        }

        if (x < 0.0) {
            x = -x;
        }

        if (x < _Xbig) { // worth adding in exp(-x)
            _Exp(&x, 1.0, -1);
            return y * (x + 0.25 / x);
        }
        switch (_Exp(&x, y, -1)) { // report over/underflow
        case 0:
            _Feraise(_FE_UNDERFLOW);
            break;
        case _INFCODE:
            _Feraise(_FE_OVERFLOW);
        }
        return x;
    }
}

_END_EXTERN_C_UNLESS_PURE
