// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// filesystem.cpp -- C++17 <filesystem> implementation
// (see filesys.cpp for <experimental/filesystem> implementation)

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

#include <corecrt_terminate.h>
#include <internal_shared.h>
#include <limits.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <xfilesystem_abi.h>

#include <Windows.h>

#include <WinIoCtl.h>

// We have several switches that do not have case statements for every possible enum value.
// Hence, disabling this warning.
#pragma warning(disable : 4061) // enumerator '__std_win_error::_Success' in switch of enum
                                // '__std_win_error' is not explicitly handled by a case label

static_assert(__std_code_page::_Utf8 == __std_code_page{CP_UTF8});

namespace {

#if !defined(_CRT_APP)
    template <class _Fn_ptr>
    [[nodiscard]] _Fn_ptr __stdcall _Runtime_dynamic_link(volatile _STD _Identity_t<_Fn_ptr>* const _Cache,
        const wchar_t* const _Module, const char* const _Fn_name, const _Fn_ptr _Fallback) noexcept {
        auto _Result = __crt_interlocked_read_pointer(_Cache);
        if (_Result) {
            return _Result;
        }

        const HMODULE _HMod = GetModuleHandleW(_Module);
        if (_HMod) {
            _Result = reinterpret_cast<_Fn_ptr>(GetProcAddress(_HMod, _Fn_name));
        }

        if (!_Result) {
            _Result = _Fallback;
        }

        __crt_interlocked_exchange_pointer(_Cache, _Result);
        return _Result;
    }
#endif // !defined(_CRT_APP)

    // MACRO __vcrt_GetFinalPathNameByHandleW
#if _STL_WIN32_WINNT < _WIN32_WINNT_VISTA
    using _GetFinalPathNameByHandleW_t = decltype(&GetFinalPathNameByHandleW);
    _GetFinalPathNameByHandleW_t GetFinalPathNameByHandleW_p{};

    unsigned long __stdcall _Not_supported_GetFinalPathNameByHandleW(HANDLE, wchar_t*, unsigned long, unsigned long) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }

#define __vcrt_GetFinalPathNameByHandleW                                                               \
    (_Runtime_dynamic_link(&GetFinalPathNameByHandleW_p, L"kernel32.dll", "GetFinalPathNameByHandleW", \
        &_Not_supported_GetFinalPathNameByHandleW))
#else // ^^^ _STL_WIN32_WINNT < _WIN32_WINNT_VISTA ^^^ // vvv _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA vvv
#define __vcrt_GetFinalPathNameByHandleW GetFinalPathNameByHandleW
#endif // _STL_WIN32_WINNT < _WIN32_WINNT_VISTA

    // MACRO __vcrt_GetFileInformationByHandleEx
#if _STL_WIN32_WINNT < _WIN32_WINNT_VISTA
    using _GetFileInformationByHandleEx_t = decltype(&GetFileInformationByHandleEx);
    _GetFileInformationByHandleEx_t _GetFileInformationByHandleEx_p{};

    BOOL __stdcall _Not_supported_GetFileInformationByHandleEx(
        HANDLE, FILE_INFO_BY_HANDLE_CLASS, void*, unsigned long) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }

#define __vcrt_GetFileInformationByHandleEx                                                                   \
    (_Runtime_dynamic_link(&_GetFileInformationByHandleEx_p, L"kernel32.dll", "GetFileInformationByHandleEx", \
        &_Not_supported_GetFileInformationByHandleEx))

    struct _GetFileInfoByHandleEx {
        const _GetFileInformationByHandleEx_t _Fn = __vcrt_GetFileInformationByHandleEx;

        [[nodiscard]] bool _Supported() const noexcept {
            return _Fn != _Not_supported_GetFileInformationByHandleEx;
        }

        [[nodiscard]] auto operator()(const HANDLE _Handle, const FILE_INFO_BY_HANDLE_CLASS _Class, void* const _Info,
            const unsigned long _Size) const noexcept {
            return _Fn(_Handle, _Class, _Info, _Size);
        }
    };
#else // ^^^ _STL_WIN32_WINNT < _WIN32_WINNT_VISTA ^^^ // vvv _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA vvv
#define __vcrt_GetFileInformationByHandleEx GetFileInformationByHandleEx

    struct _GetFileInfoByHandleEx {
        [[nodiscard]] bool _Supported() const noexcept {
            return true;
        }

        [[nodiscard]] auto operator()(const HANDLE _Handle, const FILE_INFO_BY_HANDLE_CLASS _Class, void* const _Info,
            const unsigned long _Size) const noexcept {
            return GetFileInformationByHandleEx(_Handle, _Class, _Info, _Size);
        }
    };
#endif // _STL_WIN32_WINNT < _WIN32_WINNT_VISTA

    // MACRO __vcrt_SetFileInformationByHandle
#if _STL_WIN32_WINNT < _WIN32_WINNT_VISTA
    using _SetFileInformationByHandle_t = decltype(&SetFileInformationByHandle);
    _SetFileInformationByHandle_t _SetFileInformationByHandle_p{};

    BOOL __stdcall _Not_supported_SetFileInformationByHandle(HANDLE, FILE_INFO_BY_HANDLE_CLASS, void*, unsigned long) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }

#define __vcrt_SetFileInformationByHandle                                                                 \
    (_Runtime_dynamic_link(&_SetFileInformationByHandle_p, L"kernel32.dll", "SetFileInformationByHandle", \
        &_Not_supported_SetFileInformationByHandle))
#define _STL_ALWAYS_HAS_SetFileInformationByHandle 0
#else // ^^^ _STL_WIN32_WINNT < _WIN32_WINNT_VISTA ^^^ // vvv _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA vvv
#define __vcrt_SetFileInformationByHandle SetFileInformationByHandle
#define _STL_ALWAYS_HAS_SetFileInformationByHandle 1
#endif // _STL_WIN32_WINNT < _WIN32_WINNT_VISTA

    // MACRO __vcrt_CreateSymbolicLinkW
#if defined(_CRT_APP) || _STL_WIN32_WINNT < _WIN32_WINNT_VISTA
    BOOLEAN __stdcall _Not_supported_CreateSymbolicLinkW(const wchar_t*, const wchar_t*, DWORD) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
#endif // defined(_CRT_APP) || _STL_WIN32_WINNT < _WIN32_WINNT_VISTA

