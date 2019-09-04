// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// convert character to uppercase

#include <yvals.h>

#include "awint.h"
#include <ctype.h>
#include <locale.h>
#include <stddef.h>
#include <xlocinfo.h>

// remove macro definitions of _toupper() and toupper()
#undef _toupper
#undef toupper

_EXTERN_C_UNLESS_PURE

// int _Toupper(c) - convert character to uppercase
//
// Purpose:
//     _Toupper() is a version of toupper with a locale argument.
//
// Entry:
//     c - int value of character to be converted
//     const _Ctypevec * = pointer to locale info
//
// Exit:
//     returns int value of uppercase representation of c
//
// Exceptions:
//     None.
_CRTIMP2_PURE int __CLRCALL_PURE_OR_CDECL _Toupper(int c, const _Ctypevec* ploc) {
    int size;
    unsigned char inbuffer[3];
    unsigned char outbuffer[3];

    const wchar_t* locale_name;
    UINT codepage;

    if (ploc == 0) {
        locale_name = ___lc_locale_name_func()[LC_CTYPE];
        codepage    = ___lc_codepage_func();
    } else {
        locale_name = ploc->_LocaleName;
        codepage    = ploc->_Page;
    }

    if (locale_name == nullptr) {
        if ((c >= 'a') && (c <= 'z')) {
            c = c - ('a' - 'A');
        }

        return c;
    }

    // if checking case of c does not require API call, do it
    if ((unsigned) c < 256) {
        if (ploc == 0) {
            if (!islower(c)) {
                return c;
            }
        } else {
            if (!(ploc->_Table[c] & _LOWER)) {
                return c;
            }
        }
    }

    // convert int c to multibyte string
    if (ploc == 0 ? _cpp_isleadbyte((c >> 8) & 0xff) : (ploc->_Table[(c >> 8) & 0xff] & _LEADBYTE) != 0) {
        inbuffer[0] = (c >> 8 & 0xff);
        inbuffer[1] = (unsigned char) c;
        inbuffer[2] = 0;
        size        = 2;
    } else {
        inbuffer[0] = (unsigned char) c;
        inbuffer[1] = 0;
        size        = 1;
    }

    // convert wide char to uppercase
    if (0
        == (size = __crtLCMapStringA(
                locale_name, LCMAP_UPPERCASE, (const char*) inbuffer, size, (char*) outbuffer, 3, codepage, TRUE))) {
        return c;
    }

    // construct integer return value
    if (size == 1) {
        return (int) outbuffer[0];
    } else {
        return (int) outbuffer[1] | ((int) outbuffer[0] << 8);
    }
}

_END_EXTERN_C_UNLESS_PURE
