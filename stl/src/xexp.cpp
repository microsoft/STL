// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Exp function

#include "xmath.h"

_EXTERN_C_UNLESS_PURE

// coefficients
static const double p[]    = {1.0, 420.30235984910635, 15132.70094680474802};
static const double q[]    = {30.01511290683317, 3362.72154416553028, 30265.40189360949691};
static const double c1     = 22713.0 / 32768.0;
static const double c2     = 1.4286068203094172321214581765680755e-6;
static const double hugexp = HUGE_EXP;
static const double invln2 = 1.4426950408889634073599246810018921;

_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _Exp(
    double* px, double y, short eoff) { // compute y * e^(*px), (*px) finite, |y| not huge
    if (*px < -hugexp || y == 0.0) { // certain underflow
        *px = 0.0;
        return 0;
    } else if (hugexp < *px) { // certain overflow
        *px = _Inf._Double;
        return _INFCODE;
    } else { // xexp won't overflow
        double g   = *px * invln2;
        short xexp = (short) (g + (g < 0.0 ? -0.5 : +0.5));

        g = xexp;
        g = (*px - g * c1) - g * c2;
        if (-_Eps._Double < g && g < _Eps._Double) {
            *px = y;
        } else { // g * g worth computing
            const double z = g * g;
            const double w = (q[0] * z + q[1]) * z + q[2];

            g *= (z + p[1]) * z + p[2];
            *px = (w + g) / (w - g) * 2.0 * y;
            --xexp;
        }
        return _Dscale(px, (long) xexp + eoff);
    }
}

_END_EXTERN_C_UNLESS_PURE
