// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// parameters for float floating-point type

#include <yvals.h>

#include <float.h>

#define FTYPE float
#define FBITS FLT_MANT_DIG
#define FMAXEXP FLT_MAX_EXP

#define FFUN(fun) fun##f
#define FNAME(fun) _F##fun
#define FCONST(obj) _F##obj._Float
#define FLIT(lit) lit##F

#define FPMSW(x) (*_FPmsw(&(x)))
#define FSIGNBIT _FSIGN

#define FISNEG(x) (FPMSW(x) & FSIGNBIT)
#define FNEGATE(x) (FPMSW(x) ^= FSIGNBIT)
