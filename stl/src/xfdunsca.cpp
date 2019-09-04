// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _FDunscale function -- IEEE 754 version

#include "xmath.h"

_EXTERN_C_UNLESS_PURE

short _FDunscale(short* pex, float* px) { // separate *px to 1/2 <= |frac| < 1 and 2^*pex
    _Fval* ps   = (_Fval*) (char*) px;
    short xchar = (ps->_Sh[_F0] & _FMASK) >> _FOFF;

    if (xchar == _FMAX) { // NaN or INF
        *pex = 0;
        return (ps->_Sh[_F0] & _FFRAC) != 0 || ps->_Sh[_F1] != 0 ? _NANCODE : _INFCODE;
    } else if (0 < xchar || (xchar = _FDnorm(ps)) <= 0) { // finite, reduce to [1/2, 1)
        ps->_Sh[_F0] = (ps->_Sh[_F0] & ~_FMASK) | _FBIAS << _FOFF;
        *pex         = xchar - _FBIAS;
        return _FINITE;
    } else { // zero
        *pex = 0;
        return 0;
    }
}

_END_EXTERN_C_UNLESS_PURE