#if defined(_CRT_APP)
#define __vcrt_CreateSymbolicLinkW _Not_supported_CreateSymbolicLinkW
#elif _STL_WIN32_WINNT < _WIN32_WINNT_VISTA
    using _CreateSymbolicLinkW_t = decltype(&CreateSymbolicLinkW);
    _CreateSymbolicLinkW_t _CreateSymbolicLinkW_p{};

#define __vcrt_CreateSymbolicLinkW \
    (_Runtime_dynamic_link(        \
        &_CreateSymbolicLinkW_p, L"kernel32.dll", "CreateSymbolicLinkW", &_Not_supported_CreateSymbolicLinkW))
#else // !defined(_CRT_APP) && _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA
#define __vcrt_CreateSymbolicLinkW CreateSymbolicLinkW
#endif // platform detection for CreateSymbolicLinkW

    // FUNCTION / MACRO __vcp_CreateFile
#ifdef _CRT_APP
    HANDLE __stdcall __vcp_CreateFile(const wchar_t* const _File_name, const unsigned long _Desired_access,
        const unsigned long _Share, SECURITY_ATTRIBUTES* const _Security_attributes,
        const unsigned long _Creation_disposition, const unsigned long _Flags_and_attributes,
        const HANDLE _Template_file) noexcept {
        CREATEFILE2_EXTENDED_PARAMETERS _Create_file_parameters{};
        _Create_file_parameters.dwSize               = sizeof(_Create_file_parameters);
        _Create_file_parameters.dwFileAttributes     = _Flags_and_attributes & 0x0000FFFFU;
        _Create_file_parameters.dwFileFlags          = _Flags_and_attributes & 0xFFFF0000U;
        _Create_file_parameters.lpSecurityAttributes = _Security_attributes;
        _Create_file_parameters.hTemplateFile        = _Template_file;
        return CreateFile2(_File_name, _Desired_access, _Share, _Creation_disposition, &_Create_file_parameters);
    }
#else // ^^^ _CRT_APP ^^^ // vvv !_CRT_APP vvv
#define __vcp_CreateFile CreateFileW
#endif // _CRT_APP

    // FUNCTION _Translate_CreateFile_last_error
    [[nodiscard]] __std_win_error __stdcall _Translate_CreateFile_last_error(const HANDLE _Handle) {
        if (_Handle != INVALID_HANDLE_VALUE) {
            return __std_win_error::_Success;
        }

        return __std_win_error{GetLastError()};
    }

    // FUNCTION __vcp_CopyFile
    [[nodiscard]] __std_fs_copy_file_result __stdcall __vcp_Copyfile(
        const wchar_t* const _Source, const wchar_t* const _Target, const bool _Fail_if_exists) noexcept {
#if defined(_CRT_APP)
        COPYFILE2_EXTENDED_PARAMETERS _Params{};
        _Params.dwSize      = sizeof(_Params);
        _Params.dwCopyFlags = _Fail_if_exists ? COPY_FILE_FAIL_IF_EXISTS : 0;

        const HRESULT _Copy_result = CopyFile2(_Source, _Target, &_Params);
        if (SUCCEEDED(_Copy_result)) {
            return {true, __std_win_error::_Success};
        }

        // take lower bits to undo HRESULT_FROM_WIN32
        return {false, __std_win_error{_Copy_result & 0x0000FFFFU}};
#else // ^^^ defined(_CRT_APP) ^^^ // vvv !defined(_CRT_APP) vvv
        if (CopyFileW(_Source, _Target, _Fail_if_exists)) {
            return {true, __std_win_error::_Success};
        }

        return {false, __std_win_error{GetLastError()}};
#endif // defined(_CRT_APP)
    }

    [[nodiscard]] __std_win_error __stdcall _Create_symlink(
        const wchar_t* const _Symlink_file_name, const wchar_t* const _Target_file_name, const DWORD _Flags) noexcept {
        if (__vcrt_CreateSymbolicLinkW(
                _Symlink_file_name, _Target_file_name, _Flags | 0x2 /* SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE */)
            != 0) {
            return __std_win_error::_Success;
        }

        DWORD _Last_error = GetLastError();
        if (_Last_error == ERROR_INVALID_PARAMETER) {
            if (__vcrt_CreateSymbolicLinkW(_Symlink_file_name, _Target_file_name, _Flags) != 0) {
                return __std_win_error::_Success;
            }

            _Last_error = GetLastError();
        }

        return __std_win_error{_Last_error};
    }

    [[nodiscard]] __std_win_error __stdcall _Translate_not_found_to_success(const __std_win_error _Err) noexcept {
        switch (_Err) {
        case __std_win_error::_File_not_found:
        case __std_win_error::_Path_not_found:
            return __std_win_error::_Success;
        default:
            return _Err;
        }
    }

    [[nodiscard]] __std_win_error __stdcall _Get_last_write_time_by_handle(
        const HANDLE _Handle, long long* const _Last_write_filetime) {
        // read the last write time from _Handle and store it in _Last_write_filetime
        __std_win_error _Last_error;
        FILE_BASIC_INFO _Ex_info;
        if (__vcrt_GetFileInformationByHandleEx(_Handle, FileBasicInfo, &_Ex_info, sizeof(_Ex_info)) != 0) {
            _CSTD memcpy(_Last_write_filetime, &_Ex_info.LastWriteTime, sizeof(*_Last_write_filetime));
            return __std_win_error::_Success;
        }

        _Last_error = __std_win_error{GetLastError()};
#if _STL_WIN32_WINNT < _WIN32_WINNT_VISTA
        // note: "maybe Windows XP" test also excludes _CRT_APP
        if (_Last_error == __std_win_error::_Not_supported) {
            BY_HANDLE_FILE_INFORMATION _Legacy_info;
            if (GetFileInformationByHandle(_Handle, &_Legacy_info) != 0) {
                _CSTD memcpy(_Last_write_filetime, &_Legacy_info.ftLastWriteTime, sizeof(*_Last_write_filetime));
                return __std_win_error::_Success;
            }

            _Last_error = __std_win_error{GetLastError()};
        }
#endif // _STL_WIN32_WINNT < _WIN32_WINNT_VISTA

        return _Last_error;
    }

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

    [[nodiscard]] __std_ulong_and_error __stdcall __std_fs_get_full_path_name(
        const wchar_t* _Source, unsigned long _Target_size, wchar_t* _Target) noexcept { // calls GetFullPathNameW
    const auto _Result = GetFullPathNameW(_Source, _Target_size, _Target, nullptr);
    return {_Result, _Result == 0 ? __std_win_error{GetLastError()} : __std_win_error::_Success};
}

