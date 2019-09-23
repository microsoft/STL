// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// coroutine functions

#include <coroutine>

_STD_BEGIN

static_assert(sizeof(coroutine_handle<void>) == sizeof(void*));

_CRTIMP2_IMPORT noop_coroutine_handle noop_coroutine() noexcept {
    return {};
}

_STD_END

using _Procedure = void(__cdecl*)(void*);

constexpr auto _Align_req_v = sizeof(void*) * 2;
template <typename P>
constexpr auto _Aligned_size_v = (sizeof(P) + _Align_req_v - 1u) & ~(_Align_req_v - 1u);

constexpr ptrdiff_t _Make_aligned_size(size_t _TypeSize) {
    return (_TypeSize + _Align_req_v - 1u) & ~(_Align_req_v - 1u);
}

struct _Clang_frame_prefix {
    _Procedure _Factivate;
    _Procedure _Fdestroy;
};
static_assert(_Aligned_size_v<_Clang_frame_prefix> == 16);

struct _Msvc_frame_prefix {
    _Procedure _Factivate;
    uint16_t _Index;
    uint16_t _Flag;
};
static_assert(_Aligned_size_v<_Msvc_frame_prefix> == 16);

extern "C" size_t _coro_resume(void*);
extern "C" void _coro_destroy(void*);
extern "C" size_t _coro_done(void*);
#pragma intrinsic(_coro_resume)
#pragma intrinsic(_coro_destroy)
#pragma intrinsic(_coro_done)

extern "C" bool __builtin_coro_done(void*);
extern "C" void __builtin_coro_resume(void*);
extern "C" void __builtin_coro_destroy(void*);
// void* __builtin_coro_promise(void* ptr, int align, bool p);

// replacement of the `_coro_done`
bool _coro_finished(_Portable_coro_prefix*) noexcept;

#if defined(__clang__)
static constexpr auto is_clang = true;
static constexpr auto is_msvc  = !is_clang;

struct _Portable_coro_prefix final : public _Clang_frame_prefix {};

#elif defined(_MSC_VER)
static constexpr auto is_msvc  = true;
static constexpr auto is_clang = !is_msvc;

struct _Portable_coro_prefix final : public _Msvc_frame_prefix {};

#endif // __clang__ || _MSC_VER

_CRTIMP2_IMPORT
bool _Portable_coro_done(_Portable_coro_prefix* _Handle) {
    if constexpr (is_msvc) {
        return _coro_finished(_Handle);
    } else if constexpr (is_clang) {
        return __builtin_coro_done(_Handle);
    } else {
        return false; // follow `noop_coroutine`
    }
}

_CRTIMP2_IMPORT
void _Portable_coro_resume(_Portable_coro_prefix* _Handle) {
    if constexpr (is_msvc) {
        _coro_resume(_Handle);
    } else if constexpr (is_clang) {
        __builtin_coro_resume(_Handle);
    }
}

_CRTIMP2_IMPORT
void _Portable_coro_destroy(_Portable_coro_prefix* _Handle) {
    if constexpr (is_msvc) {
        _coro_destroy(_Handle);
    } else if constexpr (is_clang) {
        __builtin_coro_destroy(_Handle);
    }
}

// 'get_promise' from frame prefix
_CRTIMP2_IMPORT
void* _Portable_coro_get_promise(_Portable_coro_prefix* _Handle, ptrdiff_t _PromSize) {
    // location of the promise object
    void* _PromAddr = nullptr;

    if constexpr (is_clang) {
        // for Clang, promise is placed just after frame prefix
        // see also: `__builtin_coro_promise`
        _PromAddr = reinterpret_cast<std::byte*>(_Handle) + _Aligned_size_v<_Clang_frame_prefix>;
    } else if constexpr (is_msvc) {
        // for MSVC, promise is placed before frame prefix
        _PromAddr = reinterpret_cast<std::byte*>(_Handle) - _Make_aligned_size(_PromSize);
    }
    return _PromAddr;
}

// 'from_promise' get frame prefix
_CRTIMP2_IMPORT
_Portable_coro_prefix* _Portable_coro_from_promise(void* _PromAddr, ptrdiff_t _PromSize) {
    // location of the frame prefix
    void* _Handle = nullptr;

    if constexpr (is_clang) {
        _Handle = reinterpret_cast<std::byte*>(_PromAddr) - _Aligned_size_v<_Clang_frame_prefix>;
    } else if constexpr (is_msvc) {
        _Handle = reinterpret_cast<std::byte*>(_PromAddr) + _Make_aligned_size(_PromSize);
    }
    return reinterpret_cast<_Portable_coro_prefix*>(_Handle);
}


#if defined(__clang__)

bool _coro_finished(_Portable_coro_prefix* _Handle) noexcept {
    auto* _Ptr = reinterpret_cast<clang_frame_prefix*>(_Handle);
    return __builtin_coro_done(_Ptr);
}

#elif defined(_MSC_VER)

// replacement of the `_coro_done`
bool _coro_finished(_Portable_coro_prefix* _Handle) noexcept {
    // expect: coroutine == suspended
    // expect: coroutine != destroyed
    return _Handle->_Index == 0;
}

#endif // __clang__ || _MSC_VER
