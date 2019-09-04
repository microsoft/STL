// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _WStopfx function

#include "xmath.h"
#include <wctype.h>

_EXTERN_C_UNLESS_PURE

int _WStopfx(const wchar_t** ps, wchar_t** endptr) { // parse prefix of floating-point field
    const wchar_t* s = *ps;
    int code         = 0;
    while (iswspace(*s)) {
        ++s;
    }

    if (*s == L'-') {
        code = FL_NEG;
        ++s;
    } else if (*s == L'+') {
        ++s;
    }

    if (*s == L'n' || *s == L'N') { // parse "nan" or fail
        if ((*++s != L'a' && *s != L'A') || (*++s != L'n' && *s != L'N')) { // parse failed, roll back pointer
            s    = *ps;
            code = FL_ERR;
        } else { // parse optional (n-char-sequence)
            const wchar_t* q = ++s;
            code             = FL_NAN;
            if (*q == L'(') { // got L'(', skip through L')'
                do {
                    ++q;
                } while (iswalnum(*q) || *q == L'_');

                if (*q == L')') {
                    s = ++q;
                }
            }
        }

        if (endptr != 0) {
            *endptr = (wchar_t*) s;
        }
    } else if (*s == L'i' || *s == L'I') { // parse "inf" or fail
        if ((*++s != L'n' && *s != L'N') || (*++s != L'f' && *s != L'F')) { // parse failed, roll back pointer
            s    = *ps;
            code = FL_ERR;
        } else { // parse optional rest of L"infinity"
            const wchar_t* q = ++s;
            code |= FL_INF;
            if ((*q == L'i' || *q == L'I') //
                && (*++q == L'n' || *q == L'N') //
                && (*++q == L'i' || *q == L'I') //
                && (*++q == L't' || *q == L'T') //
                && (*++q == L'y' || *q == L'Y')) {
                s = ++q;
            }
        }

        if (endptr != 0) {
            *endptr = (wchar_t*) s;
        }
    } else if (*s == L'0' && (s[1] == L'x' || s[1] == L'X')) { // test for valid hex field following 0x or 0X
        const wchar_t* s1 = s + 2;
        if (*s1 == L'.') {
            ++s1;
        }

        if (iswxdigit(*s1)) {
            s += 2;
            code |= FL_HEX;
        } else {
            code |= FL_DEC;
        }
    } else {
        code |= FL_DEC;
    }

    *ps = s;
    return code;
}

_END_EXTERN_C_UNLESS_PURE
