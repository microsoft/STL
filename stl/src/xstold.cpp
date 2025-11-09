// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <yvals.h>

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE double __CLRCALL_PURE_OR_CDECL _Stodx(const char* _Str, char** _Endptr, long _Pten, int* _Perr) noexcept;

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE long double __CLRCALL_PURE_OR_CDECL _Stoldx(const char* s, char** endptr, long pten, int* perr) noexcept {
    return _Stodx(s, endptr, pten, perr);
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE long double __CLRCALL_PURE_OR_CDECL _Stold(const char* s, char** endptr, long pten) noexcept {
    return _Stodx(s, endptr, pten, nullptr);
}

_END_EXTERN_C_UNLESS_PURE
