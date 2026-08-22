// __msvc_math.hpp internal header (core)

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef __MSVC_MATH_HPP
#define __MSVC_MATH_HPP

// When a user writes `#include <math.h>`, the MSVC frontend arranges for this file to be included first.
// Because this file is intended to supersede the UCRT's math components, we define the _INC_MATH macro
// to neutralize the contents of math.h when it is processed after. _INC_MATH is used as the include guard
// for the UCRT's math.h and is an intended public API.
// This file intentionally avoids providing most of the Microsoft-specific/non-Standard machinery in math.h.
#define _INC_MATH

#include <yvals_core.h>

#if _STL_COMPILER_PREPROCESSOR
#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

#pragma warning(disable : 4163) // 'meow' not available as an intrinsic function

extern "C++" {
namespace _Msvc { // duplicate type traits not provided by <yvals_core.h>
    template <class _Ty>
    constexpr bool _Is_arithmetic_v = false;

    template <>
    _INLINE_VAR constexpr bool _Is_arithmetic_v<bool> = true;
    template <>
    _INLINE_VAR constexpr bool _Is_arithmetic_v<char> = true;
    template <>
    _INLINE_VAR constexpr bool _Is_arithmetic_v<signed char> = true;
    template <>
    _INLINE_VAR constexpr bool _Is_arithmetic_v<unsigned char> = true;
#ifdef _NATIVE_WCHAR_T_DEFINED
    template <>
    _INLINE_VAR constexpr bool _Is_arithmetic_v<wchar_t> = true;
#endif // ^^^ defined(_NATIVE_WCHAR_T_DEFINED) ^^^
#ifdef __cpp_char8_t
    template <>
    _INLINE_VAR constexpr bool _Is_arithmetic_v<char8_t> = true;
#endif // ^^^ defined(__cpp_char8_t) ^^^
    template <>
    _INLINE_VAR constexpr bool _Is_arithmetic_v<char16_t> = true;
    template <>
    _INLINE_VAR constexpr bool _Is_arithmetic_v<char32_t> = true;
    template <>
    _INLINE_VAR constexpr bool _Is_arithmetic_v<short> = true;
    template <>
    _INLINE_VAR constexpr bool _Is_arithmetic_v<unsigned short> = true;
    template <>
    _INLINE_VAR constexpr bool _Is_arithmetic_v<int> = true;
    template <>
    _INLINE_VAR constexpr bool _Is_arithmetic_v<unsigned int> = true;
    template <>
    _INLINE_VAR constexpr bool _Is_arithmetic_v<long> = true;
    template <>
    _INLINE_VAR constexpr bool _Is_arithmetic_v<unsigned long> = true;
    template <>
    _INLINE_VAR constexpr bool _Is_arithmetic_v<long long> = true;
    template <>
    _INLINE_VAR constexpr bool _Is_arithmetic_v<unsigned long long> = true;
    template <>
    _INLINE_VAR constexpr bool _Is_arithmetic_v<float> = true;
    template <>
    _INLINE_VAR constexpr bool _Is_arithmetic_v<double> = true;
    template <>
    _INLINE_VAR constexpr bool _Is_arithmetic_v<long double> = true;

    template <class _Ty>
    constexpr bool _Is_arithmetic_v<const _Ty> = _Is_arithmetic_v<_Ty>;
    template <class _Ty>
    constexpr bool _Is_arithmetic_v<volatile _Ty> = _Is_arithmetic_v<_Ty>;
    template <class _Ty>
    constexpr bool _Is_arithmetic_v<const volatile _Ty> = _Is_arithmetic_v<_Ty>;

    template <bool _Test, class _Ty = void>
    struct _Enable_if {};
    template <class _Ty>
    struct _Enable_if<true, _Ty> {
        using type = _Ty;
    };

    template <bool _Test, class _Ty = void>
    using _Enable_if_t = typename _Enable_if<_Test, _Ty>::type;
} // namespace _Msvc
} // extern "C++"

