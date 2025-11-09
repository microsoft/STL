// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cmath>
#include <cstdlib>

_EXTERN_C_UNLESS_PURE

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE double __CLRCALL_PURE_OR_CDECL _Stodx(const char* _Str, char** _Endptr, long _Pten, int* _Perr) noexcept {
    int& _Errno_ref = errno; // Nonzero cost, pay it once
    const int _Orig = _Errno_ref;

    _Errno_ref  = 0;
    double _Val = _CSTD strtod(_Str, _Endptr);
    *_Perr      = _Errno_ref;
    _Errno_ref  = _Orig;

    if (_Pten != 0) {
        _Val *= _CSTD pow(10.0, static_cast<double>(_Pten));
    }

    return _Val;
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE double __CLRCALL_PURE_OR_CDECL _Stod(const char* s, char** endptr, long pten) noexcept {
    return _Stodx(s, endptr, pten, nullptr);
}

_END_EXTERN_C_UNLESS_PURE
