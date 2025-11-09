// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <yvals.h>

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE double __CLRCALL_PURE_OR_CDECL _WStodx(
    const wchar_t* _Str, wchar_t** _Endptr, long _Pten, int* _Perr) noexcept;

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE long double __CLRCALL_PURE_OR_CDECL _WStoldx(
    const wchar_t* s, wchar_t** endptr, long pten, int* perr) noexcept {
    return _WStodx(s, endptr, pten, perr);
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE long double __CLRCALL_PURE_OR_CDECL _WStold(const wchar_t* s, wchar_t** endptr, long pten) noexcept {
    return _WStodx(s, endptr, pten, nullptr);
}

_END_EXTERN_C_UNLESS_PURE
