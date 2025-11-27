// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cmath>
#include <cwchar>

_EXTERN_C_UNLESS_PURE

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE double __CLRCALL_PURE_OR_CDECL _WStodx(
    const wchar_t* _Str, wchar_t** _Endptr, long _Pten, int* _Perr) noexcept {
    int& _Errno_ref = errno; // Nonzero cost, pay it once
    const int _Orig = _Errno_ref;

    _Errno_ref  = 0;
    double _Val = _CSTD wcstod(_Str, _Endptr);
    *_Perr      = _Errno_ref;
    _Errno_ref  = _Orig;

    if (_Pten != 0) {
        _Val *= _CSTD pow(10.0, static_cast<double>(_Pten));
    }

    return _Val;
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE double __CLRCALL_PURE_OR_CDECL _WStod(const wchar_t* s, wchar_t** endptr, long pten) noexcept {
    return _WStodx(s, endptr, pten, nullptr);
}

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
