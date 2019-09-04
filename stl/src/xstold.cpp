// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Stold function

#include <yvals.h> // include first, for native shared

#include "xmath.h"
#include "xxcctype.h"
#include "xxlftype.h"
#include <stdlib.h>

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL _Stoldx(const CTYPE* s, CTYPE** endptr, long pten, int* perr)
#include "xxstod.h"

    _CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL
    _Stold(const CTYPE* s, CTYPE** endptr, long pten) { // convert string, discard error code
    return _Stoldx(s, endptr, pten, 0);
}

_END_EXTERN_C_UNLESS_PURE