[[nodiscard]] __std_win_error __stdcall __std_fs_open_handle(__std_fs_file_handle* const _Handle,
    const wchar_t* const _File_name, const __std_access_rights _Desired_access,
    const __std_fs_file_flags _Flags) noexcept { // calls CreateFile2 or CreateFileW
    const HANDLE _Result = __vcp_CreateFile(_File_name, static_cast<unsigned long>(_Desired_access),
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING,
        static_cast<unsigned long>(_Flags), 0);
    *_Handle             = static_cast<__std_fs_file_handle>(reinterpret_cast<intptr_t>(_Result));
    return _Translate_CreateFile_last_error(_Result);
}

void __stdcall __std_fs_close_handle(const __std_fs_file_handle _Handle) noexcept { // calls CloseHandle
    if (_Handle != __std_fs_file_handle::_Invalid && CloseHandle(reinterpret_cast<HANDLE>(_Handle)) == 0) {
        terminate();
    }
}

[[nodiscard]] __std_win_error __stdcall __std_fs_get_file_attributes_by_handle(const __std_fs_file_handle _Handle,
    unsigned long* const
        _File_attributes) noexcept { // read the attributes from _Handle and store it in _File_attributes
    __std_win_error _Last_error;
    const HANDLE _As_plain_handle = reinterpret_cast<HANDLE>(_Handle);

    FILE_BASIC_INFO _Ex_info;
    if (__vcrt_GetFileInformationByHandleEx(_As_plain_handle, FileBasicInfo, &_Ex_info, sizeof(_Ex_info)) != 0) {
        *_File_attributes = _Ex_info.FileAttributes;
        return __std_win_error::_Success;
    }

    _Last_error = __std_win_error{GetLastError()};
#if _STL_WIN32_WINNT < _WIN32_WINNT_VISTA
    // note: "maybe Windows XP" test also excludes _CRT_APP
    if (_Last_error == __std_win_error::_Not_supported) {
        BY_HANDLE_FILE_INFORMATION _Legacy_info;
        if (GetFileInformationByHandle(_As_plain_handle, &_Legacy_info) != 0) {
            *_File_attributes = _Legacy_info.dwFileAttributes;
            return __std_win_error::_Success;
        }

        _Last_error = __std_win_error{GetLastError()};
    }
#endif // _STL_WIN32_WINNT < _WIN32_WINNT_VISTA

    return _Last_error;
}

[[nodiscard]] __std_ulong_and_error __stdcall __std_fs_get_final_path_name_by_handle(const __std_fs_file_handle _Handle,
    wchar_t* const _Target, const unsigned long _Target_size,
    const __std_fs_volume_name_kind _Flags) noexcept { // calls GetFinalPathNameByHandleW
    const auto _Result = __vcrt_GetFinalPathNameByHandleW(
        reinterpret_cast<HANDLE>(_Handle), _Target, _Target_size, static_cast<unsigned long>(_Flags));
    return {_Result, _Result == 0 ? __std_win_error{GetLastError()} : __std_win_error::_Success};
}

static_assert(sizeof(WIN32_FIND_DATAW) == sizeof(__std_fs_find_data));
static_assert(alignof(WIN32_FIND_DATAW) == alignof(__std_fs_find_data));

[[nodiscard]] __std_win_error __stdcall __std_fs_directory_iterator_open(
    const wchar_t* const _Path_spec, __std_fs_dir_handle* const _Handle, __std_fs_find_data* const _Results) noexcept {
    __std_fs_directory_iterator_close(*_Handle);
    *_Handle = __std_fs_dir_handle{reinterpret_cast<intptr_t>(
        FindFirstFileExW(_Path_spec, FindExInfoBasic, _Results, FindExSearchNameMatch, nullptr, 0))};
    if (*_Handle != __std_fs_dir_handle::_Invalid) {
        return __std_win_error::_Success;
    }

#if _STL_WIN32_WINNT < _WIN32_WINNT_WIN7
    // If the above call failed, we might be on pre Windows 7 / Windows Server 2008 R2, which doesn't support
    // FindExInfoBasic; try again with FindExInfoStandard if we got an invalid parameter error.
    const __std_win_error _Last_error{GetLastError()};
    if (_Last_error != __std_win_error::_Not_supported && _Last_error != __std_win_error::_Invalid_parameter) {
        return _Last_error;
    }

    *_Handle = __std_fs_dir_handle{reinterpret_cast<intptr_t>(
        FindFirstFileExW(_Path_spec, FindExInfoStandard, _Results, FindExSearchNameMatch, nullptr, 0))};
    if (*_Handle != __std_fs_dir_handle::_Invalid) {
        return __std_win_error::_Success;
    }
#endif // _STL_WIN32_WINNT < _WIN32_WINNT_WIN7

    return __std_win_error{GetLastError()};
}

void __stdcall __std_fs_directory_iterator_close(const __std_fs_dir_handle _Handle) noexcept {
    if (_Handle != __std_fs_dir_handle::_Invalid && !FindClose(reinterpret_cast<HANDLE>(_Handle))) {
        terminate();
    }
}

[[nodiscard]] __std_win_error __stdcall __std_fs_directory_iterator_advance(
    const __std_fs_dir_handle _Handle, __std_fs_find_data* const _Results) noexcept {
    if (FindNextFileW(reinterpret_cast<HANDLE>(_Handle), reinterpret_cast<WIN32_FIND_DATAW*>(_Results))) {
        return __std_win_error::_Success;
    }

    return __std_win_error{GetLastError()};
}

[[nodiscard]] __std_code_page __stdcall __std_fs_code_page() noexcept {
    if (___lc_codepage_func() == CP_UTF8) {
        return __std_code_page{CP_UTF8};
    }

#if !defined(_ONECORE)
    if (!AreFileApisANSI()) {
        return __std_code_page{CP_OEMCP};
    }
#endif // !defined(_ONECORE)

    return __std_code_page{CP_ACP};
}

[[nodiscard]] __std_fs_convert_result __stdcall __std_fs_convert_narrow_to_wide(const __std_code_page _Code_page,
    const char* const _Input_str, const int _Input_len, wchar_t* const _Output_str, const int _Output_len) noexcept {
    const int _Len = MultiByteToWideChar(
        static_cast<unsigned int>(_Code_page), MB_ERR_INVALID_CHARS, _Input_str, _Input_len, _Output_str, _Output_len);
    return {_Len, _Len == 0 ? __std_win_error{GetLastError()} : __std_win_error::_Success};
}

