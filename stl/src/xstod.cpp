// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Stod function

#include <yvals.h> // include first, for native shared

#include <stdlib.h>

#include "xmath.hpp"
#include "xxcctype.hpp"
#include "xxdftype.hpp"

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL _Stodx(const CTYPE* s, CTYPE** endptr, long pten, int* perr)
#include "xxstod.hpp"

    _CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL
    _Stod(const CTYPE* s, CTYPE** endptr, long pten) { // convert string, discard error code
    return _Stodx(s, endptr, pten, 0);
}

_END_EXTERN_C_UNLESS_PURE
