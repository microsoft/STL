// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// print.cpp -- C++23 <print> implementation

#include <__msvc_print.hpp>
#include <cstdio>
#include <expected>
#include <internal_shared.h>
#include <io.h>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

#include <Windows.h>

namespace {
    [[nodiscard]] __std_unicode_console_retrieval_result __stdcall _Get_console_handle_from_file_stream(
        FILE* const _File_stream) noexcept {
        const int _Fd = _fileno(_File_stream);

        switch (_Fd) {
        // _fileno() returns -2 if _File_stream refers to either stdout or stderr and
        // there is no associated output stream. In that case, there is also no associated
        // console HANDLE. (This might happen, for instance, if a Win32 GUI application is
        // being compiled with /SUBSYSTEM:WINDOWS.)
        case -2:
            return __std_unicode_console_retrieval_result{._Error = __std_win_error::_Not_supported};

        case -1:
            [[unlikely]] return __std_unicode_console_retrieval_result{._Error = __std_win_error::_Invalid_parameter};

        default:
            break;
        }

        const HANDLE _Console_handle = reinterpret_cast<HANDLE>(_get_osfhandle(_Fd));

        if (_Console_handle == INVALID_HANDLE_VALUE) [[unlikely]] {
            return __std_unicode_console_retrieval_result{._Error = __std_win_error::_Invalid_parameter};
        }

        // We can check if _Console_handle actually refers to a console or not by checking the
        // return value of GetConsoleMode().
        DWORD _Console_mode;
        const bool _Is_unicode_console = (GetConsoleMode(_Console_handle, &_Console_mode) != 0);

        if (!_Is_unicode_console) {
            return __std_unicode_console_retrieval_result{._Error = __std_win_error::_File_not_found};
        }

        return __std_unicode_console_retrieval_result{
            ._Console_handle = static_cast<__std_unicode_console_handle>(
                reinterpret_cast<_STD underlying_type_t<__std_unicode_console_handle>>(_Console_handle))};
    }
} // unnamed namespace

_EXTERN_C

[[nodiscard]] _Success_(return._Error == __std_win_error::_Success) __std_unicode_console_retrieval_result
    __stdcall __std_get_unicode_console_handle_from_file_stream(_In_ const __std_file_stream_pointer _Stream) noexcept {
    if (_Stream == __std_file_stream_pointer::_Invalid) [[unlikely]] {
        return __std_unicode_console_retrieval_result{._Error = __std_win_error::_Invalid_parameter};
    }

    FILE* const _File_stream_ptr = reinterpret_cast<FILE*>(_Stream);
    return _Get_console_handle_from_file_stream(_File_stream_ptr);
}

_END_EXTERN_C

namespace {
    [[nodiscard]] _STD expected<_STD wstring, __std_win_error> __stdcall _Transcode_utf8_string(
        const char* const _Str, const size_t _Str_size) noexcept {
        // MultiByteToWideChar() fails if strLength == 0.
        if (_Str_size == 0) [[unlikely]] {
            return {};
        }

        // The C++ specifications for vprint_unicode() suggest that we replace invalid
        // code units with U+FFFD. This is done automatically by MultiByteToWideChar(),
        // so long as we do not use the MB_ERR_INVALID_CHARS flag.
        const int32_t _Num_chars_required =
            MultiByteToWideChar(CP_UTF8, 0, _Str, static_cast<int>(_Str_size), nullptr, 0);

        if (_Num_chars_required == 0) [[unlikely]] {
            return _STD unexpected{static_cast<__std_win_error>(GetLastError())};
        }

        _STD wstring _Wide_str;

        try {
            _Wide_str.resize_and_overwrite(static_cast<size_t>(_Num_chars_required),
                [&](wchar_t* const _Dst_buffer, const size_t _Buffer_size) noexcept {
                    return MultiByteToWideChar(
                        CP_UTF8, 0, _Str, static_cast<int>(_Str_size), _Dst_buffer, static_cast<int>(_Buffer_size));
                });
        } catch (const _STD length_error&) {
            return _STD unexpected{__std_win_error::_Insufficient_buffer};
        } catch (...) {
            return _STD unexpected{__std_win_error::_Not_enough_memory};
        }

        // Did MultiByteToWideChar() return 0?
        if (_Wide_str.empty()) [[unlikely]] {
            return _STD unexpected{static_cast<__std_win_error>(GetLastError())};
        }

        return {_STD move(_Wide_str)};
    }

    [[nodiscard]] __std_win_error _Write_console(
        const HANDLE _Console_handle, const _STD wstring_view _Wide_str) noexcept {
        const BOOL _Write_result =
            WriteConsoleW(_Console_handle, _Wide_str.data(), static_cast<DWORD>(_Wide_str.size()), nullptr, nullptr);

        if (!_Write_result) [[unlikely]] {
            return static_cast<__std_win_error>(GetLastError());
        }

        return __std_win_error::_Success;
    }
} // unnamed namespace

_EXTERN_C

[[nodiscard]] _Success_(return == __std_win_error::_Success) __std_win_error
    __stdcall __std_print_to_unicode_console(_In_ const __std_unicode_console_handle _Console_handle,
        _In_ const char* const _Str, _In_ const unsigned long long _Str_size) noexcept {
    if (_Console_handle == __std_unicode_console_handle::_Invalid || _Str == nullptr) [[unlikely]] {
        return __std_win_error::_Invalid_parameter;
    }

    // WriteConsoleW() takes a DWORD for the number of characters to write.
    if (_Str_size > (_STD numeric_limits<DWORD>::max)()) [[unlikely]] {
        return __std_win_error::_Insufficient_buffer;
    }

    const HANDLE _Actual_console_handle = reinterpret_cast<HANDLE>(_Console_handle);
    const _STD expected<_STD wstring, __std_win_error> _Transcoded_str{
        _Transcode_utf8_string(_Str, static_cast<size_t>(_Str_size))};

    if (!_Transcoded_str.has_value()) [[unlikely]] {
        return _Transcoded_str.error();
    }

    return _Write_console(_Actual_console_handle, _STD wstring_view{*_Transcoded_str});
}

_END_EXTERN_C
