// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Transform a string using the locale information as set by LC_COLLATE.

#include <__msvc_xlocinfo_types.hpp>
#include <clocale>
#include <cstdlib>
#include <cstring>
#include <malloc.h>

#include <Windows.h>

#include "awint.hpp"

_EXTERN_C_UNLESS_PURE

// size_t _Strxfrm() - Transform a string using locale information
//
// Purpose:
//     Transform the string pointed to by string2 and place the
//     resulting string into the array pointed to by string1.
//     No more than end1 - string1 characters are placed into the
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
//     char* string1        = pointer to beginning of result string
//     char* end1           = pointer past end of result string
//     const char* string2  = pointer to beginning of source string
//     const char* end2     = pointer past end of source string
//     const _Collvec* ploc = pointer to locale info
//
// Exit:
//     Length of the transformed string.
//     If the value returned is too big, the contents of the
//     string1 array are indeterminate.
//
// Exceptions:
//     Non-standard: if OM/API error, return INT_MAX.
_CRTIMP2_PURE size_t __CLRCALL_PURE_OR_CDECL _Strxfrm(_Out_writes_(end1 - string1)
                                                          _Post_readable_size_(return) char* string1,
    _In_z_ char* end1, const char* string2, const char* end2, const _Collvec* ploc) {
    size_t n1     = end1 - string1;
    size_t n2     = end2 - string2;
    size_t retval = static_cast<size_t>(-1); // NON-ANSI: default if OM or API error
    UINT codepage;
    const wchar_t* locale_name;

    if (ploc == nullptr) {
        locale_name = ___lc_locale_name_func()[LC_COLLATE];
        codepage    = ___lc_collate_cp_func();
    } else {
        locale_name = ploc->_LocaleName;
        codepage    = ploc->_Page;
    }

    if (locale_name == nullptr && codepage == CP_ACP) {
        if (n2 <= n1) {
            memcpy(string1, string2, n2);
        }
        retval = n2;
    } else {
        // Inquire size of dst string in BYTES
        const int dstlen =
            __crtLCMapStringA(locale_name, LCMAP_SORTKEY, string2, static_cast<int>(n2), nullptr, 0, codepage, TRUE);

        if (dstlen != 0) {
            retval = dstlen;

            // if not enough room, return amount needed
            if (dstlen <= static_cast<int>(n1)) {
                // Map src string to dst string
                __crtLCMapStringA(locale_name, LCMAP_SORTKEY, string2, static_cast<int>(n2), string1,
                    static_cast<int>(n1), codepage, TRUE);
            }
        }
    }

    return retval;
}

_END_EXTERN_C_UNLESS_PURE
