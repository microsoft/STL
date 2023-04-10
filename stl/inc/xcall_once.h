// xcall_once.h internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _XCALL_ONCE_H
#define _XCALL_ONCE_H
#include <yvals.h>
#if _STL_COMPILER_PREPROCESSOR

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
_EXPORT_STD struct once_flag { // opaque data structure for call_once()
    constexpr once_flag() noexcept : _Opaque(nullptr) {}

    once_flag(const once_flag&)            = delete;
    once_flag& operator=(const once_flag&) = delete;

    void* _Opaque;
};

// Returns BOOL, nonzero to indicate success, zero for failure
using _Execute_once_fp_t = int(__stdcall*)(void*, void*, void**);

// Returns BOOL, nonzero to indicate success, zero for failure
extern "C++" _CRTIMP2_PURE int __CLRCALL_PURE_OR_CDECL _Execute_once(
    once_flag& _Flag, _Execute_once_fp_t _Callback, void* _Pv) noexcept;

template <class _Ty>
union _Immortalizer_impl { // constructs _Ty, never destroys
    constexpr _Immortalizer_impl() noexcept : _Storage{} {}
    _Immortalizer_impl(const _Immortalizer_impl&)            = delete;
    _Immortalizer_impl& operator=(const _Immortalizer_impl&) = delete;
    ~_Immortalizer_impl() {
        // do nothing
    }

    _Ty _Storage;
};
_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _XCALL_ONCE_H