[[nodiscard]] __std_fs_convert_result __stdcall __std_fs_convert_wide_to_narrow(const __std_code_page _Code_page,
    const wchar_t* const _Input_str, const int _Input_len, char* const _Output_str, const int _Output_len) noexcept {
    __std_fs_convert_result _Result;

    if (_Code_page == __std_code_page{CP_UTF8} || _Code_page == __std_code_page{54936}) {
        // For UTF-8 or GB18030, attempt to use WC_ERR_INVALID_CHARS. (These codepages can't use WC_NO_BEST_FIT_CHARS
        // below, and other codepages can't use WC_ERR_INVALID_CHARS.) WC_ERR_INVALID_CHARS is Windows Vista+.
        // Windows XP will activate the ERROR_INVALID_FLAGS fallback below.
        _Result._Len = WideCharToMultiByte(static_cast<unsigned int>(_Code_page), WC_ERR_INVALID_CHARS, _Input_str,
            _Input_len, _Output_str, _Output_len, nullptr, nullptr);
    } else { // For other codepages, attempt to use WC_NO_BEST_FIT_CHARS.
             // Codepages that don't support this will activate the ERROR_INVALID_FLAGS fallback below.
        BOOL _Used_default_char = FALSE;

        _Result._Len = WideCharToMultiByte(static_cast<unsigned int>(_Code_page), WC_NO_BEST_FIT_CHARS, _Input_str,
            _Input_len, _Output_str, _Output_len, nullptr, &_Used_default_char);

        if (_Used_default_char) { // Report round-tripping failure with ERROR_NO_UNICODE_TRANSLATION,
                                  // "No mapping for the Unicode character exists in the target multi-byte code page."
            return {0, __std_win_error{ERROR_NO_UNICODE_TRANSLATION}};
        }
    }

    _Result._Err = _Result._Len == 0 ? __std_win_error{GetLastError()} : __std_win_error::_Success;

    if (_Result._Err == __std_win_error{ERROR_INVALID_FLAGS}) { // Fall back to a non-strict conversion.
        _Result._Len = WideCharToMultiByte(static_cast<unsigned int>(_Code_page), 0, _Input_str, _Input_len,
            _Output_str, _Output_len, nullptr, nullptr);

        _Result._Err = _Result._Len == 0 ? __std_win_error{GetLastError()} : __std_win_error::_Success;
    }

    return _Result;
}

[[nodiscard]] __std_fs_copy_file_result __stdcall __std_fs_copy_file(const wchar_t* const _Source,
    const wchar_t* const _Target, __std_fs_copy_options _Options) noexcept { // copy _Source to _Target
    _Options &= __std_fs_copy_options::_Existing_mask;
    if (_Options != __std_fs_copy_options::_Overwrite_existing) {
        const __std_fs_copy_file_result _First_try_result =
            __vcp_Copyfile(_Source, _Target, /* _Fail_if_exists = */ true);
        if (_First_try_result._Error != __std_win_error::_File_exists // successful copy or I/O error
            || _Options == __std_fs_copy_options::_None) { // caller requested fail if exists behavior
            return _First_try_result;
        }

        // At this point, the target exists, and we are skip_existing or update_existing. To resolve either,
        // we need to open handles to test equivalent() and last_write_time().
        // We test equivalent() not by directly doing what equivalent() does, but by opening the handles
        // in exclusive mode, so a subsequent open will fail with ERROR_SHARING_VIOLATION.
        {
            const _STD _Fs_file _Source_handle(
                __vcp_CreateFile(_Source, FILE_READ_ATTRIBUTES | FILE_READ_DATA, 0, nullptr, OPEN_EXISTING, 0, 0));
            __std_win_error _Last_error = _Translate_CreateFile_last_error(_Source_handle._Get());
            if (_Last_error != __std_win_error::_Success) {
                return {false, _Last_error};
            }

            const _STD _Fs_file _Target_handle(
                __vcp_CreateFile(_Target, FILE_READ_ATTRIBUTES | FILE_WRITE_DATA, 0, nullptr, OPEN_EXISTING, 0, 0));
            _Last_error = _Translate_CreateFile_last_error(_Target_handle._Get());
            if (_Last_error != __std_win_error::_Success) {
                // Also handles the equivalent(from, to) error case
                return {false, _Last_error};
            }

            // If we get here, we did the equivalent(from, to) test. If we were only asked to skip existing, we're done
            if (_Options == __std_fs_copy_options::_Skip_existing) {
                return {false, __std_win_error::_Success};
            }

            // Test for update_existing
            long long _Source_last_write_time;
            _Last_error = _Get_last_write_time_by_handle(_Source_handle._Get(), &_Source_last_write_time);
            if (_Last_error != __std_win_error::_Success) {
                return {false, _Last_error};
            }

            long long _Target_last_write_time;
            _Last_error = _Get_last_write_time_by_handle(_Target_handle._Get(), &_Target_last_write_time);
            if (_Last_error != __std_win_error::_Success) {
                return {false, _Last_error};
            }

            if (_Source_last_write_time <= _Target_last_write_time) { // _Target is newer, so don't update_existing
                return {false, __std_win_error::_Success};
            }

            // if we get here, we've decided we want to overwrite the destination due to update_existing,
            // so fall through to the overwrite_existing path below
        } // close handles
    } // !_Overwrite_existing

    // The following errors were observed through experimentation:
    // is_regular_file(from) is false => ERROR_ACCESS_DENIED
    // exists(to) is true and is_regular_file(to) is false => ERROR_ACCESS_DENIED
    // exists(to) is true and equivalent(from, to) is true => ERROR_SHARING_VIOLATION
    return __vcp_Copyfile(_Source, _Target, /* _Fail_if_exists = */ false);
}

