// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _WStof function

#include <cwchar>

#include "xmath.hpp"
#include "xxfftype.hpp"
#include "xxwctype.hpp"

_EXTERN_C_UNLESS_PURE

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL _WStofx(const CTYPE* s, CTYPE** endptr, long pten, int* perr)
#include "xxstod.hpp"

    // TRANSITION, ABI: preserved for binary compatibility
    _CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL
    _WStof(const CTYPE* s, CTYPE** endptr, long pten) { // convert string, discard error code
    return _WStofx(s, endptr, pten, nullptr);
}

_END_EXTERN_C_UNLESS_PURE
