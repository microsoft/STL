// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _LDtest function -- IEEE 754 version

#include "xmath.h"

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _LDtest(long double* px) { // categorize *px -- 64-bit
    return _Dtest((double*) px);
}

unsigned short* _LPlsw(long double* px) { // get pointer to lsw
    return &((_Lval*) (char*) px)->_Sh[_L3];
}

unsigned short* _LPmsw(long double* px) { // get pointer to msw
    return &((_Lval*) (char*) px)->_Sh[_L0];
}

_END_EXTERN_C_UNLESS_PURE
