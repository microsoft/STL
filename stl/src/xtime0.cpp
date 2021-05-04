// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

#include <algorithm>
#include <chrono>
#include <xtime0.h>

#include "awint.hpp"

static_assert(sizeof(__std_win_system_time) == sizeof(SYSTEMTIME));
static_assert(alignof(__std_win_system_time) == alignof(SYSTEMTIME));

_NODISCARD static constexpr long long _File_time_to_ticks(const FILETIME& _Ft) noexcept {
    return ((static_cast<long long>(_Ft.dwHighDateTime)) << 32) + static_cast<long long>(_Ft.dwLowDateTime);
}

_NODISCARD static constexpr FILETIME _File_time_from_ticks(const long long _Ticks) noexcept {
    return {
        .dwLowDateTime  = static_cast<DWORD>(_Ticks),
        .dwHighDateTime = static_cast<DWORD>(_Ticks >> 32),
    };
}

_EXTERN_C

// SystemTimeToFileTime
// returns -1 on failure
_NODISCARD long long __stdcall __std_win_system_time_to_file_time(const __std_win_system_time* _System_time) noexcept {
    if (FILETIME _Ft; SystemTimeToFileTime(reinterpret_cast<const SYSTEMTIME*>(_System_time), &_Ft) != FALSE) {
        return _File_time_to_ticks(_Ft);
    }

    return -1;
}

// FileTimeToSystemTime
_NODISCARD bool __stdcall __std_win_file_time_to_system_time(
    long long _File_time, __std_win_system_time* _Out_system_time) noexcept {
    const FILETIME _Ft = _File_time_from_ticks(_File_time);
    return FileTimeToSystemTime(&_Ft, reinterpret_cast<SYSTEMTIME*>(_Out_system_time)) != FALSE;
}

_END_EXTERN_C

static constexpr void _Increase_minute(SYSTEMTIME& _St) noexcept {
    using namespace std::chrono;

    ++_St.wMinute;
    if (_St.wMinute < 60) {
        return;
    }

    _St.wMinute -= 60;
    ++_St.wHour;
    if (_St.wHour < 24) {
        return;
    }

    _St.wHour -= 24;
    const sys_days _Sys_d     = year{_St.wYear} / month{_St.wMonth} / day{_St.wDay + 1u};
    _St.wDayOfWeek            = static_cast<WORD>(weekday{_Sys_d}.c_encoding());
    const year_month_day _Ymd = _Sys_d;
    _St.wDay                  = static_cast<WORD>(static_cast<unsigned int>(_Ymd.day()));
    _St.wMonth                = static_cast<WORD>(static_cast<unsigned int>(_Ymd.month()));
    _St.wYear                 = static_cast<WORD>(static_cast<int>(_Ymd.year()));
}

static constexpr int _Ticks_per_sec = 10'000'000;

struct _Utc_to_file_time_result {
    long long _Ticks;
    __std_utc_to_file_time_errc _Ec;
};

