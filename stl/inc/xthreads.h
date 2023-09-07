// xthreads.h internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef _THR_XTHREADS_H
#define _THR_XTHREADS_H
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#include <climits>
#include <type_traits>
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

using _Smtx_t = void*;

struct _Stl_critical_section {
    void* _Unused       = nullptr; // TRANSITION, ABI: was the vptr
    _Smtx_t _M_srw_lock = nullptr;
};

struct _Mtx_internal_imp_t {
#ifdef _CRT_WINDOWS
#ifdef _WIN64
    static constexpr size_t _Critical_section_size = 16;
#else // ^^^ defined(_WIN64) / !defined(_WIN64) vvv
    static constexpr size_t _Critical_section_size = 8;
#endif // ^^^ !defined(_WIN64) ^^^
#else // ^^^ defined(_CRT_WINDOWS) / !defined(_CRT_WINDOWS) vvv
#ifdef _WIN64
    static constexpr size_t _Critical_section_size = 64;
#else // ^^^ defined(_WIN64) / !defined(_WIN64) vvv
    static constexpr size_t _Critical_section_size = 36;
#endif // ^^^ !defined(_WIN64) ^^^
#endif // ^^^ !defined(_CRT_WINDOWS) ^^^

    static constexpr size_t _Critical_section_align = alignof(void*);

    int _Type{};
    union {
        _Stl_critical_section _Critical_section{};
        _STD _Aligned_storage_t<_Critical_section_size, _Critical_section_align> _Cs_storage;
    };
    long _Thread_id{};
    int _Count{};
};

// Size and alignment for _Cnd_internal_imp_t
#if defined(_CRT_WINDOWS) // for Windows-internal code
_INLINE_VAR constexpr size_t _Cnd_internal_imp_size = 2 * sizeof(void*);
#elif defined(_WIN64) // ordinary 64-bit code
_INLINE_VAR constexpr size_t _Cnd_internal_imp_size = 72;
#else // vvv ordinary 32-bit code vvv
_INLINE_VAR constexpr size_t _Cnd_internal_imp_size = 40;
#endif // ^^^ ordinary 32-bit code ^^^

_INLINE_VAR constexpr size_t _Cnd_internal_imp_alignment = alignof(void*);

using _Mtx_t = _Mtx_internal_imp_t*;

#ifdef _M_CEE // avoid warning LNK4248: unresolved typeref token for '_Cnd_internal_imp_t'; image may not run
using _Cnd_t = void*;
#else // ^^^ defined(_M_CEE) / !defined(_M_CEE) vvv
struct _Cnd_internal_imp_t;
using _Cnd_t = _Cnd_internal_imp_t*;
#endif // ^^^ !defined(_M_CEE) ^^^

enum class _Thrd_result : int { _Success, _Nomem, _Timedout, _Busy, _Error };

// threads
_CRTIMP2_PURE _Thrd_result __cdecl _Thrd_detach(_Thrd_t);
_CRTIMP2_PURE _Thrd_result __cdecl _Thrd_join(_Thrd_t, int*);
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

#ifdef _CRTBLD
_CRTIMP2_PURE _Thrd_result __cdecl _Mtx_init(_Mtx_t*, int);
_CRTIMP2_PURE void __cdecl _Mtx_destroy(_Mtx_t);
#endif // _CRTBLD
_CRTIMP2_PURE void __cdecl _Mtx_init_in_situ(_Mtx_t, int);
_CRTIMP2_PURE void __cdecl _Mtx_destroy_in_situ(_Mtx_t);
_CRTIMP2_PURE int __cdecl _Mtx_current_owns(_Mtx_t);
_CRTIMP2_PURE _Thrd_result __cdecl _Mtx_lock(_Mtx_t);
_CRTIMP2_PURE _Thrd_result __cdecl _Mtx_trylock(_Mtx_t);
_CRTIMP2_PURE _Thrd_result __cdecl _Mtx_unlock(_Mtx_t); // TRANSITION, ABI: Always succeeds

#ifdef _CRTBLD
_CRTIMP2_PURE void __cdecl _Mtx_clear_owner(_Mtx_t);
_CRTIMP2_PURE void __cdecl _Mtx_reset_owner(_Mtx_t);
#endif // _CRTBLD

// shared mutex
// these declarations must be in sync with those in sharedmutex.cpp
void __cdecl _Smtx_lock_exclusive(_Smtx_t*);
void __cdecl _Smtx_lock_shared(_Smtx_t*);
int __cdecl _Smtx_try_lock_exclusive(_Smtx_t*);
int __cdecl _Smtx_try_lock_shared(_Smtx_t*);
void __cdecl _Smtx_unlock_exclusive(_Smtx_t*);
void __cdecl _Smtx_unlock_shared(_Smtx_t*);

// condition variables
#ifdef _CRTBLD
_CRTIMP2_PURE _Thrd_result __cdecl _Cnd_init(_Cnd_t*);
_CRTIMP2_PURE void __cdecl _Cnd_destroy(_Cnd_t);
#endif // _CRTBLD
_CRTIMP2_PURE void __cdecl _Cnd_init_in_situ(_Cnd_t);
_CRTIMP2_PURE void __cdecl _Cnd_destroy_in_situ(_Cnd_t);
_CRTIMP2_PURE _Thrd_result __cdecl _Cnd_wait(_Cnd_t, _Mtx_t); // TRANSITION, ABI: Always succeeds
_CRTIMP2_PURE _Thrd_result __cdecl _Cnd_timedwait(_Cnd_t, _Mtx_t, const _timespec64*);
_CRTIMP2_PURE _Thrd_result __cdecl _Cnd_broadcast(_Cnd_t); // TRANSITION, ABI: Always succeeds
_CRTIMP2_PURE _Thrd_result __cdecl _Cnd_signal(_Cnd_t); // TRANSITION, ABI: Always succeeds
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
