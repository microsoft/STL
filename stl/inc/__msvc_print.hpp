// __msvc_print.hpp internal header (core)

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef __MSVC_PRINT_HPP
#define __MSVC_PRINT_HPP
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
enum class __std_unicode_console_handle : intptr_t { _Invalid = -1 };

struct __std_unicode_console_retrieval_result {
    __std_unicode_console_handle _Console_handle;

    // For this, we have a few potential return values:
    //
    //   - __std_win_error::_Success: The operation completed successfully. This is the only value for which the
    //     _Console_handle field has a well-defined value.
    //
    //   - __std_win_error::_File_not_found: The FILE* provided is valid, but it is determined to not be associated
    //     with a unicode console. In this case, printing should fall back to vprint_nonunicode().
    //
    //   - __std_win_error::_Not_supported: The FILE* provided does not actually have an associated output stream. In
    //     this case, the entire print can safely be elided, thanks to the "as-if" rule.
    //
    //   - __std_win_error::_Invalid_parameter: The FILE* provided is invalid. A std::system_error exception should be
    //     thrown if this value is returned within the FILE* overload of vprint_unicode().
    __std_win_error _Error;
};

_NODISCARD _Success_(return._Error == __std_win_error::_Success) __std_unicode_console_retrieval_result
    __stdcall __std_get_unicode_console_handle_from_file_stream(_In_ const __std_file_stream_pointer _Stream) noexcept;

_NODISCARD _Success_(return == __std_win_error::_Success) __std_win_error
    __stdcall __std_print_to_unicode_console(_In_ const __std_unicode_console_handle _Console_handle,
        _In_ const char* const _Str, _In_ const unsigned long long _Str_size) noexcept;

_END_EXTERN_C

_STD_BEGIN

inline constexpr bool _Is_ordinary_literal_encoding_utf8 = []() {
// We typically use the _MSVC_EXECUTION_CHARACTER_SET macro to get the ordinary literal encoding
// exactly. In the unlikely event that we cannot get the encoding from that, we use the hack suggested
// in P2093R14.
#ifdef _MSVC_EXECUTION_CHARACTER_SET
    // See: https://docs.microsoft.com/en-us/windows/win32/intl/code-page-identifiers
    return (_MSVC_EXECUTION_CHARACTER_SET == 65001); // Unicode (UTF-8) == 65001
#else
    constexpr unsigned char _Mystery_char[] = "\u00B5";

    // Due to a weird MSVC bug, we need to cast the values to a std::byte before doing the
    // comparison.
    return (sizeof(_Mystery_char) == 3 && static_cast<byte>(_Mystery_char[0]) == static_cast<byte>(0xC2)
            && static_cast<byte>(_Mystery_char[1]) == static_cast<byte>(0xB5));
#endif
}();

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // __MSVC_PRINT_HPP
