// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _FDtest function -- IEEE 754 version

#include "xmath.h"

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _FDtest(float* px) { // categorize *px
    _Fval* ps = (_Fval*) (char*) px;

    if ((ps->_Sh[_F0] & _FMASK) == _FMAX << _FOFF) {
        return (short) ((ps->_Sh[_F0] & _FFRAC) != 0 || ps->_Sh[_F1] != 0 ? _NANCODE : _INFCODE);
    } else if ((ps->_Sh[_F0] & ~_FSIGN) != 0 || ps->_Sh[_F1] != 0) {
        return (ps->_Sh[_F0] & _FMASK) == 0 ? _DENORM : _FINITE;
    } else {
        return 0;
    }
}

unsigned short* _FPlsw(float* px) { // get pointer to lsw
    return &((_Fval*) (char*) px)->_Sh[_Fg];
}

unsigned short* _FPmsw(float* px) { // get pointer to msw
    return &((_Fval*) (char*) px)->_Sh[_F0];
}

_END_EXTERN_C_UNLESS_PURE
