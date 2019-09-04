// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _WStoflt function

#include "xmath.h"
#include <locale.h>
#include <wchar.h>
#include <wctype.h>

_EXTERN_C_UNLESS_PURE

#define BASE 10 // decimal
#define NDIG 9 // decimal digits per long element
#define MAXSIG (5 * NDIG) // maximum significant digits to keep

int _WStoflt(const wchar_t* s0, const wchar_t* s, wchar_t** endptr, long lo[],
    int maxsig) { // convert wide string to array of long plus exponent
    char buf[MAXSIG + 1]; // worst case, with room for rounding digit
    int nsig = 0; // number of significant digits seen
    int seen = 0; // any valid field characters seen
    int word = 0; // current long word to fill

    maxsig *= NDIG; // convert word count to digit count
    if (MAXSIG < maxsig) {
        maxsig = MAXSIG; // protect against bad call
    }

    lo[0] = 0; // power of ten exponent
    lo[1] = 0; // first NDIG-digit word of fraction

    while (*s == L'0') { // strip leading zeros
        ++s;
        seen = 1;
    }

    while (iswdigit(*s)) {
        if (nsig <= maxsig) {
            buf[nsig++] = (char) (*s - L'0'); // accumulate a digit
        } else {
            ++lo[0]; // too many digits, just scale exponent
        }

        ++s;
        seen = 1;
    }

    if (*s == btowc(localeconv()->decimal_point[0])) {
        ++s;
    }

    if (nsig == 0) {
        while (*s == L'0') {
            --lo[0]; // scale for stripped zeros after point
            ++s;
            seen = 1;
        }
    }

    while (iswdigit(*s)) {
        if (nsig <= maxsig) { // accumulate a fraction digit
            buf[nsig++] = (char) (*s - L'0');
            --lo[0];
        }

        ++s;
        seen = 1;
    }

    if (maxsig < nsig) { // discard excess digit after rounding up
        if (BASE / 2 <= buf[maxsig]) {
            ++buf[maxsig - 1]; // okay if digit becomes BASE
        }

        nsig = maxsig;
        ++lo[0];
    }

    for (; 0 < nsig && buf[nsig - 1] == '\0'; --nsig) {
        ++lo[0]; // discard trailing zeros
    }

    if (nsig == 0) {
        buf[nsig++] = '\0'; // ensure at least one digit
    }

    if (seen) { // convert digit sequence to words
        int bufidx  = 0; // next digit in buffer
        int wordidx = NDIG - nsig % NDIG; // next digit in word (% NDIG)

        word = wordidx % NDIG == 0 ? 0 : 1;
        for (; bufidx < nsig; ++wordidx, ++bufidx) {
            if (wordidx % NDIG == 0) {
                lo[++word] = buf[bufidx];
            } else {
                lo[word] = lo[word] * BASE + buf[bufidx];
            }
        }

        if (*s == L'e' || *s == L'E') { // parse exponent
            const wchar_t* ssav = s;
            const wchar_t esign = *++s == L'+' || *s == L'-' ? *s++ : L'+';
            int eseen           = 0;
            long lexp           = 0;

            for (; iswdigit(*s); ++s, eseen = 1) {
                if (lexp < 100000000) { // else overflow
                    lexp = lexp * 10 + *s - L'0';
                }
            }

            if (esign == L'-') {
                lexp = -lexp;
            }

            lo[0] += lexp;
            if (!eseen) {
                s = ssav; // roll back if incomplete exponent
            }
        }
    }

    if (!seen) {
        word = 0; // return zero if bad parse
    }

    if (endptr) {
        *endptr = (wchar_t*) (seen ? s : s0); // roll back if bad parse
    }

    return word;
}

_END_EXTERN_C_UNLESS_PURE
