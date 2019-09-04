// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Stoll function

#include "xmath.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE unsigned long long __CLRCALL_PURE_OR_CDECL _Stoullx(const char*, char**, int, int*);

_CRTIMP2_PURE long long __CLRCALL_PURE_OR_CDECL _Stollx(
    const char* s, char** endptr, int base, int* perr) { // convert string to long long, with checking
    const char* sc;
    char *se, sign;
    unsigned long long x;

    if (endptr == 0) {
        endptr = &se;
    }

    sc = s;
    while (isspace((unsigned char) *sc)) {
        ++sc;
    }

    sign = (char) (*sc == '-' || *sc == '+' ? *sc++ : '+');
    x    = _Stoullx(sc, endptr, base, perr);
    if (sc == *endptr) {
        *endptr = (char*) s;
    }

    if (s == *endptr && x != 0 || sign == '+' && LLONG_MAX < x
        || sign == '-' && 0 - (unsigned long long) LLONG_MIN < x) { // overflow
        errno = ERANGE;
        if (perr != 0) {
            *perr = 1;
        }

        return sign == '-' ? LLONG_MIN : LLONG_MAX;
    }

    return (long long) (sign == '-' ? 0 - x : x);
}

_CRTIMP2_PURE long long(__CLRCALL_PURE_OR_CDECL _Stoll)(
    const char* s, char** endptr, int base) { // convert string, discard error code
    return _Stollx(s, endptr, base, 0);
}

_END_EXTERN_C_UNLESS_PURE
