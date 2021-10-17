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

// macros for _Dtest return (0 => ZERO)
#define _DENORM  (-2) // C9X only
#define _FINITE  (-1)
#define _INFCODE 1
#define _NANCODE 2

void __CLRCALL_PURE_OR_CDECL _Feraise(int);

union _Dconst { // pun float types as integer array
    unsigned short _Word[8]; // TRANSITION, ABI: Twice as large as necessary.
    float _Float;
    double _Double;
    long double _Long_double;
};

_CRTIMP2_PURE double __CLRCALL_PURE_OR_CDECL _Cosh(double, double);
_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _Dtest(double*);
_CRTIMP2_PURE double __CLRCALL_PURE_OR_CDECL _Sinh(double, double);

_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _Exp(double*, double, short);
extern _CRTIMP2_PURE_IMPORT _Dconst _Denorm;
extern _CRTIMP2_PURE_IMPORT _Dconst _Hugeval;
extern _CRTIMP2_PURE_IMPORT _Dconst _Inf;
extern _CRTIMP2_PURE_IMPORT _Dconst _Nan;
extern _CRTIMP2_PURE_IMPORT _Dconst _Snan;

_CRTIMP2_PURE float __CLRCALL_PURE_OR_CDECL _FCosh(float, float);
_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _FDtest(float*);
_CRTIMP2_PURE float __CLRCALL_PURE_OR_CDECL _FSinh(float, float);

_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _FExp(float*, float, short);
extern _CRTIMP2_PURE_IMPORT _Dconst _FDenorm;
extern _CRTIMP2_PURE_IMPORT _Dconst _FInf;
extern _CRTIMP2_PURE_IMPORT _Dconst _FNan;
extern _CRTIMP2_PURE_IMPORT _Dconst _FSnan;

_CRTIMP2_PURE long double __CLRCALL_PURE_OR_CDECL _LCosh(long double, long double);
_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _LDtest(long double*);
_CRTIMP2_PURE long double __CLRCALL_PURE_OR_CDECL _LSinh(long double, long double);

_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _LExp(long double*, long double, short);
extern _CRTIMP2_PURE_IMPORT _Dconst _LDenorm;
extern _CRTIMP2_PURE_IMPORT _Dconst _LInf;
extern _CRTIMP2_PURE_IMPORT _Dconst _LNan;
extern _CRTIMP2_PURE_IMPORT _Dconst _LSnan;

_END_EXTERN_C_UNLESS_PURE

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _YMATH
