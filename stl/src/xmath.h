// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef _XMATH
#define _XMATH
#include <errno.h>
#include <math.h>
#include <stddef.h>
#include <ymath.h>

// FLOAT PROPERTIES
#define _D0 3 // little-endian, small long doubles
#define _D1 2
#define _D2 1
#define _D3 0

// IEEE 754 double properties
#define HUGE_EXP (int) (_DMAX * 900L / 1000)

// IEEE 754 float properties
#define FHUGE_EXP (int) (_FMAX * 900L / 1000)

#define _F0 1 // little-endian
#define _F1 0

// IEEE 754 long double properties
#define LHUGE_EXP (int) (_LMAX * 900L / 1000)

#define _L0 3 // little-endian, 64-bit long doubles
#define _L1 2
#define _L2 1
#define _L3 0

#define DSIGN(x) (((_Dval*) (char*) &(x))->_Sh[_D0] & _DSIGN)
#define FSIGN(x) (((_Fval*) (char*) &(x))->_Sh[_F0] & _FSIGN)
#define LSIGN(x) (((_Lval*) (char*) &(x))->_Sh[_L0] & _LSIGN)

#define _Fg _F1 // least-significant 16-bit word
#define _Dg _D3

// return values for _Stopfx/_Stoflt
#define FL_ERR 0
#define FL_DEC 1
#define FL_HEX 2
#define FL_INF 3
#define FL_NAN 4
#define FL_NEG 8

_EXTERN_C_UNLESS_PURE

int _Stopfx(const char**, char**);
int _Stoflt(const char*, const char*, char**, long[], int);
int _Stoxflt(const char*, const char*, char**, long[], int);
int _WStopfx(const wchar_t**, wchar_t**);
int _WStoflt(const wchar_t*, const wchar_t*, wchar_t**, long[], int);
int _WStoxflt(const wchar_t*, const wchar_t*, wchar_t**, long[], int);

// double declarations
union _Dval { // pun floating type as integer array
    unsigned short _Sh[8];
    double _Val;
};

unsigned short* _Plsw(double*);
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
double* _Xp_subx(double*, int, const double*, int);
double* _Xp_ldexpx(double*, int, int);
double* _Xp_mulx(double*, int, const double*, int, double*);
double* _Xp_invx(double*, int, double*);
double* _Xp_sqrtx(double*, int, double*);

// float declarations
union _Fval { // pun floating type as integer array
    unsigned short _Sh[8];
    float _Val;
};

unsigned short* _FPlsw(float*);
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
float* _FXp_subx(float*, int, const float*, int);
float* _FXp_ldexpx(float*, int, int);
float* _FXp_mulx(float*, int, const float*, int, float*);
float* _FXp_invx(float*, int, float*);
float* _FXp_sqrtx(float*, int, float*);

// long double declarations
union _Lval { // pun floating type as integer array
    unsigned short _Sh[8];
    long double _Val;
};

unsigned short* _LPlsw(long double*);
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
long double* _LXp_subx(long double*, int, const long double*, int);
long double* _LXp_ldexpx(long double*, int, int);
long double* _LXp_mulx(long double*, int, const long double*, int, long double*);
long double* _LXp_invx(long double*, int, long double*);
long double* _LXp_sqrtx(long double*, int, long double*);

_END_EXTERN_C_UNLESS_PURE

#endif // _XMATH
