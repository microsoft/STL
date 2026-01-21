// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// values used by math functions -- IEEE 754 version

#if defined(_M_CEE_PURE)
#if defined(MRTDLL)
#undef MRTDLL
#endif
#endif

#include <yvals.h>

_EXTERN_C_UNLESS_PURE

// TRANSITION, ABI: preserved for binary compatibility
union _Dconst { // pun float types as integer array
    unsigned short _Word[8];
    float _Float;
    double _Double;
    long double _Long_double;
};
extern _CRTIMP2_PURE _Dconst _Denorm  = {1, 0, 0, 0};
extern _CRTIMP2_PURE _Dconst _LDenorm = {1, 0, 0, 0};
extern _CRTIMP2_PURE _Dconst _Hugeval = {0, 0, 0, 0x7ff0};
extern _CRTIMP2_PURE _Dconst _Inf     = {0, 0, 0, 0x7ff0};
extern _CRTIMP2_PURE _Dconst _LInf    = {0, 0, 0, 0x7ff0};
extern _CRTIMP2_PURE _Dconst _Nan     = {0, 0, 0, 0x7ff8};
extern _CRTIMP2_PURE _Dconst _LNan    = {0, 0, 0, 0x7ff8};
extern _CRTIMP2_PURE _Dconst _Snan    = {1, 0, 0, 0x7ff0};
extern _CRTIMP2_PURE _Dconst _LSnan   = {1, 0, 0, 0x7ff0};
extern _CRTIMP2_PURE _Dconst _FDenorm = {1, 0};
extern _CRTIMP2_PURE _Dconst _FInf    = {0, 0x7f80};
extern _CRTIMP2_PURE _Dconst _FNan    = {0, 0x7fc0};
extern _CRTIMP2_PURE _Dconst _FSnan   = {1, 0x7f80};

_END_EXTERN_C_UNLESS_PURE
