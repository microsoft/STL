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
// STRUCT once_flag
struct once_flag { // opaque data structure for call_once()
    constexpr once_flag() noexcept : _Opaque(nullptr) {}

    once_flag(const once_flag&) = delete;
    once_flag& operator=(const once_flag&) = delete;

    void* _Opaque;
};

using _Execute_once_fp_t = int(__stdcall*)(void*, void*, void**);

_CRTIMP2_PURE int __CLRCALL_PURE_OR_CDECL _Execute_once(
    once_flag& _Flag, _Execute_once_fp_t _Callback, void* _Pv) noexcept;

#ifdef _M_CEE_PURE
template <class _Ty>
struct _Immortalizer { // constructs _Ty, never destroys
    _Immortalizer() { // construct _Ty inside _Storage
        ::new (static_cast<void*>(&_Storage)) _Ty();
    }

    _Immortalizer(const _Immortalizer&) = delete;
    _Immortalizer& operator=(const _Immortalizer&) = delete;

    aligned_union_t<1, _Ty> _Storage;
};

#pragma warning(push)
#pragma warning(disable : 4640) // construction of local static object is not thread-safe (/Wall)
template <class _Ty>
_Ty& _Immortalize() { // return a reference to an object that will live forever
    /* MAGIC */ static _Immortalizer<_Ty> _Static;
    return reinterpret_cast<_Ty&>(_Static._Storage);
}
#pragma warning(pop)

#else // ^^^ _M_CEE_PURE ^^^ // vvv !_M_CEE_PURE vvv
template <class _Ty>
int __stdcall _Immortalize_impl(void*, void* _Storage_ptr, void**) noexcept {
    // adapt True Placement New to _Execute_once
    ::new (_Storage_ptr) _Ty();
    return 1;
}

template <class _Ty>
_Ty& _Immortalize() { // return a reference to an object that will live forever
    static_assert(sizeof(void*) == sizeof(once_flag), "TRANSITION, VSO#406237");
    static_assert(alignof(void*) == alignof(once_flag), "TRANSITION, VSO#406237");
    static void* _Flag = nullptr;
    static aligned_union_t<1, _Ty> _Storage;
    if (_Execute_once(reinterpret_cast<once_flag&>(_Flag), _Immortalize_impl<_Ty>, &_Storage) == 0) {
        // _Execute_once should never fail if the callback never fails
        _STD terminate();
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
#endif // _XCALL_ONCE_H
