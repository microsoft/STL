// xatomic_wait.h internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _XATOMIC_WAIT_H
#define _XATOMIC_WAIT_H
#include <yvals.h>
#if _STL_COMPILER_PREPROCESSOR

#include <stdlib.h>
#include <xatomic.h>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

enum _Atomic_spin_phase {
    _Atomic_wait_phase_wait_none    = 0,
    _Atomic_wait_phase_wait_locked  = 1,
    _Atomic_wait_phase_wait_counter = 2,
    _Atomic_unwait_needed           = _Atomic_wait_phase_wait_locked,
};

_INLINE_VAR constexpr unsigned long long _Atomic_wait_no_timeout = 0xFFFF'FFFF'FFFF'FFFF;

struct _Atomic_wait_context_t {
    unsigned long _Wait_phase_and_spin_count = _Atomic_wait_phase_wait_counter;
    unsigned long _Reserved                  = 0; // reserved for potential future precision improvement
    unsigned long long _Deadline             = _Atomic_wait_no_timeout; // or GetTickCount64 plus duration
    unsigned long long _Counter; // For indirect waits - value of internal variable to wait against
};

_EXTERN_C
enum class __std_atomic_api_level : unsigned long {
    __not_set,
    __detecting,
    __has_srwlock,
    __has_wait_on_address,
};

__std_atomic_api_level __stdcall __std_atomic_set_api_level(__std_atomic_api_level _Requested_api_level) noexcept;
bool __stdcall __std_atomic_wait_direct(
    const void* _Storage, const void* _Comparand, const size_t _Size, _Atomic_wait_context_t& _Wait_context) noexcept;
void __stdcall __std_atomic_notify_one_direct(const void* _Storage) noexcept;
void __stdcall __std_atomic_notify_all_direct(const void* _Storage) noexcept;
void __stdcall __std_atomic_unwait_direct(const void* _Storage, _Atomic_wait_context_t& _Wait_context) noexcept;

bool __stdcall __std_atomic_wait_indirect(const void* _Storage, _Atomic_wait_context_t& _Wait_context) noexcept;
void __stdcall __std_atomic_notify_one_indirect(const void* _Storage) noexcept;
void __stdcall __std_atomic_notify_all_indirect(const void* _Storage) noexcept;
void __stdcall __std_atomic_unwait_indirect(const void* _Storage, _Atomic_wait_context_t& _Wait_context) noexcept;

_NODISCARD unsigned long __stdcall __std_atomic_get_spin_count(bool _Is_direct) noexcept;
void __stdcall __std_atomic_wait_get_deadline(
    _Atomic_wait_context_t& _Wait_context, unsigned long long _Timeout) noexcept;
_END_EXTERN_C

inline void _Atomic_wait_get_deadline(
    _Atomic_wait_context_t& _Wait_context, const unsigned long long _Timeout) noexcept {
    if (_Timeout != _Atomic_wait_no_timeout) {
        __std_atomic_wait_get_deadline(_Wait_context, _Timeout);
    }
    // Otherwise defaults to _Atomic_wait_no_timeout
}

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _XATOMIC_WAIT_H
