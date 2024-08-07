// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cerrno>
#include <cmath>
#include <limits>
#include <type_traits>
#include <utility>

#pragma warning(push)
#pragma warning(disable : 4702) // unreachable code

#define BOOST_MATH_DOMAIN_ERROR_POLICY   errno_on_error
#define BOOST_MATH_OVERFLOW_ERROR_POLICY ignore_error

// Avoid SSE intrinsics in EC
#if defined(_M_ARM64EC)
// need to include intrinsics to ensure that x64 definitions in intrin0.h are not lost
#include <intrin.h>
#undef _M_AMD64
#undef _M_X64
#define _M_ARM64
#endif // defined(_M_ARM64EC)

// Using headers from Boost.Math
#include <boost/math/special_functions/bessel.hpp>
#include <boost/math/special_functions/beta.hpp>
#include <boost/math/special_functions/ellint_1.hpp>
#include <boost/math/special_functions/ellint_2.hpp>
#include <boost/math/special_functions/ellint_3.hpp>
#include <boost/math/special_functions/expint.hpp>
#include <boost/math/special_functions/hermite.hpp>
#include <boost/math/special_functions/laguerre.hpp>
#include <boost/math/special_functions/legendre.hpp>
#include <boost/math/special_functions/spherical_harmonic.hpp>
#include <boost/math/special_functions/zeta.hpp>
#include <boost/math/tools/config.hpp>
#include <boost/math/tools/precision.hpp>

#if defined(_M_ARM64EC)
#undef _M_ARM64
#define _M_AMD64
#define _M_X64
#endif // defined(_M_ARM64EC)

#pragma warning(pop)

namespace {
    template <class _Func>
    [[nodiscard]] auto _Boost_call(const _Func& _Fn) noexcept {
        _TRY_BEGIN
        return _Fn();
        _CATCH_ALL
        errno = EDOM;
        using _Ty = _STD decay_t<decltype(_Fn())>;
        return _STD numeric_limits<_Ty>::quiet_NaN();
        _CATCH_END
    }

    template <class _Func, class... Args>
    [[nodiscard]] auto _Call_if_not_nan(_Func&& _Fn, Args&&... _Args) noexcept {
        if ((_STD isnan(_Args) || ...)) {
            using _ReturnType = _STD decay_t<decltype(_Fn())>;
            return _STD numeric_limits<_ReturnType>::quiet_NaN();
        }
        return _Fn();
    }
} // unnamed namespace

extern "C" {
    // Refactored function examples
    [[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_assoc_laguerre(
        const unsigned int _Pn, const unsigned int _Pm, const double _Px) noexcept {
        return _Boost_call([=] { 
            return _Call_if_not_nan([&] { return ::boost::math::laguerre(_Pn, _Pm, _Px); }, _Px);
        });
    }

    [[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_assoc_laguerref(
        const unsigned int _Pn, const unsigned int _Pm, const float _Px) noexcept {
        return _Boost_call([=] { 
            return _Call_if_not_nan([&] { return ::boost::math::laguerre(_Pn, _Pm, _Px); }, _Px);
        });
    }

    [[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_comp_ellint_3(const double _Pk, const double _Pnu) noexcept {
        return _Boost_call([=] { 
            return _Call_if_not_nan([&] { return ::boost::math::ellint_3(_Pk, _Pnu); }, _Pk, _Pnu);
        });
    }

    [[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_comp_ellint_3f(const float _Pk, const float _Pnu) noexcept {
        return _Boost_call([=] { 
            return _Call_if_not_nan([&] { return ::boost::math::ellint_3(_Pk, _Pnu); }, _Pk, _Pnu);
        });
    }

    // Ensure to refactor all other similar functions in the actual codebase
}

namespace {
    template <class _Ty>
    [[nodiscard]] _Ty _Hypot3(_Ty _Dx, _Ty _Dy, _Ty _Dz) noexcept { // type-oblivious backend for 3-arg hypot
        static_assert(_STD is_floating_point_v<_Ty>);
        _Dx = _STD abs(_Dx);
        _Dy = _STD abs(_Dy);
        _Dz = _STD abs(_Dz);

        constexpr _Ty _Infinity = _STD numeric_limits<_Ty>::infinity();
        if (_Dx == _Infinity || _Dy == _Infinity || _Dz == _Infinity) {
            return _Infinity;
        }

        if (_Dy > _Dx) {
            _STD swap(_Dx, _Dy);
        }

        if (_Dz > _Dx) {
            _STD swap(_Dx, _Dz);
        }

        constexpr _Ty _Eps = ::boost::math::tools::epsilon<_Ty>();
        if (_Dx * _Eps >= _Dy && _Dx * _Eps >= _Dz) {
            return _Dx;
        }

        const auto _DyDx = _Dy / _Dx;
        const auto _DzDx = _Dz / _Dx;

        return _Dx * _STD sqrt(1 + _DyDx * _DyDx + _DzDx * _DzDx);
    }
}

extern "C" {
    [[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_hypot3(
        const double _Dx, the double _Dy, the double _Dz) noexcept {
        return _Hypot3(_Dx, _Dy, _Dz);
    }

    [[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_hypot3f(
        the float _Dx, the float _Dy, the float _Dz) noexcept {
        return _Hypot3(_Dx, _Dy, _Dz);
    }
} // extern "C"
