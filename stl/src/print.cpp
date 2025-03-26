// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// print.cpp -- C++23 <print> implementation

#include <__msvc_print.hpp>
#include <cstdio>
#include <cstdlib>
#include <internal_shared.h>
#include <io.h>
#include <type_traits>

#include <Windows.h>

extern "C" {

[[nodiscard]] _Success_(return._Error == __std_win_error::_Success) __std_unicode_console_retrieval_result
    __stdcall __std_get_unicode_console_handle_from_file_stream(_In_ FILE* const _Stream) noexcept {
    if (_Stream == nullptr) {
        return __std_unicode_console_retrieval_result{._Error = __std_win_error::_Invalid_parameter};
    }

    const int _Fd = _fileno(_Stream);

    if (_Fd == -2) {
        // According to https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fileno?view=msvc-170 ,
        // _fileno() returns -2 if _Stream refers to either stdout or stderr and there is no associated output stream.
        // In that case, there is also no associated console HANDLE. (We haven't observed this happening in practice.)
        return __std_unicode_console_retrieval_result{._Error = __std_win_error::_Not_supported};
    } else if (_Fd == -1) {
        return __std_unicode_console_retrieval_result{._Error = __std_win_error::_Invalid_parameter};
    }

    const HANDLE _Console_handle = reinterpret_cast<HANDLE>(_get_osfhandle(_Fd));

    if (_Console_handle == INVALID_HANDLE_VALUE) {
        return __std_unicode_console_retrieval_result{._Error = __std_win_error::_Invalid_parameter};
    }

    // We can check if _Console_handle actually refers to a console or not by checking the
    // return value of GetConsoleMode().
    DWORD _Console_mode;
    const bool _Is_unicode_console = GetConsoleMode(_Console_handle, &_Console_mode) != 0;

    if (!_Is_unicode_console) {
        return __std_unicode_console_retrieval_result{._Error = __std_win_error::_File_not_found};
    }

    return __std_unicode_console_retrieval_result{
        ._Console_handle = static_cast<__std_unicode_console_handle>(
            reinterpret_cast<_STD underlying_type_t<__std_unicode_console_handle>>(_Console_handle)),
        ._Error = __std_win_error::_Success};
}

} // extern "C"

namespace {
    class _Allocated_string {
    public:
        _Allocated_string() noexcept {
            _Buffer[0] = L'\0'; // Activate _Buffer
        }

        ~_Allocated_string() {
            if (_Using_heap()) {
                _Str.~_Heap_string();
            }
        }

        [[nodiscard]] wchar_t* _Data() noexcept {
            return _Using_heap() ? _Str.get() : _Buffer;
        }

        [[nodiscard]] size_t _Capacity() const noexcept {
            return _Str_capacity;
        }

        [[nodiscard]] bool _Grow(const size_t _Capacity) noexcept {
            if (_Capacity <= _Str_capacity) {
                return true;
            }

            if (_Using_heap()) {
                _Str.~_Heap_string();
                // We must not throw until restoring the invariant that:
                // (_Str_capacity == _Buffer_size && _Buffer is active) ||
                // (_Str_capacity > _Buffer_size && _Str is active)
            }

            ::new (&_Str) _Heap_string(_malloc_crt_t(wchar_t, _Capacity)); // Activate _Str

            if (!_Str) {
                _Str_capacity = _Buffer_size;
                _Buffer[0]    = L'\0'; // Activate _Buffer
                return false;
            }

            _Str_capacity = _Capacity;
            return true;
        }

    private:
        using _Heap_string = __crt_unique_heap_ptr<wchar_t>;

        // Allows small formatted strings, such as those from _Print_to_unicode_console_it, to not allocate any extra
        // internal transcoding buffer
        static constexpr size_t _Buffer_size = 2048;

        [[nodiscard]] bool _Using_heap() const noexcept {
            return _Str_capacity > _Buffer_size;
        }

