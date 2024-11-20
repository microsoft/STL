// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <__msvc_system_error_abi.hpp>

#include <Windows.h>

namespace {
    struct _Whitespace_bitmap_t {
        bool _Is_whitespace[256];

        constexpr _Whitespace_bitmap_t() noexcept : _Is_whitespace{} {
            _Is_whitespace[' ']  = true;
            _Is_whitespace['\n'] = true;
            _Is_whitespace['\r'] = true;
            _Is_whitespace['\t'] = true;
            _Is_whitespace['\0'] = true;
        }

        [[nodiscard]] constexpr bool _Test(const char _Ch) const noexcept {
            return _Is_whitespace[static_cast<unsigned char>(_Ch)];
        }
    };

    constexpr _Whitespace_bitmap_t _Whitespace_bitmap;
} // unnamed namespace

extern "C" {
[[nodiscard]] size_t __CLRCALL_PURE_OR_STDCALL __std_get_string_size_without_trailing_whitespace(
    const char* const _Str, size_t _Size) noexcept {
    while (_Size != 0 && _Whitespace_bitmap._Test(_Str[_Size - 1])) {
        --_Size;
    }

    return _Size;
}

[[nodiscard]] size_t __CLRCALL_PURE_OR_STDCALL __std_system_error_allocate_message(
    const unsigned long _Message_id, char** const _Ptr_str) noexcept {
    // convert to name of Windows error, return 0 for failure, otherwise return number of chars in buffer
    // __std_system_error_deallocate_message should be called even if 0 is returned
    // pre: *_Ptr_str == nullptr

    // We always request US English for system_category() messages.
    // This is consistent with generic_category(), which uses a table of US English strings in the STL.
    // See GH-2451 and GH-3254 for the history here - we previously tried to localize system_category() messages,
    // but attempting to use FormatMessageA's behavior for language ID 0 and attempting to use the system locale
    // had various failure scenarios.
    // Using US English (which is FormatMessageA's final fallback for language ID 0)
    // is likely to succeed with whatever the end-user's system configuration is.
    // In general, system_error messages aren't directly useful to end-users - they're meant for programmer-users.
    // Of course, the programmer-user might not speak US English, but machine translation of the message
    // (and the numeric value of the error code) should help them understand the error.
    // The previous failure scenarios of "unknown error" or a string of question marks were completely unhelpful.

    constexpr auto _Flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
    constexpr auto _Lang_id = 0x0409; // 1033 decimal, "en-US" locale

    const auto _Chars =
        FormatMessageA(_Flags, nullptr, _Message_id, _Lang_id, reinterpret_cast<char*>(_Ptr_str), 0, nullptr);

    return _CSTD __std_get_string_size_without_trailing_whitespace(*_Ptr_str, _Chars);
}

void __CLRCALL_PURE_OR_STDCALL __std_system_error_deallocate_message(char* const _Str) noexcept {
    LocalFree(_Str);
}
} // extern "C"
