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
    enum class _Console_handle_retrieval_result_type {
        _Success                     = 0,
        _No_associated_output_stream = 1,
        _Invalid_handle_value        = 2,
        _Unknown_error               = 3
    };

    struct _Console_handle_from_file_stream_results {
        HANDLE _Console_handle;
        _Console_handle_retrieval_result_type _Result_type;
    };

    [[nodiscard]] inline _Console_handle_from_file_stream_results __stdcall _Get_console_handle_from_file_stream(
        FILE* const _File_stream) noexcept {
        const int _Fd = _fileno(_File_stream);

        switch (_Fd) {
        // _fileno() returns -2 if _File_stream refers to either stdout or stderr and
        // there is no associated output stream. In that case, there is also no
        // associated console HANDLE.
        case -2:
            return _Console_handle_from_file_stream_results{
                ._Result_type = _Console_handle_retrieval_result_type::_No_associated_output_stream};

        case -1:
            [[unlikely]] return _Console_handle_from_file_stream_results{
                ._Result_type = _Console_handle_retrieval_result_type::_Unknown_error};

        default:
            [[likely]] break;
        }

        const HANDLE _Console_handle = reinterpret_cast<HANDLE>(_get_osfhandle(_Fd));

        if (_Console_handle == INVALID_HANDLE_VALUE) [[unlikely]] {
            return _Console_handle_from_file_stream_results{
                ._Result_type = _Console_handle_retrieval_result_type::_Invalid_handle_value};
        }

        return _Console_handle_from_file_stream_results{._Console_handle = _Console_handle};
    }

    struct _String_to_wide_string_results {
        __crt_unique_heap_ptr<wchar_t> _Wide_str;
        size_t _Wide_str_size;
        __std_win_error _Error;
    };

    [[nodiscard]] _String_to_wide_string_results __stdcall _String_to_wide_string(
        const char* const _Str, const size_t _Str_size) noexcept {
        // MultiByteToWideChar() fails if strLength == 0.
        if (_Str_size == 0) [[unlikely]] {
            return _String_to_wide_string_results{._Wide_str = __crt_unique_heap_ptr<wchar_t>{nullptr}};
        }

        // The C++ specifications for vprint_unicode() suggest that we replace invalid
        // code units with U+FFFD. This is done automatically by MultiByteToWideChar(),
        // so long as we do not use the MB_ERR_INVALID_CHARS flag.
        const int32_t _Num_chars_required =
            MultiByteToWideChar(CP_UTF8, 0, _Str, static_cast<int>(_Str_size), nullptr, 0);

        if (_Num_chars_required == 0) [[unlikely]] {
            return _String_to_wide_string_results{._Wide_str = __crt_unique_heap_ptr<wchar_t>{nullptr},
                ._Error                                      = static_cast<__std_win_error>(GetLastError())};
        }

        __crt_unique_heap_ptr<wchar_t> _Wide_str{_malloc_crt_t(wchar_t, _Num_chars_required)};

        if (!_Wide_str) [[unlikely]] {
            return _String_to_wide_string_results{
                ._Wide_str = __crt_unique_heap_ptr<wchar_t>{nullptr}, ._Error = __std_win_error::_Not_enough_memory};
        }

        const int32_t _Num_chars_written = MultiByteToWideChar(
            CP_UTF8, 0, _Str, static_cast<int>(_Str_size), _Wide_str.get(), static_cast<int>(_Num_chars_required));

        if (_Num_chars_written == 0) [[unlikely]] {
            return _String_to_wide_string_results{._Wide_str = __crt_unique_heap_ptr<wchar_t>{nullptr},
                ._Error                                      = static_cast<__std_win_error>(GetLastError())};
        }

        return _String_to_wide_string_results{
            ._Wide_str = _STD move(_Wide_str), ._Wide_str_size = static_cast<size_t>(_Num_chars_required)};
    }

    [[nodiscard]] __std_unicode_console_print_result _Write_console(
        const HANDLE _Console_handle, const wchar_t* const _Wide_str, const size_t _Wide_str_size) noexcept {
        const BOOL _Write_result =
            WriteConsoleW(_Console_handle, _Wide_str, static_cast<DWORD>(_Wide_str_size), nullptr, nullptr);

        if (!_Write_result) [[unlikely]] {
            return __std_unicode_console_print_result{
                ._Result_type = __std_unicode_console_print_result_type::_Win_error,
                ._Win_error   = static_cast<__std_win_error>(GetLastError())};
        }

        return __std_unicode_console_print_result{};
    }
} // unnamed namespace