__std_win_error __stdcall __std_fs_get_file_id(__std_fs_file_id* const _Id, const wchar_t* const _Path) noexcept {
    __std_win_error _Last_error;
    const _STD _Fs_file _Handle(
        _Path, __std_access_rights::_File_read_attributes, __std_fs_file_flags::_Backup_semantics, &_Last_error);
    if (_Last_error != __std_win_error::_Success) {
        return _Last_error;
    }

    static_assert(sizeof(FILE_ID_INFO) == sizeof(__std_fs_file_id));
    static_assert(alignof(FILE_ID_INFO) == alignof(__std_fs_file_id));
    if (__vcrt_GetFileInformationByHandleEx(
            _Handle._Get(), FileIdInfo, reinterpret_cast<FILE_ID_INFO*>(_Id), sizeof(*_Id))
        != 0) { // if we could get FILE_ID_INFO, use that as the source of truth
        return __std_win_error::_Success;
    }

    _Last_error = __std_win_error{GetLastError()};
    switch (_Last_error) {
    case __std_win_error::_Not_supported:
    case __std_win_error::_Invalid_parameter:
        break; // try more things
    default:
        return _Last_error; // real error, bail to the caller
    }

#ifndef _CRT_APP
    // try GetFileInformationByHandle as a fallback
    BY_HANDLE_FILE_INFORMATION _Info;
    if (GetFileInformationByHandle(_Handle._Get(), &_Info) != 0) {
        _Id->_Volume_serial_number = _Info.dwVolumeSerialNumber;
        _CSTD memcpy(&_Id->_Id[0], &_Info.nFileIndexHigh, 8);
        _CSTD memset(&_Id->_Id[8], 0, 8);
        return __std_win_error::_Success;
    }

    _Last_error = __std_win_error{GetLastError()};
#endif // _CRT_APP

    return _Last_error;
}

[[nodiscard]] __std_win_error __stdcall __std_fs_create_directory_symbolic_link(
    const wchar_t* const _Symlink_file_name, const wchar_t* const _Target_file_name) noexcept {
    return _Create_symlink(_Symlink_file_name, _Target_file_name, SYMBOLIC_LINK_FLAG_DIRECTORY);
}

[[nodiscard]] __std_win_error __stdcall __std_fs_create_hard_link(
    const wchar_t* const _File_name, const wchar_t* const _Existing_file_name) noexcept {
#if defined(_CRT_APP)
    (void) _File_name;
    (void) _Existing_file_name;
    return __std_win_error::_Not_supported;
#else // ^^^ defined(_CRT_APP) ^^^ // vvv !defined(_CRT_APP) vvv
    if (CreateHardLinkW(_File_name, _Existing_file_name, nullptr) != 0) {
        return __std_win_error::_Success;
    }

    return __std_win_error{GetLastError()};
#endif // defined(_CRT_APP)
}

[[nodiscard]] __std_win_error __stdcall __std_fs_create_symbolic_link(
    const wchar_t* const _Symlink_file_name, const wchar_t* const _Target_file_name) noexcept {
    return _Create_symlink(_Symlink_file_name, _Target_file_name, 0);
}

[[nodiscard]] __std_win_error __stdcall __std_fs_read_reparse_data_buffer(
    const __std_fs_file_handle _Handle, void* const _Buffer, const unsigned long _Buffer_size) noexcept {
    unsigned long _Bytes_returned;
    // If DeviceIoControl fails, it returns 0 and _Bytes_returned is 0.
    if (0
        == DeviceIoControl(reinterpret_cast<HANDLE>(_Handle), FSCTL_GET_REPARSE_POINT, nullptr, 0, _Buffer,
            _Buffer_size, &_Bytes_returned, nullptr)) {
        return __std_win_error{GetLastError()};
    }

    return __std_win_error::_Success;
}

[[nodiscard]] __std_win_error __stdcall __std_fs_read_name_from_reparse_data_buffer(
    __std_fs_reparse_data_buffer* const _Buffer, wchar_t** const _Offset, unsigned short* const _Length) noexcept {
    if (_Buffer->_Reparse_tag == IO_REPARSE_TAG_SYMLINK) {
        auto& _Symlink_buffer             = _Buffer->_Symbolic_link_reparse_buffer;
        const unsigned short _Temp_length = _Symlink_buffer._Print_name_length / sizeof(wchar_t);

        if (_Temp_length == 0) {
            *_Length = _Symlink_buffer._Substitute_name_length / sizeof(wchar_t);
            *_Offset = &_Symlink_buffer._Path_buffer[_Symlink_buffer._Substitute_name_offset / sizeof(wchar_t)];
        } else {
            *_Length = _Temp_length;
            *_Offset = &_Symlink_buffer._Path_buffer[_Symlink_buffer._Print_name_offset / sizeof(wchar_t)];
        }
    } else {
        return __std_win_error{ERROR_REPARSE_TAG_INVALID};
    }

    return __std_win_error::_Success;
}

[[nodiscard]] __std_win_error __stdcall __std_fs_set_last_write_time(
    const long long _Last_write_filetime, const wchar_t* const _Path) noexcept {
    __std_win_error _Last_error;
    const _STD _Fs_file _Handle(
        _Path, __std_access_rights::_File_write_attributes, __std_fs_file_flags::_Backup_semantics, &_Last_error);
    if (_Last_error != __std_win_error::_Success) {
        return _Last_error;
    }

    if (SetFileTime(_Handle._Get(), nullptr, nullptr, reinterpret_cast<const FILETIME*>(&_Last_write_filetime))) {
        return __std_win_error::_Success;
    }

    return __std_win_error{GetLastError()};
}

