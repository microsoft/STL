// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// values used by math functions -- IEEE 754 version

#if defined(_M_CEE_PURE)
#if defined(MRTDLL)
#undef MRTDLL
#endif
#endif

#include "xmath.h"

// macros
#define NBITS (48 + _DOFF)

#define INIT(w0) \
    { 0, 0, 0, w0 }
#define INIT2(w0, w1) \
    { w1, 0, 0, w0 }

// static data
extern /* const */ _Dconst _Denorm  = {INIT2(0, 1)};
extern const _Dconst _Eps           = {INIT((_DBIAS - NBITS - 1) << _DOFF)};
extern /* const */ _Dconst _Hugeval = {INIT(_DMAX << _DOFF)};
extern /* const */ _Dconst _Inf     = {INIT(_DMAX << _DOFF)};
extern /* const */ _Dconst _Nan     = {INIT((_DMAX << _DOFF) | (1 << (_DOFF - 1)))};
extern /* const */ _Dconst _Snan    = {INIT2(_DMAX << _DOFF, 1)};
extern const _Dconst _Rteps         = {INIT((_DBIAS - NBITS / 2) << _DOFF)};

extern const double _Xbig = (NBITS + 1) * 347L / 1000;
