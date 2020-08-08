// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implements numerical algorithms for <complex>

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

#include <cmath>
#include <limits>
#include <type_traits>
#include <xutility>

#include "float_multi_prec.hpp"

#define _FMP ::std::_Float_multi_prec::

namespace {
    // TRANSITION: sqrt() isn't constexpr
    // _Hypot_leg_huge = _Ty{0.5} * _STD sqrt((_STD numeric_limits<_Ty>::max)());
    // _Hypot_leg_tiny = _STD sqrt(_Ty{2.0} * (_STD numeric_limits<_Ty>::min)() / _STD numeric_limits<_Ty>::epsilon());
    template <class _Ty>
    inline constexpr _Ty _Hypot_leg_huge = _Ty{6.703903964971298e+153};
    template <>
    inline constexpr float _Hypot_leg_huge<float> = 9.2233715e+18f;

    template <class _Ty>
    inline constexpr _Ty _Hypot_leg_tiny = _Ty{1.4156865331029228e-146};
    template <>
    inline constexpr float _Hypot_leg_tiny<float> = 4.440892e-16f;

    template <class _Ty>
    _NODISCARD _Ty _Norm_minus_one(const _Ty _Xval, const _Ty _Yval) noexcept {
        // requires |_Xval| >= |_Yval| and 0.5 <= |_Xval| < 2^12
        // returns _Xval * _Xval + _Yval * _Yval - 1
        const _FMP _Fmp_t<_Ty, 2> _Xsqr    = _FMP _Sqr_x2(_Xval);
        const _FMP _Fmp_t<_Ty, 2> _Ysqr    = _FMP _Sqr_x2(_Yval);
        const _FMP _Fmp_t<_Ty, 2> _Xsqr_m1 = _FMP _Add_small_x2(_Ty{-1.0}, _Xsqr);
        return _Add_x1(_Xsqr_m1, _Ysqr);
    }

    _NODISCARD inline float _Norm_minus_one(const float _Xval, const float _Yval) noexcept {
        const auto _Dx = static_cast<double>(_Xval);
        const auto _Dy = static_cast<double>(_Yval);
        return static_cast<float>((_Dx * _Dx - 1.0) + _Dy * _Dy);
    }

    // TRANSITION: CRT log1p can be inaccurate for tiny inputs under directed rounding modes
    template <class _Ty>
    _NODISCARD _Ty _Logp1(const _Ty _Xval) { // returns log(1 + _Xval)
        static_assert(_STD is_floating_point_v<_Ty>);

        if (_STD _Is_nan(_Xval)) { // NaN
            return _Xval + _Xval; // raise FE_INVALID if _Xval is a signaling NaN
        }

        if (_Xval <= _Ty{-0.5} || _Ty{2.0} <= _Xval) { // naive formula is moderately accurate
            if (_Xval == (_STD numeric_limits<_Ty>::max)()) { // avoid overflow
                return _STD log(_Xval);
            }

            return _STD log(_Ty{1.0} + _Xval);
        }

        const _Ty _Xabs = _STD _Float_abs(_Xval);
        if (_Xabs < _STD numeric_limits<_Ty>::epsilon()) { // zero or tiny
            if (_Xval == _Ty{0.0}) {
                return _Xval;
            }

            // honor rounding mode, raise FE_INEXACT
            return _Xval - _Ty{0.5} * _Xval * _Xval;
        }

        // compute log(1 + _Xval) with fixup for small _Xval
        const _FMP _Fmp_t<_Ty, 2> _Xp1 = _FMP _Add_small_x2(_Ty{1.0}, _Xval);
        return _STD log(_Xp1._Val0) + _Xp1._Val1 / _Xp1._Val0;
    }

    template <class _Ty>
    _NODISCARD _Ty _Log_hypot(const _Ty _Xval, const _Ty _Yval) noexcept { // returns log(hypot(_Xval, _Yval))
        static_assert(_STD is_floating_point_v<_Ty>);

        if (!_STD _Is_finite(_Xval) || !_STD _Is_finite(_Yval)) { // Inf or NaN
            // raise FE_INVALID and return NaN if at least one of them is a signaling NaN
            if (_STD _Is_signaling_nan(_Xval) || _STD _Is_signaling_nan(_Yval)) {
                return _Xval + _Yval;
            }

            // return +Inf if at least one of them is an infinity, even when the other is a quiet NaN
            if (_STD _Is_inf(_Xval)) {
                return _STD _Float_abs(_Xval);
            }

            if (_STD _Is_inf(_Yval)) {
                return _STD _Float_abs(_Yval);
            }

            // at least one of them is a quiet NaN, and the other is not an infinity
            return _Xval + _Yval;
        }

        _Ty _Av = _STD _Float_abs(_Xval);
        _Ty _Bv = _STD _Float_abs(_Yval);

        if (_Av < _Bv) { // ensure that _Bv <= _Av
            _STD swap(_Av, _Bv);
        }

        if (_Bv == 0) {
            return _STD log(_Av);
        }

        if (_Hypot_leg_tiny<_Ty> < _Av && _Av < _Hypot_leg_huge<_Ty>) { // no overflow or harmful underflow
            constexpr _Ty _Norm_small = _Ty{0.5};
            constexpr _Ty _Norm_big   = _Ty{3.0};

            const _Ty _Bv_sqr = _Bv * _Bv;

            if (_Av == _Ty{1.0}) { // correctly return +0 when _Av == 1 and _Bv * _Bv underflows
                                   // _Norm_minus_one(_Av, _Bv) could return -0 under FE_DOWNWARD rounding mode
                return _Logp1(_Bv_sqr) * _Ty{0.5};
            }

            const _Ty _Norm = _Av * _Av + _Bv_sqr;

            if (_Norm_small < _Norm && _Norm < _Norm_big) { // avoid catastrophic cancellation
                return _Logp1(_Norm_minus_one(_Av, _Bv)) * _Ty{0.5};
            } else {
                return _STD log(_Norm) * _Ty{0.5};
            }
        } else { // use 1 1/2 precision to preserve bits
            constexpr _Ty _Cm = _Ty{22713.0L / 32768.0L};
            constexpr _Ty _Cl = _Ty{1.4286068203094172321214581765680755e-6L};

            const int _Exponent      = _STD ilogb(_Av);
            const _Ty _Av_scaled     = _STD scalbn(_Av, -_Exponent);
            const _Ty _Bv_scaled     = _STD scalbn(_Bv, -_Exponent);
            const _Ty _Bv_scaled_sqr = _Bv_scaled * _Bv_scaled;
            const _Ty _Norm_scaled   = _Av_scaled * _Av_scaled + _Bv_scaled_sqr;
            const _Ty _Real_shifted  = _STD log(_Norm_scaled) * _Ty{0.5};
            return (_Real_shifted + _Exponent * _Cl) + _Exponent * _Cm;
        }
    }
} // unnamed namespace

_EXTERN_C
_NODISCARD double __stdcall __std_math_log_hypot(const double _Xval, const double _Yval) noexcept {
    return _Log_hypot(_Xval, _Yval);
}

_NODISCARD float __stdcall __std_math_log_hypotf(const float _Xval, const float _Yval) noexcept {
    return _Log_hypot(_Xval, _Yval);
}
_END_EXTERN_C
