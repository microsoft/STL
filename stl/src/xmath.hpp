// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <cerrno>
#include <cmath>
#include <limits>
#include <ymath.h>

// float properties
#define _D0 3 // little-endian, small long doubles
#define _D1 2
#define _D2 1
#define _D3 0

#define _F0 1 // little-endian
#define _F1 0

#define DSIGN(x) (reinterpret_cast<_Dval*>(&(x))->_Sh[_D0] & _DSIGN)
#define FSIGN(x) (reinterpret_cast<_Fval*>(&(x))->_Sh[_F0] & _FSIGN)

// macros for _Dtest return (0 => ZERO)
#define _DENORM (-2) // C9X only
#define _FINITE (-1)

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _Dtest(double*) noexcept;
_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _FDtest(float*) noexcept;

// double declarations
union _Dval { // pun floating type as integer array
    unsigned short _Sh[8];
    double _Val;
};

// float declarations
union _Fval { // pun floating type as integer array
    unsigned short _Sh[8];
    float _Val;
};

short _FDnorm(_Fval*) noexcept;
short _FDscale(float*, long) noexcept;

_END_EXTERN_C_UNLESS_PURE

// raise IEEE 754 exceptions
#ifndef _M_CEE_PURE
#pragma float_control(except, on, push)
#endif

template <class T>
[[nodiscard]] T _Xfe_overflow(const T sign) noexcept {
    static_assert(_STD is_floating_point_v<T>, "Expected is_floating_point_v<T>.");
    constexpr T huge = _STD numeric_limits<T>::max();
    return _STD copysign(huge, sign) * huge;
}

template <class T>
[[nodiscard]] T _Xfe_underflow(const T sign) noexcept {
    static_assert(_STD is_floating_point_v<T>, "Expected is_floating_point_v<T>.");
    constexpr T tiny = _STD numeric_limits<T>::min();
    return _STD copysign(tiny, sign) * tiny;
}

#ifndef _M_CEE_PURE
#pragma float_control(pop)
#endif
