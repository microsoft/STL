// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Dunscale function -- IEEE 754 version

#include "xmath.h"

_EXTERN_C_UNLESS_PURE

short _Dunscale(short* pex, double* px) { // separate *px to 1/2 <= |frac| < 1 and 2^*pex
    _Dval* ps   = (_Dval*) (char*) px;
    short xchar = (ps->_Sh[_D0] & _DMASK) >> _DOFF;

    if (xchar == _DMAX) { // NaN or INF
        *pex = 0;
        return (ps->_Sh[_D0] & _DFRAC) != 0 || ps->_Sh[_D1] != 0 || ps->_Sh[_D2] != 0 || ps->_Sh[_D3] != 0 ? _NANCODE
                                                                                                           : _INFCODE;
    } else if (0 < xchar || (xchar = _Dnorm(ps)) <= 0) { // finite, reduce to [1/2, 1)
        ps->_Sh[_D0] = (ps->_Sh[_D0] & ~_DMASK) | _DBIAS << _DOFF;
        *pex         = xchar - _DBIAS;
        return _FINITE;
    } else { // zero
        *pex = 0;
        return 0;
    }
}

_END_EXTERN_C_UNLESS_PURE
