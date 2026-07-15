// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cmath>

_EXTERN_C_UNLESS_PURE

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _Dtest(double* px) noexcept {
    return static_cast<short>(_STD fpclassify(*px));
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _LDtest(long double* px) noexcept {
    return _Dtest(reinterpret_cast<double*>(px));
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _FDtest(float* px) noexcept {
    return static_cast<short>(_STD fpclassify(*px));
}

_END_EXTERN_C_UNLESS_PURE
