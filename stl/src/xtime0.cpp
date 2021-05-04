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

_EXTERN_C

_NODISCARD static constexpr long long file_time_to_ticks(const FILETIME& ft) noexcept {
    return ((static_cast<long long>(ft.dwHighDateTime)) << 32) + static_cast<long long>(ft.dwLowDateTime);
}

_NODISCARD static constexpr FILETIME file_time_from_ticks(const long long ticks) noexcept {
    return {
        .dwLowDateTime  = static_cast<DWORD>(ticks),
        .dwHighDateTime = static_cast<DWORD>(ticks >> 32),
    };
}

// SystemTimeToFileTime
// returns -1 on failure
_NODISCARD long long __stdcall __std_win_system_time_to_file_time(const __std_win_system_time* _System_time) noexcept {
    if (FILETIME ft; SystemTimeToFileTime(reinterpret_cast<const SYSTEMTIME*>(_System_time), &ft) != FALSE) {
        return file_time_to_ticks(ft);
    }

    return -1;
}

// FileTimeToSystemTime
_NODISCARD bool __stdcall __std_win_file_time_to_system_time(
    long long _File_time, __std_win_system_time* _Out_system_time) noexcept {
    const FILETIME ft = file_time_from_ticks(_File_time);
    return FileTimeToSystemTime(&ft, reinterpret_cast<SYSTEMTIME*>(_Out_system_time)) != FALSE;
}

static constexpr void increase_minute(SYSTEMTIME& st) noexcept {
    using namespace std::chrono;

    ++st.wMinute;
    if (st.wMinute < 60) {
        return;
    }

    st.wMinute -= 60;
    ++st.wHour;
    if (st.wHour < 24) {
        return;
    }

    st.wHour -= 24;
    const sys_days sys_d     = year{st.wYear} / month{st.wMonth} / day{st.wDay + 1u};
    st.wDayOfWeek            = static_cast<WORD>(weekday{sys_d}.c_encoding());
    const year_month_day ymd = sys_d;
    st.wDay                  = static_cast<WORD>(static_cast<unsigned int>(ymd.day()));
    st.wMonth                = static_cast<WORD>(static_cast<unsigned int>(ymd.month()));
    st.wYear                 = static_cast<WORD>(static_cast<int>(ymd.year()));
}

static constexpr int ticks_per_sec = 10'000'000;

struct utc_to_file_time_result {
    long long ticks;
    __std_utc_to_file_time_errc ec;
};

