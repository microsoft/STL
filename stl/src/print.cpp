// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// print.cpp -- C++23 <print> implementation

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

#include <__msvc_print.hpp>
#include <__msvc_unicode_iterators.hpp>
#include <cstdio>
#include <cstdlib>
#include <internal_shared.h>
#include <io.h>
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
        // _fileno() returns -2 if _Stream refers to either stdout or stderr and
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
    const bool _Is_unicode_console = GetConsoleMode(_Console_handle, &_Console_mode) != 0;

    if (!_Is_unicode_console) {
        return __std_unicode_console_retrieval_result{._Error = __std_win_error::_File_not_found};
    }

    return __std_unicode_console_retrieval_result{
        ._Console_handle = static_cast<__std_unicode_console_handle>(
            reinterpret_cast<_STD underlying_type_t<__std_unicode_console_handle>>(_Console_handle)),
        ._Error = __std_win_error::_Success};
}

_END_EXTERN_C

namespace {
    class _Allocated_string {
    public:
        _Allocated_string() = default;

        explicit _Allocated_string(__crt_unique_heap_ptr<wchar_t>&& _Other_str, const size_t _Other_capacity) noexcept
            : _Str(_STD move(_Other_str)), _Str_capacity(_Other_capacity) {}

        [[nodiscard]] wchar_t* _Data() const noexcept {
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
        size_t _Str_capacity = 0;
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

    constexpr size_t _Max_str_segment_size = 8192;

    [[nodiscard]] _Minimal_string_view __stdcall _Get_next_utf8_string_segment(
        const char* const _Str, const size_t _Str_size) noexcept {
        if (_Str_size <= _Max_str_segment_size) [[likely]] {
            return _Minimal_string_view{_Str, _Str_size};
        }

        // We want to find a pointer to the last valid code point _End_ptr such that the number of
        // bytes in [_Str, _End_ptr] is <= _Max_str_segment_size. However, we also want to make sure
        // that we end on a complete grapheme cluster to ensure that WriteConsoleW() can correctly
        // draw Unicode characters which consume more than one code point.
        _STD _Grapheme_break_property_iterator<char> _Grapheme_cluster_itr{_Str, _Str + _Str_size};
        size_t _Str_segment_size = 0;

        while (_Grapheme_cluster_itr != _STD default_sentinel) {
            ++_Grapheme_cluster_itr;
            const char* const _Next_cluster_ptr = _Grapheme_cluster_itr._Position();

            const _STD ptrdiff_t _Num_bytes_required = _Next_cluster_ptr - _Str;
            if (_Num_bytes_required > _Max_str_segment_size) {
                break;
            }

            _Str_segment_size = _Num_bytes_required;
        }

        // In the highly unlikely event that the entire string is one grapheme cluster larger than
        // _Max_str_segment_size, then we'll need to just write out that string all at once.
        if (_Str_segment_size == 0) [[unlikely]] {
            return _Minimal_string_view{_Str, _Str_size};
        }

        return _Minimal_string_view{_Str, _Str_segment_size};
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

    [[nodiscard]] _Transcode_result __stdcall _Transcode_utf8_string(
        _Allocated_string& _Dst_str, const _Minimal_string_view _Src_str) noexcept {
        // MultiByteToWideChar() fails if strLength == 0.
        if (_Src_str._Empty()) [[unlikely]] {
            return {};
        }

        // For vprint_unicode(), N4928 [ostream.formatted.print]/4 suggests replacing invalid code units with U+FFFD.
        // This is done automatically by MultiByteToWideChar(), so long as we do not use the MB_ERR_INVALID_CHARS flag.
        // We transcode up to 8,192 bytes per segment, which easily fits in an int.
        const int32_t _Num_chars_required =
            MultiByteToWideChar(CP_UTF8, 0, _Src_str._Data(), static_cast<int>(_Src_str._Size()), nullptr, 0);

        if (_Num_chars_required == 0) [[unlikely]] {
            return static_cast<__std_win_error>(GetLastError());
        }

        if (static_cast<size_t>(_Num_chars_required) > _Dst_str._Capacity()) {
            _Dst_str._Reset();

            __crt_unique_heap_ptr<wchar_t> _Wide_str{_malloc_crt_t(wchar_t, _Num_chars_required)};
            if (!_Wide_str) [[unlikely]] {
                return __std_win_error::_Not_enough_memory;
            }

            _Dst_str = _Allocated_string{_STD move(_Wide_str), static_cast<size_t>(_Num_chars_required)};
        }

        const int32_t _Conversion_result = MultiByteToWideChar(CP_UTF8, 0, _Src_str._Data(),
            static_cast<int>(_Src_str._Size()), _Dst_str._Data(), static_cast<int>(_Dst_str._Capacity()));

        if (_Conversion_result == 0) [[unlikely]] {
            // This shouldn't happen...
            _CSTD abort();
        }

        return _Minimal_wstring_view{_Dst_str._Data(), static_cast<size_t>(_Conversion_result)};
    }

    [[nodiscard]] __std_win_error _Write_console(
        const HANDLE _Console_handle, const _Minimal_wstring_view _Wide_str) noexcept {
        const BOOL _Write_result =
            WriteConsoleW(_Console_handle, _Wide_str._Data(), static_cast<DWORD>(_Wide_str._Size()), nullptr, nullptr);

        if (!_Write_result) [[unlikely]] {
            return static_cast<__std_win_error>(GetLastError());
        }

        return __std_win_error::_Success;
    }

    class _Critical_section {
    public:
        _Critical_section() noexcept {
            InitializeCriticalSection(&_Mtx);
        }

        ~_Critical_section() noexcept {
            DeleteCriticalSection(&_Mtx);
        }

        _Critical_section(const _Critical_section&)            = delete;
        _Critical_section& operator=(const _Critical_section&) = delete;

        void _Lock() noexcept {
            EnterCriticalSection(&_Mtx);
        }

        void _Unlock() noexcept {
            LeaveCriticalSection(&_Mtx);
        }

    private:
        CRITICAL_SECTION _Mtx;
    };

    class _Scoped_critical_section_lock {
    public:
        explicit _Scoped_critical_section_lock(_Critical_section& _Mtx) noexcept : _Mtx_ptr(&_Mtx) {
            _Mtx_ptr->_Lock();
        }

        ~_Scoped_critical_section_lock() noexcept {
            _Mtx_ptr->_Unlock();
        }

        _Scoped_critical_section_lock(const _Scoped_critical_section_lock&)            = delete;
        _Scoped_critical_section_lock& operator=(const _Scoped_critical_section_lock&) = delete;

    private:
        _Critical_section* _Mtx_ptr;
    };
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

    _Minimal_string_view _Curr_str_segment = _Get_next_utf8_string_segment(_Remaining_str, _Remaining_str_size);
    _Allocated_string _Allocated_str{};
    _Transcode_result _Transcoded_str{_Transcode_utf8_string(_Allocated_str, _Curr_str_segment)};

    if (!_Transcoded_str._Has_value()) [[unlikely]] {
        return _Transcoded_str._Error();
    }

    {
        // We acquire a lock here to prevent multiple threads from writing interleaved text,
        // since we only print segments of a string to the console at a time.
        static _Critical_section _Mtx{};
        const _Scoped_critical_section_lock _Lock{_Mtx};

        while (true) {
            const __std_win_error _Write_result = _Write_console(_Actual_console_handle, _Transcoded_str._Value());

            if (_Write_result != __std_win_error::_Success) [[unlikely]] {
                return _Write_result;
            }

            _Remaining_str_size -= _Curr_str_segment._Size();

            if (_Remaining_str_size == 0) {
                return __std_win_error::_Success;
            }

            _Remaining_str += _Curr_str_segment._Size();

            _Curr_str_segment = _Get_next_utf8_string_segment(_Remaining_str, _Remaining_str_size);
            _Transcoded_str   = _Transcode_utf8_string(_Allocated_str, _Curr_str_segment);

            if (!_Transcoded_str._Has_value()) [[unlikely]] {
                return _Transcoded_str._Error();
            }
        }
    }
}

_END_EXTERN_C
