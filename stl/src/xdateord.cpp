// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// look up date ordering

#include "awint.h"
#include <locale>

_EXTERN_C_UNLESS_PURE

int __CLRCALL_PURE_OR_CDECL _Getdateorder() { // return date order for current locale
    wchar_t buf[2] = {0};
    __crtGetLocaleInfoEx(___lc_locale_name_func()[LC_TIME], LOCALE_ILDATE, buf, sizeof(buf) / sizeof(buf[0]));

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
