// xtgmath.h internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _XTGMATH
#define _XTGMATH
#include <yvals.h>
#if _STL_COMPILER_PREPROCESSOR

#include <cstdlib>
#include <xtr1common>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
template <class _Ty1, class _Ty2>
using _Common_float_type_t = conditional_t<is_same_v<_Ty1, long double> || is_same_v<_Ty2, long double>, long double,
    conditional_t<is_same_v<_Ty1, float> && is_same_v<_Ty2, float>, float,
        double>>; // find type for two-argument math function
_STD_END

// FUNCTION TEMPLATE frexp
template <class _Ty, class = _STD enable_if_t<_STD is_integral_v<_Ty>>>
double frexp(_Ty _Value, _Out_ int* const _Exp) noexcept /* strengthened */ {
    return _CSTD frexp(static_cast<double>(_Value), _Exp);
}

// FUNCTION TEMPLATE pow
template <class _Ty1, class _Ty2, class = _STD enable_if_t<_STD is_arithmetic_v<_Ty1> && _STD is_arithmetic_v<_Ty2>>>
_NODISCARD _STD _Common_float_type_t<_Ty1, _Ty2> pow(const _Ty1 _Left, const _Ty2 _Right) noexcept /* strengthened */ {
    using _Common = _STD _Common_float_type_t<_Ty1, _Ty2>;
    return _CSTD pow(static_cast<_Common>(_Left), static_cast<_Common>(_Right));
}

// FUNCTION TEMPLATE fma
#if !_HAS_IF_CONSTEXPR
inline float _Fma(float _Left, float _Middle, float _Right) noexcept {
    return _CSTD fmaf(_Left, _Middle, _Right);
}

inline double _Fma(double _Left, double _Middle, double _Right) noexcept {
    return _CSTD fma(_Left, _Middle, _Right);
}

inline long double _Fma(long double _Left, long double _Middle, long double _Right) noexcept {
    return _CSTD fmal(_Left, _Middle, _Right);
}
#endif // !_HAS_IF_CONSTEXPR

template <class _Ty1, class _Ty2, class _Ty3,
    class = _STD enable_if_t<_STD is_arithmetic_v<_Ty1> && _STD is_arithmetic_v<_Ty2> && _STD is_arithmetic_v<_Ty3>>>
_NODISCARD _STD _Common_float_type_t<_Ty1, _STD _Common_float_type_t<_Ty2, _Ty3>> fma(
    _Ty1 _Left, _Ty2 _Middle, _Ty3 _Right) noexcept /* strengthened */ {
    using _Common = _STD _Common_float_type_t<_Ty1, _STD _Common_float_type_t<_Ty2, _Ty3>>;
#if _HAS_IF_CONSTEXPR
    if constexpr (_STD is_same_v<_Common, float>) {
        return _CSTD fmaf(static_cast<_Common>(_Left), static_cast<_Common>(_Middle), static_cast<_Common>(_Right));
    } else if constexpr (_STD is_same_v<_Common, double>) {
        return _CSTD fma(static_cast<_Common>(_Left), static_cast<_Common>(_Middle), static_cast<_Common>(_Right));
    } else {
        return _CSTD fmal(static_cast<_Common>(_Left), static_cast<_Common>(_Middle), static_cast<_Common>(_Right));
    }
#else // ^^^ use "if constexpr" dispatch / use overload resolution vvv
    return _Fma(static_cast<_Common>(_Left), static_cast<_Common>(_Middle), static_cast<_Common>(_Right));
#endif // _HAS_IF_CONSTEXPR
}

// FUNCTION TEMPLATE remquo
#if !_HAS_IF_CONSTEXPR
inline float _Remquo(float _Left, float _Right, int* _Pquo) noexcept {
    return _CSTD remquof(_Left, _Right, _Pquo);
}

inline double _Remquo(double _Left, double _Right, int* _Pquo) noexcept {
    return _CSTD remquo(_Left, _Right, _Pquo);
}

inline long double _Remquo(long double _Left, long double _Right, int* _Pquo) noexcept {
    return _CSTD remquol(_Left, _Right, _Pquo);
}
#endif // !_HAS_IF_CONSTEXPR

template <class _Ty1, class _Ty2, class = _STD enable_if_t<_STD is_arithmetic_v<_Ty1> && _STD is_arithmetic_v<_Ty2>>>
_STD _Common_float_type_t<_Ty1, _Ty2> remquo(_Ty1 _Left, _Ty2 _Right, int* _Pquo) noexcept /* strengthened */ {
    using _Common = _STD _Common_float_type_t<_Ty1, _Ty2>;
#if _HAS_IF_CONSTEXPR
    if constexpr (_STD is_same_v<_Common, float>) {
        return _CSTD remquof(static_cast<_Common>(_Left), static_cast<_Common>(_Right), _Pquo);
    } else if constexpr (_STD is_same_v<_Common, double>) {
        return _CSTD remquo(static_cast<_Common>(_Left), static_cast<_Common>(_Right), _Pquo);
    } else {
        return _CSTD remquol(static_cast<_Common>(_Left), static_cast<_Common>(_Right), _Pquo);
    }
#else // ^^^ use "if constexpr" dispatch / use overload resolution vvv
    return _Remquo(static_cast<_Common>(_Left), static_cast<_Common>(_Right), _Pquo);
#endif // _HAS_IF_CONSTEXPR
}

