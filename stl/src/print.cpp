// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// print.cpp -- C++23 <print> implementation

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

#include <cstdio>
#include <cstring>
#include <internal_shared.h>
#include <io.h>
#include <type_traits>
#include <xprint.h>

#include <Windows.h>

namespace {
    [[nodiscard]] inline HANDLE __stdcall _Get_console_handle_from_file_stream(FILE* const _File_stream) noexcept {
        return reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(_File_stream)));
    }

    struct _String_to_wide_string_results {
        __crt_unique_heap_ptr<wchar_t> _Wide_str;
        size_t _Wide_str_size;
        __std_win_error _Error;
    };

    template <bool _Add_newline>
    [[nodiscard]] _String_to_wide_string_results __stdcall _String_to_wide_string(
        const char* const _Str, const size_t _Str_size) noexcept {
        // MultiByteToWideChar() fails if strLength == 0.
        if (_Str_size == 0) [[unlikely]] {
            return _String_to_wide_string_results{};
        }

        // The C++ specifications for vprint_unicode() suggest that we replace invalid
        // code units with U+FFFD. This is done automatically by MultiByteToWideChar(),
        // so long as we do not use the MB_ERR_INVALID_CHARS flag.
        const int32_t _Num_chars_required =
            MultiByteToWideChar(CP_UTF8, 0, _Str, static_cast<int>(_Str_size), nullptr, 0);

        if (_Num_chars_required == 0) [[unlikely]] {
            return _String_to_wide_string_results{._Error = static_cast<__std_win_error>(GetLastError())};
        }

        const size_t _Buffer_size_in_wchars = [_Num_chars_required]() {
            // If necessary, add one to the count so that we can append a newline character.
            // We don't explicitly add a null-terminating character because we don't need to;
            // the WriteConsoleW() API allows one to define exactly how many characters are
            // to be written to the console.
            if constexpr (_Add_newline) {
                // Add one for the newline character.
                return _Num_chars_required + 1;
            } else {
                return _Num_chars_required;
            }
        }();

        __crt_unique_heap_ptr<wchar_t> _Wide_str{_malloc_crt_t(wchar_t, _Buffer_size_in_wchars)};

        if (!_Wide_str) [[unlikely]] {
            return _String_to_wide_string_results{._Error = __std_win_error::_Not_enough_memory};
        }

        const int32_t _Num_chars_written = MultiByteToWideChar(
            CP_UTF8, 0, _Str, static_cast<int>(_Str_size), _Wide_str.get(), static_cast<int>(_Buffer_size_in_wchars));

        if (_Num_chars_written == 0) [[unlikely]] {
            return _String_to_wide_string_results{._Error = static_cast<__std_win_error>(GetLastError())};
        }

        // If necessary, add a newline character.
        if constexpr (_Add_newline) {
            _Wide_str.get()[_Buffer_size_in_wchars - 1] = L'\n';
        }

        return _String_to_wide_string_results{
            ._Wide_str = _STD move(_Wide_str), ._Wide_str_size = _Buffer_size_in_wchars};
    }

    [[nodiscard]] __std_unicode_console_print_result _Write_console(
        const HANDLE _Console_handle, const wchar_t* const _Wide_str, const size_t _Wide_str_size) noexcept {
        if (_Wide_str_size > UINT32_MAX) [[unlikely]] {
            return __std_unicode_console_print_result{
                ._Result_type = __std_unicode_console_print_result_type::_Win_error,
                ._Win_error   = __std_win_error::_Insufficient_buffer};
        }

        const BOOL _Write_result =
            WriteConsoleW(_Console_handle, _Wide_str, static_cast<DWORD>(_Wide_str_size), nullptr, nullptr);

        if (!_Write_result) [[unlikely]] {
            return __std_unicode_console_print_result{
                ._Result_type = __std_unicode_console_print_result_type::_Win_error,
                ._Win_error   = static_cast<__std_win_error>(GetLastError())};
        }

        return __std_unicode_console_print_result{};
    }

    template <bool _Add_newline>
    [[nodiscard]] __std_unicode_console_print_result _Print_to_unicode_console_impl(
        const __std_file_stream_pointer _Stream, const char* const _Str, const size_t _Str_size) noexcept {
        if (_Stream == __std_file_stream_pointer::_Invalid || _Str == nullptr) [[unlikely]] {
            return __std_unicode_console_print_result{
                ._Result_type = __std_unicode_console_print_result_type::_Win_error,
                ._Win_error   = __std_win_error::_Invalid_parameter};
        }

        FILE* const _File_stream = reinterpret_cast<FILE*>(_Stream);

        // We always need to flush the stream before writing to the console. This is required by the C++
        // standard.
        const bool _Was_flush_successful = (_STD fflush(_File_stream) == 0);

        if (!_Was_flush_successful) [[unlikely]] {
            return __std_unicode_console_print_result{
                ._Result_type = __std_unicode_console_print_result_type::_Posix_error, ._Posix_error = errno};
        }

        const HANDLE _Console_handle = _Get_console_handle_from_file_stream(_File_stream);

        // If the size of the string is set to zero, then we don't need to call _String_to_wide_string()
        // to do any heap allocations. In fact, if _Add_newline == false, we can just exit immediately
        // after flushing the stream if the string is empty and still comply with the C++ standard, thanks
        // to the "as-if" rule.
        if (_Str_size == 0) [[unlikely]] {
            if constexpr (_Add_newline) {
                return _Write_console(_Console_handle, L"\n", 1);
            } else {
                return __std_unicode_console_print_result{};
            }
        }

        const _String_to_wide_string_results _Str_conversion_results{
            _String_to_wide_string<_Add_newline>(_Str, _Str_size)};

        if (_Str_conversion_results._Error != __std_win_error::_Success) [[unlikely]] {
            return __std_unicode_console_print_result{
                ._Result_type = __std_unicode_console_print_result_type::_Win_error,
                ._Win_error   = _Str_conversion_results._Error};
        }

        return _Write_console(
            _Console_handle, _Str_conversion_results._Wide_str, _Str_conversion_results._Wide_str_size);
    }
} // unnamed namespace

