// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Compare two strings using the locale LC_COLLATE information.

#include "awint.h"
#include <crtdbg.h>
#include <errno.h>
#include <locale.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <xlocinfo.h> // for _Collvec, _Strcoll

_EXTERN_C_UNLESS_PURE

// int _Strcoll() - Collate locale strings
//
// Purpose:
//     Compare two strings using the locale LC_COLLATE information.
//     [ANSI].
//
//     In the C locale, strcoll() simply resolves to strcmp().
//
// Entry:
//     const char* _string1 = pointer to beginning of the first string
//     const char* _end1    = pointer past end of the first string
//     const char* _string2 = pointer to beginning of the second string
//     const char* _end2    = pointer past end of the second string
//     const _Collvec* ploc = pointer to locale info
//
// Exit:
//     Less than 0    = first string less than second string
//     0              = strings are equal
//     Greater than 0 = first string greater than second string
//
// Exceptions:
//     _NLSCMPERROR = error
//     errno = EINVAL
_CRTIMP2_PURE int __CLRCALL_PURE_OR_CDECL _Strcoll(
    const char* _string1, const char* _end1, const char* _string2, const char* _end2, const _Collvec* ploc) {
    int ret = 0;
    UINT codepage;
    int n1 = (int) (_end1 - _string1);
    int n2 = (int) (_end2 - _string2);
    const wchar_t* locale_name;

    if (ploc == 0) {
        locale_name = ___lc_locale_name_func()[LC_COLLATE];
        codepage    = ___lc_collate_cp_func();
    } else {
        locale_name = ploc->_LocaleName;
        codepage    = ploc->_Page;
    }

    if (locale_name == nullptr) {
        int ans;
        ans = memcmp(_string1, _string2, n1 < n2 ? n1 : n2);
        ret = (ans != 0 || n1 == n2 ? ans : n1 < n2 ? -1 : +1);
    } else {
        if (0 == (ret = __crtCompareStringA(locale_name, SORT_STRINGSORT, _string1, n1, _string2, n2, codepage))) {
            errno = EINVAL;
            ret   = _NLSCMPERROR;
        } else {
            ret -= 2;
        }
    }

    return ret;
}

// _Collvec _Getcoll() - get collation info for current locale
_CRTIMP2_PURE _Collvec __CLRCALL_PURE_OR_CDECL _Getcoll() {
    _Collvec coll;

    coll._Page       = ___lc_collate_cp_func();
    coll._LocaleName = ___lc_locale_name_func()[LC_COLLATE];
    if (coll._LocaleName) {
        coll._LocaleName = _wcsdup_dbg(coll._LocaleName, _CRT_BLOCK, __FILE__, __LINE__);
    }

    return coll;
}

_END_EXTERN_C_UNLESS_PURE
