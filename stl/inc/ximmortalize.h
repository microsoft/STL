// ximmortalize.h internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _XIMMORTALIZE_H
#define _XIMMORTALIZE_H
#include <yvals.h>
#ifndef _M_CEE_PURE
#include <atomic>
#endif

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

template <class _Ty>
_Ty& _Immortalize_pointer() noexcept {
    return _Immortalize<_Ty>();
}

template <class _Ty>
_Ty& _Immortalize_two_pointers() noexcept {
    return _Immortalize<_Ty>();
}

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

template <class _Ty>
_Ty& _Immortalize_pointer() noexcept {
    static atomic<uintptr_t> _Storage;
    static_assert(sizeof(_Storage) == sizeof(_Ty), "Bad _Immortalize_pointer storage size");
    if (_Storage.load(_STD memory_order_acquire) == 0) {
        _Ty _Target;
        _Storage.store(reinterpret_cast<uintptr_t&>(_Target), _STD memory_order_release);
    }

    return reinterpret_cast<_Ty&>(_Storage);
}

template <class _Ty>
_Ty& _Immortalize_two_pointers() noexcept {
    using _Arr = uintptr_t[2];
    static atomic<uintptr_t> _Storage[2];
    static_assert(sizeof(_Storage) == sizeof(_Ty), "Bad _Immortalize_two_pointers storage size");
    static_assert(sizeof(_Storage) == sizeof(_Arr), "Bad assumptions about atomic layout");
    if (_Storage[0].load(_STD memory_order_acquire) == 0) {
        _Ty _Target;
        _Storage[1].store(reinterpret_cast<_Arr&>(_Target)[1], _STD memory_order_relaxed);
        _Storage[0].store(reinterpret_cast<_Arr&>(_Target)[0], _STD memory_order_release);
    }

    return reinterpret_cast<_Ty&>(_Storage);
}
#endif // _M_CEE_PURE
_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _XIMMORTALIZE_H
