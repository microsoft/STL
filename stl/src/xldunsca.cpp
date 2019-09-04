// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _LDunscale function -- IEEE 754 version

#include "xmath.h"

_EXTERN_C_UNLESS_PURE

short _LDunscale(short* pex, long double* px) { // separate *px to 1/2 <= |frac| < 1 and 2^*pex -- 64-bit
    return _Dunscale(pex, (double*) px);
}

_END_EXTERN_C_UNLESS_PURE