[[nodiscard]] __std_fs_remove_result __stdcall __std_fs_remove(const wchar_t* const _Target) noexcept {
    // remove _Target without caring whether _Target is a file or directory
    __std_win_error _Last_error;
#if _STL_ALWAYS_HAS_SetFileInformationByHandle
#define _SetFileInformationByHandle SetFileInformationByHandle
#else // ^^^ _STL_ALWAYS_HAS_SetFileInformationByHandle ^^^ // vvv !_STL_ALWAYS_HAS_SetFileInformationByHandle vvv
    const auto _SetFileInformationByHandle = __vcrt_SetFileInformationByHandle;
    if (_SetFileInformationByHandle == _Not_supported_SetFileInformationByHandle) { // Windows XP
        if (RemoveDirectoryW(_Target)) {
            // try RemoveDirectoryW first because it gives a specific error code for "the input was a file";
            // DeleteFileW on a directory input returns ERROR_ACCESS_DENIED
            return {true, __std_win_error::_Success};
        }

        _Last_error = __std_win_error{GetLastError()};
        if (_Last_error == __std_win_error::_Directory_name_is_invalid) { // input may have been a file
            if (DeleteFileW(_Target)) {
                return {true, __std_win_error::_Success};
            }

            _Last_error = __std_win_error{GetLastError()};
        }

        return {false, _Translate_not_found_to_success(__std_win_error{GetLastError()})};
    }
#endif // _STL_ALWAYS_HAS_SetFileInformationByHandle

    constexpr auto _Flags = __std_fs_file_flags::_Backup_semantics | __std_fs_file_flags::_Open_reparse_point;
    const _STD _Fs_file _Handle(_Target, __std_access_rights::_Delete, _Flags, &_Last_error);
    if (_Last_error != __std_win_error::_Success) {
        return {false, _Translate_not_found_to_success(_Last_error)};
    }

    // From newer Windows SDK than currently used to build vctools:
    // #define FILE_DISPOSITION_FLAG_DELETE                     0x00000001
    // #define FILE_DISPOSITION_FLAG_POSIX_SEMANTICS            0x00000002

    // typedef struct _FILE_DISPOSITION_INFO_EX {
    //     DWORD Flags;
    // } FILE_DISPOSITION_INFO_EX, *PFILE_DISPOSITION_INFO_EX;

    struct _File_disposition_info_ex {
        DWORD _Flags;
    };
    _File_disposition_info_ex _Info_ex{0x3};

    // FileDispositionInfoEx isn't documented in MSDN at the time of this writing, but is present
    // in minwinbase.h as of at least 10.0.16299.0
    constexpr auto _FileDispositionInfoExClass = static_cast<FILE_INFO_BY_HANDLE_CLASS>(21);
    if (_SetFileInformationByHandle(_Handle._Get(), _FileDispositionInfoExClass, &_Info_ex, sizeof(_Info_ex))) {
        return {true, __std_win_error::_Success};
    }

    _Last_error = __std_win_error{GetLastError()};
    if (_Last_error != __std_win_error::_Invalid_parameter && _Last_error != __std_win_error::_Invalid_function) {
        return {false, _Last_error};
    }

    // Filesystem without POSIX delete support, or older than Windows 10 RS1 version without such support:
    FILE_DISPOSITION_INFO _Info{/* .Delete= */ TRUE};
    if (_SetFileInformationByHandle(_Handle._Get(), FileDispositionInfo, &_Info, sizeof(_Info))) {
        return {true, __std_win_error::_Success};
    }

    return {false, __std_win_error{GetLastError()}};

#undef _SetFileInformationByHandle
}

[[nodiscard]] __std_win_error __stdcall __std_fs_change_permissions(
    const wchar_t* const _Path, const bool _Follow_symlinks, const bool _Readonly) noexcept {
    const DWORD _Old_attributes = GetFileAttributesW(_Path);
    if (_Old_attributes == INVALID_FILE_ATTRIBUTES) {
        return __std_win_error{GetLastError()};
    }

    const DWORD _Readonly_test = _Readonly ? FILE_ATTRIBUTE_READONLY : 0;
    if ((_Old_attributes & FILE_ATTRIBUTE_REPARSE_POINT) && _Follow_symlinks) {
        __std_win_error _Err;
        _STD _Fs_file _Handle(_Path,
            __std_access_rights::_File_read_attributes | __std_access_rights::_File_write_attributes,
            __std_fs_file_flags::_Backup_semantics, &_Err);

        if (_Err != __std_win_error::_Success) {
            return _Err;
        }

        FILE_BASIC_INFO _Basic_info;
        if (!__vcrt_GetFileInformationByHandleEx(_Handle._Get(), FileBasicInfo, &_Basic_info, sizeof(_Basic_info))) {
            return __std_win_error{GetLastError()};
        }

        if ((_Basic_info.FileAttributes & FILE_ATTRIBUTE_READONLY) == _Readonly_test) { // nothing to do
            return __std_win_error::_Success;
        }

        _Basic_info.FileAttributes ^= FILE_ATTRIBUTE_READONLY;
        if (__vcrt_SetFileInformationByHandle(_Handle._Get(), FileBasicInfo, &_Basic_info, sizeof(_Basic_info))) {
            return __std_win_error::_Success;
        }

        return __std_win_error{GetLastError()};
    }

    if ((_Old_attributes & FILE_ATTRIBUTE_READONLY) == _Readonly_test) { // nothing to do
        return __std_win_error::_Success;
    }

    if (SetFileAttributesW(_Path, _Old_attributes ^ FILE_ATTRIBUTE_READONLY)) {
        return __std_win_error::_Success;
    }

    return __std_win_error{GetLastError()};
}

[[nodiscard]] __std_win_error __stdcall __std_fs_rename(
    const wchar_t* const _Source, const wchar_t* const _Target) noexcept {
    if (MoveFileExW(_Source, _Target, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING)) {
        return __std_win_error::_Success;
    }

    return __std_win_error{GetLastError()};
}

[[nodiscard]] __std_win_error __stdcall __std_fs_resize_file(
    const wchar_t* const _Target, const uintmax_t _New_size) noexcept {
    __std_win_error _Err;
    const _STD _Fs_file _Handle(_Target, __std_access_rights::_File_generic_write, __std_fs_file_flags::_None, &_Err);
    if (_Err != __std_win_error::_Success) {
        return _Err;
    }

    LARGE_INTEGER _Large;
    _Large.QuadPart = _New_size;
    if (SetFilePointerEx(_Handle._Get(), _Large, 0, FILE_BEGIN) == 0 || SetEndOfFile(_Handle._Get()) == 0) {
        return __std_win_error{GetLastError()};
    }

    return __std_win_error::_Success;
}

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

[[nodiscard]] __std_ulong_and_error __stdcall __std_fs_get_temp_path(wchar_t* const _Target) noexcept {
    // calls GetTempPathW
    // If getting the path failed, returns 0 size; otherwise, returns the size of the
    // expected directory. If the path could be resolved to an existing directory,
    // returns __std_win_error::_Success; otherwise, returns __std_win_error::_Max.
    const auto _Size = GetTempPathW(__std_fs_temp_path_max, _Target);
    if (_Size == 0) {
        return {0, __std_win_error{GetLastError()}};
    }

    // Effects: If exists(p) is false or is_directory(p) is false, an error is reported
    const DWORD _Attributes = GetFileAttributesW(_Target);
    if (_Attributes == INVALID_FILE_ATTRIBUTES || !(_Attributes & FILE_ATTRIBUTE_DIRECTORY)) {
        return {_Size, __std_win_error::_Max};
    }

    if (_Attributes & FILE_ATTRIBUTE_REPARSE_POINT) {
        __std_fs_file_handle _Handle;
        const auto _Last_error = __std_fs_open_handle(
            &_Handle, _Target, __std_access_rights::_File_read_attributes, __std_fs_file_flags::_Backup_semantics);
        __std_fs_close_handle(_Handle);
        if (_Last_error != __std_win_error::_Success) {
            return {_Size, __std_win_error::_Max};
        }
    }

    return {_Size, __std_win_error::_Success};
}

