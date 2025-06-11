// xatomic_wait.h internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef _XATOMIC_WAIT_H
#define _XATOMIC_WAIT_H
#include <yvals.h>
#if _STL_COMPILER_PREPROCESSOR

#include <cstdlib>
#include <xatomic.h>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

extern "C" {
inline constexpr unsigned long __std_atomic_wait_no_timeout = 0xFFFF'FFFF; // Pass as partial timeout

// Support for atomic waits.
// The "direct" functions are used when the underlying infrastructure can use WaitOnAddress directly; that is, _Size is
// 1, 2, 4, or 8. The contract is the same as the WaitOnAddress function from the Windows SDK.
int __stdcall __std_atomic_wait_direct(
    const void* _Storage, void* _Comparand, size_t _Size, unsigned long _Remaining_timeout) noexcept;
void __stdcall __std_atomic_notify_one_direct(const void* _Storage) noexcept;
void __stdcall __std_atomic_notify_all_direct(const void* _Storage) noexcept;

// The "indirect" functions are used when the size is not 1, 2, 4, or 8; these notionally wait on another value which is
// of one of those sizes whose value changes upon notify, hence "indirect". (As of 2020-07-24, this always uses the
// fallback SRWLOCK and CONDITION_VARIABLE implementation but that is not contractual.)
using _Atomic_wait_indirect_equal_callback_t = bool(__stdcall*)(
    const void* _Storage, void* _Comparand, size_t _Size, void* _Param) _NOEXCEPT_FNPTR;

int __stdcall __std_atomic_wait_indirect(const void* _Storage, void* _Comparand, size_t _Size, void* _Param,
    _Atomic_wait_indirect_equal_callback_t _Are_equal, unsigned long _Remaining_timeout) noexcept;
void __stdcall __std_atomic_notify_one_indirect(const void* _Storage) noexcept;
void __stdcall __std_atomic_notify_all_indirect(const void* _Storage) noexcept;

} // extern "C"

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _XATOMIC_WAIT_H
