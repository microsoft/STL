// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Stoull function

#include "xmath.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

_EXTERN_C_UNLESS_PURE

// macros
#define BASE_MAX 36 // largest valid base

// static data
static const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz"; // valid digits

// 64-bits!
static const char ndigs[BASE_MAX + 1] = {0, 0, 65, 41, 33, 28, 25, 23, 22, 21, 20, 19, 18, 18, 17, 17, 17, 16, 16, 16,
    15, 15, 15, 15, 14, 14, 14, 14, 14, 14, 14, 13, 13, 13, 13, 13, 13};

_CRTIMP2_PURE unsigned long long __CLRCALL_PURE_OR_CDECL _Stoullx(
    const char* s, char** endptr, int base, int* perr) { // convert string to unsigned long long, with checking
    const char *sc, *sd;
    const char *s1, *s2;
    char dig, sign;
    ptrdiff_t n;
    unsigned long long x, y;

    if (perr != 0) {
        *perr = 0;
    }

    sc = s;
    while (isspace((unsigned char) *sc)) {
        ++sc;
    }

    sign = (char) (*sc == '-' || *sc == '+' ? *sc++ : '+');
    if (base < 0 || base == 1 || BASE_MAX < base) { // silly base
        if (endptr != 0) {
            *endptr = (char*) s;
        }

        return 0;
    }

    if (0 < base) { // strip 0x or 0X
        if (base == 16 && *sc == '0' && (sc[1] == 'x' || sc[1] == 'X')) {
            sc += 2;
        }
    } else if (*sc != '0') {
        base = 10;
    } else if (sc[1] == 'x' || sc[1] == 'X') {
        base = 16;
        sc += 2;
    } else {
        base = 8;
    }

    for (s1 = sc; *sc == '0'; ++sc) { // skip leading zeros
    }

    x = 0;
    for (s2 = sc, y = 0, dig = 0; (sd = (char*) memchr(&digits[0], tolower(*sc), base)) != 0;
         ++sc) { // accumulate digits
        y   = x;
        dig = (char) (sd - digits); // for overflow checking
        x   = x * base + dig;
    }

    if (s1 == sc) { // check string validity
        if (endptr != 0) {
            *endptr = (char*) s;
        }

        return 0;
    }

    n = sc - s2 - ndigs[base];
    if (n >= 0 && (0 < n || x < x - dig || (x - dig) / base != y)) { // overflow
        errno = ERANGE;
        if (perr != 0) {
            *perr = 1;
        }

        x    = ULLONG_MAX;
        sign = '+';
    }

    if (sign == '-') { // get final value
        x = 0 - x;
    }

    if (endptr != 0) {
        *endptr = (char*) sc;
    }

    return x;
}

_CRTIMP2_PURE unsigned long long __CLRCALL_PURE_OR_CDECL _Stoull(
    const char* s, char** endptr, int base) { // convert string, discard error code
    return _Stoullx(s, endptr, base, 0);
}

_END_EXTERN_C_UNLESS_PURE
