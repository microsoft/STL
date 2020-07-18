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

enum _Atomic_wait_result {
    _Atomic_wait_timeout  = 0,
    _Atomic_wait_fallback = 1,
    _Atomic_wait_success  = 2,
};

_INLINE_VAR constexpr unsigned long long _Atomic_wait_no_deadline = 0xFFFF'FFFF'FFFF'FFFF;

_EXTERN_C
enum class __std_atomic_api_level : unsigned long {
    __not_set,
    __detecting,
    __has_srwlock,
    __has_wait_on_address,
};

__std_atomic_api_level __stdcall __std_atomic_set_api_level(__std_atomic_api_level _Requested_api_level) noexcept;
_Atomic_wait_result __stdcall __std_atomic_wait_direct(
    const void* _Storage, const void* _Comparand, const size_t _Size, unsigned long long deadline) noexcept;
void __stdcall __std_atomic_notify_one_direct(const void* _Storage) noexcept;
void __stdcall __std_atomic_notify_all_direct(const void* _Storage) noexcept;

void __stdcall __std_atomic_wait_fallback_init(const void* _Storage) noexcept;
void __stdcall __std_atomic_wait_fallback_uninit(const void* _Storage) noexcept;
_Atomic_wait_result __stdcall __std_atomic_wait_fallback(const void* _Storage, unsigned long long deadline) noexcept;
void __stdcall __std_atomic_notify_one_indirect(const void* _Storage) noexcept;
void __stdcall __std_atomic_notify_all_indirect(const void* _Storage) noexcept;

unsigned long long __stdcall __std_atomic_wait_get_deadline(unsigned long long _Timeout) noexcept;
_END_EXTERN_C

inline unsigned long long _Atomic_wait_get_deadline(const unsigned long long _Timeout) noexcept {
    if (_Timeout == _Atomic_wait_no_deadline) {
        return _Atomic_wait_no_deadline;
    }
    return __std_atomic_wait_get_deadline(_Timeout);
}

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _XATOMIC_WAIT_H
