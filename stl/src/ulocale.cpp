// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Machinery for the UTF-8 codecvt facets

#if !defined(_DLL) || defined(_BUILDING_SATELLITE_CODECVT_IDS)
#include <xlocale>

#ifdef _DLL
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

_STD_BEGIN
EXPORT locale::id codecvt<char16_t, char8_t, mbstate_t>::id;
EXPORT locale::id codecvt<char32_t, char8_t, mbstate_t>::id;

// These specializations are exported from msvcp140, *except* for their ids
EXPORT locale::id codecvt<char16_t, char, mbstate_t>::id;
EXPORT locale::id codecvt<char32_t, char, mbstate_t>::id;
_STD_END
#endif // !defined(_DLL) || defined(_BUILDING_SATELLITE_CODECVT_IDS)
