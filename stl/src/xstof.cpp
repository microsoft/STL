// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cmath>
#include <cstdlib>

_EXTERN_C_UNLESS_PURE

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE float __CLRCALL_PURE_OR_CDECL _Stofx(const char* _Str, char** _Endptr, long _Pten, int* _Perr) noexcept {
    int& _Errno_ref = errno; // Nonzero cost, pay it once
    const int _Orig = _Errno_ref;

    _Errno_ref = 0;
    float _Val = _CSTD strtof(_Str, _Endptr);
    *_Perr     = _Errno_ref;
    _Errno_ref = _Orig;

    if (_Pten != 0) {
        _Val *= _CSTD powf(10.0f, static_cast<float>(_Pten));
    }

    return _Val;
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE float __CLRCALL_PURE_OR_CDECL _Stof(const char* s, char** endptr, long pten) noexcept {
    return _Stofx(s, endptr, pten, nullptr);
}

_END_EXTERN_C_UNLESS_PURE
