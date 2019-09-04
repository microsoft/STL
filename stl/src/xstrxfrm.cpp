// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Transform a string using the locale information as set by LC_COLLATE.

#include "awint.h"
#include <limits.h>
#include <locale.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <xlocinfo.h> // for _Collvec

#include <Windows.h>

_EXTERN_C_UNLESS_PURE

// size_t _Strxfrm() - Transform a string using locale information
//
// Purpose:
//     Transform the string pointed to by _string2 and place the
//     resulting string into the array pointed to by _string1.
//     No more than _end1 - _string1 characters are placed into the
//     resulting string (including the null).
//
//     The transformation is such that if strcmp() is applied to
//     the two transformed strings, the return value is equal to
//     the result of strcoll() applied to the two original strings.
//     Thus, the conversion must take the locale LC_COLLATE info
//     into account.
//     [ANSI]
//
//     The value of the following expression is the size of the array
//     needed to hold the transformation of the source string:
//
//     1 + strxfrm(nullptr,string,0)
//
//     In the "C" locale, _Strxfrm() simply resolves to strncpy()/strlen().
//
// Entry:
//     char* _string1       = pointer to beginning of result string
//     char* _end1          = pointer past end of result string
//     const char* _string2 = pointer to beginning of source string
//     const char* _end2    = pointer past end of source string
//     const _Collvec* ploc = pointer to locale info
//
// Exit:
//     Length of the transformed string.
//     If the value returned is too big, the contents of the
//     _string1 array are indeterminate.
//
// Exceptions:
//     Non-standard: if OM/API error, return INT_MAX.
_CRTIMP2_PURE size_t __CLRCALL_PURE_OR_CDECL _Strxfrm(
    char* _string1, char* _end1, const char* _string2, const char* _end2, const _Collvec* ploc) {
    size_t _n1 = _end1 - _string1;
    size_t _n2 = _end2 - _string2;
    int dstlen;
    size_t retval = (size_t) -1; // NON-ANSI: default if OM or API error
    UINT codepage;
    const wchar_t* locale_name;

    if (ploc == 0) {
        locale_name = ___lc_locale_name_func()[LC_COLLATE];
        codepage    = ___lc_collate_cp_func();
    } else {
        locale_name = ploc->_LocaleName;
        codepage    = ploc->_Page;
    }

    if ((locale_name == nullptr) && (codepage == CP_ACP)) {
        if (_n2 <= _n1) {
            memcpy(_string1, _string2, _n2);
        }
        retval = _n2;
    } else {
        // Inquire size of dst string in BYTES
        if (0
            != (dstlen =
                    __crtLCMapStringA(locale_name, LCMAP_SORTKEY, _string2, (int) _n2, nullptr, 0, codepage, TRUE))) {
            retval = dstlen;

            // if not enough room, return amount needed
            if (dstlen <= (int) (_n1)) {
                // Map src string to dst string
                __crtLCMapStringA(locale_name, LCMAP_SORTKEY, _string2, (int) _n2, _string1, (int) _n1, codepage, TRUE);
            }
        }
    }

    return retval;
}

_END_EXTERN_C_UNLESS_PURE
