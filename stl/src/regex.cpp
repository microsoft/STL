// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// This file is compiled into the import library.
// MAJOR LIMITATIONS apply to what can be included here!
// Before editing this file, read: /docs/import_library.md

#include <__msvc_xlocinfo_types.hpp>
#include <clocale>
#include <crtdefs.h>
#include <cstdlib>
#include <cstring>
#include <internal_shared.h>

#include <Windows.h>

#undef _ENFORCE_ONLY_CORE_HEADERS
#include "awint.hpp"

extern "C" {

// derived from xstrxfrm.cpp
size_t __stdcall __std_regex_transform_primary_char(
    _Out_writes_(end1 - string1) _Post_readable_size_(return) char* string1, char* end1,
    _In_reads_(end2 - string2) const char* string2, const char* end2, _In_opt_ const _Collvec* ploc) noexcept {
    size_t n1     = end1 - string1;
    size_t n2     = end2 - string2;
    size_t retval = static_cast<size_t>(-1);
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
        const int dstlen = __crtLCMapStringA(locale_name,
            LCMAP_SORTKEY | LINGUISTIC_IGNORECASE | LINGUISTIC_IGNOREDIACRITIC | NORM_IGNOREKANATYPE | NORM_IGNOREWIDTH,
            string2, static_cast<int>(n2), nullptr, 0, codepage, TRUE);

        if (dstlen != 0) {
            retval = dstlen;

            // if not enough room, return amount needed
            if (dstlen <= static_cast<int>(n1)) {
                // Map src string to dst string
                __crtLCMapStringA(locale_name,
                    LCMAP_SORTKEY | LINGUISTIC_IGNORECASE | LINGUISTIC_IGNOREDIACRITIC | NORM_IGNOREKANATYPE
                        | NORM_IGNOREWIDTH,
                    string2, static_cast<int>(n2), string1, static_cast<int>(n1), codepage, TRUE);
            }
        }
    }

    return retval;
}

// derived from xwcsxfrm.cpp
size_t __stdcall __std_regex_transform_primary_wchar_t(
    _Out_writes_(end1 - string1) _Post_readable_size_(return) wchar_t* string1, wchar_t* end1,
    _In_reads_(end2 - string2) const wchar_t* string2, const wchar_t* end2, _In_opt_ const _Collvec* ploc) noexcept {
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
#pragma warning(push)
#pragma warning(disable : 6386) // PREfast doesn't understand LCMAP_SORTKEY
            size = __crtLCMapStringW(locale_name,
                LCMAP_SORTKEY | LINGUISTIC_IGNORECASE | LINGUISTIC_IGNOREDIACRITIC | NORM_IGNOREKANATYPE
                    | NORM_IGNOREWIDTH,
                string2, static_cast<int>(n2), reinterpret_cast<wchar_t*>(bbuffer.get()), static_cast<int>(n1));
#pragma warning(pop)

            if (size == 0) {
                // buffer not big enough, get size required.
                size = __crtLCMapStringW(locale_name,
                    LCMAP_SORTKEY | LINGUISTIC_IGNORECASE | LINGUISTIC_IGNOREDIACRITIC | NORM_IGNOREKANATYPE
                        | NORM_IGNOREWIDTH,
                    string2, static_cast<int>(n2), nullptr, 0);

                if (size == 0) {
                    size = static_cast<size_t>(-1); // default error
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
} // extern "C"