extern "C" {

#define _STL_DEF_FUNC1(specs, name, ret, arg0)                                \
    _NODISCARD specs ret __cdecl name(arg0 _Xx) noexcept /* strengthened */ { \
        return __builtin_##name(_Xx);                                         \
    }

#define _STL_DEF_FUNC2(specs, name, ret, arg0, arg1)                                      \
    _NODISCARD specs ret __cdecl name(arg0 _Xx0, arg1 _Xx1) noexcept /* strengthened */ { \
        return __builtin_##name(_Xx0, _Xx1);                                              \
    }

#define _STL_DEF_FUNC3(specs, name, ret, arg0, arg1, arg2)                                           \
    _NODISCARD specs ret __cdecl name(arg0 _Xx0, arg1 _Xx1, arg2 _Xx2) noexcept /* strengthened */ { \
        return __builtin_##name(_Xx0, _Xx1, _Xx2);                                                   \
    }

// Defines three non-overloaded functions with signature `fp-type (fp-type)`.
#define _STL_DEF_FAMILY1(specs, name)            \
    _STL_DEF_FUNC1(specs, name##f, float, float) \
    _STL_DEF_FUNC1(specs, name, double, double)  \
    _STL_DEF_FUNC1(specs, name##l, long double, long double)

// Defines three non-overloaded functions with signature `fp-type (fp-type, fp-type)`.
#define _STL_DEF_FAMILY2(specs, name)                   \
    _STL_DEF_FUNC2(specs, name##f, float, float, float) \
    _STL_DEF_FUNC2(specs, name, double, double, double) \
    _STL_DEF_FUNC2(specs, name##l, long double, long double, long double)

// Defines three non-overloaded functions with signature `fp-type (fp-type, fp-type, fp-type)`.
#define _STL_DEF_FAMILY3(specs, name)                           \
    _STL_DEF_FUNC3(specs, name##f, float, float, float, float)  \
    _STL_DEF_FUNC3(specs, name, double, double, double, double) \
    _STL_DEF_FUNC3(specs, name##l, long double, long double, long double, long double)

// Defines three non-overloaded functions with signature `ret (fp-type)`.
#define _STL_DEF_RETURN_FAMILY1(specs, name, ret) \
    _STL_DEF_FUNC1(specs, name##f, ret, float)    \
    _STL_DEF_FUNC1(specs, name, ret, double)      \
    _STL_DEF_FUNC1(specs, name##l, ret, long double)

// Defines three non-overloaded functions with signature `fp-type (fp-type, lastarg)`.
#define _STL_DEF_LASTARG_FAMILY2(specs, name, lastarg)    \
    _STL_DEF_FUNC2(specs, name##f, float, float, lastarg) \
    _STL_DEF_FUNC2(specs, name, double, double, lastarg)  \
    _STL_DEF_FUNC2(specs, name##l, long double, long double, lastarg)

// Defines three non-overloaded functions with signature `fp-type (fp-type, fp-type, lastarg)`.
#define _STL_DEF_LASTARG_FAMILY3(specs, name, lastarg)           \
    _STL_DEF_FUNC3(specs, name##f, float, float, float, lastarg) \
    _STL_DEF_FUNC3(specs, name, double, double, double, lastarg) \
    _STL_DEF_FUNC3(specs, name##l, long double, long double, long double, lastarg)

#define _STL_DEF_OVERLOADED_CLASSIFICATION1(specs, ret, name, op_val, builtin, fptype)     \
    _NODISCARD specs ret __cdecl name(fptype _Xx) noexcept /* strengthened */ {            \
        return builtin(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, _Xx) op_val; \
    }

// Defines three overloaded functions with signature `ret (fp-type)`.
#define _STL_DEF_OVERLOADED_CLASSIFICATION_FAMILY1(specs, ret, name, op_val)                    \
    _STL_DEF_OVERLOADED_CLASSIFICATION1(specs, ret, name, op_val, __builtin_fpclassifyf, float) \
    _STL_DEF_OVERLOADED_CLASSIFICATION1(specs, ret, name, op_val, __builtin_fpclassify, double) \
    _STL_DEF_OVERLOADED_CLASSIFICATION1(specs, ret, name, op_val, __builtin_fpclassifyl, long double)

// N.B. static_cast<bool> is used when the builtin returns int.
#define _STL_DEF_OVERLOADED_COMPARISON1(specs, name, builtin, fptype)            \
    _NODISCARD specs bool __cdecl name(fptype _Xx) noexcept /* strengthened */ { \
        return static_cast<bool>(builtin(_Xx));                                  \
    }

#define _STL_DEF_OVERLOADED_COMPARISON2(specs, name, builtin, fptype)                          \
    _NODISCARD specs bool __cdecl name(fptype _Xx0, fptype _Xx1) noexcept /* strengthened */ { \
        return static_cast<bool>(builtin(_Xx0, _Xx1));                                         \
    }

// Defines three overloaded functions with signature `bool (fp-type)`.
#define _STL_DEF_OVERLOADED_COMPARISON_FAMILY1(specs, name)                  \
    _STL_DEF_OVERLOADED_COMPARISON1(specs, name, __builtin_##name##f, float) \
    _STL_DEF_OVERLOADED_COMPARISON1(specs, name, __builtin_##name, double)   \
    _STL_DEF_OVERLOADED_COMPARISON1(specs, name, __builtin_##name##l, long double)

// Defines four overloaded functions with signature `bool (fp-type, fp-type)`.
#define _STL_DEF_OVERLOADED_COMPARISON_FAMILY2(specs, name)                                                \
    _STL_DEF_OVERLOADED_COMPARISON2(specs, name, __builtin_##name##f, float)                               \
    _STL_DEF_OVERLOADED_COMPARISON2(specs, name, __builtin_##name, double)                                 \
    _STL_DEF_OVERLOADED_COMPARISON2(specs, name, __builtin_##name##l, long double)                         \
    template <class _Ty0, class _Ty1,                                                                      \
        ::_Msvc::_Enable_if_t<::_Msvc::_Is_arithmetic_v<_Ty0>&& ::_Msvc::_Is_arithmetic_v<_Ty1>, int> = 0> \
    _NODISCARD specs bool __cdecl name(_Ty0 _Xx0, _Ty1 _Xx1) noexcept /* strengthened */ {                 \
        return static_cast<bool>(__builtin_##name(static_cast<double>(_Xx0), static_cast<double>(_Xx1)));  \
    }

// *Declares* three non-overloaded functions with signature `fp-type (fp-type)`.
// These declarations are intended to match UCRT function definitions because libc does not yet implement them fully.
#define _STL_DECLARE_FAMILY1(specs, name)                                      \
    _NODISCARD specs float __cdecl name##f(float) noexcept /* strengthened */; \
    _NODISCARD specs double __cdecl name(double) noexcept /* strengthened */;  \
    _NODISCARD specs long double __cdecl name##l(long double) noexcept /* strengthened */;

// Some functions that are needed for C++26 constexpr cmath are not yet implemented by libc.
// We polyfill the library with the UCRT and mark these functions with this macro.
// The resulting declarations are not able to be used during constant evaluation yet.
#define _CONSTEXPR_CMATH26_NYI      inline
#define _CONSTEXPR_CMATH26_NYI_DECL _EMPTY_ARGUMENT

// On x86, when not using /arch:SSE2 or greater, floating-point operations are performed
// using the x87 instruction set and FLT_EVAL_METHOD is 2. (When /fp:fast is used,
// floating-point operations may be consistent, so we use the default types.)
#if defined(_M_IX86) && _M_IX86_FP < 2 && !defined(_M_FP_FAST)
using float_t  = long double;
using double_t = long double;
#else
using float_t  = float;
using double_t = double;
#endif

#define MATH_ERRNO     1
#define MATH_ERREXCEPT 2
// The UCRT defines this to be MATH_ERRNO | MATH_ERREXCEPT, but we're unsure of the quality of errno handling in libc.
// We broadcast MATH_ERREXCEPT as a minimum, keeping MATH_ERRNO as a potential future option.
#define math_errhandling (MATH_ERREXCEPT)

// These macro definitions were copied verbatim from the UCRT. The intent was
// to mirror their definitions exactly to avoid macro redefinition warnings.
#define _DENORM  (-2)
#define _FINITE  (-1)
#define _INFCODE 1
#define _NANCODE 2

#define FP_INFINITE  _INFCODE
#define FP_NAN       _NANCODE
#define FP_NORMAL    _FINITE
#define FP_SUBNORMAL _DENORM
#define FP_ZERO      0

#define _C2         1 // 0 if not 2's complement
#define FP_ILOGB0   (-0x7fffffff - _C2)
#define FP_ILOGBNAN 0x7fffffff

// These macro definitions were *not* copied verbatim from the UCRT because we
// are now able to take advantage of the __builtin_inf family.
#define INFINITY  (__builtin_inff())
#define HUGE_VALF (__builtin_inff())
#define HUGE_VAL  (__builtin_inf())
#define HUGE_VALL (__builtin_infl())
#define NAN       (__builtin_nanf(""))

// The nan family of functions is not routed to builtins because the builtins
// require a string literal argument and so cannot uphold the interface of these functions.
_NODISCARD float __cdecl nanf(const char*) noexcept /* strengthened */;
_NODISCARD double __cdecl nan(const char*) noexcept /* strengthened */;
_NODISCARD long double __cdecl nanl(const char*) noexcept /* strengthened */;

// Under /Oi (implied by /O2), MSVC treats the following functions as intrinsics that cannot be redefined.
// We use `#pragma function` to instruct MSVC to treat them as regular functions that can be defined,
// but we first need to declare them.
_NODISCARD _CONSTEXPR_CMATH26 float __cdecl acosf(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double __cdecl acos(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float __cdecl asinf(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double __cdecl asin(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float __cdecl atan2f(float, float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double __cdecl atan2(double, double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float __cdecl atanf(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double __cdecl atan(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 float __cdecl ceilf(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 double __cdecl ceil(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 float __cdecl copysignf(float, float) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 double __cdecl copysign(double, double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float __cdecl cosf(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double __cdecl cos(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float __cdecl expf(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double __cdecl exp(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 float __cdecl fabsf(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 double __cdecl fabs(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 float __cdecl floorf(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 double __cdecl floor(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 float __cdecl fmaf(float, float, float) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 double __cdecl fma(double, double, double) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 long double __cdecl fmal(long double, long double, long double) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 float __cdecl fmaxf(float, float) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 double __cdecl fmax(double, double) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 long double __cdecl fmaxl(long double, long double) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 float __cdecl fminf(float, float) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 double __cdecl fmin(double, double) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 long double __cdecl fminl(long double, long double) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 float __cdecl fmodf(float, float) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 double __cdecl fmod(double, double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float __cdecl log10f(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double __cdecl log10(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float __cdecl log2f(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double __cdecl log2(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float __cdecl logf(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double __cdecl log(double) noexcept;
_NODISCARD inline long __cdecl lrintf(float) noexcept;
_NODISCARD inline long __cdecl lrint(double) noexcept;
_NODISCARD inline long __cdecl lrintl(long double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float __cdecl powf(float, float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double __cdecl pow(double, double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float __cdecl sinf(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double __cdecl sin(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float __cdecl sqrtf(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double __cdecl sqrt(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float __cdecl tanf(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double __cdecl tan(double) noexcept;

#pragma function(acosf, acos)
#pragma function(asinf, asin)
#pragma function(atan2f, atan2)
#pragma function(atanf, atan)
#pragma function(ceilf, ceil)
#pragma function(copysignf, copysign)
#pragma function(cosf, cos)
#pragma function(expf, exp)
#pragma function(fabsf, fabs)
#pragma function(floorf, floor)
#pragma function(fmaf, fma, fmal)
#pragma function(fmaxf, fmax, fmaxl)
#pragma function(fminf, fmin, fminl)
#pragma function(fmodf, fmod)
#pragma function(log10f, log10)
#pragma function(log2f, log2)
#pragma function(logf, log)
#pragma function(lrintf, lrint, lrintl)
#pragma function(powf, pow)
#pragma function(sinf, sin)
#pragma function(sqrtf, sqrt)
#pragma function(tanf, tan)

// The following order matches N5054 [cmath.syn].
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, acos)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, asin)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, atan)
// atan2() is hand-crafted because of the workaround for LLVM-214934
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, cos)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, sin)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, tan)
_STL_DECLARE_FAMILY1(_CONSTEXPR_CMATH26_NYI_DECL, acosh)
_STL_DECLARE_FAMILY1(_CONSTEXPR_CMATH26_NYI_DECL, asinh)
_STL_DECLARE_FAMILY1(_CONSTEXPR_CMATH26_NYI_DECL, atanh)
_STL_DECLARE_FAMILY1(_CONSTEXPR_CMATH26_NYI_DECL, cosh) // additional workarounds for this polyfill are defined below
_STL_DECLARE_FAMILY1(_CONSTEXPR_CMATH26_NYI_DECL, sinh) // additional workarounds for this polyfill are defined below
_STL_DECLARE_FAMILY1(_CONSTEXPR_CMATH26_NYI_DECL, tanh) // additional workarounds for this polyfill are defined below
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, exp)
_STL_DECLARE_FAMILY1(_CONSTEXPR_CMATH26_NYI_DECL, exp2)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, expm1)
_STL_DEF_LASTARG_FAMILY2(_CONSTEXPR_CMATH23, frexp, int*) // _NODISCARD is desirable, despite the out-param
_STL_DEF_RETURN_FAMILY1(_CONSTEXPR_CMATH23, ilogb, int)
_STL_DEF_LASTARG_FAMILY2(_CONSTEXPR_CMATH23, ldexp, int)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, log)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, log10)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, log1p)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, log2)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH23, logb)
// modf() is hand-crafted because it has a unique signature
_STL_DEF_LASTARG_FAMILY2(_CONSTEXPR_CMATH23, scalbn, int)
_STL_DEF_LASTARG_FAMILY2(_CONSTEXPR_CMATH23, scalbln, long)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, cbrt)
// abs() is in <__msvc_stdlib.hpp>
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH23, fabs)
_STL_DEF_FAMILY2(_CONSTEXPR_CMATH26, hypot)
_STL_DEF_FAMILY2(_CONSTEXPR_CMATH26, pow)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, sqrt)
_STL_DECLARE_FAMILY1(_CONSTEXPR_CMATH26_NYI_DECL, erf)
_STL_DECLARE_FAMILY1(_CONSTEXPR_CMATH26_NYI_DECL, erfc)
// lgamma() is hand-crafted because it is not yet implemented by libc
// tgamma() is hand-crafted because it is not yet implemented by libc
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH23, ceil)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH23, floor)
_STL_DEF_FAMILY1(inline, nearbyint)
_STL_DEF_FAMILY1(inline, rint)
_STL_DEF_RETURN_FAMILY1(inline, lrint, long)
_STL_DEF_RETURN_FAMILY1(inline, llrint, long long)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH23, round)
_STL_DEF_RETURN_FAMILY1(_CONSTEXPR_CMATH23, lround, long)
_STL_DEF_RETURN_FAMILY1(_CONSTEXPR_CMATH23, llround, long long)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH23, trunc)
_STL_DEF_FAMILY2(_CONSTEXPR_CMATH23, fmod)
_STL_DEF_FAMILY2(_CONSTEXPR_CMATH23, remainder)
_STL_DEF_LASTARG_FAMILY3(_CONSTEXPR_CMATH23, remquo, int*) // _NODISCARD is desirable, despite the out-param
_STL_DEF_FAMILY2(_CONSTEXPR_CMATH23, copysign)
// nan() is declared above
_STL_DEF_FAMILY2(_CONSTEXPR_CMATH23, nextafter)
_STL_DEF_LASTARG_FAMILY2(_CONSTEXPR_CMATH23, nexttoward, long double)
_STL_DEF_FAMILY2(_CONSTEXPR_CMATH23, fdim)
_STL_DEF_FAMILY2(_CONSTEXPR_CMATH23, fmax)
_STL_DEF_FAMILY2(_CONSTEXPR_CMATH23, fmin)
_STL_DEF_FAMILY3(_CONSTEXPR_CMATH23, fma)

extern "C++" {
_STL_DEF_OVERLOADED_CLASSIFICATION_FAMILY1(_CONSTEXPR_CMATH23, int, fpclassify, /*unchanged*/)
_STL_DEF_OVERLOADED_CLASSIFICATION_FAMILY1(_CONSTEXPR_CMATH23, bool, isfinite, <= 0)
_STL_DEF_OVERLOADED_CLASSIFICATION_FAMILY1(_CONSTEXPR_CMATH23, bool, isinf, == FP_INFINITE)
_STL_DEF_OVERLOADED_CLASSIFICATION_FAMILY1(_CONSTEXPR_CMATH23, bool, isnan, == FP_NAN)
_STL_DEF_OVERLOADED_CLASSIFICATION_FAMILY1(_CONSTEXPR_CMATH23, bool, isnormal, == FP_NORMAL)

_STL_DEF_OVERLOADED_COMPARISON_FAMILY1(_CONSTEXPR_CMATH23, signbit)
_STL_DEF_OVERLOADED_COMPARISON_FAMILY2(_CONSTEXPR_CMATH23, isgreater)
_STL_DEF_OVERLOADED_COMPARISON_FAMILY2(_CONSTEXPR_CMATH23, isgreaterequal)
_STL_DEF_OVERLOADED_COMPARISON_FAMILY2(_CONSTEXPR_CMATH23, isless)
_STL_DEF_OVERLOADED_COMPARISON_FAMILY2(_CONSTEXPR_CMATH23, islessequal)
_STL_DEF_OVERLOADED_COMPARISON_FAMILY2(_CONSTEXPR_CMATH23, islessgreater)
_STL_DEF_OVERLOADED_COMPARISON_FAMILY2(_CONSTEXPR_CMATH23, isunordered)
} // extern "C++"

// TRANSITION, LLVM-214934: Preserve the sign of zero when atan2 underflows.
// __builtin_atan2 can return +0 for tiny negative results.
_NODISCARD _CONSTEXPR_CMATH26 float __cdecl atan2f(float _Xx0, float _Xx1) noexcept /* strengthened */ {
    float _Result = __builtin_atan2f(_Xx0, _Xx1);
    if (_Result == 0.0F && _Xx0 != 0.0F) {
        return __builtin_copysignf(0.0F, _Xx0);
    }
    return _Result;
}
_NODISCARD _CONSTEXPR_CMATH26 double __cdecl atan2(double _Xx0, double _Xx1) noexcept /* strengthened */ {
    double _Result = __builtin_atan2(_Xx0, _Xx1);
    if (_Result == 0.0 && _Xx0 != 0.0) {
        return __builtin_copysign(0.0, _Xx0);
    }
    return _Result;
}
_NODISCARD _CONSTEXPR_CMATH26 long double __cdecl atan2l(long double _Xx0, long double _Xx1) noexcept
/* strengthened */ {
    long double _Result = __builtin_atan2l(_Xx0, _Xx1);
    if (_Result == 0.0L && _Xx0 != 0.0L) {
        return __builtin_copysignl(0.0L, _Xx0);
    }
    return _Result;
}

// _NODISCARD is desirable, despite the out-param
_NODISCARD _CONSTEXPR_CMATH23 float __cdecl modff(float _Xx0, float* _Xx1) noexcept /* strengthened */ {
    return __builtin_modff(_Xx0, _Xx1);
}
_NODISCARD _CONSTEXPR_CMATH23 double __cdecl modf(double _Xx0, double* _Xx1) noexcept /* strengthened */ {
    return __builtin_modf(_Xx0, _Xx1);
}
_NODISCARD _CONSTEXPR_CMATH23 long double __cdecl modfl(long double _Xx0, long double* _Xx1) noexcept
/* strengthened */ {
    return __builtin_modfl(_Xx0, _Xx1);
}

// libc does not yet implement lgamma or tgamma. Polyfilling with the UCRT's implementation
// pulls in the UCRT's fma, which results in linker errors due to our conflicting definition.
// Instead, we prearranged for implementations of lgamma and tgamma to be ready in the satellite lib.
//
// _CRT_SATELLITE_2 is commented out because it is undefined at the point of this header's inclusion.
// We'd normally address that by including yvals.h, but that makes this header non-core.
// Otherwise, this header can be a core header. The macro definition is empty in usage,
// so the comment has the same physical effect (though maybe a different mental effect).
_NODISCARD /*_CRT_SATELLITE_2*/ float __stdcall __std_smf_lgammaf(float) noexcept;
_NODISCARD /*_CRT_SATELLITE_2*/ double __stdcall __std_smf_lgamma(double) noexcept;
_NODISCARD /*_CRT_SATELLITE_2*/ float __stdcall __std_smf_tgammaf(float) noexcept;
_NODISCARD /*_CRT_SATELLITE_2*/ double __stdcall __std_smf_tgamma(double) noexcept;

_NODISCARD _CONSTEXPR_CMATH26_NYI float __cdecl lgammaf(float _Xx) noexcept /* strengthened */ {
    return __std_smf_lgammaf(_Xx);
}
_NODISCARD _CONSTEXPR_CMATH26_NYI double __cdecl lgamma(double _Xx) noexcept /* strengthened */ {
    return __std_smf_lgamma(_Xx);
}
_NODISCARD _CONSTEXPR_CMATH26_NYI long double __cdecl lgammal(long double _Xx) noexcept /* strengthened */ {
    return __std_smf_lgamma(static_cast<double>(_Xx));
}

_NODISCARD _CONSTEXPR_CMATH26_NYI float __cdecl tgammaf(float _Xx) noexcept /* strengthened */ {
    return __std_smf_tgammaf(_Xx);
}
_NODISCARD _CONSTEXPR_CMATH26_NYI double __cdecl tgamma(double _Xx) noexcept /* strengthened */ {
    return __std_smf_tgamma(_Xx);
}
_NODISCARD _CONSTEXPR_CMATH26_NYI long double __cdecl tgammal(long double _Xx) noexcept /* strengthened */ {
    return __std_smf_tgamma(static_cast<double>(_Xx));
}

// The UCRT defines the following functions as inline functions that forward to their double siblings,
// so forward-declaring them without defining them would not result in a successful polyfill.
#if defined(_M_IX86) && !defined(_M_HYBRID_X86_ARM64)
#pragma function(coshf, sinhf, tanhf)
_NODISCARD _CONSTEXPR_CMATH26_NYI float __cdecl coshf(float _Xx) noexcept /* strengthened */ {
    return __builtin_coshf(_Xx);
}
_NODISCARD _CONSTEXPR_CMATH26_NYI float __cdecl sinhf(float _Xx) noexcept /* strengthened */ {
    return __builtin_sinhf(_Xx);
}
_NODISCARD _CONSTEXPR_CMATH26_NYI float __cdecl tanhf(float _Xx) noexcept /* strengthened */ {
    return __builtin_tanhf(_Xx);
}
#endif // ^^^ defined(_M_IX86) && !defined(_M_HYBRID_X86_ARM64) ^^^

#pragma function(coshl, sinhl, tanhl)
_NODISCARD _CONSTEXPR_CMATH26_NYI long double __cdecl coshl(long double _Xx) noexcept /* strengthened */ {
    return _CSTD cosh(static_cast<double>(_Xx));
}
_NODISCARD _CONSTEXPR_CMATH26_NYI long double __cdecl sinhl(long double _Xx) noexcept /* strengthened */ {
    return _CSTD sinh(static_cast<double>(_Xx));
}
_NODISCARD _CONSTEXPR_CMATH26_NYI long double __cdecl tanhl(long double _Xx) noexcept /* strengthened */ {
    return _CSTD tanh(static_cast<double>(_Xx));
}

#undef _STL_DEF_FUNC1
#undef _STL_DEF_FUNC2
#undef _STL_DEF_FUNC3
#undef _STL_DEF_FAMILY1
#undef _STL_DEF_FAMILY2
#undef _STL_DEF_FAMILY3
#undef _STL_DEF_RETURN_FAMILY1
#undef _STL_DEF_LASTARG_FAMILY2
#undef _STL_DEF_LASTARG_FAMILY3
#undef _STL_DEF_OVERLOADED_CLASSIFICATION1
#undef _STL_DEF_OVERLOADED_CLASSIFICATION_FAMILY1
#undef _STL_DEF_OVERLOADED_COMPARISON1
#undef _STL_DEF_OVERLOADED_COMPARISON2
#undef _STL_DEF_OVERLOADED_COMPARISON_FAMILY1
#undef _STL_DEF_OVERLOADED_COMPARISON_FAMILY2
#undef _STL_DECLARE_FAMILY1
#undef _CONSTEXPR_CMATH26_NYI
#undef _CONSTEXPR_CMATH26_NYI_DECL

} // extern "C"

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR

#endif // __MSVC_MATH_HPP
