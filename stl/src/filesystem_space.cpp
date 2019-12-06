// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

// TRANSITION, the code in this file should be moved back to filesystem.cpp
// when a Windows 10 SDK beyond version 1903 is available (see GH-322).

#include <internal_shared.h>
#include <limits.h>
#include <xfilesystem_abi.h>

#include <Windows.h>

namespace {
    static_assert(sizeof(uintmax_t) == sizeof(ULARGE_INTEGER) && alignof(uintmax_t) == alignof(ULARGE_INTEGER),
        "Size and alignment must match for reinterpret_cast<PULARGE_INTEGER>");

    [[nodiscard]] __std_win_error _Fs_space_attempt(wchar_t* const _Temp_buffer, const DWORD _Temp_buffer_characters,
        const wchar_t* const _Target, uintmax_t* const _Available, uintmax_t* const _Total_bytes,
        uintmax_t* const _Free_bytes) noexcept {
        if (GetVolumePathNameW(_Target, _Temp_buffer, _Temp_buffer_characters)) {
            if (GetDiskFreeSpaceExW(_Temp_buffer, reinterpret_cast<PULARGE_INTEGER>(_Available),
                    reinterpret_cast<PULARGE_INTEGER>(_Total_bytes), reinterpret_cast<PULARGE_INTEGER>(_Free_bytes))) {
                return __std_win_error::_Success;
            }
        }

        return __std_win_error{GetLastError()};
    }
} // unnamed namespace

_EXTERN_C

    [[nodiscard]] __std_win_error __stdcall __std_fs_space(const wchar_t* const _Target, uintmax_t* const _Available,
        uintmax_t* const _Total_bytes, uintmax_t* const _Free_bytes) noexcept {
    // get capacity information for the volume on which the file _Target resides
    __std_win_error _Last_error;
    if (GetFileAttributesW(_Target) == INVALID_FILE_ATTRIBUTES) {
        _Last_error = __std_win_error{GetLastError()};
    } else {
        {
            constexpr DWORD _Static_size = MAX_PATH;
            wchar_t _Temp_buf[_Static_size];
            _Last_error = _Fs_space_attempt(_Temp_buf, _Static_size, _Target, _Available, _Total_bytes, _Free_bytes);
            if (_Last_error == __std_win_error::_Success) {
                return __std_win_error::_Success;
            }
        }

        if (_Last_error == __std_win_error::_Filename_exceeds_range) {
            constexpr DWORD _Dynamic_size = USHRT_MAX + 1; // assuming maximum NT path fits in a UNICODE_STRING
            const auto _Temp_buf          = _malloc_crt_t(wchar_t, _Dynamic_size);
            if (_Temp_buf) {
                _Last_error =
                    _Fs_space_attempt(_Temp_buf.get(), _Dynamic_size, _Target, _Available, _Total_bytes, _Free_bytes);
                if (_Last_error == __std_win_error::_Success) {
                    return __std_win_error::_Success;
                }
            } else {
                _Last_error = __std_win_error::_Not_enough_memory;
            }
        }
    }

    *_Available   = ~0ull;
    *_Total_bytes = ~0ull;
    *_Free_bytes  = ~0ull;
    return _Last_error;
}
_END_EXTERN_C
