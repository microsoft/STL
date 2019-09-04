// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Poly function

#include "xmath.h"

_EXTERN_C_UNLESS_PURE

double _Poly(double x, const double* tab, int n) { // compute polynomial
    double y;

    for (y = *tab; 0 <= --n;) {
        y = y * x + *++tab;
    }

    return y;
}

_END_EXTERN_C_UNLESS_PURE
