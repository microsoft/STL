// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Transform a wide-character string using the locale information as set by LC_COLLATE.

#include "awint.h"
#include <internal_shared.h>
#include <limits.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <xlocinfo.h> // for _Collvec, _Wcsxfrm

_EXTERN_C_UNLESS_PURE

// size_t _Wcsxfrm() - Transform a string using locale information
//
// Purpose:
//     Transform the wide string pointed to by _string2 and place the
//     resulting wide string into the array pointed to by _string1.
//     No more than _end1 - _string1 wide characters are placed into the
//     resulting string (including the null).
//
//     The transformation is such that if wcscmp() is applied to
//     the two transformed strings, the return value is equal to
//     the result of wcscoll() applied to the two original strings.
//     Thus, the conversion must take the locale LC_COLLATE info
//     into account.
//
//     In the C locale, wcsxfrm() simply resolves to wcsncpy()/wcslen().
//
// Entry:
//     wchar_t* _string1       = pointer to beginning of result string
//     wchar_t* _end1          = pointer past end of result string
//     const wchar_t* _string2 = pointer to beginning of source string
//     const wchar_t* _end2    = pointer past end of source string
//     const _Collvec* ploc    = pointer to locale info
//
// Exit:
//     Length of the transformed string.
//     If the value returned is too big, the contents of the
//     _string1 array are indeterminate.
//
// Exceptions:
//     Non-standard: if OM/API error, return INT_MAX.
_CRTIMP2_PURE size_t __CLRCALL_PURE_OR_CDECL _Wcsxfrm(
    wchar_t* _string1, wchar_t* _end1, const wchar_t* _string2, const wchar_t* _end2, const _Collvec* ploc) {
    size_t _n1             = _end1 - _string1;
    size_t _n2             = _end2 - _string2;
    size_t size            = (size_t) -1;
    unsigned char* bbuffer = nullptr;
    const wchar_t* locale_name;

    if (ploc == 0) {
        locale_name = ___lc_locale_name_func()[LC_COLLATE];
    } else {
        locale_name = ploc->_LocaleName;
    }

    if (locale_name == nullptr) {
        if (_n2 <= _n1) {
            memcpy(_string1, _string2, _n2 * sizeof(wchar_t));
        }
        size = _n2;
    } else {

        // When using LCMAP_SORTKEY, LCMapStringW handles BYTES not wide
        // chars. We use a byte buffer to hold bytes and then convert the
        // byte string to a wide char string and return this so it can be
        // compared using wcscmp(). User's buffer is _n1 wide chars, so
        // use an internal buffer of _n1 bytes.

        if (nullptr != (bbuffer = (unsigned char*) _malloc_crt(_n1))) {
            if (0
                == (size = __crtLCMapStringW(
                        locale_name, LCMAP_SORTKEY, _string2, (int) _n2, (wchar_t*) bbuffer, (int) _n1))) {
                // buffer not big enough, get size required.

                if (0 == (size = __crtLCMapStringW(locale_name, LCMAP_SORTKEY, _string2, (int) _n2, nullptr, 0))) {
                    size = INT_MAX; // default error
                }
            } else {
                size_t i;
                // string successfully mapped, convert to wide char

                for (i = 0; i < size; i++) {
                    _string1[i] = (wchar_t) bbuffer[i];
                }
            }
        }
    }

    if (bbuffer) {
        _free_crt(bbuffer);
    }

    return size;
}

#ifdef MRTDLL
_CRTIMP2_PURE size_t __CLRCALL_PURE_OR_CDECL _Wcsxfrm(unsigned short* _string1, unsigned short* _end1,
    const unsigned short* _string2, const unsigned short* _end2, const _Collvec* ploc) {
    return _Wcsxfrm((wchar_t*) _string1, (wchar_t*) _end1, (const wchar_t*) _string2, (const wchar_t*) _end2, ploc);
}
#endif // MRTDLL

_END_EXTERN_C_UNLESS_PURE