_EXTERN_C

[[nodiscard]] _Success_(return._Error == __std_win_error::_Success) __std_unicode_console_detect_result
    __stdcall __std_is_file_stream_unicode_console(_In_ const __std_file_stream_pointer _Stream) noexcept {
    if (_Stream == __std_file_stream_pointer::_Invalid) [[unlikely]] {
        return __std_unicode_console_detect_result{._Error = __std_win_error::_Invalid_parameter};
    }

    FILE* const _File_stream     = reinterpret_cast<FILE*>(_Stream);
    const HANDLE _Console_handle = _Get_console_handle_from_file_stream(_File_stream);
    DWORD _Console_mode;

    // If GetConsoleMode() returns a non-zero value, then _File_stream refers to a
    // console. We can then use the WriteConsoleW() API to write unicode text to
    // the console.
    const bool _Is_console = (GetConsoleMode(_Console_handle, &_Console_mode) != 0);

    return __std_unicode_console_detect_result{
        ._Is_unicode_console = _Is_console, ._Error = static_cast<__std_win_error>(GetLastError())};
}

[[nodiscard]] _Success_(
    return._Result_type == __std_unicode_console_print_result_type::_Success) __std_unicode_console_print_result
    __stdcall __std_print_to_unicode_console(_In_ const __std_file_stream_pointer _Stream, _In_ const char* const _Str,
        _In_ const unsigned long long _Str_size) noexcept {
    return _Print_to_unicode_console_impl<false>(_Stream, _Str, _Str_size);
}

[[nodiscard]] _Success_(
    return._Result_type == __std_unicode_console_print_result_type::_Success) __std_unicode_console_print_result
    __stdcall __std_println_to_unicode_console(_In_ const __std_file_stream_pointer _Stream,
        _In_ const char* const _Str, _In_ const unsigned long long _Str_size) noexcept {
    return _Print_to_unicode_console_impl<true>(_Stream, _Str, _Str_size);
}

_END_EXTERN_C