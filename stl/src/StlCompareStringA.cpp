// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "awint.h"
#include <crtdbg.h>
#include <internal_shared.h>
#include <string.h>

#include <Windows.h>


// int __cdecl __crtCompareStringA - Get type information about an ANSI string.
//
// Purpose:
//        Internal support function. Assumes info in ANSI string format. Tries
//        to use NLS API call CompareStringA if available and uses CompareStringW
//        if it must. If neither is available it fails and returns 0.
//
// Entry:
//        LPCWSTR LocaleName  - locale context for the comparison.
//        DWORD   dwCmpFlags  - see docs.microsoft.com
//        LPCSTR  lpStringn   - multibyte string to be compared
//        int     cchCountn   - char (byte) count (NOT including null terminator)
//                              (-1 if null terminated)
//        int     code_page   - for MB/WC conversion. If 0, use __lc_codepage
//
// Exit:
//        Success: 1 - if lpString1 <  lpString2
//                 2 - if lpString1 == lpString2
//                 3 - if lpString1 >  lpString2
//        Failure: 0
extern "C" int __cdecl __crtCompareStringA(LPCWSTR LocaleName, DWORD dwCmpFlags, LPCSTR lpString1, int cchCount1,
    LPCSTR lpString2, int cchCount2, int code_page) {
    // CompareString will compare past null terminator. Must find null terminator if in string before cchCountn chars.
    if (cchCount1 > 0) {
        cchCount1 = static_cast<int>(__strncnt(lpString1, cchCount1));
    } else if (cchCount1 < -1) {
        return FALSE;
    }

    if (cchCount2 > 0) {
        cchCount2 = static_cast<int>(__strncnt(lpString2, cchCount2));
    } else if (cchCount2 < -1) {
        return FALSE;
    }

    // Special case: at least one count is zero
    if (!cchCount1 || !cchCount2) {
        // both strings zero
        if (cchCount1 == cchCount2) {
            return 2;
        }

        // string 1 greater
        if (cchCount2 > 1) {
            return 1;
        }

        // string 2 greater
        if (cchCount1 > 1) {
            return 3;
        }

        // one has zero count, the other has a count of one
        // - if the one count is a naked lead byte, the strings are equal
        // - otherwise it is a single character and they are unequal
        CPINFO cpInfo;
        if (GetCPInfo(code_page, &cpInfo) == FALSE) {
            return 0;
        }

        _ASSERTE((cchCount1 == 0 && cchCount2 == 1) || (cchCount1 == 1 && cchCount2 == 0));

        // string 1 has count of 1
        if (cchCount1 > 0) {
            if (cpInfo.MaxCharSize < 2) {
                return 3;
            }

            for (auto cp = &cpInfo.LeadByte[0]; cp[0] && cp[1]; cp += 2) {
                const auto ch = static_cast<unsigned char>(*lpString1);
                if (ch >= cp[0] && ch <= cp[1]) {
                    return 2;
                }
            }

            return 3;
        }

        // string 2 has count of 1
        if (cchCount2 > 0) {
            if (cpInfo.MaxCharSize < 2) {
                return 1;
            }

            for (auto cp = &cpInfo.LeadByte[0]; cp[0] && cp[1]; cp += 2) {
                const auto ch = static_cast<unsigned char>(*lpString2);
                if (ch >= cp[0] && ch <= cp[1]) {
                    return 2;
                }
            }

            return 1;
        }
    }

    // Convert strings and return the requested information.

    // find out how big a buffer we need (includes null terminator if any)
    const int buff_size1 =
        MultiByteToWideChar(code_page, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, lpString1, cchCount1, nullptr, 0);
    if (0 == buff_size1) {
        return 0;
    }

    // allocate enough space for chars
    __crt_scoped_stack_ptr<wchar_t> wbuffer1(_malloca_crt_t(wchar_t, buff_size1));
    if (wbuffer1.get() == nullptr) {
        return 0;
    }

    // do the conversion
    if (0 == MultiByteToWideChar(code_page, MB_PRECOMPOSED, lpString1, cchCount1, wbuffer1.get(), buff_size1)) {
        return 0;
    }

    // find out how big a buffer we need (includes null terminator if any)
    const int buff_size2 =
        MultiByteToWideChar(code_page, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, lpString2, cchCount2, nullptr, 0);
    if (0 == buff_size2) {
        return 0;
    }

    // allocate enough space for chars
    const __crt_scoped_stack_ptr<wchar_t> wbuffer2(_malloca_crt_t(wchar_t, buff_size2));
    if (wbuffer2.get() == nullptr) {
        return 0;
    }

    const int actual_size =
        MultiByteToWideChar(code_page, MB_PRECOMPOSED, lpString2, cchCount2, wbuffer2.get(), buff_size2);
    if (actual_size == 0) {
        return 0;
    }

    return __crtCompareStringEx(LocaleName, dwCmpFlags, wbuffer1.get(), buff_size1, wbuffer2.get(), buff_size2);
}
