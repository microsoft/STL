// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// print.cpp -- C++23 <print> implementation

#include <__msvc_print.hpp>
#include <cstdio>
#include <expected>
#include <internal_shared.h>
#include <io.h>
#include <limits>
#include <mutex>
#include <stdexcept>
#include <string_view>
#include <type_traits>

#include <Windows.h>

_EXTERN_C

[[nodiscard]] _Success_(return._Error == __std_win_error::_Success) __std_unicode_console_retrieval_result
    __stdcall __std_get_unicode_console_handle_from_file_stream(_In_ FILE* const _Stream) noexcept {
    if (_Stream == nullptr) [[unlikely]] {
        return __std_unicode_console_retrieval_result{._Error = __std_win_error::_Invalid_parameter};
    }

    const int _Fd = _fileno(_Stream);

    if (_Fd == -2) {
        // _fileno() returns -2 if _File_stream refers to either stdout or stderr and
        // there is no associated output stream. In that case, there is also no associated
        // console HANDLE. (This might happen, for instance, if a Win32 GUI application is
        // being compiled with /SUBSYSTEM:WINDOWS.)
        return __std_unicode_console_retrieval_result{._Error = __std_win_error::_Not_supported};
    } else if (_Fd == -1) [[unlikely]] {
        return __std_unicode_console_retrieval_result{._Error = __std_win_error::_Invalid_parameter};
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

_END_EXTERN_C

namespace {
    // NOTE: We don't use std::wstring in here. Otherwise, compiling with /MD and
    // _ITERATOR_DEBUG_LEVEL > 0 will lead to bad code generation. This happens because
    // sizeof(std::_String_val) changes based on whether or not _ITERATOR_DEBUG_LEVEL > 0.
    //
    // (This is an issue because these functions are packaged into the msvcprt.lib static
    // import library, and we don't create different versions of this library based on the
    // _ITERATOR_DEBUG_LEVEL value, unlike with libcpmt.lib. So, even if we use /MD, the
    // functions are going to be statically defined within dependent projects.)
    class _Allocated_string {
    public:
        _Allocated_string() : _Str(nullptr), _Str_capacity(0) {}

        explicit _Allocated_string(__crt_unique_heap_ptr<wchar_t>&& _Other_str, size_t _Other_capacity)
            : _Str(_STD move(_Other_str)), _Str_capacity(_Other_capacity) {}

        [[nodiscard]] wchar_t* _Data() noexcept {
            return _Str.get();
        }

        [[nodiscard]] size_t _Capacity() const noexcept {
            return _Str_capacity;
        }

        void _Reset() noexcept {
            _Str.release();
            _Str_capacity = 0;
        }

    private:
        __crt_unique_heap_ptr<wchar_t> _Str;
        size_t _Str_capacity;
    };

    static constexpr size_t _Max_str_segment_size = 8192;

    [[nodiscard]] _STD string_view __stdcall _Get_next_utf8_string_segment(
        const char* const _Str, const size_t _Str_size) noexcept {
        if (_Str_size <= _Max_str_segment_size) [[likely]] {
            return _STD string_view{_Str, _Str_size};
        }

        // We want to find a pointer to the last valid code point _End_ptr such that the number of
        // bytes in [_Str, _End_ptr] is <= _Max_str_segment_size.
        static constexpr size_t _Base_ptr_adjustment = _Max_str_segment_size - 1;
        const char* const _End_ptr                   = _Str + _Base_ptr_adjustment;

        // clang-format off
        const bool _Include_end_byte = 
            ((*_End_ptr & 0b1000'0000) == 0) ||   // 1-Byte Code Point
            ((*(_End_ptr - 1) >> 5) == 0b110) ||  // 2nd Byte in 2-Byte Code Point
            ((*(_End_ptr - 2) >> 4) == 0b1110) || // 3rd Byte in 3-Byte Code Point
            ((*(_End_ptr - 3) >> 3) == 0b1111'0); // 4th Byte in 4-Byte Code Point
        // clang-format on

        if (_Include_end_byte) {
            return _STD string_view{_Str, _Max_str_segment_size};
        }

        // If _End_ptr doesn't point to the end of a code point, then we return the segment ending
        // with the last byte of the previous code point. It is possible, however, that we aren't
        // dealing with a valid code point in the first place. So, since UTF-8 code points consist
        // of at most four bytes, we only check the preceding four bytes.
        for (size_t _Offset = 1; _Offset < 5; ++_Offset) {
            const char* const _Curr_end_ptr = _End_ptr - _Offset;

            // If _Curr_end_ptr points to the sole byte in a 1-byte code point, then end the
            // segment on that byte.
            if ((*_Curr_end_ptr & 0b1000'0000) == 0) {
                return _STD string_view{_Str, _Max_str_segment_size - _Offset};
            }

            // Otherwise, if _Curr_end_ptr points to the beginning of a multi-byte code point,
            // then end the segment on the byte just before this one.
            if ((*_Curr_end_ptr & 0b1100'0000) == 0b1100'0000) {
                return _STD string_view{_Str, _Max_str_segment_size - (_Offset + 1)};
            }
        }

        // If that failed, then the string definitely ends in an invalid code point. In that case,
        // we just return the segment containing it, since MultiByteToWideChar() will end up
        // replacing it with U+FFFD, anyways.
        return _STD string_view{_Str, _Max_str_segment_size};
    }

    [[nodiscard]] _STD expected<_STD wstring_view, __std_win_error> __stdcall _Transcode_utf8_string(
        _Allocated_string& _Dst_str, const _STD string_view _Src_str) noexcept {
        // MultiByteToWideChar() fails if strLength == 0.
        if (_Src_str.empty()) [[unlikely]] {
            return {};
        }

        // The C++ specifications for vprint_unicode() suggest that we replace invalid
        // code units with U+FFFD. This is done automatically by MultiByteToWideChar(),
        // so long as we do not use the MB_ERR_INVALID_CHARS flag.
        const int32_t _Num_chars_required =
            MultiByteToWideChar(CP_UTF8, 0, _Src_str.data(), static_cast<int>(_Src_str.size()), nullptr, 0);

        if (_Num_chars_required == 0) [[unlikely]] {
            return _STD unexpected{static_cast<__std_win_error>(GetLastError())};
        }

        if (static_cast<size_t>(_Num_chars_required) > _Dst_str._Capacity()) {
            _Dst_str._Reset();

            __crt_unique_heap_ptr<wchar_t> _Wide_str{_malloc_crt_t(wchar_t, _Num_chars_required)};
            if (!_Wide_str) [[unlikely]] {
                return _STD unexpected{__std_win_error::_Not_enough_memory};
            }

            _Dst_str = _Allocated_string{_STD move(_Wide_str), static_cast<size_t>(_Num_chars_required)};
        }

        const int32_t _Conversion_result = MultiByteToWideChar(
            CP_UTF8, 0, _Src_str.data(), static_cast<int>(_Src_str.size()), _Dst_str._Data(), _Dst_str._Capacity());

        if (_Conversion_result == 0) [[unlikely]] {
            return _STD unexpected{static_cast<__std_win_error>(GetLastError())};
        }

        return _STD wstring_view{_Dst_str._Data(), _Dst_str._Capacity()};
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
        _In_ const char* const _Str, _In_ const size_t _Str_size) noexcept {
    if (_Console_handle == __std_unicode_console_handle::_Invalid || _Str == nullptr) [[unlikely]] {
        return __std_win_error::_Invalid_parameter;
    }

    const HANDLE _Actual_console_handle = reinterpret_cast<HANDLE>(_Console_handle);

    // We can do the first transcode operation without acquiring the lock. This should also
    // handle the vast majority of strings, since we transcode in fairly large segments of
    // 8,192 bytes per segment.
    const char* _Remaining_str = _Str;
    size_t _Remaining_str_size = _Str_size;

    _STD string_view _Curr_str_segment = _Get_next_utf8_string_segment(_Remaining_str, _Remaining_str_size);
    _Allocated_string _Allocated_str{};
    _STD expected<_STD wstring_view, __std_win_error> _Transcoded_str{
        _Transcode_utf8_string(_Allocated_str, _Curr_str_segment)};

    if (!_Transcoded_str.has_value()) [[unlikely]] {
        return _Transcoded_str.error();
    }

    {
        static _STD mutex _Mtx{};
        const _STD scoped_lock _Lock{_Mtx};

        while (true) {
            const __std_win_error _Write_result = _Write_console(_Actual_console_handle, *_Transcoded_str);

            if (_Write_result != __std_win_error::_Success) [[unlikely]] {
                return _Write_result;
            }

            _Remaining_str_size -= _Curr_str_segment.size();

            if (_Remaining_str_size == 0) {
                return __std_win_error::_Success;
            }

            _Remaining_str += _Curr_str_segment.size();

            _Curr_str_segment = _Get_next_utf8_string_segment(_Remaining_str, _Remaining_str_size);
            _Transcoded_str   = _Transcode_utf8_string(_Allocated_str, _Curr_str_segment);

            if (!_Transcoded_str.has_value()) [[unlikely]] {
                return _Transcoded_str.error();
            }
        }
    }
}

_END_EXTERN_C
