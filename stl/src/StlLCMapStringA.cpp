// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "awint.h"
#include <internal_shared.h>
#include <locale.h>

// int __cdecl __crtLCMapStringA - Get type information about an ANSI string.
//
// Purpose:
//        Internal support function. Assumes info in ANSI string format. Tries
//        to use NLS API call LCMapStringA if available and uses LCMapStringW
//        if it must. If neither is available it fails and returns 0.
//
// Entry:
//        LPCWSTR  LocaleName  - locale context for the comparison.
//        DWORD    dwMapFlags  - see docs.microsoft.com
//        LPCSTR   lpSrcStr    - pointer to string to be mapped
//        int      cchSrc      - wide char (word) count of input string
//                               (including null terminator if any)
//                               (-1 if null terminated)
//        LPSTR    lpDestStr   - pointer to memory to store mapping
//        int      cchDest     - char (byte) count of buffer (including null terminator)
//        int      code_page   - for MB/WC conversion. If 0, use __lc_codepage
//        BOOL     bError      - TRUE if MB_ERR_INVALID_CHARS set on call to MultiByteToWideChar when GetStringTypeW
//                               used.
//
// Exit:
//        Success: number of chars written to lpDestStr (including null terminator)
//        Failure: 0
extern "C" int __cdecl __crtLCMapStringA(LPCWSTR LocaleName, DWORD dwMapFlags, LPCSTR lpSrcStr, int cchSrc,
    LPSTR lpDestStr, int cchDest, int code_page, BOOL bError) {
    // LCMapString will map past the null terminator.  We must find the null
    // terminator if it occurs in the string before cchSrc characters
    // and cap the number of characters to be considered.
    if (cchSrc > 0) {
        const int cchSrcCnt = static_cast<int>(__strncnt(lpSrcStr, cchSrc));

        // Include the null terminator if the source string is terminated within
        // the buffer.
        if (cchSrcCnt < cchSrc) {
            cchSrc = cchSrcCnt + 1;
        } else {
            cchSrc = cchSrcCnt;
        }
    }

    // Convert string and return the requested information. Note that we are converting to a wide string so there is not
    // a one-to-one correspondence between number of wide chars in the input string and the number of *bytes* in the
    // buffer. However, there had *better be* a one-to-one correspondence between the number of wide characters and the
    // number of multibyte characters or the resulting mapped string will be worthless to the user.

    // find out how big a buffer we need (includes null terminator if any)
    const int inbuff_size = MultiByteToWideChar(
        code_page, bError ? MB_PRECOMPOSED | MB_ERR_INVALID_CHARS : MB_PRECOMPOSED, lpSrcStr, cchSrc, nullptr, 0);
    if (0 == inbuff_size) {
        return 0;
    }

    // allocate enough space for wide chars
    const __crt_scoped_stack_ptr<wchar_t> inwbuffer(_malloca_crt_t(wchar_t, inbuff_size));
    if (!inwbuffer) {
        return 0;
    }

    // do the conversion
    if (0 == MultiByteToWideChar(code_page, MB_PRECOMPOSED, lpSrcStr, cchSrc, inwbuffer.get(), inbuff_size)) {
        return 0;
    }

    // get size required for string mapping
    int retval = __crtLCMapStringEx(LocaleName, dwMapFlags, inwbuffer.get(), inbuff_size, nullptr, 0);
    if (0 == retval) {
        return 0;
    }

    if (dwMapFlags & LCMAP_SORTKEY) {
        // retval is size in BYTES
        if (0 != cchDest) {
            if (retval > cchDest) {
                return retval;
            }

            // do string mapping
            if (0
                == __crtLCMapStringEx(
                    LocaleName, dwMapFlags, inwbuffer.get(), inbuff_size, (LPWSTR) lpDestStr, cchDest)) {
                return retval;
            }
        }
    } else {
        // retval is size in wide chars
        int outbuff_size = retval;

        // allocate enough space for wide chars (includes null terminator if any)
        const __crt_scoped_stack_ptr<wchar_t> outwbuffer(_malloca_crt_t(wchar_t, outbuff_size));
        if (!outwbuffer) {
            return retval;
        }

        // do string mapping
        if (0
            == __crtLCMapStringEx(
                LocaleName, dwMapFlags, inwbuffer.get(), inbuff_size, outwbuffer.get(), outbuff_size)) {
            return retval;
        }

        if (0 == cchDest) {
            // get size required
            retval = WideCharToMultiByte(code_page, 0, outwbuffer.get(), outbuff_size, nullptr, 0, nullptr, nullptr);
        } else {
            // convert mapping
            retval =
                WideCharToMultiByte(code_page, 0, outwbuffer.get(), outbuff_size, lpDestStr, cchDest, nullptr, nullptr);
        }
    }

    return retval;
}
