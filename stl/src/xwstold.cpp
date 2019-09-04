// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _WStold function

#include "xmath.h"
#include "xxlftype.h"
#include "xxwctype.h"
#include <wchar.h>

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL _WStoldx(const CTYPE* s, CTYPE** endptr, long pten, int* perr)
#include "xxstod.h"

    _CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL
    _WStold(const CTYPE* s, CTYPE** endptr, long pten) { // convert string, discard error code
    return _WStoldx(s, endptr, pten, 0);
}

_END_EXTERN_C_UNLESS_PURE
