// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstring> // for wcsnlen

#include "awint.hpp"

// int __cdecl __crtLCMapStringW - Get type information about a wide string.
//
// Purpose:
//        Internal support function. Assumes info in wide string format.
//
// Entry:
//        LPCWSTR  locale_name       - locale context for the comparison.
//        DWORD    map_flags         - see docs.microsoft.com
//        LPCWSTR  source            - pointer to string to be mapped
//        int      source_count      - wide char (word) count of input string
//                                     (including null terminator if any)
//                                     (-1 if null terminated)
//        LPWSTR   destination       - pointer to memory to store mapping
//        int      destination_count - wide char (word) count of buffer (including L'\0')
//
//        NOTE:    if LCMAP_SORTKEY is specified, then cchDest refers to number
//                 of BYTES, not number of wide chars. The return string will be
//                 a series of bytes with a null terminator.
//
// Exit:
//        Success: if LCMAP_SORTKEY:
//                    number of bytes written to destination (including null terminator)
//                 else
//                    number of wide characters written to destination (including null terminator)
//        Failure: 0
extern "C" int __cdecl __crtLCMapStringW(_In_opt_z_ LPCWSTR const locale_name, _In_ DWORD const map_flags,
    _In_reads_(source_count) LPCWSTR const source, _In_ int source_count,
    _Out_writes_opt_(destination_count) wchar_t* const destination, _In_ int const destination_count) {
    // LCMapString will map past the null terminator.  We must find the null terminator if it occurs in the string
    // before source_count characters and cap the number of characters to be considered.
    if (source_count > 0) {
        const int source_length = static_cast<int>(wcsnlen(source, source_count));

        // Include the null terminator if the source string is terminated within the buffer.
        if (source_length < source_count) {
            source_count = source_length + 1;
        } else {
            source_count = source_length;
        }
    }

    return LCMapStringEx(
        locale_name, map_flags, source, source_count, destination, destination_count, nullptr, nullptr, 0);
}