#define _GENERIC_MATH1R(FUN, RET)                                           \
    template <class _Ty, class = _STD enable_if_t<_STD is_integral_v<_Ty>>> \
    _NODISCARD RET FUN(_Ty _Left) noexcept /* strengthened */ {             \
        return _CSTD FUN(static_cast<double>(_Left));                       \
    }

#define _GENERIC_MATH1(FUN) _GENERIC_MATH1R(FUN, double)

#define _GENERIC_MATH1X(FUN, ARG2)                                             \
    template <class _Ty, class = _STD enable_if_t<_STD is_integral_v<_Ty>>>    \
    _NODISCARD double FUN(_Ty _Left, ARG2 _Arg2) noexcept /* strengthened */ { \
        return _CSTD FUN(static_cast<double>(_Left), _Arg2);                   \
    }

#define _GENERIC_MATH2(FUN)                                                                                     \
    template <class _Ty1, class _Ty2,                                                                           \
        class = _STD enable_if_t<_STD is_arithmetic_v<_Ty1> && _STD is_arithmetic_v<_Ty2>>>                     \
    _NODISCARD _STD _Common_float_type_t<_Ty1, _Ty2> FUN(_Ty1 _Left, _Ty2 _Right) noexcept /* strengthened */ { \
        using _Common = _STD _Common_float_type_t<_Ty1, _Ty2>;                                                  \
        return _CSTD FUN(static_cast<_Common>(_Left), static_cast<_Common>(_Right));                            \
    }

// The following order matches N4820 26.8.1 [cmath.syn].
_GENERIC_MATH1(acos)
_GENERIC_MATH1(asin)
_GENERIC_MATH1(atan)
_GENERIC_MATH2(atan2)
_GENERIC_MATH1(cos)
_GENERIC_MATH1(sin)
_GENERIC_MATH1(tan)
_GENERIC_MATH1(acosh)
_GENERIC_MATH1(asinh)
_GENERIC_MATH1(atanh)
_GENERIC_MATH1(cosh)
_GENERIC_MATH1(sinh)
_GENERIC_MATH1(tanh)
_GENERIC_MATH1(exp)
_GENERIC_MATH1(exp2)
_GENERIC_MATH1(expm1)
// frexp() is hand-crafted
_GENERIC_MATH1R(ilogb, int)
_GENERIC_MATH1X(ldexp, int)
_GENERIC_MATH1(log)
_GENERIC_MATH1(log10)
_GENERIC_MATH1(log1p)
_GENERIC_MATH1(log2)
_GENERIC_MATH1(logb)
// No modf(), types must match
_GENERIC_MATH1X(scalbn, int)
_GENERIC_MATH1X(scalbln, long)
_GENERIC_MATH1(cbrt)
// abs() has integer overloads
_GENERIC_MATH1(fabs)
_GENERIC_MATH2(hypot)
// 3-arg hypot() is hand-crafted
// pow() is hand-crafted
_GENERIC_MATH1(sqrt)
_GENERIC_MATH1(erf)
_GENERIC_MATH1(erfc)
_GENERIC_MATH1(lgamma)
_GENERIC_MATH1(tgamma)
_GENERIC_MATH1(ceil)
_GENERIC_MATH1(floor)
_GENERIC_MATH1(nearbyint)
_GENERIC_MATH1(rint)
_GENERIC_MATH1R(lrint, long)
_GENERIC_MATH1R(llrint, long long)
_GENERIC_MATH1(round)
_GENERIC_MATH1R(lround, long)
_GENERIC_MATH1R(llround, long long)
_GENERIC_MATH1(trunc)
_GENERIC_MATH2(fmod)
_GENERIC_MATH2(remainder)
// remquo() is hand-crafted
_GENERIC_MATH2(copysign)
// nan(const char*) is exempt
_GENERIC_MATH2(nextafter)
_GENERIC_MATH1X(nexttoward, long double)
_GENERIC_MATH2(fdim)
_GENERIC_MATH2(fmax)
_GENERIC_MATH2(fmin)
// fma() is hand-crafted
// lerp() should be exempt, LWG-3223
// The "classification/comparison functions" (fpclassify(), etc.) are exempt, LWG-1327
// TRANSITION, VSO-945789, Special Math shouldn't be exempt

#undef _GENERIC_MATH1R
#undef _GENERIC_MATH1
#undef _GENERIC_MATH1X
#undef _GENERIC_MATH2

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _XTGMATH
