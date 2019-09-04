// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// parameters for long double floating-point type

#include <yvals.h>

#include <float.h>

#define FTYPE long double
#define FBITS LDBL_MANT_DIG
#define FMAXEXP LDBL_MAX_EXP

#define FFUN(fun) fun##l
#define FNAME(fun) _L##fun
#define FCONST(obj) _L##obj._Long_double
#define FLIT(lit) lit##L

#define FPMSW(x) (*_LPmsw(&(x)))
#define FSIGNBIT _LSIGN

#define FISNEG(x) (FPMSW(x) & FSIGNBIT)
#define FNEGATE(x) (FPMSW(x) ^= FSIGNBIT)
