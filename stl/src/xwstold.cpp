// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cwchar>

#include "xmath.hpp"
#include "xxlftype.hpp"
#include "xxwctype.hpp"

_EXTERN_C_UNLESS_PURE

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL _WStoldx(const CTYPE* s, CTYPE** endptr, long pten, int* perr) noexcept
#include "xxstod.hpp"

    // TRANSITION, ABI: preserved for binary compatibility
    _CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL
    _WStold(const CTYPE* s, CTYPE** endptr, long pten) noexcept { // convert string, discard error code
    return _WStoldx(s, endptr, pten, nullptr);
}

_END_EXTERN_C_UNLESS_PURE
