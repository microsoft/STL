// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Stoflt function

#include "xmath.h"
#include <ctype.h>
#include <locale.h>
#include <stdlib.h>

_EXTERN_C_UNLESS_PURE

#define BASE 10 // decimal
#define NDIG 9 // decimal digits per long word
#define MAXSIG (5 * NDIG) // maximum significant digits to keep

int _Stoflt(const char* s0, const char* s, char** endptr, long lo[],
    int maxsig) { // convert string to array of long plus exponent
    char buf[MAXSIG + 1]; // worst case, with room for rounding digit
    int nsig   = 0; // number of significant digits seen
    int seen   = 0; // any valid field characters seen
    int sticky = 0; // note any nonzero digits discarded
    int word   = 0; // just before current long word to fill

    maxsig *= NDIG; // convert word count to digit count
    if (MAXSIG < maxsig) {
        maxsig = MAXSIG; // protect against bad call
    }

    lo[0] = 0; // power of ten exponent
    lo[1] = 0; // first NDIG-digit word of fraction

    while (*s == '0') { // strip leading zeros
        seen = 1;
        ++s;
    }

    while (isdigit((unsigned char) *s)) {
        if (nsig < maxsig) {
            buf[nsig++] = (char) (*s - '0'); // accumulate a digit
        } else { // just scale exponent and note nonzero digits
            ++lo[0];
            if (*s != '0') {
                sticky = 1;
            }
        }

        ++s;
        seen = 1;
    }

    if (*s == localeconv()->decimal_point[0]) {
        ++s;
    }

    if (nsig == 0) {
        for (; *s == '0'; ++s, seen = 1) {
            --lo[0]; // scale for stripped zeros after point
        }
    }

    for (; isdigit((unsigned char) *s); ++s, seen = 1) {
        if (nsig < maxsig) { // accumulate a fraction digit
            buf[nsig++] = (char) (*s - '0');
            --lo[0];
        } else if (*s != '0') {
            sticky = 1;
        }
    }

    if (sticky) {
        buf[maxsig - 1] += 1; // okay if digit becomes BASE
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

        if (*s == 'e' || *s == 'E') { // parse exponent
            const char* ssav = s;
            const char esign = *++s == '+' || *s == '-' ? *s++ : '+';
            int eseen        = 0;
            long lexp        = 0;

            for (; isdigit((unsigned char) *s); ++s, eseen = 1) {
                if (lexp < 100000000) { // else overflow
                    lexp = lexp * 10 + (unsigned char) *s - '0';
                }
            }

            if (esign == '-') {
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
        *endptr = (char*) (seen ? s : s0); // roll back if bad parse
    }

    return word;
}

_END_EXTERN_C_UNLESS_PURE