        size_t _Str_capacity = _Buffer_size;
        union {
            wchar_t _Buffer[_Buffer_size];
            _Heap_string _Str;
        };
    };

    template <class _Char_type>
    class _Really_basic_string_view {
    public:
        _Really_basic_string_view() = default;

        explicit _Really_basic_string_view(const _Char_type* const _Other_str, const size_t _Other_size) noexcept
            : _Str(_Other_str), _Str_size(_Other_size) {}

        [[nodiscard]] const _Char_type* _Data() const noexcept {
            return _Str;
        }

        [[nodiscard]] size_t _Size() const noexcept {
            return _Str_size;
        }

        [[nodiscard]] bool _Empty() const noexcept {
            return _Str_size == 0;
        }

    private:
        const _Char_type* _Str = nullptr;
        size_t _Str_size       = 0;
    };

    using _Minimal_string_view  = _Really_basic_string_view<char>;
    using _Minimal_wstring_view = _Really_basic_string_view<wchar_t>;

    [[nodiscard]] _Minimal_string_view _Get_next_utf8_string_segment(
        const char* const _Str, const size_t _Str_size) noexcept {
        constexpr size_t _Max_str_segment_size = 8192;

        if (_Str_size <= _Max_str_segment_size) {
            return _Minimal_string_view{_Str, _Str_size};
        }

        // Let's refer to _Max_str_segment_size as M.
        // Now we know _Str_size > M, so we can read _Str[M].
        // We might need to shrink this segment down to M - 3 bytes, in this worst case scenario:

        //  Values:        [byte1][byte2][byte3] | [byte4]
        // Indices: [M - 4][M - 3][M - 2][M - 1] | [  M  ]
        //   Sizes: [M - 3][M - 2][M - 1][  M  ] | [M + 1]
        //              Maximum segment boundary ^

        for (size_t _Shrink = 0; _Shrink < 3; ++_Shrink) {
            const size_t _Kx = _Max_str_segment_size - _Shrink; // consider a segment of _Kx bytes

            // The first byte after the segment is at index _Kx, which we can read (see above).
            // If _Str[_Kx] is a non-trailing byte, then it's the beginning of a code point.
            const bool _Trailing_byte = (static_cast<unsigned char>(_Str[_Kx]) >> 6) == 0b10;

            if (!_Trailing_byte) {
                return _Minimal_string_view{_Str, _Kx}; // found a boundary between code points
            }
        }

        return _Minimal_string_view{_Str, _Max_str_segment_size - 3}; // worst case scenario
    }

    class _Transcode_result {
    public:
        _Transcode_result() noexcept : _Transcoded_str(), _Successful(true) {}

        _Transcode_result(_Minimal_wstring_view _Result_str) noexcept
            : _Transcoded_str(_Result_str), _Successful(true) {}

        _Transcode_result(__std_win_error _Result_error) noexcept : _Win_error(_Result_error), _Successful(false) {}

        [[nodiscard]] bool _Has_value() const noexcept {
            return _Successful;
        }

        [[nodiscard]] _Minimal_wstring_view _Value() const noexcept {
            return _Transcoded_str;
        }

        [[nodiscard]] __std_win_error _Error() const noexcept {
            return _Win_error;
        }

    private:
        union {
            _Minimal_wstring_view _Transcoded_str;
            __std_win_error _Win_error;
        };

        bool _Successful;
    };

