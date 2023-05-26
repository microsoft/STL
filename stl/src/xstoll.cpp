// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Stoll function

#include <yvals.h>

#include <cctype>
#include <cerrno>
#include <climits>
#include <cstdlib>

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE unsigned long long __CLRCALL_PURE_OR_CDECL _Stoullx(const char*, char**, int, int*);

_CRTIMP2_PURE long long __CLRCALL_PURE_OR_CDECL _Stollx(
    const char* s, char** endptr, int base, int* perr) { // convert string to long long, with checking
    const char* sc;
    char* se;
    char sign;
    unsigned long long x;

    if (endptr == nullptr) {
        endptr = &se;
    }

    sc = s;
    while (isspace(static_cast<unsigned char>(*sc))) {
        ++sc;
    }

    sign = static_cast<char>(*sc == '-' || *sc == '+' ? *sc++ : '+');
    x    = _Stoullx(sc, endptr, base, perr);
    if (sc == *endptr) {
        *endptr = const_cast<char*>(s);
    }

    if (s == *endptr && x != 0 || sign == '+' && LLONG_MAX < x || sign == '-' && (1ull << 63) < x) { // overflow
        errno = ERANGE;
        if (perr != nullptr) {
            *perr = 1;
        }

        return sign == '-' ? LLONG_MIN : LLONG_MAX;
    }

    return static_cast<long long>(sign == '-' ? 0 - x : x);
}

_CRTIMP2_PURE long long(__CLRCALL_PURE_OR_CDECL _Stoll)(
    const char* s, char** endptr, int base) { // convert string, discard error code
    return _Stollx(s, endptr, base, nullptr);
}

_END_EXTERN_C_UNLESS_PURE
