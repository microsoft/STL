// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// coroutine functions

#include <coroutine>

//#include <experimental/generator>
#include <experimental/resumable>

_STD_BEGIN

_CRTIMP2_IMPORT noop_coroutine_handle noop_coroutine() noexcept {
    return {};
}

_STD_END

using procedure_t = void(__cdecl*)(void*);

struct clang_frame_prefix {
    procedure_t factivate;
    procedure_t fdestroy;
};
static_assert(aligned_size_v<clang_frame_prefix> == 16);

struct msvc_frame_prefix {
    procedure_t factivate;
    uint16_t index;
    uint16_t flag;
};
static_assert(aligned_size_v<msvc_frame_prefix> == 16);

extern "C" size_t _coro_resume(void*);
extern "C" void _coro_destroy(void*);
// extern "C" size_t _coro_done(void*);
bool _coro_finished(const msvc_frame_prefix*) noexcept; // replacement of the `_coro_done`

extern "C" bool __builtin_coro_done(void*);
extern "C" void __builtin_coro_resume(void*);
extern "C" void __builtin_coro_destroy(void*);
// void* __builtin_coro_promise(void* ptr, int align, bool p);

#if defined(__clang__)
static constexpr auto is_clang = true;
static constexpr auto is_msvc  = !is_clang;
static constexpr auto is_gcc   = !is_clang;

struct _portable_coro_prefix final : public clang_frame_prefix {};

#elif defined(_MSC_VER)
static constexpr auto is_msvc  = true;
static constexpr auto is_clang = !is_msvc;
static constexpr auto is_gcc   = !is_msvc;

struct _portable_coro_prefix final : public msvc_frame_prefix {};

#endif // __clang__ || _MSC_VER

_CRTIMP2_IMPORT
bool _portable_coro_done(_portable_coro_prefix* handle) {
    if constexpr (is_msvc) {
        return _coro_finished(handle);
    } else if constexpr (is_clang) {
        return __builtin_coro_done(handle);
    } else {
        return false; // follow `noop_coroutine`
    }
}

_CRTIMP2_IMPORT
void _portable_coro_resume(_portable_coro_prefix* handle) {
    if constexpr (is_msvc) {
        _coro_resume(handle);
    } else if constexpr (is_clang) {
        __builtin_coro_resume(handle);
    }
}

_CRTIMP2_IMPORT
void _portable_coro_destroy(_portable_coro_prefix* handle) {
    if constexpr (is_msvc) {
        _coro_destroy(handle);
    } else if constexpr (is_clang) {
        __builtin_coro_destroy(handle);
    }
}

_CRTIMP2_IMPORT
_portable_coro_prefix* _portable_coro_from_promise(void* promise, ptrdiff_t psize) {
    // calculate the frame prefix's location with the promise object
    if constexpr (is_clang) {
        auto* handle = reinterpret_cast<std::byte*>(promise) - psize;
        return reinterpret_cast<_portable_coro_prefix*>(handle);
    } else if constexpr (is_msvc) {
        auto* handle = reinterpret_cast<std::byte*>(promise) + psize;
        return reinterpret_cast<_portable_coro_prefix*>(handle);
    } else {
        // !!! dangerous !!!
        return nullptr;
    }
}

_CRTIMP2_IMPORT
void* _portable_coro_get_promise(_portable_coro_prefix* handle, ptrdiff_t psize) {
    // calculate the promise object's location with the frame's prefix
    if constexpr (is_clang) {
        // for clang, promise is placed just after frame prefix
        // see also: `__builtin_coro_promise`
        auto* promise = reinterpret_cast<std::byte*>(handle) + psize;
        return promise;
    } else if constexpr (is_msvc) {
        // for msvc, promise is placed before frame prefix
        auto* promise = reinterpret_cast<std::byte*>(handle) - psize;
        return promise;
    } else {
        // !!! dangerous !!!
        return nullptr;
    }
}

#if defined(__clang__)
//
//  Note
//      VC++ header expects msvc intrinsics. Redirect them to Clang intrinsics.
//      If the project uses libc++ header files, this code won't be a problem
//      because they wont't be used
//  Reference
//      https://clang.llvm.org/docs/LanguageExtensions.html#c-coroutines-support-builtins
//      https://llvm.org/docs/Coroutines.html#example
//

bool _coro_finished(const msvc_frame_prefix* m) noexcept {
    // expect: coroutine == suspended
    // expect: coroutine != destroyed
    auto* c = reinterpret_cast<const clang_frame_prefix*>(m);
    return __builtin_coro_done(const_cast<clang_frame_prefix*>(c));
}

size_t _coro_resume(void* addr) {
    auto* c = reinterpret_cast<clang_frame_prefix*>(addr);
    __builtin_coro_resume(c);
    return 0;
}

void _coro_destroy(void* addr) {
    auto* c = reinterpret_cast<clang_frame_prefix*>(addr);
    __builtin_coro_destroy(c);
}

#elif defined(_MSC_VER)

bool _coro_finished(const msvc_frame_prefix* prefix) noexcept {
    // expect: coroutine == suspended
    // expect: coroutine != destroyed
    return prefix->index == 0;
}

#endif // __clang__ || _MSC_VER