// converts UTC (whole seconds) into file_clock
_NODISCARD static _Utc_to_file_time_result _Utc_components_to_file_time(
    const __std_utc_components_1s& _Utc_time) noexcept {
    using enum __std_utc_to_file_time_errc;

    SYSTEMTIME _St{
        .wYear         = static_cast<WORD>(_Utc_time._Year),
        .wMonth        = static_cast<WORD>(_Utc_time._Month),
        .wDay          = static_cast<WORD>(_Utc_time._Day),
        .wHour         = static_cast<WORD>(_Utc_time._Hour),
        .wMinute       = static_cast<WORD>(_Utc_time._Minute),
        .wSecond       = static_cast<WORD>(_Utc_time._Second),
        .wMilliseconds = 0,
    };

    if (_St.wSecond < 60) {
        // second 00-59
        if (FILETIME _Ft; SystemTimeToFileTime(&_St, &_Ft) != FALSE) {
            const long long _Ticks = _File_time_to_ticks(_Ft);
            return {._Ticks = _Ticks, ._Ec = _Success};
        }

        if (_St.wSecond == 59) {
            --_St.wSecond;
            if (FILETIME _Prev_sec_ft; SystemTimeToFileTime(&_St, &_Prev_sec_ft) != FALSE) {
                // negative leap second
                const long long _Prev_sec_ticks = _File_time_to_ticks(_Prev_sec_ft);
                return {._Ticks = _Prev_sec_ticks + _Ticks_per_sec, ._Ec = _Nonexistent};
            }
        }

        return {._Ticks = -1, ._Ec = _Invalid_parameter};
    }

    if (_St.wSecond != 60) {
        return {._Ticks = -1, ._Ec = _Invalid_parameter};
    }

    // second 60
    //
    // https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/ns-processthreadsapi-process_leap_second_info
    // The bahavior of SystemTimeToFileTime and FileTimeToSystemTime during the 2-second period of
    // [23:59:59 UTC, 00:00:00 UTC) around a leap second insertion changes depending on whether
    // PROCESS_LEAP_SECOND_INFO_FLAG_ENABLE_SIXTY_SECOND is set for the process.
    //
    // When the flag is not set (default), SYSTEMTIME doesn't observe second 60. It stretches second 59 to cover the
    // 2-second period instead.
    //
    // When the flag is set, SYSTEMTIME observes second 60 and represents UTC as-is.
    //
    //     FILETIME     |    FileTimeToSystemTime
    //                  | Flag Not Set |   Flag Set
    // ---------------- | ------------ | ------------
    // 23:59:59.000 UTC | 23:59:59.000 | 23:59:59.000
    // 23:59:59.500 UTC | 23:59:59.250 | 23:59:59.500
    // 23:59:60.000 UTC | 23:59:59.500 | 23:59:60.000
    // 23:59:60.500 UTC | 23:59:59.750 | 23:59:60.500
    // 00:00:00.000 UTC | 00:00:00.000 | 00:00:00.000
    //
    //  SYSTEMTIME  |        SystemTimeToFileTime
    //              |   Flag Not Set   |     Flag Set
    // ------------ | ---------------- | ----------------
    // 23:59:59.000 | 23:59:59.000 UTC | 23:59:59.000 UTC
    // 23:59:59.500 | 23:59:60.000 UTC | 23:59:59.500 UTC
    // 23:59:60.000 |      Error       | 23:59:60.000 UTC
    // 23:59:60.500 |      Error       | 23:59:60.500 UTC
    // 00:00:00.000 | 00:00:00.000 UTC | 00:00:00.000 UTC
    //
    // To handle both cases properly, we call SystemTimeToFileTime with the previous second 59 and the next second 00,
    // and then determine whether the second 60 represents a valid positive leap second.

    // the previous second 59
    --_St.wSecond;
    FILETIME _Prev_sec_ft;
    if (SystemTimeToFileTime(&_St, &_Prev_sec_ft) == FALSE) {
        // second 59 is invalid, try second 58
        --_St.wSecond;
        if (SystemTimeToFileTime(&_St, &_Prev_sec_ft) != FALSE) {
            // negative leap second
            const long long _Prev_sec_ticks = _File_time_to_ticks(_Prev_sec_ft);
            return {._Ticks = _Prev_sec_ticks + _Ticks_per_sec, ._Ec = _Nonexistent};
        }

        return {._Ticks = -1, ._Ec = _Invalid_parameter};
    }

    const long long _Prev_sec_ticks = _File_time_to_ticks(_Prev_sec_ft);

    // the next second 00
    _St.wSecond = 0;
    _Increase_minute(_St);
    FILETIME _Next_sec_ft;
    if (SystemTimeToFileTime(&_St, &_Next_sec_ft) == FALSE) {
        return {._Ticks = -1, ._Ec = _Invalid_parameter};
    }

    const long long _Next_sec_ticks = _File_time_to_ticks(_Next_sec_ft);
    const long long _Difference     = _Next_sec_ticks - _Prev_sec_ticks;

    if (_Difference == 2 * _Ticks_per_sec) {
        // positive leap second
        return {._Ticks = _Prev_sec_ticks + _Ticks_per_sec, ._Ec = _Success};
    }

    if (_Difference == _Ticks_per_sec) {
        // non-existent second 60
        return {._Ticks = _Next_sec_ticks, ._Ec = _Nonexistent};
    }

    // unknown leap second smearing behavior
    return {._Ticks = _Next_sec_ticks, ._Ec = _Unknown_smear};
}

