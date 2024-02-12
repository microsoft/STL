// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Compare two wchar_t strings using the locale LC_COLLATE information.

#include <__msvc_xlocinfo_types.hpp>
#include <cerrno>
#include <clocale>
#include <cstdlib>
#include <cwchar>

#include "awint.hpp"

_EXTERN_C_UNLESS_PURE

// int _Wcscoll() - Collate wide-character locale strings
//
// Purpose:
//     Compare two wchar_t strings using the locale LC_COLLATE information.
//     In the C locale, wcscmp() is used to make the comparison.
//
// Entry:
//     const wchar_t* string1  = pointer to beginning of the first string
//     const wchar_t* end1     = pointer past end of the first string
//     const wchar_t* string2  = pointer to beginning of the second string
//     const wchar_t* end2     = pointer past end of the second string
//     const _Collvec* ploc    = pointer to locale info
//
// Exit:
//     -1 = first string less than second string
//     0 = strings are equal
//     1 = first string greater than second string
//     This range of return values may differ from other *cmp / *coll functions.
//
// Exceptions:
//     _NLSCMPERROR = error
//     errno = EINVAL
_CRTIMP2_PURE int __CLRCALL_PURE_OR_CDECL _Wcscoll(const wchar_t* string1, const wchar_t* end1, const wchar_t* string2,
    const wchar_t* end2, const _Collvec* ploc) noexcept {
    int n1  = static_cast<int>(end1 - string1);
    int n2  = static_cast<int>(end2 - string2);
    int ret = 0;
    const wchar_t* locale_name;

    if (ploc == nullptr) {
        locale_name = ___lc_locale_name_func()[LC_COLLATE];
    } else {
        locale_name = ploc->_LocaleName;
    }

    if (locale_name == nullptr) {
        int ans = wmemcmp(string1, string2, n1 < n2 ? n1 : n2);
        ret     = (ans != 0 || n1 == n2 ? ans : n1 < n2 ? -1 : +1);
    } else {
        ret = __crtCompareStringW(locale_name, SORT_STRINGSORT, string1, n1, string2, n2);

        if (ret == 0) {
            errno = EINVAL;
            ret   = _NLSCMPERROR;
        } else {
            ret -= 2;
        }
    }

    return ret;
}

#ifdef MRTDLL
_CRTIMP2_PURE int __CLRCALL_PURE_OR_CDECL _Wcscoll(const unsigned short* string1, const unsigned short* end1,
    const unsigned short* string2, const unsigned short* end2, const _Collvec* ploc) noexcept {
    return _Wcscoll(reinterpret_cast<const wchar_t*>(string1), reinterpret_cast<const wchar_t*>(end1),
        reinterpret_cast<const wchar_t*>(string2), reinterpret_cast<const wchar_t*>(end2), ploc);
}
#endif // defined(MRTDLL)

_END_EXTERN_C_UNLESS_PURE
