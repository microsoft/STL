// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Stold function

#include <yvals.h> // include first, for native shared

#include <cstdlib>

#include "xmath.hpp"
#include "xxcctype.hpp"
#include "xxlftype.hpp"

_EXTERN_C_UNLESS_PURE

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL _Stoldx(const CTYPE* s, CTYPE** endptr, long pten, int* perr)
#include "xxstod.hpp"

    // TRANSITION, ABI: preserved for binary compatibility
    _CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL
    _Stold(const CTYPE* s, CTYPE** endptr, long pten) { // convert string, discard error code
    return _Stoldx(s, endptr, pten, nullptr);
}

_END_EXTERN_C_UNLESS_PURE
