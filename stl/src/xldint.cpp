// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _LDint function -- IEEE 754 version

#include "xmath.h"

_EXTERN_C_UNLESS_PURE

short _LDint(long double* px, short xexp) { // test and drop (scaled) fraction bits -- 64-bit
    return _Dint((double*) px, xexp);
}

_END_EXTERN_C_UNLESS_PURE
