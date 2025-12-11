// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "xmath.hpp"

_EXTERN_C_UNLESS_PURE

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE double __CLRCALL_PURE_OR_CDECL _Sinh(double x, double y) noexcept {
    return _STD sinh(x) * y;
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE long double __CLRCALL_PURE_OR_CDECL _LSinh(long double x, long double y) noexcept {
    return _STD sinh(x) * y;
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE float __CLRCALL_PURE_OR_CDECL _FSinh(float x, float y) noexcept {
    return _STD sinh(x) * y;
}

_END_EXTERN_C_UNLESS_PURE
