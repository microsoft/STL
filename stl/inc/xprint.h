// xprint.h internal header (core)

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _XPRINT_H
#define _XPRINT_H
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#include <xfilesystem_abi.h>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_EXTERN_C

enum class __std_file_stream_pointer : uintptr_t { _Invalid = 0 };

struct __std_unicode_console_detect_result {
    bool _Is_unicode_console;
    __std_win_error _Error;
};

_NODISCARD _Success_(return._Error == __std_win_error::_Success) __std_unicode_console_detect_result
    __stdcall __std_is_file_stream_unicode_console(_In_ const __std_file_stream_pointer _Stream) noexcept;

enum class __std_unicode_console_print_result_type : unsigned long { _Success = 0, _Win_error = 1, _Posix_error = 2 };

struct __std_unicode_console_print_result {
    __std_unicode_console_print_result_type _Result_type;
    union {
        __std_win_error _Win_error;
        int _Posix_error;
    };
};

_NODISCARD _Success_(
    return._Result_type == __std_unicode_console_print_result_type::_Success) __std_unicode_console_print_result
    __stdcall __std_print_to_unicode_console(_In_ const __std_file_stream_pointer _Stream, _In_ const char* const _Str,
        _In_ const unsigned long long _Str_size) noexcept;

_END_EXTERN_C

_STD_BEGIN

inline constexpr bool _Is_ordinary_literal_encoding_utf8 = []() {
// For the MSVC, we use the _MSVC_EXECUTION_CHARACTER_SET to get the ordinary literal encoding
// exactly.
//
// For Clang, we use the hack suggested in P2093R14. Ideally, we would use a better solution.
#ifdef _MSVC_EXECUTION_CHARACTER_SET
    // See: https://docs.microsoft.com/en-us/windows/win32/intl/code-page-identifiers
    return (_MSVC_EXECUTION_CHARACTER_SET == 65001);  // Unicode (UTF-8) == 65001
#else
    constexpr unsigned char _Mystery_char[] = "\u00B5";
    return (sizeof(_Mystery_char) == 3 && _Mystery_char[0] == 0xC2 && _Mystery_char[1] == 0xB5);
#endif
}();

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _XPRINT_H
