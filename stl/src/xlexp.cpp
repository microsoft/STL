// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _LExp function

#include "xmath.h"

_EXTERN_C_UNLESS_PURE

// coefficients
static const long double p[] = {42.038913947607355L, 10096.353102778762831L, 333228.767219512631062L};
static const long double q[] = {1.0L, 841.167880526530790L, 75730.834075476293976L, 666457.534439025262146L};

static const long double c1     = (22713.0L / 32768.0L);
static const long double c2     = 1.4286068203094172321214581765680755e-6L;
static const long double hugexp = LHUGE_EXP;
static const long double invln2 = 1.4426950408889634073599246810018921L;

_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _LExp(
    long double* px, long double y, short eoff) { // compute y * e^(*px), (*px) finite, |y| not huge
    if (*px < -hugexp || y == 0.0L) { // certain underflow
        *px = 0.0L;
        return 0;
    } else if (hugexp < *px) { // certain overflow
        *px = _LInf._Long_double;
        return _INFCODE;
    } else { // xexp won't overflow
        long double g = *px * invln2;
        short xexp    = (short) (g + (g < 0 ? -0.5L : +0.5L));

        g = xexp;
        g = (*px - g * c1) - g * c2;
        if (-_LEps._Long_double < g && g < _LEps._Long_double) {
            *px = y;
        } else { // g * g worth computing
            const long double z = g * g;
            const long double w = ((z + q[1]) * z + q[2]) * z + q[3];

            g *= (p[0] * z + p[1]) * z + p[2];
            *px = (w + g) / (w - g) * 2.0L * y;
            --xexp;
        }
        return _LDscale(px, (long) xexp + eoff);
    }
}

_END_EXTERN_C_UNLESS_PURE
