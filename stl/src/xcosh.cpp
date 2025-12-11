// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cmath>

_EXTERN_C_UNLESS_PURE

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE double __CLRCALL_PURE_OR_CDECL _Cosh(double x, double y) noexcept {
    return _STD cosh(x) * y;
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE long double __CLRCALL_PURE_OR_CDECL _LCosh(long double x, long double y) noexcept {
    return _STD cosh(x) * y;
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE float __CLRCALL_PURE_OR_CDECL _FCosh(float x, float y) noexcept {
    return _STD cosh(x) * y;
}

_END_EXTERN_C_UNLESS_PURE
