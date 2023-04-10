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
        errno     = EDOM;
        using _Ty = _STD decay_t<decltype(_Fn())>;
        return _STD numeric_limits<_Ty>::quiet_NaN();
        _CATCH_END
    }
} // unnamed namespace

_EXTERN_C
[[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_assoc_laguerre(
    const unsigned int _Pn, const unsigned int _Pm, const double _Px) noexcept {
    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::laguerre(_Pn, _Pm, _Px); });
}

[[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_assoc_laguerref(
    const unsigned int _Pn, const unsigned int _Pm, const float _Px) noexcept {
    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::laguerre(_Pn, _Pm, _Px); });
}

[[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_assoc_legendre(
    const unsigned int _Pl, const unsigned int _Pm, const double _Px) noexcept {
    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] {
        auto _Result = ::boost::math::legendre_p(_Pl, _Pm, _Px);
        // Boost includes the Condon-Shortley phase term (-1)^m, std does not
        if ((_Pm & 1u) != 0) {
            _Result = -_Result;
        }
        return _Result;
    });
}

[[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_assoc_legendref(
    const unsigned int _Pl, const unsigned int _Pm, const float _Px) noexcept {
    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] {
        auto _Result = ::boost::math::legendre_p(_Pl, _Pm, _Px);
        // Boost includes the Condon-Shortley phase term (-1)^m, std does not
        if ((_Pm & 1u) != 0) {
            _Result = -_Result;
        }
        return _Result;
    });
}