// converts UTC (whole seconds) into file_clock
_NODISCARD static utc_to_file_time_result utc_components_to_file_time(
    const __std_utc_components_1s& utc_time) noexcept {
    using enum __std_utc_to_file_time_errc;

    SYSTEMTIME st{
        .wYear         = static_cast<WORD>(utc_time._Year),
        .wMonth        = static_cast<WORD>(utc_time._Month),
        .wDay          = static_cast<WORD>(utc_time._Day),
        .wHour         = static_cast<WORD>(utc_time._Hour),
        .wMinute       = static_cast<WORD>(utc_time._Minute),
        .wSecond       = static_cast<WORD>(utc_time._Second),
        .wMilliseconds = 0,
    };

    if (st.wSecond < 60) {
        // second 00-59
        if (FILETIME ft; SystemTimeToFileTime(&st, &ft) != FALSE) {
            const long long ticks = file_time_to_ticks(ft);
            return {.ticks = ticks, .ec = _Success};
        }

        if (st.wSecond == 59) {
            --st.wSecond;
            if (FILETIME prev_sec_ft; SystemTimeToFileTime(&st, &prev_sec_ft) != FALSE) {
                // negative leap second
                const long long prev_sec_ticks = file_time_to_ticks(prev_sec_ft);
                return {.ticks = prev_sec_ticks + ticks_per_sec, .ec = _Nonexistent};
            }
        }

        return {.ticks = -1, .ec = _Invalid_parameter};
    }

    if (st.wSecond != 60) {
        return {.ticks = -1, .ec = _Invalid_parameter};
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
    --st.wSecond;
    FILETIME prev_sec_ft;
    if (SystemTimeToFileTime(&st, &prev_sec_ft) == FALSE) {
        // second 59 is invalid, try second 58
        --st.wSecond;
        if (SystemTimeToFileTime(&st, &prev_sec_ft) != FALSE) {
            // negative leap second
            const long long prev_sec_ticks = file_time_to_ticks(prev_sec_ft);
            return {.ticks = prev_sec_ticks + ticks_per_sec, .ec = _Nonexistent};
        }

        return {.ticks = -1, .ec = _Invalid_parameter};
    }

    const long long prev_sec_ticks = file_time_to_ticks(prev_sec_ft);

    // the next second 00
    st.wSecond = 0;
    increase_minute(st);
    FILETIME next_sec_ft;
    if (SystemTimeToFileTime(&st, &next_sec_ft) == FALSE) {
        return {.ticks = -1, .ec = _Invalid_parameter};
    }

    const long long next_sec_ticks = file_time_to_ticks(next_sec_ft);
    const long long difference     = next_sec_ticks - prev_sec_ticks;

    if (difference == 2 * ticks_per_sec) {
        // positive leap second
        return {.ticks = prev_sec_ticks + ticks_per_sec, .ec = _Success};
    }

    if (difference == ticks_per_sec) {
        // non-existent second 60
        return {.ticks = next_sec_ticks, .ec = _Nonexistent};
    }

    // unknown leap second smearing behavior
    return {.ticks = next_sec_ticks, .ec = _Unknown_smear};
}

// converts UTC (whole seconds) into file_time<seconds>
_NODISCARD __std_utc_to_file_time_errc __stdcall __std_utc_components_to_file_seconds(
    const __std_utc_components_1s* _Utc_time, long long* _Out_file_seconds) noexcept {
    using enum __std_utc_to_file_time_errc;

    const auto [ticks, ec] = utc_components_to_file_time(*_Utc_time);

    if (ec == _Invalid_parameter) {
        *_Out_file_seconds = -1;
        return ec;
    }

    *_Out_file_seconds = ticks / ticks_per_sec;

    if (ticks % ticks_per_sec == 0) {
        return ec;
    }

    // unknown leap second smearing behavior
    // assuming positive leap second, smeared clock runs behind UTC before the leap second
    if (_Utc_time->_Day >= 15) {
        ++*_Out_file_seconds;
    }

    return ec | _Unknown_smear;
}

// converts file_time<seconds> into UTC
_NODISCARD __std_file_time_to_utc_errc __stdcall __std_file_seconds_to_utc_components(
    const long long _File_seconds, __std_utc_components_1s* _Out_utc_time) noexcept {
    using enum __std_file_time_to_utc_errc;

    const long long ticks = _File_seconds * ticks_per_sec;
    const FILETIME ft     = file_time_from_ticks(ticks);
    SYSTEMTIME st;
    if (FileTimeToSystemTime(&ft, &st) == FALSE) {
        *_Out_utc_time = {};
        return _Invalid_parameter;
    }

    _Out_utc_time->_Year    = static_cast<short>(st.wYear);
    _Out_utc_time->_Month   = static_cast<signed char>(st.wMonth);
    _Out_utc_time->_Day     = static_cast<signed char>(st.wDay);
    _Out_utc_time->_Weekday = static_cast<signed char>(st.wDayOfWeek);
    _Out_utc_time->_Hour    = static_cast<signed char>(st.wHour);
    _Out_utc_time->_Minute  = static_cast<signed char>(st.wMinute);
    _Out_utc_time->_Second  = static_cast<signed char>(st.wSecond);

    if (st.wMilliseconds == 0) {
        // regular time point, or
        // during leap second insertion and process is leap second aware
        return _Success;
    }

    if (st.wMilliseconds == 500 && st.wSecond == 59 && st.wMinute == 59) {
        // during leap second insertion
        // process is leap second unaware, 23:59:60.000 UTC is reported as 23:59:59.500 in SYSTEMTIME
        ++_Out_utc_time->_Second;
        return _Success;
    }

    // unknown leap second smearing behavior
    // assuming positive leap second, smeared clock runs behind UTC before the leap second
    if (st.wSecond < 60 && st.wDay >= 15) {
        if (st.wSecond < 59) {
            ++_Out_utc_time->_Second;
        } else if (st.wMinute < 59) {
            _Out_utc_time->_Second = 0;
            ++_Out_utc_time->_Minute;
        } else if (st.wHour < 23) {
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
