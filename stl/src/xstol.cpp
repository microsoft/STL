// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Stolx function

#include <yvals.h>

#include "xmath.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE unsigned long __CLRCALL_PURE_OR_CDECL _Stoulx(const char*, char**, int, int*);

_CRTIMP2_PURE long __CLRCALL_PURE_OR_CDECL _Stolx(
    const char* s, char** endptr, int base, int* perr) { // convert string to long, with checking
    const char* sc;
    char *se, sign;
    unsigned long x;

    if (endptr == 0) {
        endptr = &se;
    }

    sc = s;
    while (isspace((unsigned char) *sc)) {
        ++sc;
    }

    sign = *sc == '-' || *sc == '+' ? *sc++ : '+';
    x    = _Stoulx(sc, endptr, base, perr);
    if (sc == *endptr) {
        *endptr = (char*) s;
    }

    if (s == *endptr && x != 0 || sign == '+' && LONG_MAX < x
        || sign == '-' && 0 - (unsigned long) LONG_MIN < x) { // overflow
        errno = ERANGE;
        if (perr != 0) {
            *perr = 1;
        }

        return sign == '-' ? LONG_MIN : LONG_MAX;
    }

    return (long) (sign == '-' ? 0 - x : x);
}

_END_EXTERN_C_UNLESS_PURE