// This structure is meant to be embedded into __std_fs_stats that properly aligned it,
// so that 64-bit values are fully aligned. Note that _File_size fields are flipped to be in low:high order
// and represented by __std_fs_filetime which is a pair of ulongs.
// If this structure is used in GetFileAttributesEx, after successful read, File_size parts must be put in
// low:high order, as GetFileAttributesEx returns them in high:low order.
struct _File_attr_data { // typedef struct _WIN32_FILE_ATTRIBUTE_DATA {
    __std_fs_file_attr _Attributes; //     DWORD dwFileAttributes;
    __std_fs_filetime _Creation_time; //     FILETIME ftCreationTime;
    __std_fs_filetime _Last_access_time; //     FILETIME ftLastAccessTime;
    __std_fs_filetime _Last_write_time; //     FILETIME ftLastWriteTime;
    unsigned long _File_size_high; //     DWORD nFileSizeHigh;
    unsigned long _File_size_low; //     DWORD nFileSizeLow;
}; // } WIN32_FILE_ATTRIBUTE_DATA, *LPWIN32_FILE_ATTRIBUTE_DATA;

struct alignas(long long) _Aligned_file_attrs {
    unsigned long _Padding; // align the __std_fs_filetime inside _Data to make the memcpy below an ordinary 64-bit load
    _File_attr_data _Data;

    [[nodiscard]] long long _Last_write_time() const noexcept {
        long long _Result;
        _CSTD memcpy(&_Result, &_Data._Last_write_time, sizeof(_Result));
        return _Result;
    }

    [[nodiscard]] unsigned long long _File_size() const noexcept {
        return (static_cast<unsigned long long>(_Data._File_size_high) << 32) + _Data._File_size_low;
    }
};

[[nodiscard]] __std_win_error __stdcall __std_fs_get_stats(const wchar_t* const _Path, __std_fs_stats* const _Stats,
    __std_fs_stats_flags _Flags, const __std_fs_file_attr _Symlink_attribute_hint) noexcept {
    static_assert((offsetof(_Aligned_file_attrs, _Data._Last_write_time) % 8) == 0, "_Last_write_time not aligned");
    static_assert(sizeof(_File_attr_data) == sizeof(WIN32_FILE_ATTRIBUTE_DATA));
    static_assert(alignof(_File_attr_data) == alignof(WIN32_FILE_ATTRIBUTE_DATA));
    static_assert(alignof(_File_attr_data) == 4);

    const bool _Follow_symlinks = _Bitmask_includes(_Flags, __std_fs_stats_flags::_Follow_symlinks);
    _Flags &= ~__std_fs_stats_flags::_Follow_symlinks;
    if (_Follow_symlinks && _Bitmask_includes(_Flags, __std_fs_stats_flags::_Reparse_tag)) {
        return __std_win_error::_Invalid_parameter;
    }

    if (_Bitmask_includes(_Flags, __std_fs_stats_flags::_Attributes)
        && _Symlink_attribute_hint != __std_fs_file_attr::_Invalid
        && (!_Bitmask_includes(_Symlink_attribute_hint, __std_fs_file_attr::_Reparse_point)
            || !_Follow_symlinks)) { // if the hint can't be a symlink, we already have the attributes
        _Flags &= ~__std_fs_stats_flags::_Attributes;
        _Stats->_Attributes = _Symlink_attribute_hint;
    }

    if (_Flags == __std_fs_stats_flags::_None) { // no more data to get, report success
        return __std_win_error::_Success;
    }

    constexpr auto _Get_file_attributes_data =
        __std_fs_stats_flags::_Attributes | __std_fs_stats_flags::_File_size | __std_fs_stats_flags::_Last_write_time;
    if (_Bitmask_includes(
            _Flags, _Get_file_attributes_data)) { // caller wants something GetFileAttributesExW might provide
        if (_Symlink_attribute_hint == __std_fs_file_attr::_Invalid
            || !_Bitmask_includes(_Symlink_attribute_hint, __std_fs_file_attr::_Reparse_point)
            || !_Follow_symlinks) { // we might not be a symlink or not following symlinks, so GetFileAttributesExW
                                    // would return the right answer
            _Aligned_file_attrs _Aligned_attrs;
            auto& _Data = _Aligned_attrs._Data;
            if (!GetFileAttributesExW(_Path, GetFileExInfoStandard, &_Data)) {
                return __std_win_error{GetLastError()};
            }

            if (!_Follow_symlinks
                || !_Bitmask_includes(_Data._Attributes,
                    __std_fs_file_attr::_Reparse_point)) { // if we aren't following symlinks or can't be a
                                                           // symlink, that data was useful, record
                _Stats->_Attributes      = _Data._Attributes;
                _Stats->_File_size       = _Aligned_attrs._File_size();
                _Stats->_Last_write_time = _Aligned_attrs._Last_write_time();
                _Flags &= ~_Get_file_attributes_data;
            }
        }
    }

    if (_Flags == __std_fs_stats_flags::_None) { // no more data to get, report success
        return __std_win_error::_Success;
    }

    // Let's try to open the file (either reparse point or destination).
    __std_win_error _Last_error;
    const auto _File_flags = _Follow_symlinks
                                 ? __std_fs_file_flags::_Backup_semantics
                                 : __std_fs_file_flags::_Backup_semantics | __std_fs_file_flags::_Open_reparse_point;
    const _STD _Fs_file _Handle(_Path, __std_access_rights::_File_read_attributes, _File_flags, &_Last_error);
    if (_Last_error != __std_win_error::_Success) {
        return _Last_error;
    }

    _GetFileInfoByHandleEx _Get_info_fn;
    if (_Get_info_fn._Supported()) {
        constexpr auto _Basic_info_data = __std_fs_stats_flags::_Attributes | __std_fs_stats_flags::_Last_write_time;
        constexpr auto _Attribute_tag_info_data =
            __std_fs_stats_flags::_Attributes | __std_fs_stats_flags::_Reparse_tag;
        constexpr auto _Standard_info_data = __std_fs_stats_flags::_File_size | __std_fs_stats_flags::_Link_count;

        if (_Flags != _Attribute_tag_info_data && _Bitmask_includes(_Flags, _Basic_info_data)) {
            // we have data FileBasicInfo can fill in, that FileAttributeTagInfo wouldn't exactly fill in
            FILE_BASIC_INFO _Info;
            if (!_Get_info_fn(_Handle._Get(), FileBasicInfo, &_Info, sizeof(_Info))) {
                return __std_win_error{GetLastError()};
            }

            _Stats->_Attributes      = __std_fs_file_attr{_Info.FileAttributes};
            _Stats->_Last_write_time = _Info.LastWriteTime.QuadPart;
            _Flags &= ~_Basic_info_data;
        }

        if (_Bitmask_includes(_Flags, _Attribute_tag_info_data)) {
            FILE_ATTRIBUTE_TAG_INFO _Info;
            if (!_Get_info_fn(_Handle._Get(), FileAttributeTagInfo, &_Info, sizeof(_Info))) {
                return __std_win_error{GetLastError()};
            }

            _Stats->_Attributes        = __std_fs_file_attr{_Info.FileAttributes};
            _Stats->_Reparse_point_tag = __std_fs_reparse_tag{_Info.ReparseTag};
            _Flags &= ~_Attribute_tag_info_data;
        }

        if (_Bitmask_includes(_Flags, _Standard_info_data)) {
            FILE_STANDARD_INFO _Info;
            if (!_Get_info_fn(_Handle._Get(), FileStandardInfo, &_Info, sizeof(_Info))) {
                return __std_win_error{GetLastError()};
            }

            _Stats->_File_size  = _Info.EndOfFile.QuadPart;
            _Stats->_Link_count = _Info.NumberOfLinks;
            _Flags &= ~_Standard_info_data;
        }
    }

#if _STL_WIN32_WINNT < _WIN32_WINNT_VISTA
    if (_Flags == __std_fs_stats_flags::_None) { // no more data to get, report success
        return __std_win_error::_Success;
    }

    BY_HANDLE_FILE_INFORMATION _Legacy_info;
    if (!GetFileInformationByHandle(_Handle._Get(), &_Legacy_info)) {
        return __std_win_error{GetLastError()};
    }

    _Stats->_Attributes = __std_fs_file_attr{_Legacy_info.dwFileAttributes};
    _CSTD memcpy(&_Stats->_Last_write_time, &_Legacy_info.ftLastWriteTime, sizeof(_Stats->_Last_write_time));
    _Stats->_File_size =
        (static_cast<unsigned long long>(_Legacy_info.nFileIndexHigh) << 32) + _Legacy_info.nFileIndexLow;
    _Stats->_Link_count = _Legacy_info.nNumberOfLinks;
    _Flags &= ~(__std_fs_stats_flags::_Attributes | __std_fs_stats_flags::_Last_write_time
                | __std_fs_stats_flags::_File_size | __std_fs_stats_flags::_Link_count);

    if (_Bitmask_includes(_Flags, __std_fs_stats_flags::_Reparse_tag)) {
        if (_Bitmask_includes(_Stats->_Attributes, __std_fs_file_attr::_Reparse_point)) {
            WIN32_FIND_DATAW _Data;
            const auto _Dir = FindFirstFileExW(_Path, FindExInfoStandard, &_Data, FindExSearchNameMatch, nullptr, 0);
            if (_Dir == INVALID_HANDLE_VALUE) {
                return __std_win_error{GetLastError()};
            }

            FindClose(_Dir);
            _Stats->_Reparse_point_tag = __std_fs_reparse_tag{_Data.dwReserved0};
        } else {
            _Stats->_Reparse_point_tag = __std_fs_reparse_tag::_None;
        }

        _Flags &= ~__std_fs_stats_flags::_Reparse_tag;
    }
#endif // _STL_WIN32_WINNT < _WIN32_WINNT_VISTA

    if (_Flags == __std_fs_stats_flags::_None) { // no more data to get, report success
        return __std_win_error::_Success;
    }

    // somehow the caller asked for more data than we yet know about, maybe new headers
    // targeting old separately compiled bits
    return __std_win_error::_Not_supported;
}

