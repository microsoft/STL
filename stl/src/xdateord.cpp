// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// look up date ordering

#include <locale>

#include <Windows.h>

_EXTERN_C_UNLESS_PURE

_CRTIMP2_PURE int __CLRCALL_PURE_OR_CDECL _Getdateorder() noexcept { // return date order for current locale
    wchar_t buf[2]                   = {0};
    const wchar_t* const locale_name = ___lc_locale_name_func()[LC_TIME];
    if (locale_name == nullptr) {
        // Enforce std::time_base::mdy for "C" locale
        return std::time_base::mdy;
    } else {
        GetLocaleInfoEx(locale_name, LOCALE_ILDATE, buf, static_cast<int>(std::size(buf)));
    }

    switch (buf[0]) {
    case L'0':
        return std::time_base::mdy;
    case L'1':
        return std::time_base::dmy;
    case L'2':
        return std::time_base::ymd;
    default:
        return std::time_base::no_order;
    }
}

_END_EXTERN_C_UNLESS_PURE
