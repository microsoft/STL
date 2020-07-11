// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Dtest function -- IEEE 754 version

#include "xmath.hpp"

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _Dtest(double* px) { // categorize *px
    const auto ps = reinterpret_cast<_Dval*>(px);

    if ((ps->_Sh[_D0] & _DMASK) == _DMAX << _DOFF) {
        return static_cast<short>(
            (ps->_Sh[_D0] & _DFRAC) != 0 || ps->_Sh[_D1] != 0 || ps->_Sh[_D2] != 0 || ps->_Sh[_D3] != 0 ? _NANCODE
                                                                                                        : _INFCODE);
    } else if ((ps->_Sh[_D0] & ~_DSIGN) != 0 || ps->_Sh[_D1] != 0 || ps->_Sh[_D2] != 0 || ps->_Sh[_D3] != 0) {
        return (ps->_Sh[_D0] & _DMASK) == 0 ? _DENORM : _FINITE;
    } else {
        return 0;
    }
}

unsigned short* _Plsw(double* px) { // get pointer to lsw
    return &reinterpret_cast<_Dval*>(px)->_Sh[_Dg];
}

unsigned short* _Pmsw(double* px) { // get pointer to msw
    return &reinterpret_cast<_Dval*>(px)->_Sh[_D0];
}

_END_EXTERN_C_UNLESS_PURE
