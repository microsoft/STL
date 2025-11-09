// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _LDtest function -- IEEE 754 version

#include "xmath.hpp"

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _LDtest(long double* px) noexcept { // categorize *px -- 64-bit
    return _Dtest(reinterpret_cast<double*>(px));
}

_END_EXTERN_C_UNLESS_PURE
