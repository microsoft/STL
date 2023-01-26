// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _WStoxflt function

#include <cctype>
#include <clocale>
#include <cwchar>
#include <cwctype>

#include "xmath.hpp"

_EXTERN_C_UNLESS_PURE

constexpr int _Base   = 16; // hexadecimal
constexpr int _Ndig   = 7; // hexadecimal digits per long element
constexpr int _Maxsig = 5 * _Ndig; // maximum significant digits to keep

_In_range_(0, maxsig) int _WStoxflt(const wchar_t* s0, const wchar_t* s, wchar_t** endptr,
    _Out_writes_(maxsig) long lo[], _In_range_(1, 4) int maxsig) {
    // convert wide string to array of long plus exponent
    char buf[_Maxsig + 1]; // worst case, with room for rounding digit
    int nsig = 0; // number of significant digits seen
    int seen = 0; // any valid field characters seen

    const wchar_t* pd;
    static constexpr wchar_t digits[] = L"0123456789abcdefABCDEF"; // hex digits in both cases
    static constexpr char vals[]      = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 10, 11, 12, 13, 14, 15}; // values of hex digits

    maxsig *= _Ndig; // convert word count to digit count
    if (_Maxsig < maxsig) {
        maxsig = _Maxsig; // protect against bad call
    }

    lo[0] = 0; // power of ten exponent
    lo[1] = 0; // first _Ndig-digit word of fraction

    while (*s == L'0') { // strip leading zeros
        ++s;
        seen = 1;
    }

    while ((pd = wmemchr(&digits[0], *s, 22)) != nullptr) {
        if (nsig <= maxsig) {
            buf[nsig++] = vals[pd - digits]; // accumulate a digit
        } else {
            ++lo[0]; // too many digits, just scale exponent
        }

        ++s;
        seen = 1;
    }

    if (*s == localeconv()->_W_decimal_point[0]) {
        ++s;
    }

    if (nsig == 0) {
        for (; *s == '0'; ++s, seen = 1) {
            --lo[0]; // strip zeros after point
        }
    }

    while ((pd = wmemchr(&digits[0], *s, 22)) != nullptr) {
        if (nsig <= maxsig) { // accumulate a fraction digit
            buf[nsig++] = vals[pd - digits];
            --lo[0];
        }

        ++s;
        seen = 1;
    }

    if (maxsig < nsig) { // discard excess digit after rounding up
        if (_Base / 2 <= buf[maxsig]) {
            ++buf[maxsig - 1]; // okay if digit becomes _Base
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

    lo[0] <<= 2; // change hex exponent to binary exponent

    int word; // current long word to fill

    if (seen) { // convert digit sequence to words
        int bufidx  = 0; // next digit in buffer
        int wordidx = _Ndig - nsig % _Ndig; // next digit in word (% _Ndig)

        word = wordidx % _Ndig == 0 ? 0 : 1;
        for (; bufidx < nsig; ++wordidx, ++bufidx) {
            if (wordidx % _Ndig == 0) {
                lo[++word] = buf[bufidx];
            } else {
                lo[word] = lo[word] * _Base + buf[bufidx];
            }
        }

        if (*s == L'p' || *s == L'P') { // parse exponent
            const wchar_t* ssav = s;
            const wchar_t esign = *++s == L'+' || *s == L'-' ? *s++ : L'+';
            int eseen           = 0;
            long lexp           = 0;

            for (; iswdigit(*s); ++s, eseen = 1) {
                if (lexp < 100000000) { // else overflow
                    lexp = lexp * 10 + *s - L'0';
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
    } else {
        word = 0; // return zero if bad parse
    }

    if (endptr) {
        *endptr = const_cast<wchar_t*>(seen ? s : s0); // roll back if bad parse
    }

    return word;
}

_END_EXTERN_C_UNLESS_PURE
