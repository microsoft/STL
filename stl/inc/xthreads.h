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
_INLINE_VAR constexpr size_t _Mtx_internal_imp_size          = 80;
_INLINE_VAR constexpr size_t _Mtx_internal_imp_alignment     = 8;
_INLINE_VAR constexpr size_t _Cnd_internal_imp_size          = 72;
_INLINE_VAR constexpr size_t _Cnd_internal_imp_alignment     = 8;
#else // _WIN64
_INLINE_VAR constexpr size_t _Mtx_internal_imp_size      = 48;
_INLINE_VAR constexpr size_t _Mtx_internal_imp_alignment = 4;
_INLINE_VAR constexpr size_t _Cnd_internal_imp_size      = 40;
_INLINE_VAR constexpr size_t _Cnd_internal_imp_alignment = 4;
#endif // _WIN64
#endif // _CRT_WINDOWS

using _Mtx_t = struct _Mtx_internal_imp_t*;

using _Cnd_t = struct _Cnd_internal_imp_t*;

enum { _Thrd_success, _Thrd_nomem, _Thrd_timedout, _Thrd_busy, _Thrd_error };

// threads
_CRTIMP2_PURE int __cdecl _Thrd_detach(_Thrd_t);
_CRTIMP2_PURE int __cdecl _Thrd_join(_Thrd_t, int*);
_CRTIMP2_PURE void __cdecl _Thrd_sleep(const xtime*);
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
_CRTIMP2_PURE int __cdecl _Mtx_timedlock(_Mtx_t, const xtime*);
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
_CRTIMP2_PURE int __cdecl _Cnd_timedwait(_Cnd_t, _Mtx_t, const xtime*);
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

[[noreturn]] _CRTIMP2_PURE void __cdecl _Throw_C_error(int _Code);
[[noreturn]] _CRTIMP2_PURE void __cdecl _Throw_Cpp_error(int _Code);

inline int _Check_C_return(int _Res) { // throw exception on failure
    if (_Res != _Thrd_success) {
        _Throw_C_error(_Res);
    }

    return _Res;
}
_STD_END

#if defined(_M_IX86) || defined(_M_ARM)
_INLINE_VAR constexpr size_t _Stl_critical_section_size      = 36;
_INLINE_VAR constexpr size_t _Stl_critical_section_alignment = 4;
#elif defined(_M_X64) || defined(_M_ARM64)
_INLINE_VAR constexpr size_t _Stl_critical_section_size      = 64;
_INLINE_VAR constexpr size_t _Stl_critical_section_alignment = 8;
#else
#error Unknown architecture
#endif


// Corresponds to internal Concurrency::details::stl_critical_section with changes to expose it in headers
class __declspec(novtable) _Stl_critical_section_interface {
public:
    virtual void __cdecl _Lock()                     = 0;
    virtual bool __cdecl _Try_lock()                 = 0;
    virtual bool __cdecl _Try_lock_for(unsigned int) = 0;
    virtual void __cdecl _Unlock()                   = 0;
    virtual void __cdecl _Destroy()                  = 0;
};

// Corresponds to internal Concurrency::details::stl_critical_section_win7 with changes to expose it in headers
// and make its construction constexpr
class _Stl_critical_section_interface_win7 final : public _Stl_critical_section_interface {
public:
    constexpr _Stl_critical_section_interface_win7() noexcept = default;

    _Stl_critical_section_interface_win7(const _Stl_critical_section_interface_win7&) = delete;
    _Stl_critical_section_interface_win7& operator=(const _Stl_critical_section_interface_win7&) = delete;

    void _Destroy() override {}

    void _Lock() override {
        _Smtx_lock_exclusive(&_Mtx);
    }

    bool _Try_lock() override {
        return _Smtx_try_lock_exclusive(&_Mtx) != 0;
    }

    bool _Try_lock_for(unsigned int) override {
        // STL will call try_lock_for once again if this call will not succeed
        return _Stl_critical_section_interface_win7::_Try_lock();
    }

    void _Unlock() override {
        _Smtx_unlock_exclusive(&_Mtx);
    }

private:
    _Smtx_t _Mtx = {};
};

// Corresponds to internal _Mtx_internal_imp_t with changes to expose it in headers and make its construction constexpr
struct _Mtx_internal_imp_2_t {
    int _Type;
    union alignas(_Stl_critical_section_alignment) _Cs_t {
        _Stl_critical_section_interface_win7 _Cs_win7;
        char _Data[_Stl_critical_section_size];

        constexpr _Cs_t() : _Cs_win7() {}
        ~_Cs_t() {}
    } _Cs;
    long _Thread_id;
    int _Count;
};

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
