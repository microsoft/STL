// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// convert character to lower case

#include <yvals.h>

#include <__msvc_xlocinfo_types.hpp>
#include <cctype>
#include <clocale>
#include <cstdlib>
#include <internal_shared.h>

#include "awint.hpp"

// remove macro definitions of _tolower() and tolower()
#undef _tolower
#undef tolower

_EXTERN_C_UNLESS_PURE

// int _Tolower(c) - convert character to lower case
//
// Purpose:
//     _Tolower() is a version of tolower with a locale argument.
//
// Entry:
//     c - int value of character to be converted
//     const _Ctypevec * = pointer to locale info
//
// Exit:
//     returns int value of lower case representation of c
//
// Exceptions:
//     None.
_CRTIMP2_PURE int __CLRCALL_PURE_OR_CDECL _Tolower(int c, const _Ctypevec* ploc) noexcept {
    int size;
    unsigned char inbuffer[3];
    unsigned char outbuffer[3];

    UINT codepage;
    const wchar_t* locale_name;

    if (ploc == nullptr) {
        locale_name = ___lc_locale_name_func()[LC_CTYPE];
        codepage    = ___lc_codepage_func();
    } else {
        locale_name = ploc->_LocaleName;
        codepage    = ploc->_Page;
    }

    if (locale_name == nullptr) {
        if (c >= 'A' && c <= 'Z') {
            c = c + ('a' - 'A');
        }

        return c;
    }

    // if checking case of c does not require API call, do it
    if (static_cast<unsigned int>(c) < 256) {
        if (ploc == nullptr) {
            if (!isupper(c)) {
                return c;
            }
        } else {
            if (!(ploc->_Table[c] & _UPPER)) {
                return c;
            }
        }
    }

    // convert int c to multibyte string
    if (ploc == nullptr ? _cpp_isleadbyte((c >> 8) & 0xff) : (ploc->_Table[(c >> 8) & 0xff] & _LEADBYTE) != 0) {
        inbuffer[0] = (c >> 8 & 0xff);
        inbuffer[1] = static_cast<unsigned char>(c);
        inbuffer[2] = 0;
        size        = 2;
    } else {
        inbuffer[0] = static_cast<unsigned char>(c);
        inbuffer[1] = 0;
        size        = 1;
    }

    // convert wide char to lowercase
    size = __crtLCMapStringA(locale_name, LCMAP_LOWERCASE, reinterpret_cast<const char*>(inbuffer), size,
        reinterpret_cast<char*>(outbuffer), 3, codepage, TRUE);

    if (size == 0) {
        return c;
    }

    // construct integer return value
    if (size == 1) {
        return static_cast<int>(outbuffer[0]);
    } else {
        return static_cast<int>(outbuffer[1]) | (static_cast<int>(outbuffer[0]) << 8);
    }
}

_CRTIMP2_PURE _Ctypevec __CLRCALL_PURE_OR_CDECL _Getctype() noexcept {
    // get ctype info for current locale
    _Ctypevec ctype;

    ctype._Page  = ___lc_codepage_func();
    ctype._Table = static_cast<const short*>(_calloc_crt(256, sizeof(*__pctype_func())));
    if (ctype._Table != nullptr) {
        memcpy(const_cast<short*>(ctype._Table), __pctype_func(), 256 * sizeof(*__pctype_func()));
        ctype._Delfl = 1;
    } else {
        ctype._Table = reinterpret_cast<const short*>(__pctype_func());
        ctype._Delfl = 0;
    }
    ctype._LocaleName = ___lc_locale_name_func()[LC_COLLATE];
    if (ctype._LocaleName) {
        ctype._LocaleName = _wcsdup_dbg(ctype._LocaleName, _CRT_BLOCK, __FILE__, __LINE__);
    }

    return ctype;
}

_END_EXTERN_C_UNLESS_PURE
