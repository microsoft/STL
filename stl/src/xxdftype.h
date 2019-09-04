// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// parameters for double floating-point type

#include <yvals.h>

#include <float.h>

#define FTYPE double
#define FBITS DBL_MANT_DIG
#define FMAXEXP DBL_MAX_EXP

#define FFUN(fun) fun
#define FNAME(fun) _##fun
#define FCONST(obj) _##obj._Double
#define FLIT(lit) lit

#define FPMSW(x) (*_Pmsw(&(x)))
#define FSIGNBIT _DSIGN

#define FISNEG(x) (FPMSW(x) & FSIGNBIT)
#define FNEGATE(x) (FPMSW(x) ^= FSIGNBIT)
