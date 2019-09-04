// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _WStod function

#include "xmath.h"
#include "xxdftype.h"
#include "xxwctype.h"
#include <stdlib.h>
#include <wchar.h>

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL _WStodx(const CTYPE* s, CTYPE** endptr, long pten, int* perr)
#include "xxstod.h"

    _CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL
    _WStod(const CTYPE* s, CTYPE** endptr, long pten) { // convert string, discard error code
    return _WStodx(s, endptr, pten, 0);
}

_END_EXTERN_C_UNLESS_PURE
