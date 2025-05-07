// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Internal definitions for A&W Win32 wrapper routines.

// This file is compiled into the import library (via regex.cpp => awint.hpp).
// MAJOR LIMITATIONS apply to what can be included here!
// Before editing this file, read: /docs/import_library.md

#pragma once

#include <yvals.h>

#include <sdkddkver.h>

#include <Windows.h>

#define _cpp_isleadbyte(c) (__pctype_func()[static_cast<unsigned char>(c)] & _LEADBYTE)

_CRT_BEGIN_C_HEADER

_CRTIMP2 int __cdecl __crtCompareStringA(_In_z_ LPCWSTR _LocaleName, _In_ DWORD _DwCmpFlags,
    _In_reads_(_CchCount1) LPCSTR _LpString1, _In_ int _CchCount1, _In_reads_(_CchCount2) LPCSTR _LpString2,
    _In_ int _CchCount2, _In_ int _CodePage) noexcept;

_CRTIMP2 int __cdecl __crtCompareStringW(_In_z_ LPCWSTR _LocaleName, _In_ DWORD _DwCmpFlags,
    _In_reads_(_CchCount1) LPCWSTR _LpString1, _In_ int _CchCount1, _In_reads_(_CchCount2) LPCWSTR _LpString2,
    _In_ int _CchCount2) noexcept;

_CRTIMP2 int __cdecl __crtLCMapStringA(_In_opt_z_ LPCWSTR _LocaleName, _In_ DWORD _DwMapFlag,
    _In_reads_(_CchSrc) LPCSTR _LpSrcStr, _In_ int _CchSrc, _Out_writes_opt_(_CchDest) char* _LpDestStr,
    _In_ int _CchDest, _In_ int _CodePage, _In_ BOOL _BError) noexcept;

_CRTIMP2 int __cdecl __crtLCMapStringW(_In_opt_z_ LPCWSTR _LocaleName, _In_ DWORD _DWMapFlag,
    _In_reads_(_CchSrc) LPCWSTR _LpSrcStr, _In_ int _CchSrc, _Out_writes_opt_(_CchDest) wchar_t* _LpDestStr,
    _In_ int _CchDest) noexcept;

_CRT_END_C_HEADER
