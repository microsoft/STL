// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <cerrno>
#include <cmath>
#include <limits>
#include <ymath.h>

// macros for _Feraise argument
#define _FE_DIVBYZERO 0x04
#define _FE_INVALID   0x01
#define _FE_OVERFLOW  0x08
#define _FE_UNDERFLOW 0x10

// float properties
#define _D0 3 // little-endian, small long doubles
#define _D1 2
#define _D2 1
#define _D3 0

// IEEE 754 double properties
#define HUGE_EXP static_cast<int>(_DMAX * 900L / 1000)

// IEEE 754 float properties
#define FHUGE_EXP static_cast<int>(_FMAX * 900L / 1000)

#define _F0 1 // little-endian
#define _F1 0

// IEEE 754 long double properties
#define LHUGE_EXP static_cast<int>(_LMAX * 900L / 1000)

#define _L0 3 // little-endian, 64-bit long doubles
#define _L1 2
#define _L2 1
#define _L3 0

#define DSIGN(x) (reinterpret_cast<_Dval*>(&(x))->_Sh[_D0] & _DSIGN)
#define FSIGN(x) (reinterpret_cast<_Fval*>(&(x))->_Sh[_F0] & _FSIGN)
#define LSIGN(x) (reinterpret_cast<_Lval*>(&(x))->_Sh[_L0] & _LSIGN)

// return values for _Stopfx/_Stoflt
#define FL_ERR 0
#define FL_DEC 1
#define FL_HEX 2
#define FL_INF 3
#define FL_NAN 4
#define FL_NEG 8

// macros for _Dtest return (0 => ZERO)
#define _DENORM (-2) // C9X only
#define _FINITE (-1)

_EXTERN_C_UNLESS_PURE

void __CLRCALL_PURE_OR_CDECL _Feraise(int);

union _Dconst { // pun float types as integer array
    unsigned short _Word[8]; // TRANSITION, ABI: Twice as large as necessary.
    float _Float;
    double _Double;
    long double _Long_double;
};

_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _Dtest(double*);

extern _CRTIMP2_PURE _Dconst _Denorm;
extern _CRTIMP2_PURE _Dconst _Hugeval;
extern _CRTIMP2_PURE _Dconst _Inf;
extern _CRTIMP2_PURE _Dconst _Nan;
extern _CRTIMP2_PURE _Dconst _Snan;

_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _FDtest(float*);

extern _CRTIMP2_PURE _Dconst _FDenorm;
extern _CRTIMP2_PURE _Dconst _FInf;
extern _CRTIMP2_PURE _Dconst _FNan;
extern _CRTIMP2_PURE _Dconst _FSnan;

extern _CRTIMP2_PURE _Dconst _LDenorm;
extern _CRTIMP2_PURE _Dconst _LInf;
extern _CRTIMP2_PURE _Dconst _LNan;
extern _CRTIMP2_PURE _Dconst _LSnan;

int _Stopfx(const char**, char**);
_In_range_(0, maxsig) int _Stoflt(
    const char*, const char*, char**, _Out_writes_(maxsig) long[], _In_range_(1, 4) int maxsig);
_In_range_(0, maxsig) int _Stoxflt(
    const char*, const char*, char**, _Out_writes_(maxsig) long[], _In_range_(1, 4) int maxsig);
int _WStopfx(const wchar_t**, wchar_t**);
_In_range_(0, maxsig) int _WStoflt(
    const wchar_t*, const wchar_t*, wchar_t**, _Out_writes_(maxsig) long[], _In_range_(1, 4) int maxsig);
_In_range_(0, maxsig) int _WStoxflt(
    const wchar_t*, const wchar_t*, wchar_t**, _Out_writes_(maxsig) long[], _In_range_(1, 4) int maxsig);

// double declarations
union _Dval { // pun floating type as integer array
    unsigned short _Sh[8];
    double _Val;
};

unsigned short* _Pmsw(double*);

short _Dint(double*, short);
short _Dnorm(_Dval*);
short _Dscale(double*, long);
short _Dunscale(short*, double*);

double _Poly(double, const double*, int);

extern const _Dconst _Eps;
extern const _Dconst _Rteps;
extern const double _Xbig;

double _Xp_getw(const double*, int);
double* _Xp_setn(double*, int, long);
double* _Xp_setw(double*, int, double);
double* _Xp_addh(double*, int, double);
double* _Xp_mulh(double*, int, double);
double* _Xp_movx(double*, int, const double*);
double* _Xp_addx(double*, int, const double*, int);
double* _Xp_ldexpx(double*, int, int);
double* _Xp_mulx(double*, int, const double*, int, double*);

// float declarations
union _Fval { // pun floating type as integer array
    unsigned short _Sh[8];
    float _Val;
};

unsigned short* _FPmsw(float*);

short _FDint(float*, short);
short _FDnorm(_Fval*);
short _FDscale(float*, long);
short _FDunscale(short*, float*);

float _FPoly(float, const float*, int);

extern const _Dconst _FEps;
extern const _Dconst _FRteps;
extern const float _FXbig;

float _FXp_getw(const float*, int);
float* _FXp_setn(float*, int, long);
float* _FXp_setw(float*, int, float);
float* _FXp_addh(float*, int, float);
float* _FXp_mulh(float*, int, float);
float* _FXp_movx(float*, int, const float*);
float* _FXp_addx(float*, int, const float*, int);
float* _FXp_ldexpx(float*, int, int);
float* _FXp_mulx(float*, int, const float*, int, float*);

// long double declarations
union _Lval { // pun floating type as integer array
    unsigned short _Sh[8];
    long double _Val;
};

unsigned short* _LPmsw(long double*);

short _LDint(long double*, short);
short _LDnorm(_Lval*);
short _LDscale(long double*, long);
short _LDunscale(short*, long double*);
long double _LPoly(long double, const long double*, int);

extern const _Dconst _LEps;
extern const _Dconst _LRteps;
extern const long double _LXbig;

long double _LXp_getw(const long double*, int);
long double* _LXp_setn(long double*, int, long);
long double* _LXp_setw(long double*, int, long double);
long double* _LXp_addh(long double*, int, long double);
long double* _LXp_mulh(long double*, int, long double);
long double* _LXp_movx(long double*, int, const long double*);
long double* _LXp_addx(long double*, int, const long double*, int);
long double* _LXp_ldexpx(long double*, int, int);
long double* _LXp_mulx(long double*, int, const long double*, int, long double*);

_END_EXTERN_C_UNLESS_PURE

// raise IEEE 754 exceptions
#ifndef _M_CEE_PURE
#pragma float_control(except, on, push)
#endif

template <typename T>
[[nodiscard]] T _Xfe_overflow(const T sign) noexcept {
    static_assert(_STD is_floating_point_v<T>, "Expected is_floating_point_v<T>.");
    constexpr T huge = _STD numeric_limits<T>::max();
    return _STD copysign(huge, sign) * huge;
}

template <typename T>
[[nodiscard]] T _Xfe_underflow(const T sign) noexcept {
    static_assert(_STD is_floating_point_v<T>, "Expected is_floating_point_v<T>.");
    constexpr T tiny = _STD numeric_limits<T>::min();
    return _STD copysign(tiny, sign) * tiny;
}

#ifndef _M_CEE_PURE
#pragma float_control(pop)
#endif