_EXTERN_C

// converts UTC (whole seconds) into file_time<seconds>
_NODISCARD __std_utc_to_file_time_errc __stdcall __std_utc_components_to_file_seconds(
    const __std_utc_components_1s* _Utc_time, long long* _Out_file_seconds) noexcept {
    using enum __std_utc_to_file_time_errc;

    const auto [_Ticks, _Ec] = _Utc_components_to_file_time(*_Utc_time);

    if (_Ec == _Invalid_parameter) {
        *_Out_file_seconds = -1;
        return _Ec;
    }

    *_Out_file_seconds = _Ticks / _Ticks_per_sec;

    if (_Ticks % _Ticks_per_sec == 0) {
        return _Ec;
    }

    // unknown leap second smearing behavior
    // assuming positive leap second, smeared clock runs behind UTC before the leap second
    if (_Utc_time->_Day >= 15) {
        ++*_Out_file_seconds;
    }

    return _Ec | _Unknown_smear;
}

// converts file_time<seconds> into UTC
_NODISCARD __std_file_time_to_utc_errc __stdcall __std_file_seconds_to_utc_components(
    const long long _File_seconds, __std_utc_components_1s* _Out_utc_time) noexcept {
    using enum __std_file_time_to_utc_errc;

    const long long _Ticks = _File_seconds * _Ticks_per_sec;
    const FILETIME _Ft     = _File_time_from_ticks(_Ticks);
    SYSTEMTIME _St;
    if (FileTimeToSystemTime(&_Ft, &_St) == FALSE) {
        *_Out_utc_time = {};
        return _Invalid_parameter;
    }

    _Out_utc_time->_Year    = static_cast<short>(_St.wYear);
    _Out_utc_time->_Month   = static_cast<unsigned char>(_St.wMonth);
    _Out_utc_time->_Day     = static_cast<unsigned char>(_St.wDay);
    _Out_utc_time->_Weekday = static_cast<unsigned char>(_St.wDayOfWeek);
    _Out_utc_time->_Hour    = static_cast<unsigned char>(_St.wHour);
    _Out_utc_time->_Minute  = static_cast<unsigned char>(_St.wMinute);
    _Out_utc_time->_Second  = static_cast<unsigned char>(_St.wSecond);

    if (_St.wMilliseconds == 0) {
        // regular time point, or
        // during leap second insertion and process is leap second aware
        return _Success;
    }

    if (_St.wMilliseconds == 500 && _St.wSecond == 59 && _St.wMinute == 59) {
        // during leap second insertion
        // process is leap second unaware, 23:59:60.000 UTC is reported as 23:59:59.500 in SYSTEMTIME
        ++_Out_utc_time->_Second;
        return _Success;
    }

    // unknown leap second smearing behavior
    // assuming positive leap second, smeared clock runs behind UTC before the leap second
    if (_St.wSecond < 60 && _St.wDay >= 15) {
        if (_St.wSecond < 59) {
            ++_Out_utc_time->_Second;
        } else if (_St.wMinute < 59) {
            _Out_utc_time->_Second = 0;
            ++_Out_utc_time->_Minute;
        } else if (_St.wHour < 23) {
            _Out_utc_time->_Second = 0;
            _Out_utc_time->_Minute = 0;
            ++_Out_utc_time->_Hour;
        } else {
            ++_Out_utc_time->_Second;
        }
    }

    return _Unknown_smear;
}

_END_EXTERN_C
