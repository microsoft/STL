// __msvc_math.hpp internal header (core)

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// This lies outside of the inclusion guard because the UCRT's math.h allows
// for repeated inclusion with behavioral differences dictated by this macro.
// math.h can be included once without this macro defined and a second time
// with it defined, resulting in the nonstandard math constants being defined.
#ifdef _USE_MATH_DEFINES
#include <corecrt_math_defines.h>
#endif

#ifndef __MSVC_MATH_HPP
#define __MSVC_MATH_HPP

// When a user writes `#include <math.h>`, the MSVC frontend arranges for this
// file to be included first. Because this file is intended to supplant the
// CRT's math components, we define the _INC_MATH macro to neutralize the
// contents of math.h when it is processed after. _INC_MATH is used as the
// include guard for the CRT's math file and is an intended public API.
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

extern "C" {

#define _STL_DEF_FUNC1(specs, name, ret, fptype)                        \
    _NODISCARD specs ret name(fptype _Xx) noexcept /* strengthened */ { \
        return __builtin_##name(_Xx);                                   \
    }

#define _STL_DEF_FUNC2(specs, name, ret, arg0, arg1)                              \
    _NODISCARD specs ret name(arg0 _Xx0, arg1 _Xx1) noexcept /* strengthened */ { \
        return __builtin_##name(_Xx0, _Xx1);                                      \
    }

#define _STL_DEF_FUNC3(specs, name, ret, fptype)                                                   \
    _NODISCARD specs ret name(fptype _Xx0, fptype _Xx1, fptype _Xx2) noexcept /* strengthened */ { \
        return __builtin_##name(_Xx0, _Xx1, _Xx2);                                                 \
    }

// `long double` and `double` are the same width on MSVC, so `long double`
// variants of math functions forward to the `double` variants.
#define _STL_DEF_LDOUBLE_FORWARD_FUNC1(specs, name)                                      \
    _NODISCARD specs long double name##l(long double _Xx0) noexcept /* strengthened */ { \
        return name(static_cast<double>(_Xx0));                                          \
    }

// N.B. Declarations stamped out by these macros are not noexcept because they
// are intended to match the definitions in the UCRT, which are not themselves
// noexcept.
#define _STL_DECLARE_FUNC1(specs, name, ret, fptype) _NODISCARD specs ret name(fptype);

