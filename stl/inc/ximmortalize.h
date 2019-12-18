// ximmortalize.h internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _XIMMORTALIZE_H
#define _XIMMORTALIZE_H
#include <yvals.h>
#if _STL_COMPILER_PREPROCESSOR

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
#ifdef _M_CEE_PURE
template <class _Ty>
union _Immortalizer_impl { // constructs _Ty, never destroys
    _Immortalizer_impl() noexcept : _Storage{} {}
    _Immortalizer_impl(const _Immortalizer_impl&) = delete;
    _Immortalizer_impl& operator=(const _Immortalizer_impl&) = delete;
    ~_Immortalizer_impl() {
        // do nothing
    }

    _Ty _Storage;
};

#pragma warning(push)
#pragma warning(disable : 4640) // construction of local static object is not thread-safe (/Wall)
template <class _Ty>
constexpr _Ty& _Immortalize() noexcept { // return a reference to an object that will live forever
    /* MAGIC */ static _Immortalizer_impl<_Ty> _Static;
    return _Static._Storage;
}
#pragma warning(pop)

#else // ^^^ _M_CEE_PURE ^^^ // vvv !_M_CEE_PURE vvv
template <class _Ty>
union _Immortalizer_impl {
public:
    constexpr _Immortalizer_impl() : _Storage{} {}
    _Immortalizer_impl(const _Immortalizer_impl&) = delete;
    _Immortalizer_impl& operator=(const _Immortalizer_impl&) = delete;
    ~_Immortalizer_impl() {
        // do nothing
    }

    _Ty _Storage;

    static _Immortalizer_impl _Instance;
};

template <class _Ty>
#ifdef __clang__
[[clang::require_constant_initialization]]
#endif // __clang__
_Immortalizer_impl<_Ty>
    _Immortalizer_impl<_Ty>::_Instance;

template <class _Ty>
constexpr _Ty& _Immortalize() noexcept { // return a reference to an object that will live forever
    return _Immortalizer_impl<_Ty>::_Instance._Storage;
}
#endif // _M_CEE_PURE
_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _XIMMORTALIZE_H
