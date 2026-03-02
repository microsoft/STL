// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cmath>

_STD_BEGIN
// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE float __CLRCALL_PURE_OR_CDECL _XLgamma(float x) noexcept {
    return _STD lgamma(x);
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE double __CLRCALL_PURE_OR_CDECL _XLgamma(double x) noexcept {
    return _STD lgamma(x);
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE long double __CLRCALL_PURE_OR_CDECL _XLgamma(long double x) noexcept {
    return _STD lgamma(x);
}
_STD_END
