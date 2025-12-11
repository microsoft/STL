// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cmath>

_EXTERN_C_UNLESS_PURE

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _Exp(double* px, double y, short eoff) noexcept {
    *px = _STD exp(*px) * y * _STD exp2(static_cast<double>(eoff));
    return static_cast<short>(_STD fpclassify(*px));
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _LExp(long double* px, long double y, short eoff) noexcept {
    return _Exp(reinterpret_cast<double*>(px), static_cast<double>(y), eoff);
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _FExp(float* px, float y, short eoff) noexcept {
    *px = _STD exp(*px) * y * _STD exp2(static_cast<float>(eoff));
    return static_cast<short>(_STD fpclassify(*px));
}

_END_EXTERN_C_UNLESS_PURE
