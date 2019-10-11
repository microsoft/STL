// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Convert multibyte char to wide char.

#include <crtdbg.h>
#include <ctype.h>
#include <errno.h>
#include <internal_shared.h>
#include <limits.h> // for INT_MAX
#include <locale.h>
#include <stdio.h> // for EOF
#include <stdlib.h>
#include <xlocinfo.h> // for _Cvtvec, _Mbrtowc

_EXTERN_C_UNLESS_PURE

static int _Utf8_trailing_byte_count(unsigned long* partialCh, unsigned char ch) {
    if ((ch & 0x80u) == 0x00u) { // high bit unset, plain ASCII
        return 0;
    }

    if ((ch & 0xE0u) == 0xC0u) { // first 3 bits are 110, 1 trailing byte
        *partialCh = ch & 0x1Fu;
        return 1;
    }

    if ((ch & 0xF0u) == 0xE0u) { // first 4 bits are 1110, 2 trailing bytes
        *partialCh = ch & 0x0Fu;
        return 2;
    }

    if ((ch & 0xF8u) == 0xF0u) { // first 5 bits are 11110, 3 trailing bytes
        *partialCh = ch & 0x07u;
        return 3;
    }

    return INT_MAX;
}

static int _Decode_utf8_trailing_byte(unsigned long* partialCh, unsigned char ch) {
    if ((ch & 0xC0) == 0x80) {
        *partialCh <<= 6;
        *partialCh |= ch & 0x3Fu;
        return 0;
    }

    return 1;
}

// int _Mbrtowc() - Convert multibyte char to wide character.
//
// Purpose:
//     Convert a multi-byte character into the equivalent wide character,
//     according to the specified LC_CTYPE category, or the current locale.
//     [ANSI].
//
// Entry:
//     wchar_t* pwc        = pointer to destination wide character
//     const char* s       = pointer to multibyte character
//     size_t n            = maximum length of multibyte character to consider
//     mbstate_t* pst      = pointer to state
//     const _Cvtvec* ploc = pointer to locale info
//
// Returns: 0 (if *s = null char)
//          -1 (if the next n or fewer bytes not valid mbc)
//          -2 (if partial conversion)
//          number of bytes comprising converted mbc
_MRTIMP2 int __cdecl _Mbrtowc(wchar_t* pwc, const char* s, size_t n, mbstate_t* pst, const _Cvtvec* ploc) {
    (void) pst;
    if (n == 0) { // indicate do not have state-dependent encodings, handle zero length string
        return 0;
    }

    if (!*s) { // handle null terminator, TRANSITION, VSO-654347
        *pwc = 0;
        return 0;
    }

    // perform locale-dependent parse
    unsigned char ch = static_cast<unsigned char>(*s);
    if (ploc->_Isclocale) {
        *pwc = ch;
        return 1;
    }

    unsigned long wch;
    int consumedCount;
    int trailingUtf8Units;
    switch (ploc->_Mbcurmax) {
    default:
        _ASSERTE(!"Bad number of encoding units for this code page");
        // [[fallthrough]];
    case 1:
        if (MultiByteToWideChar(ploc->_Page, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, s, 1, pwc, 1) == 0) {
            errno = EILSEQ;
            return -1;
        }

        return 1;

    case 4: // UTF-8
        _ASSERTE(ploc->_Page == CP_UTF8);
        trailingUtf8Units = _Utf8_trailing_byte_count(&wch, ch);
        if (trailingUtf8Units == 0) { // optimistic ASCII passsthrough
            *pwc = static_cast<wchar_t>(ch);
            return 1;
        }

        if (trailingUtf8Units > 2) { // this would result in a UTF-16 surrogate pair, which we can't emit in our
                                     // singular output wchar_t, so fail
                                     // see N4750 [locale.codecvt.virtuals]/3
            errno = EILSEQ;
            return -1;
        }

        consumedCount = 1;
        for (; trailingUtf8Units; ++consumedCount, --trailingUtf8Units) { // consume remaining trailing bytes
            if (static_cast<size_t>(consumedCount) >= n) { // no byte to consume, return partial
                return -2;
            }

            if (_Decode_utf8_trailing_byte(&wch, static_cast<unsigned char>(s[consumedCount]))) {
                errno = EILSEQ;
                return -1;
            }
        }

        if (wch >= 0xD800u && wch <= 0xDFFFu) { // tried to decode unpaired surrogate
            errno = EILSEQ;
            return -1;
        }

        *pwc = static_cast<wchar_t>(wch);
        return consumedCount;

    case 2:
        if (ploc->_Isleadbyte[ch >> 3] & (1u << (ch & 7u))) {
            if (n == 1) { // partial multibyte character
                return -2;
            }

            consumedCount = 2;
        } else {
            consumedCount = 1;
        }

        if (MultiByteToWideChar(ploc->_Page, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, s, consumedCount, pwc, 1) == 0) {
            errno = EILSEQ;
            return -1;
        }

        return consumedCount;
    }
}

#ifdef MRTDLL
_MRTIMP2 int __cdecl _Mbrtowc(unsigned short* pwc, const char* s, size_t n, mbstate_t* pst, const _Cvtvec* ploc) {
    return _Mbrtowc(reinterpret_cast<wchar_t*>(pwc), s, n, pst, ploc);
}
#endif // MRTDLL

_END_EXTERN_C_UNLESS_PURE
