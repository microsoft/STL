// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// values used by math functions -- IEEE 754 float version

#if defined(_M_CEE_PURE)
#if defined(MRTDLL)
#undef MRTDLL
#endif
#endif

#include "xmath.h"
// macros
#define NBITS (16 + _FOFF)

#define INIT(w0) \
    { 0, w0 }
#define INIT2(w0, w1) \
    { w1, w0 }

_EXTERN_C_UNLESS_PURE

// static data
extern /* const */ _Dconst _FDenorm = {INIT2(0, 1)};
extern const _Dconst _FEps          = {INIT((_FBIAS - NBITS - 1) << _FOFF)};
extern /* const */ _Dconst _FInf    = {INIT(_FMAX << _FOFF)};
extern /* const */ _Dconst _FNan    = {INIT((_FMAX << _FOFF) | (1 << (_FOFF - 1)))};
extern /* const */ _Dconst _FSnan   = {INIT2(_FMAX << _FOFF, 1)};
extern const _Dconst _FRteps        = {INIT((_FBIAS - NBITS / 2) << _FOFF)};

extern const float _FXbig = (NBITS + 1) * 347L / 1000;

_END_EXTERN_C_UNLESS_PURE
