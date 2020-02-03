// __msvc_system_error_abi.h internal header (core)

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef __MSVC_SYSTEM_ERROR_ABI_HPP
#define __MSVC_SYSTEM_ERROR_ABI_HPP
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#include <stddef.h>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

#ifdef _M_CEE_PURE
#define __CLRCALL_PURE_OR_STDCALL __clrcall
#else
#define __CLRCALL_PURE_OR_STDCALL __stdcall
#endif

_EXTERN_C

_NODISCARD size_t __CLRCALL_PURE_OR_STDCALL __std_get_string_size_without_trailing_whitespace(
    _In_ const char* const _Str, _In_ size_t _Size) noexcept;

_NODISCARD size_t __CLRCALL_PURE_OR_STDCALL __std_system_error_allocate_message(
    const unsigned long _Message_id, _Out_ char** const _Ptr_str) noexcept;
void __CLRCALL_PURE_OR_STDCALL __std_system_error_free_message(_Post_invalid_ char* const _Str) noexcept;

_END_EXTERN_C

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // __MSVC_SYSTEM_ERROR_ABI_HPP
