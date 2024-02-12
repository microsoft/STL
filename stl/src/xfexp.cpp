// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "xmath.hpp"

_EXTERN_C_UNLESS_PURE

static const float p[]    = {1.0F, 60.09114349F};
static const float q[]    = {12.01517514F, 120.18228722F};
static const float c1     = (22713.0F / 32768.0F);
static const float c2     = 1.4286068203094172321214581765680755e-6F;
static const float hugexp = FHUGE_EXP;
static const float invln2 = 1.4426950408889634073599246810018921F;

_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _FExp(float* px, float y, short eoff) noexcept {
    // compute y * e^(*px), (*px) finite, |y| not huge
    if (y == 0.0F) { // zero
        *px = y;
        return 0;
    } else if (*px < -hugexp) { // certain underflow
        *px = _Xfe_underflow(y);
        return 0;
    } else if (hugexp < *px) { // certain overflow
        *px = _Xfe_overflow(y);
        return _INFCODE;
    } else { // xexp won't overflow
        float g    = *px * invln2;
        short xexp = static_cast<short>(g + (g < 0.0F ? -0.5F : +0.5F));

        g = xexp;
        g = static_cast<float>((*px - g * c1) - g * c2);
        if (-_FEps._Float < g && g < _FEps._Float) {
            *px = y;
        } else { // g * g worth computing
            const float z = g * g;
            const float w = q[0] * z + q[1];

            g *= z + p[1];
            *px = (w + g) / (w - g) * 2.0F * y;
            --xexp;
        }

        const short result_code = _FDscale(px, static_cast<long>(xexp) + eoff);

        switch (result_code) {
        case 0:
            *px = _Xfe_underflow(y);
            break;
        case _INFCODE:
            *px = _Xfe_overflow(y);
            break;
        default:
            break;
        }

        return result_code;
    }
}

_END_EXTERN_C_UNLESS_PURE