    [[nodiscard]] _Transcode_result _Transcode_utf8_string(
        _Allocated_string& _Dst_str, const _Minimal_string_view _Src_str) noexcept {
        // MultiByteToWideChar() fails if strLength == 0.
        if (_Src_str._Empty()) {
            return {};
        }

        // For vprint_unicode(), N4950 [ostream.formatted.print]/4 suggests replacing invalid code units with U+FFFD.
        // This is done automatically by MultiByteToWideChar(), so long as we do not use the MB_ERR_INVALID_CHARS flag.
        // We transcode up to 8,192 bytes per segment, which easily fits in an int.
        const int32_t _Num_chars_required =
            MultiByteToWideChar(CP_UTF8, 0, _Src_str._Data(), static_cast<int>(_Src_str._Size()), nullptr, 0);

        if (_Num_chars_required == 0) {
            return static_cast<__std_win_error>(GetLastError());
        }

        const bool _Has_space = _Dst_str._Grow(static_cast<size_t>(_Num_chars_required));
        if (!_Has_space) {
            return __std_win_error::_Not_enough_memory;
        }

        const int32_t _Conversion_result = MultiByteToWideChar(CP_UTF8, 0, _Src_str._Data(),
            static_cast<int>(_Src_str._Size()), _Dst_str._Data(), static_cast<int>(_Dst_str._Capacity()));

        if (_Conversion_result == 0) {
            // This shouldn't happen...
            _CSTD abort();
        }

        return _Minimal_wstring_view{_Dst_str._Data(), static_cast<size_t>(_Conversion_result)};
    }

    [[nodiscard]] __std_win_error _Write_console(
        const HANDLE _Console_handle, const _Minimal_wstring_view _Wide_str) noexcept {
        const BOOL _Write_result =
            WriteConsoleW(_Console_handle, _Wide_str._Data(), static_cast<DWORD>(_Wide_str._Size()), nullptr, nullptr);

        if (!_Write_result) {
            return static_cast<__std_win_error>(GetLastError());
        }

        return __std_win_error::_Success;
    }
} // unnamed namespace

extern "C" {

[[nodiscard]] _Success_(return == __std_win_error::_Success) __std_win_error
    __stdcall __std_print_to_unicode_console(_In_ const __std_unicode_console_handle _Console_handle,
        _In_reads_(_Str_size) const char* const _Str, _In_ const size_t _Str_size) noexcept {
    if (_Console_handle == __std_unicode_console_handle::_Invalid || _Str == nullptr) {
        return __std_win_error::_Invalid_parameter;
    }

    const HANDLE _Actual_console_handle = reinterpret_cast<HANDLE>(_Console_handle);

    // We transcode in fairly large segments of 8,192 bytes per segment,
    // so one iteration should handle the vast majority of strings.
    const char* _Remaining_str = _Str;
    size_t _Remaining_str_size = _Str_size;

    _Minimal_string_view _Curr_str_segment{};
    _Allocated_string _Allocated_str{};
    _Transcode_result _Transcoded_str{};

    for (;;) {
        _Curr_str_segment = _Get_next_utf8_string_segment(_Remaining_str, _Remaining_str_size);
        _Transcoded_str   = _Transcode_utf8_string(_Allocated_str, _Curr_str_segment);

        if (!_Transcoded_str._Has_value()) {
            return _Transcoded_str._Error();
        }

        const __std_win_error _Write_result = _Write_console(_Actual_console_handle, _Transcoded_str._Value());

        if (_Write_result != __std_win_error::_Success) {
            return _Write_result;
        }

        _Remaining_str_size -= _Curr_str_segment._Size();

        if (_Remaining_str_size == 0) {
            return __std_win_error::_Success;
        }

        _Remaining_str += _Curr_str_segment._Size();
    }
}

[[nodiscard]] _Success_(return == __std_win_error::_Success) __std_win_error
    __stdcall __std_print_newline_only_to_unicode_console(
        _In_ const __std_unicode_console_handle _Console_handle) noexcept {
    if (_Console_handle == __std_unicode_console_handle::_Invalid) {
        return __std_win_error::_Invalid_parameter;
    }

    const auto _Actual_console_handle = reinterpret_cast<HANDLE>(_Console_handle);

    const BOOL _Write_result = WriteConsoleW(_Actual_console_handle, L"\n", 1, nullptr, nullptr);

    if (!_Write_result) {
        return static_cast<__std_win_error>(GetLastError());
    }

    return __std_win_error::_Success;
}

} // extern "C"
