// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _LDscale function -- IEEE 754 version

#include "xmath.h"

_EXTERN_C_UNLESS_PURE

short _LDscale(long double* px, long lexp) { // scale *px by 2^lexp with checking -- 64-bit
    return _Dscale((double*) px, lexp);
}

_END_EXTERN_C_UNLESS_PURE
