// xthreads.h internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _THR_XTHREADS_H
#define _THR_XTHREADS_H
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#include <climits>
#include <xtimec.h>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_EXTERN_C
using _Thrd_id_t = unsigned int;
struct _Thrd_t { // thread identifier for Win32
    void* _Hnd; // Win32 HANDLE
    _Thrd_id_t _Id;
};

// Size and alignment for _Mtx_internal_imp_t and _Cnd_internal_imp_t
#ifdef _CRT_WINDOWS
#ifdef _WIN64
_INLINE_VAR constexpr size_t _Mtx_internal_imp_size      = 32;
_INLINE_VAR constexpr size_t _Mtx_internal_imp_alignment = 8;
_INLINE_VAR constexpr size_t _Cnd_internal_imp_size      = 16;
_INLINE_VAR constexpr size_t _Cnd_internal_imp_alignment = 8;
#else // _WIN64
_INLINE_VAR constexpr size_t _Mtx_internal_imp_size      = 20;
_INLINE_VAR constexpr size_t _Mtx_internal_imp_alignment = 4;
_INLINE_VAR constexpr size_t _Cnd_internal_imp_size      = 8;
_INLINE_VAR constexpr size_t _Cnd_internal_imp_alignment = 4;
#endif // _WIN64
#else // _CRT_WINDOWS
#ifdef _WIN64
_INLINE_VAR constexpr size_t _Mtx_internal_imp_size      = 80;
_INLINE_VAR constexpr size_t _Mtx_internal_imp_alignment = 8;
_INLINE_VAR constexpr size_t _Cnd_internal_imp_size      = 72;
_INLINE_VAR constexpr size_t _Cnd_internal_imp_alignment = 8;
#else // _WIN64
_INLINE_VAR constexpr size_t _Mtx_internal_imp_size      = 48;
_INLINE_VAR constexpr size_t _Mtx_internal_imp_alignment = 4;
_INLINE_VAR constexpr size_t _Cnd_internal_imp_size      = 40;
_INLINE_VAR constexpr size_t _Cnd_internal_imp_alignment = 4;
#endif // _WIN64
#endif // _CRT_WINDOWS

#ifdef _M_CEE // avoid warning LNK4248: unresolved typeref token for '_Mtx_internal_imp_t'; image may not run
using _Mtx_t = void*;
using _Cnd_t = void*;
#else // ^^^ defined(_M_CEE) / !defined(_M_CEE) vvv
struct _Mtx_internal_imp_t;
struct _Cnd_internal_imp_t;
using _Mtx_t = _Mtx_internal_imp_t*;
using _Cnd_t = _Cnd_internal_imp_t*;
#endif // ^^^ !defined(_M_CEE) ^^^

enum { _Thrd_success, _Thrd_nomem, _Thrd_timedout, _Thrd_busy, _Thrd_error };

// threads
_CRTIMP2_PURE int __cdecl _Thrd_detach(_Thrd_t);
_CRTIMP2_PURE int __cdecl _Thrd_join(_Thrd_t, int*);
_CRTIMP2_PURE void __cdecl _Thrd_sleep(const _timespec64*);
_CRTIMP2_PURE void __cdecl _Thrd_yield();
_CRTIMP2_PURE unsigned int __cdecl _Thrd_hardware_concurrency();
_CRTIMP2_PURE _Thrd_id_t __cdecl _Thrd_id();

// mutexes
enum { // mutex types
    _Mtx_plain     = 0x01,
    _Mtx_try       = 0x02,
    _Mtx_timed     = 0x04,
    _Mtx_recursive = 0x100
};

_CRTIMP2_PURE int __cdecl _Mtx_init(_Mtx_t*, int);
_CRTIMP2_PURE void __cdecl _Mtx_destroy(_Mtx_t);
_CRTIMP2_PURE void __cdecl _Mtx_init_in_situ(_Mtx_t, int);
_CRTIMP2_PURE void __cdecl _Mtx_destroy_in_situ(_Mtx_t);
_CRTIMP2_PURE int __cdecl _Mtx_current_owns(_Mtx_t);
_CRTIMP2_PURE int __cdecl _Mtx_lock(_Mtx_t);
_CRTIMP2_PURE int __cdecl _Mtx_trylock(_Mtx_t);
_CRTIMP2_PURE int __cdecl _Mtx_timedlock(_Mtx_t, const _timespec64*);
_CRTIMP2_PURE int __cdecl _Mtx_unlock(_Mtx_t); // TRANSITION, ABI: always returns _Thrd_success

_CRTIMP2_PURE void* __cdecl _Mtx_getconcrtcs(_Mtx_t);
_CRTIMP2_PURE void __cdecl _Mtx_clear_owner(_Mtx_t);
_CRTIMP2_PURE void __cdecl _Mtx_reset_owner(_Mtx_t);

// shared mutex
// these declarations must be in sync with those in sharedmutex.cpp
using _Smtx_t = void*;
void __cdecl _Smtx_lock_exclusive(_Smtx_t*);
void __cdecl _Smtx_lock_shared(_Smtx_t*);
int __cdecl _Smtx_try_lock_exclusive(_Smtx_t*);
int __cdecl _Smtx_try_lock_shared(_Smtx_t*);
void __cdecl _Smtx_unlock_exclusive(_Smtx_t*);
void __cdecl _Smtx_unlock_shared(_Smtx_t*);

// condition variables
_CRTIMP2_PURE int __cdecl _Cnd_init(_Cnd_t*);
_CRTIMP2_PURE void __cdecl _Cnd_destroy(_Cnd_t);
_CRTIMP2_PURE void __cdecl _Cnd_init_in_situ(_Cnd_t);
_CRTIMP2_PURE void __cdecl _Cnd_destroy_in_situ(_Cnd_t);
_CRTIMP2_PURE int __cdecl _Cnd_wait(_Cnd_t, _Mtx_t); // TRANSITION, ABI: Always returns _Thrd_success
_CRTIMP2_PURE int __cdecl _Cnd_timedwait(_Cnd_t, _Mtx_t, const _timespec64*);
_CRTIMP2_PURE int __cdecl _Cnd_broadcast(_Cnd_t); // TRANSITION, ABI: Always returns _Thrd_success
_CRTIMP2_PURE int __cdecl _Cnd_signal(_Cnd_t); // TRANSITION, ABI: Always returns _Thrd_success
_CRTIMP2_PURE void __cdecl _Cnd_register_at_thread_exit(_Cnd_t, _Mtx_t, int*);
_CRTIMP2_PURE void __cdecl _Cnd_unregister_at_thread_exit(_Mtx_t);
_CRTIMP2_PURE void __cdecl _Cnd_do_broadcast_at_thread_exit();
_END_EXTERN_C

_STD_BEGIN
enum { // constants for error codes
    _DEVICE_OR_RESOURCE_BUSY,
    _INVALID_ARGUMENT,
    _NO_SUCH_PROCESS,
    _NOT_ENOUGH_MEMORY,
    _OPERATION_NOT_PERMITTED,
    _RESOURCE_DEADLOCK_WOULD_OCCUR,
    _RESOURCE_UNAVAILABLE_TRY_AGAIN
};

extern "C++" [[noreturn]] _CRTIMP2_PURE void __cdecl _Throw_Cpp_error(int _Code);
_STD_END
#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _THR_XTHREADS_H

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
