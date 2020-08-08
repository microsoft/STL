// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implements multi-precision floating point arithmetic for numerical algorithms

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

#pragma once

#if defined(_M_FP_FAST) || defined(__FAST_MATH__)
#error float_multi_prec.hpp should not be compiled with /fp:fast or -ffast-math
#endif // defined(_M_FP_FAST) || defined(__FAST_MATH__)

#include <intrin.h>
#include <isa_availability.h>
#include <type_traits>
#include <xutility>

extern "C" int __isa_available;

_STD_BEGIN
namespace _Float_multi_prec {
    // multi-precision floating point types
    template <class _Ty, int _Prec>
    struct _Fmp_t;

    template <class _Ty>
    struct _Fmp_t<_Ty, 2> {
        static_assert(_STD is_floating_point_v<_Ty>);
        _Ty _Val0;
        _Ty _Val1;
    };

    // addition
    template <class _Ty>
    _NODISCARD constexpr _Fmp_t<_Ty, 2> _Add_x2(const _Ty _Xval, const _Ty _Yval) noexcept {
        const _Ty _Sum0 = _Xval + _Yval;
        const _Ty _Ymod = _Sum0 - _Xval;
        const _Ty _Xmod = _Sum0 - _Ymod;
        const _Ty _Yerr = _Yval - _Ymod;
        const _Ty _Xerr = _Xval - _Xmod;
        return {_Sum0, _Xerr + _Yerr};
    }

    // requires: exponent(_Xval) + countr_zero(significand(_Xval)) >= exponent(_Yval) || _Xval == 0
    template <class _Ty>
    _NODISCARD constexpr _Fmp_t<_Ty, 2> _Add_small_x2(const _Ty _Xval, const _Ty _Yval) noexcept {
        const _Ty _Sum0 = _Xval + _Yval;
        const _Ty _Ymod = _Sum0 - _Xval;
        const _Ty _Yerr = _Yval - _Ymod;
        return {_Sum0, _Yerr};
    }

    // requires: exponent(_Xval) + countr_zero(significand(_Xval)) >= exponent(_Yval._Val0) || _Xval == 0
    template <class _Ty>
    _NODISCARD constexpr _Fmp_t<_Ty, 2> _Add_small_x2(const _Ty _Xval, const _Fmp_t<_Ty, 2>& _Yval) noexcept {
        const _Fmp_t<_Ty, 2> _Sum0 = _Add_small_x2(_Xval, _Yval._Val0);
        return _Add_small_x2(_Sum0._Val0, _Sum0._Val1 + _Yval._Val1);
    }

    template <class _Ty>
    _NODISCARD constexpr _Ty _Add_x1(const _Fmp_t<_Ty, 2>& _Xval, const _Fmp_t<_Ty, 2>& _Yval) noexcept {
        const _Fmp_t<_Ty, 2> _Sum00 = _Add_x2(_Xval._Val0, _Yval._Val0);
        return _Sum00._Val0 + (_Sum00._Val1 + (_Xval._Val1 + _Yval._Val1));
    }

    // multiplication
    _NODISCARD inline constexpr double _High_half(const double _Val) {
        const auto _Bits           = _STD _Bit_cast<unsigned long long>(_Val);
        const auto _High_half_bits = (_Bits + 0x3ff'ffffULL) & 0xffff'ffff'f800'0000ULL;
        return _STD _Bit_cast<double>(_High_half_bits);
    }

    _NODISCARD inline constexpr double _Sqr_error_fallback(const double _Xval, const double _Prod0) noexcept {
        const double _Xhigh = _High_half(_Xval);
        const double _Xlow  = _Xval - _Xhigh;
        return ((_Xhigh * _Xhigh - _Prod0) + 2.0 * _Xhigh * _Xlow) + _Xlow * _Xlow;
    }

#if defined(_M_IX86) || defined(_M_X64)
    _NODISCARD inline double _Sqr_error_x86_x64_fma(const double _Xval, const double _Prod0) noexcept {
        const __m128d _Mx      = _mm_set_sd(_Xval);
        const __m128d _Mprod0  = _mm_set_sd(_Prod0);
        const __m128d _Mresult = _mm_fmsub_sd(_Mx, _Mx, _Mprod0);
        double _Result;
        _mm_store_sd(&_Result, _Mresult);
        return _Result;
    }
#endif // defined(_M_IX86) || defined(_M_X64)

#if defined(_M_ARM64)
    _NODISCARD inline double _Sqr_error_arm64_neon(const double _Xval, const double _Prod0) noexcept {
        const float64x1_t _Mx      = vld1_f64(&_Xval);
        const float64x1_t _Mprod0  = vld1_f64(&_Prod0);
        const float64x1_t _Mresult = vfma_f64(vneg_f64(_Mprod0), _Mx, _Mx);
        double _Result;
        vst1_f64(&_Result, _Mresult);
        return _Result;
    }
#endif // defined(_M_ARM64)

    _NODISCARD inline constexpr _Fmp_t<double, 2> _Sqr_x2(const double _Xval) noexcept {
        const double _Prod0 = _Xval * _Xval;

        if (_STD is_constant_evaluated()) {
            return {_Prod0, _Sqr_error_fallback(_Xval, _Prod0)};
        } else {
#if defined(_M_IX86) || defined(_M_X64)
            if (__isa_available >= __ISA_AVAILABLE_AVX2) {
                return {_Prod0, _Sqr_error_x86_x64_fma(_Xval, _Prod0)};
            } else {
                return {_Prod0, _Sqr_error_fallback(_Xval, _Prod0)};
            }
#elif defined(_M_ARM64) // ^^^ x86, x64 / arm64 vvv
            // https://docs.microsoft.com/en-us/cpp/build/arm64-windows-abi-conventions?view=vs-2019#base-requirements
            // Both floating-point and NEON support are presumed to be present in hardware.
            return {_Prod0, _Sqr_error_arm64_neon(_Xval, _Prod0)};
#else // ^^^ arm64 / arm vvv
            return {_Prod0, _Sqr_error_fallback(_Xval, _Prod0)};
#endif // ^^^ arm ^^^
        }
    }
} // namespace _Float_multi_prec
_STD_END
