// xtimec.h internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _THR_XTIMEC_H
#define _THR_XTIMEC_H
#include <yvals.h>
#if _STL_COMPILER_PREPROCESSOR

#include <time.h>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_EXTERN_C

struct xtime { // store time with nanosecond resolution
    __time64_t sec;
    long nsec;
};

_CRTIMP2_PURE int __cdecl xtime_get(xtime*, int);

_CRTIMP2_PURE long __cdecl _Xtime_diff_to_millis(const xtime*);
_CRTIMP2_PURE long __cdecl _Xtime_diff_to_millis2(const xtime*, const xtime*);
_CRTIMP2_PURE long long __cdecl _Xtime_get_ticks();
#define _XTIME_NSECS_PER_TICK 100
#define _XTIME_TICKS_PER_TIME_T 10000000LL

_CRTIMP2_PURE long long __cdecl _Query_perf_counter();
_CRTIMP2_PURE long long __cdecl _Query_perf_frequency();

_END_EXTERN_C

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _THR_XTIMEC_H

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
