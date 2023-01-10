// xprint.h internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _XPRINT_H
#define _XPRINT_H
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR

#include <format>
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

template <class _CharT, class... _Types>
struct _Basic_print_string : private basic_format_string<_CharT, _Types...> {
public:
    enum class _Format_string_type {_Directly_printable, _Requires_formatting};

    template <class _Ty>
        requires convertible_to<const _Ty&, basic_string_view<_CharT>>
    consteval _Basic_print_string(const _Ty& _Str_val) : basic_format_string<_CharT, _Types...>(_Str_val) {
        // We expect the case where std::print() is called with a string without
        // formatting arguments present (e.g., std::print("Hello, world!"); to be common enough 
        // to warrant some optimization. Specifically, if no formatting arguments are present, 
        // then we *may* not actually need to call std::vformat() at all.
        //
        // What we need to watch out for, however, are escaped brace characters (i.e., {{
        // and }}). If these are present, then we need to manually replace them with single
        // characters at runtime. Otherwise, we can just print the provided format string.
        // We check for this special case at compile time. Future work might be to optimize the 
        // string replacement to use a specialized function, rather than just deferring it to 
        // std::vformat().
        if constexpr (sizeof...(_Types) > 0) {
            _Str_type = _Format_string_type::_Requires_formatting;
        } else {
            const basic_string_view<_CharT> _Fmt_str = _Get();

            if (_Fmt_str.empty()) {
                _Str_type = _Format_string_type::_Directly_printable;
            } else {
                char _Prev_char = _Fmt_str.front();
                for (const auto _Curr_char : _Fmt_str.substr(1)) {
                    if ((_Curr_char == '{' && _Prev_char == '{') || (_Curr_char == '}' && _Prev_char == '}')) {
                        _Str_type = _Format_string_type::_Requires_formatting;
                        return;
                    }

                    _Prev_char = _Curr_char;
                }

                _Str_type = _Format_string_type::_Directly_printable;
            }
        }
    }

    _NODISCARD constexpr basic_string_view<_CharT> _Get() const noexcept {
        return basic_format_string<_CharT, _Types...>::get();
    }

    _NODISCARD constexpr _Format_string_type _Get_type() const noexcept {
        return _Str_type;
    }

private:
    _Format_string_type _Str_type;
};

template <class... _Types>
using _Print_string = _Basic_print_string<char, type_identity_t<_Types>...>;

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _XPRINT_H
