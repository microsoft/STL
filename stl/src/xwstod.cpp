// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _WStod function

#include <cstdlib>
#include <cwchar>

#include "xmath.hpp"
#include "xxdftype.hpp"
#include "xxwctype.hpp"

_EXTERN_C_UNLESS_PURE

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL _WStodx(const CTYPE* s, CTYPE** endptr, long pten, int* perr)
#include "xxstod.hpp"

    // TRANSITION, ABI: preserved for binary compatibility
    _CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL
    _WStod(const CTYPE* s, CTYPE** endptr, long pten) { // convert string, discard error code
    return _WStodx(s, endptr, pten, nullptr);
}

_END_EXTERN_C_UNLESS_PURE
