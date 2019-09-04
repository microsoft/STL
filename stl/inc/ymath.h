// ymath.h internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _YMATH
#define _YMATH
#include <yvals.h>
#if _STL_COMPILER_PREPROCESSOR
#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_EXTERN_C_UNLESS_PURE

// MACROS FOR _Dtest RETURN (0 => ZERO)
#define _DENORM (-2) // C9X only
#define _FINITE (-1)
#define _INFCODE 1
#define _NANCODE 2

// MACROS FOR _Feraise ARGUMENT
#define _FE_DIVBYZERO 0x04
#define _FE_INEXACT 0x20
#define _FE_INVALID 0x01
#define _FE_OVERFLOW 0x08
#define _FE_UNDERFLOW 0x10

void __CLRCALL_PURE_OR_CDECL _Feraise(int);

union _Dconst { // pun float types as integer array
    unsigned short _Word[8]; // TRANSITION, ABI: Twice as large as necessary.
    float _Float;
    double _Double;
    long double _Long_double;
};

// double DECLARATIONS
_CRTIMP2_PURE double __CLRCALL_PURE_OR_CDECL _Cosh(double, double);
_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _Dtest(double*);
_CRTIMP2_PURE double __CLRCALL_PURE_OR_CDECL _Sinh(double, double);

_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _Exp(double*, double, short);
extern _CRTIMP2_PURE_IMPORT /* const */ _Dconst _Denorm, _Hugeval, _Inf, _Nan, _Snan;

// float DECLARATIONS
_CRTIMP2_PURE float __CLRCALL_PURE_OR_CDECL _FCosh(float, float);
_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _FDtest(float*);
_CRTIMP2_PURE float __CLRCALL_PURE_OR_CDECL _FSinh(float, float);

_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _FExp(float*, float, short);
extern _CRTIMP2_PURE_IMPORT /* const */ _Dconst _FDenorm, _FInf, _FNan, _FSnan;

// long double DECLARATIONS
_CRTIMP2_PURE long double __CLRCALL_PURE_OR_CDECL _LCosh(long double, long double);
_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _LDtest(long double*);
_CRTIMP2_PURE long double __CLRCALL_PURE_OR_CDECL _LSinh(long double, long double);

_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _LExp(long double*, long double, short);
extern _CRTIMP2_PURE_IMPORT /* const */ _Dconst _LDenorm, _LInf, _LNan, _LSnan;

_END_EXTERN_C_UNLESS_PURE

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _YMATH