// Defines three non-overloaded functions with signature `fp-type(fp-type)`.
// The names of the float and long double variants are suffixed with `f` and `l`, respectively.
#define _STL_DEF_FAMILY1(specs, name)            \
    _STL_DEF_FUNC1(specs, name##f, float, float) \
    _STL_DEF_FUNC1(specs, name, double, double)  \
    _STL_DEF_FUNC1(specs, name##l, long double, long double)

// Defines three non-overloaded functions with signature `fp-type(fp-type, fp-type)`.
// The names of the float and long double variants are suffixed with `f` and `l`, respectively.
#define _STL_DEF_FAMILY2(specs, name)                   \
    _STL_DEF_FUNC2(specs, name##f, float, float, float) \
    _STL_DEF_FUNC2(specs, name, double, double, double) \
    _STL_DEF_FUNC2(specs, name##l, long double, long double, long double)

// Defines three non-overloaded functions with signature `fp-type(fp-type, fp-type, fp-type)`.
// The names of the float and long double variants are suffixed with `f` and `l`, respectively.
#define _STL_DEF_FAMILY3(specs, name)            \
    _STL_DEF_FUNC3(specs, name##f, float, float) \
    _STL_DEF_FUNC3(specs, name, double, double)  \
    _STL_DEF_FUNC3(specs, name##l, long double, long double)

// Defines three non-overloaded functions with signature `int-type(fp-type)`.
// The names of the float and long double variants are suffixed with `f` and `l`, respectively.
#define _STL_DEF_RETURN_FAMILY1(specs, name, ret) \
    _STL_DEF_FUNC1(specs, name##f, ret, float)    \
    _STL_DEF_FUNC1(specs, name, ret, double)      \
    _STL_DEF_FUNC1(specs, name##l, ret, long double)

// Defines three non-overloaded functions with signature `fp-type(fp-type, lastarg)`.
// The names of the float and long double variants are suffixed with `f` and `l`, respectively.
#define _STL_DEF_LASTARG_FAMILY2(specs, name, lastarg)    \
    _STL_DEF_FUNC2(specs, name##f, float, float, lastarg) \
    _STL_DEF_FUNC2(specs, name, double, double, lastarg)  \
    _STL_DEF_FUNC2(specs, name##l, long double, long double, lastarg)

// N.B. static_cast<bool> is used when the builtin returns int.
#define _STL_DEF_OVERLOADED_PREDICATE1(specs, name, builtin, fptype)     \
    _NODISCARD specs bool name(fptype _Xx) noexcept /* strengthened */ { \
        return static_cast<bool>(builtin(_Xx));                          \
    }

#define _STL_DEF_OVERLOADED_PREDICATE2(specs, name, builtin, fptype)                   \
    _NODISCARD specs bool name(fptype _Xx0, fptype _Xx1) noexcept /* strengthened */ { \
        return static_cast<bool>(builtin(_Xx0, _Xx1));                                 \
    }

// Defines three overloaded functions with signature `bool(fp-type)`.
#define _STL_DEF_OVERLOADED_PREDICATE_FAMILY1(specs, name)                  \
    _STL_DEF_OVERLOADED_PREDICATE1(specs, name, __builtin_##name##f, float) \
    _STL_DEF_OVERLOADED_PREDICATE1(specs, name, __builtin_##name, double)   \
    _STL_DEF_OVERLOADED_PREDICATE1(specs, name, __builtin_##name##l, long double)

// Defines four overloaded functions with signature `bool(fp-type, fp-type)`.
#define _STL_DEF_OVERLOADED_PREDICATE_FAMILY2(specs, name)                                                \
    _STL_DEF_OVERLOADED_PREDICATE2(specs, name, __builtin_##name##f, float)                               \
    _STL_DEF_OVERLOADED_PREDICATE2(specs, name, __builtin_##name, double)                                 \
    _STL_DEF_OVERLOADED_PREDICATE2(specs, name, __builtin_##name##l, long double)                         \
    template <class _Ty0, class _Ty1>                                                                     \
    _NODISCARD specs bool name(_Ty0 _Xx0, _Ty1 _Xx1) noexcept /* strengthened */ {                        \
        return static_cast<bool>(__builtin_##name(static_cast<double>(_Xx0), static_cast<double>(_Xx1))); \
    }

// **Declares** two non-overloaded functions with signature `fp-type(fp-type)`.
// A third function, corresponding to the `long double` variant of the function,
// is **defined** to call the double variant. These declarations are intended to
// match functions defined within the UCRT because libc does not yet implement
// them fully.
#define _STL_DECLARE_FAMILY1(specs, name)            \
    _STL_DECLARE_FUNC1(specs, name##f, float, float) \
    _STL_DECLARE_FUNC1(specs, name, double, double)  \
    _STL_DEF_LDOUBLE_FORWARD_FUNC1(specs, name)

// Some functions that are needed for C++26 constexpr cmath are not yet
// implemented by libc. We polyfill the library with the UCRT and mark these
// functions with this macro. The resulting declarations are not able to be
// used during constant-evaluation yet.
#define _CONSTEXPR_CMATH26_NYI inline

// On x86, when not using /arch:SSE2 or greater, floating point operations
// are performed using the x87 instruction set and FLT_EVAL_METHOD is 2.
// (When /fp:fast is used, floating point operations may be consistent, so
// we use the default types.)
#if defined(_M_IX86) && _M_IX86_FP < 2 && !defined(_M_FP_FAST)
using float_t  = long double;
using double_t = long double;
#else
using float_t  = float;
using double_t = double;
#endif

#define MATH_ERRNO     1
#define MATH_ERREXCEPT 2
// The UCRT defines this to be MATH_ERRNO | MATH_ERREXCEPT, but we are unsure
// of the quality of errno handling in libc. We broadcast MATH_ERREXCEPT as a
// minimum, leaving extension to include MATH_ERRNO as an option in the future.
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

// These macro definitions were _not_ copied verbatim from the UCRT because we
// are now able to take advantage of the __builtin_inf family.
#define INFINITY  (__builtin_inff())
#define HUGE_VALF (__builtin_inff())
#define HUGE_VAL  (__builtin_inf())
#define HUGE_VALL (__builtin_infl())
#define NAN       (__builtin_nanf(""))

// The nan family of functions is not routed to builtins because the builtins
// require a string literal argument and so cannot uphold the interface of
// these functions.
_NODISCARD float nanf(const char*);
_NODISCARD double nan(const char*);
_NODISCARD long double nanl(const char*);

// Under /Oi (implied by /O2), MSVC treats the following functions as intrinsics that
// cannot be redefined. We use #pragma function to instruct MSVC to treat them as regular
// functions that can be defined, but we first need to declare the function before we can
// do so.
_NODISCARD _CONSTEXPR_CMATH23 float ceilf(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 double ceil(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 double fabs(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 float floorf(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 double floor(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 float fmaxf(float, float) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 double fmax(double, double) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 long double fmaxl(long double, long double) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 float fminf(float, float) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 double fmin(double, double) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 long double fminl(long double, long double) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 float fmodf(float, float) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 double fmod(double, double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float acosf(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double acos(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float asinf(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double asin(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float atanf(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double atan(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 float copysignf(float, float) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 double copysign(double, double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float cosf(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double cos(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float expf(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double exp(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float logf(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double log(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float log10f(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double log10(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float log2f(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double log2(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float powf(float, float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double pow(double, double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float sinf(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double sin(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float sqrtf(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double sqrt(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 float tanf(float) noexcept;
_NODISCARD _CONSTEXPR_CMATH26 double tan(double) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 float fmaf(float, float, float) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 double fma(double, double, double) noexcept;
_NODISCARD _CONSTEXPR_CMATH23 long double fmal(long double, long double, long double) noexcept;

#pragma function(acosf, acos)
#pragma function(asinf, asin)
#pragma function(atanf, atan)
#pragma function(ceilf, ceil)
#pragma function(copysignf, copysign)
#pragma function(cosf, cos)
#pragma function(expf, exp)
#pragma function(fabs)
#pragma function(floorf, floor)
#pragma function(fmaf, fma, fmal)
#pragma function(fmaxf, fmax, fmaxl)
#pragma function(fminf, fmin, fminl)
#pragma function(fmodf, fmod)
#pragma function(log10f, log10)
#pragma function(log2f, log2)
#pragma function(logf, log)
#pragma function(powf, pow)
#pragma function(sinf, sin)
#pragma function(sqrtf, sqrt)
#pragma function(tanf, tan)

_STL_DEF_FAMILY1(_CONSTEXPR_CMATH23, ceil)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH23, logb)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH23, fabs)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH23, round)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH23, trunc)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH23, floor)
_STL_DEF_FAMILY2(_CONSTEXPR_CMATH23, fmod)
_STL_DEF_FAMILY2(_CONSTEXPR_CMATH23, remainder)
_STL_DEF_FAMILY2(_CONSTEXPR_CMATH23, copysign)
_STL_DEF_FAMILY2(_CONSTEXPR_CMATH23, nextafter)
_STL_DEF_FAMILY2(_CONSTEXPR_CMATH23, fdim)
_STL_DEF_FAMILY2(_CONSTEXPR_CMATH23, fmax)
_STL_DEF_FAMILY2(_CONSTEXPR_CMATH23, fmin)
_STL_DEF_RETURN_FAMILY1(_CONSTEXPR_CMATH23, lround, long)
_STL_DEF_RETURN_FAMILY1(_CONSTEXPR_CMATH23, llround, long long)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, acos)
_STL_DECLARE_FAMILY1(_CONSTEXPR_CMATH26_NYI, acosh)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, asin)
_STL_DECLARE_FAMILY1(_CONSTEXPR_CMATH26_NYI, asinh)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, atan)
_STL_DECLARE_FAMILY1(_CONSTEXPR_CMATH26_NYI, atanh)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, cbrt)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, cos)
_STL_DECLARE_FAMILY1(_CONSTEXPR_CMATH26_NYI, cosh)
_STL_DECLARE_FAMILY1(_CONSTEXPR_CMATH26_NYI, erf)
_STL_DECLARE_FAMILY1(_CONSTEXPR_CMATH26_NYI, erfc)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, exp)
_STL_DECLARE_FAMILY1(_CONSTEXPR_CMATH26_NYI, exp2)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, expm1)
_STL_DEF_FAMILY2(_CONSTEXPR_CMATH26, hypot)
_STL_DEF_FAMILY1(inline, nearbyint)

// libc does not yet implement lgamma or tgamma. Polyfilling with the UCRT's
// implementation pulls in the UCRT's fma, which results in linker errors due
// to our conflicting definition. Instead, we prearranged for implementations
// of lgamma and tgamma to be ready in the satellite lib.
//
// _CRT_SATELLITE_2 is commented out because it is undefined at the point of
// this header's inclusion. We'd normally address that by including yvals.h,
// but that makes this header non-core. Otherwise, this header can be a core
// header. The macro definition is empty in usage, so the comment has the same
// physical effect (though maybe a different mental effect).
_NODISCARD /*_CRT_SATELLITE_2*/ float __stdcall __std_smf_lgammaf(float) noexcept;
_NODISCARD /*_CRT_SATELLITE_2*/ double __stdcall __std_smf_lgamma(double) noexcept;
_NODISCARD /*_CRT_SATELLITE_2*/ float __stdcall __std_smf_tgammaf(float) noexcept;
_NODISCARD /*_CRT_SATELLITE_2*/ double __stdcall __std_smf_tgamma(double) noexcept;

_NODISCARD _CONSTEXPR_CMATH26_NYI float lgammaf(float _Xx) noexcept /* strengthened */ {
    return __std_smf_lgammaf(_Xx);
}

_NODISCARD _CONSTEXPR_CMATH26_NYI double lgamma(double _Xx) noexcept /* strengthened */ {
    return __std_smf_lgamma(_Xx);
}

_NODISCARD _CONSTEXPR_CMATH26_NYI long double lgammal(long double _Xx) noexcept /* strengthened */ {
    return __std_smf_lgamma(static_cast<double>(_Xx));
}

_NODISCARD _CONSTEXPR_CMATH26_NYI float tgammaf(float _Xx) noexcept /* strengthened */ {
    return __std_smf_tgammaf(_Xx);
}

_NODISCARD _CONSTEXPR_CMATH26_NYI double tgamma(double _Xx) noexcept /* strengthened */ {
    return __std_smf_tgamma(_Xx);
}

_NODISCARD _CONSTEXPR_CMATH26_NYI long double tgammal(long double _Xx) noexcept /* strengthened */ {
    return __std_smf_tgamma(static_cast<double>(_Xx));
}

_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, log)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, log10)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, log1p)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, log2)
_STL_DEF_FAMILY2(_CONSTEXPR_CMATH26, pow)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, sin)
_STL_DECLARE_FAMILY1(_CONSTEXPR_CMATH26_NYI, sinh)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, sqrt)
_STL_DEF_FAMILY1(_CONSTEXPR_CMATH26, tan)
_STL_DECLARE_FAMILY1(_CONSTEXPR_CMATH26_NYI, tanh)
_STL_DEF_FAMILY3(_CONSTEXPR_CMATH23, fma)

// The UCRT defines the following functions on x86 as inline functions that
// forward to their double siblings, so forward-declaring them without defining
// them does not a successful polyfill make.
#pragma function(coshf, cosh, coshl)
_NODISCARD _CONSTEXPR_CMATH26_NYI float coshf(float _Xx) {
    return __builtin_coshf(_Xx);
}

#pragma function(sinhf, sinh, sinhl)
_NODISCARD _CONSTEXPR_CMATH26_NYI float sinhf(float _Xx) {
    return __builtin_sinhf(_Xx);
}

#pragma function(tanhf, tanh, tanhl)
_NODISCARD _CONSTEXPR_CMATH26_NYI float tanhf(float _Xx) {
    return __builtin_tanhf(_Xx);
}

extern "C++" {
_NODISCARD _CONSTEXPR_CMATH23 int fpclassify(float _Xx) noexcept /* strengthened */ {
    return __builtin_fpclassifyf(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, _Xx);
}

_NODISCARD _CONSTEXPR_CMATH23 int fpclassify(double _Xx) noexcept /* strengthened */ {
    return __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, _Xx);
}

_NODISCARD _CONSTEXPR_CMATH23 int fpclassify(long double _Xx) noexcept /* strengthened */ {
    return __builtin_fpclassifyl(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, _Xx);
}

_NODISCARD _CONSTEXPR_CMATH23 bool isfinite(float _Xx) noexcept /* strengthened */ {
    return fpclassify(_Xx) <= 0;
}

_NODISCARD _CONSTEXPR_CMATH23 bool isfinite(double _Xx) noexcept /* strengthened */ {
    return fpclassify(_Xx) <= 0;
}

_NODISCARD _CONSTEXPR_CMATH23 bool isfinite(long double _Xx) noexcept /* strengthened */ {
    return fpclassify(_Xx) <= 0;
}

_NODISCARD _CONSTEXPR_CMATH23 bool isinf(float _Xx) noexcept /* strengthened */ {
    return fpclassify(_Xx) == FP_INFINITE;
}

_NODISCARD _CONSTEXPR_CMATH23 bool isinf(double _Xx) noexcept /* strengthened */ {
    return fpclassify(_Xx) == FP_INFINITE;
}

_NODISCARD _CONSTEXPR_CMATH23 bool isinf(long double _Xx) noexcept /* strengthened */ {
    return fpclassify(_Xx) == FP_INFINITE;
}

_NODISCARD _CONSTEXPR_CMATH23 bool isnan(float _Xx) noexcept /* strengthened */ {
    return fpclassify(_Xx) == FP_NAN;
}

_NODISCARD _CONSTEXPR_CMATH23 bool isnan(double _Xx) noexcept /* strengthened */ {
    return fpclassify(_Xx) == FP_NAN;
}

_NODISCARD _CONSTEXPR_CMATH23 bool isnan(long double _Xx) noexcept /* strengthened */ {
    return fpclassify(_Xx) == FP_NAN;
}

_NODISCARD _CONSTEXPR_CMATH23 bool isnormal(float _Xx) noexcept /* strengthened */ {
    return fpclassify(_Xx) == FP_NORMAL;
}

_NODISCARD _CONSTEXPR_CMATH23 bool isnormal(double _Xx) noexcept /* strengthened */ {
    return fpclassify(_Xx) == FP_NORMAL;
}

_NODISCARD _CONSTEXPR_CMATH23 bool isnormal(long double _Xx) noexcept /* strengthened */ {
    return fpclassify(_Xx) == FP_NORMAL;
}

_STL_DEF_OVERLOADED_PREDICATE_FAMILY1(_CONSTEXPR_CMATH23, signbit)
_STL_DEF_OVERLOADED_PREDICATE_FAMILY2(_CONSTEXPR_CMATH23, isgreater)
_STL_DEF_OVERLOADED_PREDICATE_FAMILY2(_CONSTEXPR_CMATH23, isgreaterequal)
_STL_DEF_OVERLOADED_PREDICATE_FAMILY2(_CONSTEXPR_CMATH23, isless)
_STL_DEF_OVERLOADED_PREDICATE_FAMILY2(_CONSTEXPR_CMATH23, islessequal)
_STL_DEF_OVERLOADED_PREDICATE_FAMILY2(_CONSTEXPR_CMATH23, islessgreater)
_STL_DEF_OVERLOADED_PREDICATE_FAMILY2(_CONSTEXPR_CMATH23, isunordered)
} // extern "C++"

_STL_DEF_LASTARG_FAMILY2(_CONSTEXPR_CMATH23, nexttoward, long double)
_STL_DEF_LASTARG_FAMILY2(_CONSTEXPR_CMATH23, frexp, int*)
_STL_DEF_RETURN_FAMILY1(_CONSTEXPR_CMATH23, ilogb, int)
_STL_DEF_LASTARG_FAMILY2(_CONSTEXPR_CMATH23, ldexp, int)

_NODISCARD _CONSTEXPR_CMATH23 float modff(float _Xx0, float* _Xx1) noexcept /* strengthened */ {
    return __builtin_modff(_Xx0, _Xx1);
}

_NODISCARD _CONSTEXPR_CMATH23 double modf(double _Xx0, double* _Xx1) noexcept /* strengthened */ {
    return __builtin_modf(_Xx0, _Xx1);
}

_NODISCARD _CONSTEXPR_CMATH23 long double modfl(long double _Xx0, long double* _Xx1) noexcept /* strengthened */ {
    return __builtin_modfl(_Xx0, _Xx1);
}

_STL_DEF_RETURN_FAMILY1(inline, lrint, long)
_STL_DEF_RETURN_FAMILY1(inline, llrint, long long)
_STL_DEF_FAMILY1(inline, rint)
_STL_DEF_LASTARG_FAMILY2(_CONSTEXPR_CMATH23, scalbn, int)
_STL_DEF_LASTARG_FAMILY2(_CONSTEXPR_CMATH23, scalbln, long)

_NODISCARD _CONSTEXPR_CMATH23 float remquof(float _Xx0, float _Xx1, int* _Xx2) noexcept /* strengthened */ {
    return __builtin_remquof(_Xx0, _Xx1, _Xx2);
}

_NODISCARD _CONSTEXPR_CMATH23 double remquo(double _Xx0, double _Xx1, int* _Xx2) noexcept /* strengthened */ {
    return __builtin_remquo(_Xx0, _Xx1, _Xx2);
}

_NODISCARD _CONSTEXPR_CMATH23 long double remquol(long double _Xx0, long double _Xx1, int* _Xx2) noexcept
/* strengthened */ {
    return __builtin_remquol(_Xx0, _Xx1, _Xx2);
}

// Preserve the sign of zero when atan2 underflows.
// __builtin_atan2 can return +0 for tiny negative results.
_NODISCARD _CONSTEXPR_CMATH26 float atan2f(float _Xx0, float _Xx1) noexcept /* strengthened */ {
    float _Result = static_cast<float>(__builtin_atan2f(_Xx0, _Xx1));
    if (_Result == 0.0F && _Xx0 != 0.0F) {
        return __builtin_copysignf(0.0F, _Xx0);
    }
    return _Result;
}

_NODISCARD _CONSTEXPR_CMATH26 double atan2(double _Xx0, double _Xx1) noexcept /* strengthened */ {
    double _Result = static_cast<double>(__builtin_atan2(_Xx0, _Xx1));
    if (_Result == 0.0 && _Xx0 != 0.0) {
        return __builtin_copysign(0.0, _Xx0);
    }
    return _Result;
}

_NODISCARD _CONSTEXPR_CMATH26 long double atan2l(long double _Xx0, long double _Xx1) noexcept /* strengthened */ {
    long double _Result = static_cast<long double>(__builtin_atan2l(_Xx0, _Xx1));
    if (_Result == 0.0L && _Xx0 != 0.0L) {
        return __builtin_copysignl(0.0L, _Xx0);
    }
    return _Result;
}

_NODISCARD double _j0(double);
_NODISCARD double _j1(double);
_NODISCARD double _jn(int, double);
_NODISCARD double _y0(double);
_NODISCARD double _y1(double);
_NODISCARD double _yn(int, double);

#if defined(_CRT_INTERNAL_NONSTDC_NAMES) && _CRT_INTERNAL_NONSTDC_NAMES

// These functions are included in the Open Group Standard Base Specifications.
_NODISCARD double j0(double);
_NODISCARD double j1(double);
_NODISCARD double jn(int, double);
_NODISCARD double y0(double);
_NODISCARD double y1(double);
_NODISCARD double yn(int, double);

#endif // defined(_CRT_INTERNAL_NONSTDC_NAMES) && _CRT_INTERNAL_NONSTDC_NAMES

#undef _STL_DEF_FUNC1
#undef _STL_DEF_FUNC2
#undef _STL_DEF_FUNC3
#undef _STL_DEF_LDOUBLE_FORWARD_FUNC1
#undef _STL_DECLARE_FUNC1
#undef _STL_DEF_FAMILY1
#undef _STL_DEF_FAMILY2
#undef _STL_DEF_FAMILY3
#undef _STL_DEF_RETURN_FAMILY1
#undef _STL_DEF_LASTARG_FAMILY2
#undef _STL_DEF_OVERLOADED_PREDICATE1
#undef _STL_DEF_OVERLOADED_PREDICATE2
#undef _STL_DEF_OVERLOADED_PREDICATE_FAMILY1
#undef _STL_DEF_OVERLOADED_PREDICATE_FAMILY2
#undef _STL_DECLARE_FAMILY1
#undef _CONSTEXPR_CMATH26_NYI

} // extern "C"

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR

#endif // __MSVC_MATH_HPP