_EXTERN_C

[[nodiscard]] _Success_(return._Error == __std_win_error::_Success) __std_unicode_console_detect_result
    __stdcall __std_is_file_stream_unicode_console(_In_ const __std_file_stream_pointer _Stream) noexcept {
    if (_Stream == __std_file_stream_pointer::_Invalid) [[unlikely]] {
        return __std_unicode_console_detect_result{._Error = __std_win_error::_Invalid_parameter};
    }

    FILE* const _File_stream = reinterpret_cast<FILE*>(_Stream);

    const _Console_handle_from_file_stream_results _Console_handle_retrieval_results{
        _Get_console_handle_from_file_stream(_File_stream)};

    switch (_Console_handle_retrieval_results._Result_type) {
    case _Console_handle_retrieval_result_type::_Success:
        break;

    case _Console_handle_retrieval_result_type::_No_associated_output_stream:
        return __std_unicode_console_detect_result{._Error = __std_win_error::_File_not_found};

    default:
        [[unlikely]] return __std_unicode_console_detect_result{._Error = __std_win_error::_Invalid_parameter};
    }

    DWORD _Console_mode;

    // If GetConsoleMode() returns a non-zero value, then _File_stream refers to a
    // console. We can then use the WriteConsoleW() API to write unicode text to
    // the console.
    const bool _Is_console = (GetConsoleMode(_Console_handle_retrieval_results._Console_handle, &_Console_mode) != 0);

    return __std_unicode_console_detect_result{._Is_unicode_console = _Is_console};
}

[[nodiscard]] _Success_(
    return._Result_type == __std_unicode_console_print_result_type::_Success) __std_unicode_console_print_result
    __stdcall __std_print_to_unicode_console(_In_ const __std_file_stream_pointer _Stream, _In_ const char* const _Str,
        _In_ const unsigned long long _Str_size) noexcept {
    if (_Stream == __std_file_stream_pointer::_Invalid || _Str == nullptr) [[unlikely]] {
        return __std_unicode_console_print_result{._Result_type = __std_unicode_console_print_result_type::_Win_error,
            ._Win_error                                         = __std_win_error::_Invalid_parameter};
    }

    // WriteConsoleW() uses a DWORD to determine the number of characters to write.
    if (_Str_size > UINT32_MAX) [[unlikely]] {
        return __std_unicode_console_print_result{._Result_type = __std_unicode_console_print_result_type::_Win_error,
            ._Win_error                                         = __std_win_error::_Insufficient_buffer};
    }

    FILE* const _File_stream = reinterpret_cast<FILE*>(_Stream);

    // We always need to flush the stream before writing to the console. This is required by the C++
    // standard.
    const bool _Was_flush_successful = (_STD fflush(_File_stream) == 0);

    if (!_Was_flush_successful) [[unlikely]] {
        return __std_unicode_console_print_result{
            ._Result_type = __std_unicode_console_print_result_type::_Posix_error, ._Posix_error = errno};
    }

    const _Console_handle_from_file_stream_results _Console_handle_retrieval_results{
        _Get_console_handle_from_file_stream(_File_stream)};

    if (_Console_handle_retrieval_results._Result_type != _Console_handle_retrieval_result_type::_Success)
        [[unlikely]] {
        return __std_unicode_console_print_result{._Result_type = __std_unicode_console_print_result_type::_Win_error,
            ._Win_error                                         = __std_win_error::_Invalid_parameter};
    }

    // If the size of the string is set to zero, then we don't need to call _String_to_wide_string()
    // to do any heap allocations. In fact, we can just exit immediately after flushing the stream if
    // the string is empty and still comply with the C++ standard, thanks to the "as-if" rule.
    if (_Str_size == 0) [[unlikely]] {
        return __std_unicode_console_print_result{};
    }

    const _String_to_wide_string_results _Str_conversion_results{_String_to_wide_string(_Str, static_cast<size_t>(_Str_size))};

    if (_Str_conversion_results._Error != __std_win_error::_Success) [[unlikely]] {
        return __std_unicode_console_print_result{._Result_type = __std_unicode_console_print_result_type::_Win_error,
            ._Win_error                                         = _Str_conversion_results._Error};
    }

    return _Write_console(_Console_handle_retrieval_results._Console_handle, _Str_conversion_results._Wide_str.get(),
        _Str_conversion_results._Wide_str_size);
}

_END_EXTERN_C