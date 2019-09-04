// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "awint.h"

#if _STL_WIN32_WINNT < _WIN32_WINNT_VISTA

#include "nlsdownlevel.h"
#include <ctype.h>
#include <stdlib.h>

// __wcsnicmp_ascii
// Implements wcsnicmp for ASCII chars only.
// We can't use wcsncmp in this context because we may end up trying to modify
// locale data structs or even calling the same function in NLS code.
extern "C" int __wcsnicmp_ascii(__in const wchar_t* string1, __in const wchar_t* string2, __in size_t count) {
    wchar_t f;
    wchar_t l;
    int result = 0;

    if (count) {
        // validation section
        do {
            f = static_cast<wchar_t>(__ascii_towlower(*string1));
            l = static_cast<wchar_t>(__ascii_towlower(*string2));
            ++string1;
            ++string2;
        } while (--count && f && f == l);
        result = static_cast<int>(f - l);
    }

    return result;
}

// Maps input locale name to the index on LcidToLocaleNameTable
extern "C" int GetTableIndexFromLocaleName(__in const wchar_t* localeName) {
    const LOCALENAMEINDEX* localeNamesIndex = LocaleNameToIndexTable;
    int bottom                              = 0;
    int top                                 = _countof(LocaleNameToIndexTable) - 1;

    while (bottom <= top) {
        int middle    = (bottom + top) / 2;
        int testIndex = __wcsnicmp_ascii(localeName, localeNamesIndex[middle].name, LOCALE_NAME_MAX_LENGTH);

        if (testIndex == 0) {
            return localeNamesIndex[middle].index;
        }

        if (testIndex < 0) {
            top = middle - 1;
        } else {
            bottom = middle + 1;
        }
    }

    return -1;
}

// Maps input LCID to an index in LcidToLocaleNameTable
extern "C" int GetTableIndexFromLcid(__in LCID lcid) {
    int bottom = 0;
    int top    = _countof(LcidToLocaleNameTable) - 1;

    while (bottom <= top) {
        int middle    = (bottom + top) / 2;
        int testIndex = lcid - LcidToLocaleNameTable[middle].lcid;

        if (testIndex == 0) {
            return middle;
        }

        if (testIndex < 0) {
            top = middle - 1;
        } else {
            bottom = middle + 1;
        }
    }

    return -1;
}

// __crtDownlevelLocaleNameToLCID() - Maps the locale name to LCID on Windows XP.
extern "C" LCID __cdecl __crtDownlevelLocaleNameToLCID(LPCWSTR localeName) {
    int index;

    if (localeName == nullptr) {
        return 0;
    }

    index = GetTableIndexFromLocaleName(localeName);

    if (index < 0 || index >= _countof(LcidToLocaleNameTable)) {
        return 0;
    }

    return LcidToLocaleNameTable[index].lcid;
}

// __crtDownlevelLCIDToLocaleName() - Maps the LCID to locale name on Windows XP.
extern "C" int __cdecl __crtDownlevelLCIDToLocaleName(LCID lcid, LPWSTR outLocaleName, int cchLocaleName) {
    if (lcid == 0 || lcid == LOCALE_USER_DEFAULT || lcid == LOCALE_SYSTEM_DEFAULT) {
        return 0;
    }

    if ((outLocaleName == nullptr && cchLocaleName > 0) || cchLocaleName < 0) {
        return 0;
    }

    const int index = GetTableIndexFromLcid(lcid);
    if (index < 0) {
        return 0;
    }

    const wchar_t* const buffer = LcidToLocaleNameTable[index].localeName;
    // the following cast is safe because LOCALE_NAME_MAX_LENGTH fits in int:
    const int count = static_cast<int>(wcsnlen(buffer, LOCALE_NAME_MAX_LENGTH));

    if (cchLocaleName > 0) {
        if (count >= cchLocaleName) {
            return 0;
        }

        wcscpy_s(outLocaleName, cchLocaleName, buffer);
    }

    return count + 1;
}

// __crtCompareStringEx() - Wrapper for CompareStringEx().
extern "C" int __cdecl __crtCompareStringEx(
    LPCWSTR lpLocaleName, DWORD dwCmpFlags, LPCWSTR lpString1, int cchCount1, LPCWSTR lpString2, int cchCount2) {
    // use CompareStringEx if it is available (only on Windows Vista+)...
    IFDYNAMICGETCACHEDFUNCTION(PFNCOMPARESTRINGEX, CompareStringEx, pfCompareStringEx) {
        return (*pfCompareStringEx)(
            lpLocaleName, dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2, nullptr, nullptr, 0);
    }

    // ...otherwise fall back to using CompareString.
    return CompareStringW(
        __crtDownlevelLocaleNameToLCID(lpLocaleName), dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2);
}

// __crtLCMapStringEx() - Wrapper for LCMapStringEx().
extern "C" int __cdecl __crtLCMapStringEx(
    LPCWSTR lpLocaleName, DWORD dwMapFlags, LPCWSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest) {
    // use LCMapStringEx if it is available (only on Windows Vista+)...
    IFDYNAMICGETCACHEDFUNCTION(PFNLCMAPSTRINGEX, LCMapStringEx, pfLCMapStringEx) {
        return (*pfLCMapStringEx)(lpLocaleName, dwMapFlags, lpSrcStr, cchSrc, lpDestStr, cchDest, nullptr, nullptr, 0);
    }

    // ...otherwise fall back to using LCMapString.
    return LCMapStringW(__crtDownlevelLocaleNameToLCID(lpLocaleName), dwMapFlags, lpSrcStr, cchSrc, lpDestStr, cchDest);
}


// __crtGetLocaleInfoEx() - Wrapper for GetLocaleInfoEx().
extern "C" int __cdecl __crtGetLocaleInfoEx(
    LPCWSTR const lpLocaleName, LCTYPE const LCType, LPWSTR const lpLCData, int const cchData) {
    IFDYNAMICGETCACHEDFUNCTION(PFNGETLOCALEINFOEX, GetLocaleInfoEx, pfGetLocaleInfoEx) {
        return pfGetLocaleInfoEx(lpLocaleName, LCType, lpLCData, cchData);
    }

    return GetLocaleInfoW(__crtDownlevelLocaleNameToLCID(lpLocaleName), LCType, lpLCData, cchData);
}

#endif // _STL_WIN32_WINNT < _WIN32_WINNT_VISTA
