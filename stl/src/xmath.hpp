// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <cerrno>
#include <cmath>
#include <limits>
#include <ymath.h>

_EXTERN_C_UNLESS_PURE

// double declarations
extern const double _Xbig;

// float declarations
extern const float _FXbig;

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
