// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _timespec64 functions for system time

#include <atomic>
#include <xtimec.h>

#include "awint.hpp"

constexpr long _Nsec_per_sec  = 1000000000L;
constexpr long _Nsec_per_msec = 1000000L;
constexpr int _Msec_per_sec   = 1000;

static void _timespec64_normalize(_timespec64* xt) { // adjust so that 0 <= tv_nsec < 1 000 000 000
    while (xt->tv_nsec < 0) { // normalize target time
        xt->tv_sec -= 1;
        xt->tv_nsec += _Nsec_per_sec;
    }
    while (_Nsec_per_sec <= xt->tv_nsec) { // normalize target time
        xt->tv_sec += 1;
        xt->tv_nsec -= _Nsec_per_sec;
    }
}

// return _timespec64 object holding difference between xt and now, treating negative difference as 0
static _timespec64 _timespec64_diff(const _timespec64* xt, const _timespec64* now) {
    _timespec64 diff = *xt;
    _timespec64_normalize(&diff);
    if (diff.tv_nsec < now->tv_nsec) { // avoid underflow
        diff.tv_sec -= now->tv_sec + 1;
        diff.tv_nsec += _Nsec_per_sec - now->tv_nsec;
    } else { // no underflow
        diff.tv_sec -= now->tv_sec;
        diff.tv_nsec -= now->tv_nsec;
    }

    if (diff.tv_sec < 0 || (diff.tv_sec == 0 && diff.tv_nsec <= 0)) { // time is zero
        diff.tv_sec  = 0;
        diff.tv_nsec = 0;
    }
    return diff;
}

constexpr long long _Epoch      = 0x19DB1DED53E8000LL;
constexpr long _Nsec100_per_sec = _Nsec_per_sec / 100;

_EXTERN_C

_CRTIMP2_PURE long long __cdecl _Xtime_get_ticks() { // get system time in 100-nanosecond intervals since the epoch
    FILETIME ft;
    __crtGetSystemTimePreciseAsFileTime(&ft);
    return ((static_cast<long long>(ft.dwHighDateTime)) << 32) + static_cast<long long>(ft.dwLowDateTime) - _Epoch;
}

// Used by several src files, but not dllexported.
void _Timespec64_get_sys(_timespec64* xt) { // get system time with nanosecond resolution
    unsigned long long now = _Xtime_get_ticks();
    xt->tv_sec             = static_cast<__time64_t>(now / _Nsec100_per_sec);
    xt->tv_nsec            = static_cast<long>(now % _Nsec100_per_sec) * 100;
}

// convert time to milliseconds
_CRTIMP2_PURE long __cdecl _Xtime_diff_to_millis2(const _timespec64* xt1, const _timespec64* xt2) {
    _timespec64 diff = _timespec64_diff(xt1, xt2);
    return static_cast<long>(diff.tv_sec * _Msec_per_sec + (diff.tv_nsec + _Nsec_per_msec - 1) / _Nsec_per_msec);
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE long __cdecl _Xtime_diff_to_millis(const _timespec64* xt) { // convert time to milliseconds
    _timespec64 now;
    _Timespec64_get_sys(&now);
    return _Xtime_diff_to_millis2(xt, &now);
}

// TRANSITION, ABI: preserved for binary compatibility
_CRTIMP2_PURE int __cdecl xtime_get(_timespec64* xt, int type) { // get current time
    if (type != TIME_UTC || xt == nullptr) {
        type = 0;
    } else {
        _Timespec64_get_sys(xt);
    }

    return type;
}

_CRTIMP2_PURE long long __cdecl _Query_perf_counter() { // get current value of performance counter
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li); // always succeeds
    return li.QuadPart;
}

_CRTIMP2_PURE long long __cdecl _Query_perf_frequency() { // get frequency of performance counter
    static std::atomic<long long> freq_cached{0};
    long long freq = freq_cached.load(std::memory_order_relaxed);
    if (freq == 0) {
        LARGE_INTEGER li;
        QueryPerformanceFrequency(&li); // always succeeds
        freq = li.QuadPart; // doesn't change after system boot
        freq_cached.store(freq, std::memory_order_relaxed);
    }
    return freq;
}

_END_EXTERN_C

/*
 * This file is derived from software bearing the following
 * restrictions:
 *
 * (c) Copyright William E. Kempf 2001
 *
 * Permission to use, copy, modify, distribute and sell this
 * software and its documentation for any purpose is hereby
 * granted without fee, provided that the above copyright
 * notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting
 * documentation. William E. Kempf makes no representations
 * about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 */
