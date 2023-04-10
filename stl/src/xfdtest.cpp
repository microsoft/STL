// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _FDtest function -- IEEE 754 version

#include "xmath.hpp"

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _FDtest(float* px) { // categorize *px
    const auto ps = reinterpret_cast<_Fval*>(px);

    if ((ps->_Sh[_F0] & _FMASK) == _FMAX << _FOFF) {
        return (ps->_Sh[_F0] & _FFRAC) != 0 || ps->_Sh[_F1] != 0 ? _NANCODE : _INFCODE;
    } else if ((ps->_Sh[_F0] & ~_FSIGN) != 0 || ps->_Sh[_F1] != 0) {
        return (ps->_Sh[_F0] & _FMASK) == 0 ? _DENORM : _FINITE;
    } else {
        return 0;
    }
}

unsigned short* _FPmsw(float* px) { // get pointer to msw
    return &reinterpret_cast<_Fval*>(px)->_Sh[_F0];
}

_END_EXTERN_C_UNLESS_PURE
