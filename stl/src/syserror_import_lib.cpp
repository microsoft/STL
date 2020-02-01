// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

#include <__msvc_system_error_abi.hpp>

#include <Windows.h>

namespace {

    struct _Whitespace_bitmap_t {
        bool _Is_whitespace[256];

        constexpr _Whitespace_bitmap_t() : _Is_whitespace{} {
            _Is_whitespace[' ']  = true;
            _Is_whitespace['\n'] = true;
            _Is_whitespace['\r'] = true;
            _Is_whitespace['\t'] = true;
            _Is_whitespace['\0'] = true;
        }

        constexpr bool _Test(const char _Ch) const noexcept {
            return _Is_whitespace[static_cast<unsigned char>(_Ch)];
        }
    };

    constexpr _Whitespace_bitmap_t _Whitespace_bitmap;

} // unnamed namespace

extern "C" {
unsigned long __CLRCALL_PURE_OR_STDCALL __std_get_string_size_without_trailing_whitespace(
    const char* const _Str, unsigned long _Size) noexcept {
    if (_Size != 0) {
        do {
            --_Size;
        } while (_Whitespace_bitmap._Test(_Str[_Size]));
    }

    return _Size;
}

_NODISCARD size_t __CLRCALL_PURE_OR_STDCALL __std_system_error_allocate_message(
    unsigned long _Message_id, _Out_ char** _Ptr_str) noexcept {
    // convert to name of Windows error, return 0 for failure, otherwise return number of chars written
    // pre: *_Ptr_str == nullptr
    const unsigned long _Chars =
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0,
            _Message_id, 0, reinterpret_cast<char*>(_Ptr_str), 0, 0);

    const size_t _Length = _CSTD __std_get_string_size_without_trailing_whitespace(*_Ptr_str, _Chars);

    // FormatMessageA returned a message being only whitespaces?
    if (_Length == 0 && *_Ptr_str != nullptr) {
        _CSTD __std_system_error_free_message(*_Ptr_str);
        *_Ptr_str = nullptr;
    }

    return _Length;
}

void __CLRCALL_PURE_OR_STDCALL __std_system_error_free_message(_Post_invalid_ char* _Str) noexcept {
    LocalFree(_Str);
}
}
