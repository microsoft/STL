// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Internal definitions for A&W Win32 wrapper routines.
#pragma once

#include <yvals.h>

#include <sdkddkver.h>

#include <Windows.h>

_CRT_BEGIN_C_HEADER

#if _STL_WIN32_WINNT >= _WIN32_WINNT_WIN8

#define __crtGetSystemTimePreciseAsFileTime(lpSystemTimeAsFileTime) \
    GetSystemTimePreciseAsFileTime(lpSystemTimeAsFileTime)

#else // _STL_WIN32_WINNT >= _WIN32_WINNT_WIN8

_CRTIMP2 void __cdecl __crtGetSystemTimePreciseAsFileTime(_Out_ LPFILETIME lpSystemTimeAsFileTime);

#endif // _STL_WIN32_WINNT >= _WIN32_WINNT_WIN8

_CRTIMP2 int __cdecl __crtCompareStringA(_In_z_ LPCWSTR _LocaleName, _In_ DWORD _DwCmpFlags,
    _In_reads_(_CchCount1) LPCSTR _LpString1, _In_ int _CchCount1, _In_reads_(_CchCount2) LPCSTR _LpString2,
    _In_ int _CchCount2, _In_ int _CodePage);

_CRTIMP2 int __cdecl __crtCompareStringW(_In_z_ LPCWSTR _LocaleName, _In_ DWORD _DwCmpFlags,
    _In_reads_(_CchCount1) LPCWSTR _LpString1, _In_ int _CchCount1, _In_reads_(_CchCount2) LPCWSTR _LpString2,
    _In_ int _CchCount2);

_CRTIMP2 int __cdecl __crtLCMapStringA(_In_opt_z_ LPCWSTR _LocaleName, _In_ DWORD _DwMapFlag,
    _In_reads_(_CchSrc) LPCSTR _LpSrcStr, _In_ int _CchSrc, _Out_writes_opt_(_CchDest) char* _LpDestStr,
    _In_ int _CchDest, _In_ int _CodePage, _In_ BOOL _BError);

_CRTIMP2 int __cdecl __crtLCMapStringW(_In_opt_z_ LPCWSTR _LocaleName, _In_ DWORD _DWMapFlag,
    _In_reads_(_CchSrc) LPCWSTR _LpSrcStr, _In_ int _CchSrc, _Out_writes_opt_(_CchDest) wchar_t* _LpDestStr,
    _In_ int _CchDest);

_CRT_END_C_HEADER
