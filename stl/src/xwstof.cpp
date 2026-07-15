// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cmath>
#include <cwchar>

_EXTERN_C_UNLESS_PURE

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE float __CLRCALL_PURE_OR_CDECL _WStofx(
    const wchar_t* _Str, wchar_t** _Endptr, long _Pten, int* _Perr) noexcept {
    int& _Errno_ref = errno; // Nonzero cost, pay it once
    const int _Orig = _Errno_ref;

    _Errno_ref = 0;
    float _Val = _CSTD wcstof(_Str, _Endptr);
    *_Perr     = _Errno_ref;
    _Errno_ref = _Orig;

    if (_Pten != 0) {
        _Val *= _CSTD powf(10.0f, static_cast<float>(_Pten));
    }

    return _Val;
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE float __CLRCALL_PURE_OR_CDECL _WStof(const wchar_t* s, wchar_t** endptr, long pten) noexcept {
    return _WStofx(s, endptr, pten, nullptr);
}

_END_EXTERN_C_UNLESS_PURE
