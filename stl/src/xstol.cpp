// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Stolx function

#include <yvals.h>

#include <cctype>
#include <cerrno>
#include <climits>
#include <cstdlib>

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE unsigned long __CLRCALL_PURE_OR_CDECL _Stoulx(const char*, char**, int, int*);

_CRTIMP2_PURE long __CLRCALL_PURE_OR_CDECL _Stolx(
    const char* s, char** endptr, int base, int* perr) { // convert string to long, with checking
    const char* sc;
    char* se;
    char sign;
    unsigned long x;

    if (endptr == nullptr) {
        endptr = &se;
    }

    sc = s;
    while (isspace(static_cast<unsigned char>(*sc))) {
        ++sc;
    }

    sign = *sc == '-' || *sc == '+' ? *sc++ : '+';
    x    = _Stoulx(sc, endptr, base, perr);
    if (sc == *endptr) {
        *endptr = const_cast<char*>(s);
    }

    if (s == *endptr && x != 0 || sign == '+' && LONG_MAX < x || sign == '-' && (1ul << 31) < x) { // overflow
        errno = ERANGE;
        if (perr != nullptr) {
            *perr = 1;
        }

        return sign == '-' ? LONG_MIN : LONG_MAX;
    }

    return static_cast<long>(sign == '-' ? 0 - x : x);
}

_END_EXTERN_C_UNLESS_PURE