[[nodiscard]] __std_fs_create_directory_result __stdcall __std_fs_create_directory(
    const wchar_t* const _New_directory) noexcept {
    if (CreateDirectoryW(_New_directory, nullptr)) {
        return {true, __std_win_error::_Success};
    }

    __std_win_error _Last_error{GetLastError()};
    if (_Last_error == __std_win_error::_Already_exists) {
        __std_fs_stats _Stats;
        _Last_error = __std_fs_get_stats(
            _New_directory, &_Stats, __std_fs_stats_flags::_Attributes | __std_fs_stats_flags::_Follow_symlinks);
        if (_Last_error == __std_win_error::_Success
            && !_Bitmask_includes(_Stats._Attributes, __std_fs_file_attr::_Directory)) {
            _Last_error = __std_win_error::_Already_exists;
        }
    }

    return {false, _Last_error};
}

// TRANSITION, ABI: __std_fs_create_directory_template() is preserved for binary compatibility
[[nodiscard]] __std_fs_create_directory_result __stdcall __std_fs_create_directory_template(
    const wchar_t* const _Template_directory, const wchar_t* const _New_directory) noexcept {
#if defined(_CRT_APP)
    (void) _Template_directory;
    return __std_fs_create_directory(_New_directory);
#else // ^^^ defined(_CRT_APP) ^^^ // vvv !defined(_CRT_APP) vvv
    if (CreateDirectoryExW(_Template_directory, _New_directory, nullptr)) {
        return {true, __std_win_error::_Success};
    }

    __std_win_error _Last_error{GetLastError()};
    if (_Last_error == __std_win_error::_Already_exists) {
        _Last_error = __std_win_error::_Success;
    }

    return {false, _Last_error};
#endif // defined(_CRT_APP)
}

[[nodiscard]] __std_ulong_and_error __stdcall __std_fs_get_current_path(
    const unsigned long _Target_size, wchar_t* const _Target) noexcept {
    // If getting the path failed, GetCurrentDirectoryW returns 0; otherwise, returns the size of the expected
    // directory.
    const auto _Size = GetCurrentDirectoryW(_Target_size, _Target);
    if (_Size == 0 || _Size > _Target_size) {
        return {_Size, __std_win_error{GetLastError()}};
    }

    return {_Size, __std_win_error::_Success};
}

[[nodiscard]] __std_win_error __stdcall __std_fs_set_current_path(const wchar_t* const _Target) noexcept {
    // If setting the path failed, SetCurrentDirectoryW returns 0; otherwise returns non-zero.
    const auto _Succeeded = SetCurrentDirectoryW(_Target);
    if (_Succeeded == 0) {
        return __std_win_error{GetLastError()};
    }
    return __std_win_error::_Success;
}

_END_EXTERN_C
