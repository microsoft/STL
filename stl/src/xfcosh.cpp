// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _FCosh function

#include "xmath.h"

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE float __CLRCALL_PURE_OR_CDECL _FCosh(float x, float y) { // compute y * cosh(x), |y| <= 1
    switch (_FDtest(&x)) { // test for special codes
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

        if (x < _FXbig) { // worth adding in exp(-x)
            _FExp(&x, 1.0F, -1);
            return y * (x + 0.25F / x);
        }
        switch (_FExp(&x, y, -1)) { // report over/underflow
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
