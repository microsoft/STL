// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Once upon a time, this implemented multiprecision math for linear_congruential_engine.

#include <yvals.h>

#include <__msvc_int128.hpp>
#include <cstdint>

namespace {
    using _STD _Unsigned128;

    using _MP_arr = uint64_t[5]; // Stores a 128-bit value in four 32-bit parts.
    // Each part was uint64_t for intermediate computations (now unused) and the fifth part was always unused.

    [[nodiscard]] _Unsigned128 _Get_u128_from_mp(_MP_arr _Wx) noexcept {
        const uint64_t _Lo = (_Wx[1] << 32) + _Wx[0];
        const uint64_t _Hi = (_Wx[3] << 32) + _Wx[2];

        return _Unsigned128{_Lo, _Hi};
    }

    void _Assign_mp_from_u128(_MP_arr _Wx, const _Unsigned128 _Result) noexcept {
        const uint64_t _Lo = _Result._Word[0];
        const uint64_t _Hi = _Result._Word[1];

        _Wx[0] = static_cast<uint32_t>(_Lo);
        _Wx[1] = _Lo >> 32;
        _Wx[2] = static_cast<uint32_t>(_Hi);
        _Wx[3] = _Hi >> 32;
        _Wx[4] = 0; // unused, but zeroed out to preserve behavior exactly
    }
} // unnamed namespace

_STD_BEGIN
// TRANSITION, ABI: preserved for binary compatibility
[[nodiscard]] _CRTIMP2_PURE uint64_t __CLRCALL_PURE_OR_CDECL _MP_Get(_MP_arr _Wx) noexcept {
    // convert multi-word value to scalar value; always called with a value that won't exceed 64 bits
    return (_Wx[1] << 32) + _Wx[0];
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _MP_Add(_MP_arr _Wx, const uint64_t _Cx) noexcept {
    // perform "_Wx += _Cx"; always called with values that won't overflow 128 bits
    const auto _Result = _Get_u128_from_mp(_Wx) + _Cx;
    _Assign_mp_from_u128(_Wx, _Result);
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _MP_Mul(_MP_arr _Wx, const uint64_t _Prev, const uint64_t _Ax) noexcept {
    // set _Wx to the 128-bit product of _Prev and _Ax
    const auto _Result = _Unsigned128{_Prev} * _Ax;
    _Assign_mp_from_u128(_Wx, _Result);
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _MP_Rem(_MP_arr _Wx, const uint64_t _Mx) noexcept {
    // perform "_Wx %= _Mx"
    const auto _Result = _Get_u128_from_mp(_Wx) % _Mx;
    _Assign_mp_from_u128(_Wx, _Result);
}
_STD_END
