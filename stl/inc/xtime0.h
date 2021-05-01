// xtime0.h internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

#pragma once
#ifndef _XTIME0_H
#define _XTIME0_H

#include <yvals_core.h>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_EXTERN_C

// clang-format off
struct __std_win_system_time {    // typedef struct _SYSTEMTIME {
    unsigned short _Year;         //     WORD wYear;
    unsigned short _Month;        //     WORD wMonth;
    unsigned short _Day_of_week;  //     WORD wDayOfWeek;
    unsigned short _Day;          //     WORD wDay;
    unsigned short _Hour;         //     WORD wHour;
    unsigned short _Minute;       //     WORD wMinute;
    unsigned short _Second;       //     WORD wSecond;
    unsigned short _Milliseconds; //     WORD wMilliseconds;
};                                // } SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;
// clang-format on

struct __std_utc_components_1s {
    short _Year;
    signed char _Month;
    signed char _Day;
    signed char _Weekday;
    signed char _Hour;
    signed char _Minute;
    signed char _Second;
};

// SystemTimeToFileTime
// returns -1 on failure
_NODISCARD long long __stdcall __std_win_system_time_to_file_time(const __std_win_system_time* _System_time) noexcept;

// FileTimeToSystemTime
_NODISCARD bool __stdcall __std_win_file_time_to_system_time(
    long long _File_time, __std_win_system_time* _Out_system_time) noexcept;

enum class __std_utc_components_to_file_seconds_errc {
    _Success = 0,

    // input is invalid regardless of whether there are leap seconds
    // _Out_file_seconds is not set
    _Invalid_parameter = 1 << 0,

    // __std_utc_components_to_file_seconds only
    // second 60 without a positive leap second, or second 59 deleted by a negative leap second
    // sets _Out_file_seconds to the value corresponding to second 00 of the next minute
    _Nonexistent = 1 << 1,

    // SystemTimeToFileTime returns unexpected sub-second values
    _Unknown_smear = 1 << 2,
};

_BITMASK_OPS(__std_utc_components_to_file_seconds_errc)

enum class __std_file_seconds_to_utc_components_errc {
    _Success = 0,

    _Invalid_parameter = 1,

    // FileTimeToSystemTime returns unexpected sub-second values
    _Unknown_smear = 4,
};

// converts UTC (whole seconds) into file_time<seconds>
_NODISCARD int __stdcall __std_utc_components_to_file_seconds(
    const __std_utc_components_1s* _Utc_time, long long* _Out_file_seconds) noexcept;

// converts file_time<seconds> into UTC
_NODISCARD int __stdcall __std_file_seconds_to_utc_components(
    long long _File_seconds, __std_utc_components_1s* _Out_utc_time) noexcept;

_END_EXTERN_C

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // defined(_XTIME0_H)
