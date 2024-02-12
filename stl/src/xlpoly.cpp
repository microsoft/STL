// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "xmath.hpp"

_EXTERN_C_UNLESS_PURE

long double _LPoly(long double x, const long double* tab, int n) noexcept { // compute polynomial
    long double y;

    for (y = *tab; 0 <= --n;) {
        y = y * x + *++tab;
    }

    return y;
}

_END_EXTERN_C_UNLESS_PURE