[[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_beta(const double _Px, const double _Py) noexcept {
    return _Boost_call([=] { return ::boost::math::beta(_Px, _Py); });
}

[[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_betaf(const float _Px, const float _Py) noexcept {
    return _Boost_call([=] { return ::boost::math::beta(_Px, _Py); });
}

[[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_comp_ellint_1(const double _Pk) noexcept {
    return _Boost_call([=] { return ::boost::math::ellint_1(_Pk); });
}

[[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_comp_ellint_1f(const float _Pk) noexcept {
    return _Boost_call([=] { return ::boost::math::ellint_1(_Pk); });
}

[[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_comp_ellint_2(const double _Pk) noexcept {
    if (_STD isnan(_Pk)) {
        return _Pk;
    }

    return _Boost_call([=] { return ::boost::math::ellint_2(_Pk); });
}

[[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_comp_ellint_2f(const float _Pk) noexcept {
    if (_STD isnan(_Pk)) {
        return _Pk;
    }

    return _Boost_call([=] { return ::boost::math::ellint_2(_Pk); });
}

[[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_comp_ellint_3(const double _Pk, const double _Pnu) noexcept {
    if (_STD isnan(_Pk)) {
        return _Pk;
    }

    if (_STD isnan(_Pnu)) {
        return _Pnu;
    }

    return _Boost_call([=] { return ::boost::math::ellint_3(_Pk, _Pnu); });
}

[[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_comp_ellint_3f(const float _Pk, const float _Pnu) noexcept {
    if (_STD isnan(_Pk)) {
        return _Pk;
    }

    if (_STD isnan(_Pnu)) {
        return _Pnu;
    }

    return _Boost_call([=] { return ::boost::math::ellint_3(_Pk, _Pnu); });
}

[[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_cyl_bessel_i(const double _Pnu, const double _Px) noexcept {
    if (_STD isnan(_Pnu)) {
        return _Pnu;
    }

    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::cyl_bessel_i(_Pnu, _Px); });
}

[[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_cyl_bessel_if(const float _Pnu, const float _Px) noexcept {
    if (_STD isnan(_Pnu)) {
        return _Pnu;
    }

    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::cyl_bessel_i(_Pnu, _Px); });
}

[[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_cyl_bessel_j(const double _Pnu, const double _Px) noexcept {
    if (_STD isnan(_Pnu)) {
        return _Pnu;
    }

    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::cyl_bessel_j(_Pnu, _Px); });
}

[[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_cyl_bessel_jf(const float _Pnu, const float _Px) noexcept {
    if (_STD isnan(_Pnu)) {
        return _Pnu;
    }

    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::cyl_bessel_j(_Pnu, _Px); });
}

[[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_cyl_bessel_k(const double _Pnu, const double _Px) noexcept {
    if (_STD isnan(_Pnu)) {
        return _Pnu;
    }

    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::cyl_bessel_k(_Pnu, _Px); });
}

[[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_cyl_bessel_kf(const float _Pnu, const float _Px) noexcept {
    if (_STD isnan(_Pnu)) {
        return _Pnu;
    }

    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::cyl_bessel_k(_Pnu, _Px); });
}

[[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_cyl_neumann(const double _Pnu, const double _Px) noexcept {
    if (_STD isnan(_Pnu)) {
        return _Pnu;
    }

    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::cyl_neumann(_Pnu, _Px); });
}

[[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_cyl_neumannf(const float _Pnu, const float _Px) noexcept {
    if (_STD isnan(_Pnu)) {
        return _Pnu;
    }

    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::cyl_neumann(_Pnu, _Px); });
}

[[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_ellint_1(const double _Pk, const double _Pphi) noexcept {
    if (_STD isnan(_Pk)) {
        return _Pk;
    }

    if (_STD isnan(_Pphi)) {
        return _Pphi;
    }

    return _Boost_call([=] { return ::boost::math::ellint_1(_Pk, _Pphi); });
}

[[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_ellint_1f(const float _Pk, const float _Pphi) noexcept {
    if (_STD isnan(_Pk)) {
        return _Pk;
    }

    if (_STD isnan(_Pphi)) {
        return _Pphi;
    }

    return _Boost_call([=] { return ::boost::math::ellint_1(_Pk, _Pphi); });
}

[[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_ellint_2(const double _Pk, const double _Pphi) noexcept {
    if (_STD isnan(_Pk)) {
        return _Pk;
    }

    if (_STD isnan(_Pphi)) {
        return _Pphi;
    }

    return _Boost_call([=] { return ::boost::math::ellint_2(_Pk, _Pphi); });
}

[[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_ellint_2f(const float _Pk, const float _Pphi) noexcept {
    if (_STD isnan(_Pk)) {
        return _Pk;
    }

    if (_STD isnan(_Pphi)) {
        return _Pphi;
    }

    return _Boost_call([=] { return ::boost::math::ellint_2(_Pk, _Pphi); });
}

[[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_ellint_3(
    const double _Pk, const double _Pnu, const double _Pphi) noexcept {
    if (_STD isnan(_Pk)) {
        return _Pk;
    }

    if (_STD isnan(_Pnu)) {
        return _Pnu;
    }

    if (_STD isnan(_Pphi)) {
        return _Pphi;
    }

    return _Boost_call([=] { return ::boost::math::ellint_3(_Pk, _Pnu, _Pphi); });
}

[[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_ellint_3f(
    const float _Pk, const float _Pnu, const float _Pphi) noexcept {
    if (_STD isnan(_Pk)) {
        return _Pk;
    }

    if (_STD isnan(_Pnu)) {
        return _Pnu;
    }

    if (_STD isnan(_Pphi)) {
        return _Pphi;
    }

    return _Boost_call([=] { return ::boost::math::ellint_3(_Pk, _Pnu, _Pphi); });
}

[[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_expint(const double _Px) noexcept {
    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::expint(_Px); });
}

[[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_expintf(const float _Px) noexcept {
    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::expint(_Px); });
}

[[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_hermite(const unsigned int _Pn, const double _Px) noexcept {
    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::hermite(_Pn, _Px); });
}

[[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_hermitef(const unsigned int _Pn, const float _Px) noexcept {
    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::hermite(_Pn, _Px); });
}

[[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_laguerre(const unsigned int _Pn, const double _Px) noexcept {
    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::laguerre(_Pn, _Px); });
}

[[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_laguerref(const unsigned int _Pn, const float _Px) noexcept {
    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::laguerre(_Pn, _Px); });
}

[[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_legendre(const unsigned int _Pl, const double _Px) noexcept {
    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::legendre_p(_Pl, _Px); });
}

[[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_legendref(const unsigned int _Pl, const float _Px) noexcept {
    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::legendre_p(_Pl, _Px); });
}

[[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_riemann_zeta(const double _Px) noexcept {
    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::zeta(_Px); });
}

[[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_riemann_zetaf(const float _Px) noexcept {
    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::zeta(_Px); });
}

[[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_sph_bessel(
    const unsigned int _Pn, const double _Px) noexcept {
    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::sph_bessel(_Pn, _Px); });
}

[[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_sph_besself(const unsigned int _Pn, const float _Px) noexcept {
    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::sph_bessel(_Pn, _Px); });
}

[[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_sph_legendre(
    const unsigned int _Pl, const unsigned int _Pm, const double _Ptheta) noexcept {
    if (_STD isnan(_Ptheta)) {
        return _Ptheta;
    }

    return _Boost_call([=] { return ::boost::math::spherical_harmonic_r(_Pl, _Pm, _Ptheta, 0.0); });
}

[[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_sph_legendref(
    const unsigned int _Pl, const unsigned int _Pm, const float _Ptheta) noexcept {
    if (_STD isnan(_Ptheta)) {
        return _Ptheta;
    }

    return _Boost_call([=] { return ::boost::math::spherical_harmonic_r(_Pl, _Pm, _Ptheta, 0.0f); });
}

[[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_sph_neumann(
    const unsigned int _Pn, const double _Px) noexcept {
    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::sph_neumann(_Pn, _Px); });
}

[[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_sph_neumannf(
    const unsigned int _Pn, const float _Px) noexcept {
    if (_STD isnan(_Px)) {
        return _Px;
    }

    return _Boost_call([=] { return ::boost::math::sph_neumann(_Pn, _Px); });
}
_END_EXTERN_C

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
} // unnamed namespace

_EXTERN_C
[[nodiscard]] _CRT_SATELLITE_2 double __stdcall __std_smf_hypot3(
    const double _Dx, const double _Dy, const double _Dz) noexcept {
    return _Hypot3(_Dx, _Dy, _Dz);
}

[[nodiscard]] _CRT_SATELLITE_2 float __stdcall __std_smf_hypot3f(
    const float _Dx, const float _Dy, const float _Dz) noexcept {
    return _Hypot3(_Dx, _Dy, _Dz);
}
_END_EXTERN_C
