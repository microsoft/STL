// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Transform a wide-character string using the locale information as set by LC_COLLATE.

#include <__msvc_xlocinfo_types.hpp>
#include <climits>
#include <clocale>
#include <cstdlib>
#include <cstring>
#include <internal_shared.h>

#include "awint.hpp"

_EXTERN_C_UNLESS_PURE

// size_t _Wcsxfrm() - Transform a string using locale information
//
// Purpose:
//     Transform the wide string pointed to by string2 and place the
//     resulting wide string into the array pointed to by string1.
//     No more than end1 - string1 wide characters are placed into the
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
//     wchar_t* string1        = pointer to beginning of result string
//     wchar_t* end1           = pointer past end of result string
//     const wchar_t* string2  = pointer to beginning of source string
//     const wchar_t* end2     = pointer past end of source string
//     const _Collvec* ploc    = pointer to locale info
//
// Exit:
//     Length of the transformed string.
//     If the value returned is too big, the contents of the
//     string1 array are indeterminate.
//
// Exceptions:
//     Non-standard: if OM/API error, return INT_MAX.
_CRTIMP2_PURE size_t __CLRCALL_PURE_OR_CDECL _Wcsxfrm(_Out_writes_(end1 - string1) _Post_readable_size_(return)
                                                          wchar_t* string1,
    _In_z_ wchar_t* end1, const wchar_t* string2, const wchar_t* end2, const _Collvec* ploc) {
    size_t n1   = end1 - string1;
    size_t n2   = end2 - string2;
    size_t size = static_cast<size_t>(-1);
    const wchar_t* locale_name;

    if (ploc == nullptr) {
        locale_name = ___lc_locale_name_func()[LC_COLLATE];
    } else {
        locale_name = ploc->_LocaleName;
    }

    if (locale_name == nullptr) {
        if (n2 <= n1) {
            memcpy(string1, string2, n2 * sizeof(wchar_t));
        }
        size = n2;
    } else {

        // When using LCMAP_SORTKEY, LCMapStringW handles BYTES not wide
        // chars. We use a byte buffer to hold bytes and then convert the
        // byte string to a wide char string and return this so it can be
        // compared using wcscmp(). User's buffer is n1 wide chars, so
        // use an internal buffer of n1 bytes.

        auto bbuffer = _malloc_crt_t(unsigned char, n1);

        if (bbuffer) {
#pragma warning(suppress : 6386) // PREfast doesn't understand LCMAP_SORTKEY
            size = __crtLCMapStringW(locale_name, LCMAP_SORTKEY, string2, static_cast<int>(n2),
                reinterpret_cast<wchar_t*>(bbuffer.get()), static_cast<int>(n1));

            if (size == 0) {
                // buffer not big enough, get size required.
                size = __crtLCMapStringW(locale_name, LCMAP_SORTKEY, string2, static_cast<int>(n2), nullptr, 0);

                if (size == 0) {
                    size = INT_MAX; // default error
                }
            } else {
                // string successfully mapped, convert to wide char

                for (size_t i = 0; i < size; ++i) {
                    string1[i] = static_cast<wchar_t>(bbuffer.get()[i]);
                }
            }
        }
    }

    return size;
}

#ifdef MRTDLL
_CRTIMP2_PURE size_t __CLRCALL_PURE_OR_CDECL _Wcsxfrm(unsigned short* string1, unsigned short* end1,
    const unsigned short* string2, const unsigned short* end2, const _Collvec* ploc) {
    return _Wcsxfrm(reinterpret_cast<wchar_t*>(string1), reinterpret_cast<wchar_t*>(end1),
        reinterpret_cast<const wchar_t*>(string2), reinterpret_cast<const wchar_t*>(end2), ploc);
}
#endif // MRTDLL

_END_EXTERN_C_UNLESS_PURE
