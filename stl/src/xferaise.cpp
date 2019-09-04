// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Feraise function

#include <yvals.h>

#include <errno.h>
#include <ymath.h>

_EXTERN_C_UNLESS_PURE

void __CLRCALL_PURE_OR_CDECL _Feraise(int except) { // report floating-point exception
    if ((except & (_FE_DIVBYZERO | _FE_INVALID)) != 0) {
        errno = EDOM;
    } else if ((except & (_FE_UNDERFLOW | _FE_OVERFLOW)) != 0) {
        errno = ERANGE;
    }
}

_END_EXTERN_C_UNLESS_PURE
