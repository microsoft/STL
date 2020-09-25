// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstring> // for wcsnlen

#include <Windows.h>

#include "awint.hpp"

// int __cdecl __crtCompareStringW - Get type information about a wide string.
//
// Purpose:
//   Internal support function. Assumes info in wide string format.
//
// Entry:
//   LPCWSTR  LocaleName  - locale context for the comparison.
//   DWORD    dwCmpFlags  - see docs.microsoft.com
//   LPCWSTR  lpStringn   - wide string to be compared
//   int      cchCountn   - wide char (word) count (NOT including null terminator)
//                        (-1 if null terminated)
//
// Exit:
//   Success: 1 - if lpString1 <  lpString2
//            2 - if lpString1 == lpString2
//            3 - if lpString1 >  lpString2
//   Failure: 0
extern "C" int __cdecl __crtCompareStringW(_In_z_ LPCWSTR LocaleName, _In_ DWORD dwCmpFlags,
    _In_reads_(cchCount1) LPCWSTR lpString1, _In_ int cchCount1, _In_reads_(cchCount2) LPCWSTR lpString2,
    _In_ int cchCount2) {
    // CompareString will compare past null terminator. Must find null terminator if in string before cchCountn wide
    // characters.
    if (cchCount1 > 0) {
        cchCount1 = static_cast<int>(wcsnlen(lpString1, cchCount1));
    }

    if (cchCount2 > 0) {
        cchCount2 = static_cast<int>(wcsnlen(lpString2, cchCount2));
    }

    if (!cchCount1 || !cchCount2) {
        return (cchCount1 - cchCount2 == 0) ? 2 : (cchCount1 - cchCount2 < 0) ? 1 : 3;
    }

    return CompareStringEx(LocaleName, dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2, nullptr, nullptr, 0);
}
