// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// filesystem.cpp -- C++17 <filesystem> implementation
// (see filesys.cpp for <experimental/filesystem> implementation)

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

#include <clocale>
#include <corecrt_terminate.h>
#include <cstdlib>
#include <cstring>
#include <internal_shared.h>
#include <xfilesystem_abi.h>

#include <Windows.h>
#include <winioctl.h>

// We have several switches that do not have case statements for every possible enum value.
// Hence, disabling this warning.
#pragma warning(disable : 4061) // enumerator '__std_win_error::_Success' in switch of enum
                                // '__std_win_error' is not explicitly handled by a case label

static_assert(__std_code_page::_Utf8 == __std_code_page{CP_UTF8});

namespace {

#ifdef _CRT_APP
    BOOLEAN __stdcall _Not_supported_CreateSymbolicLinkW(const wchar_t*, const wchar_t*, DWORD) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
#define __vcrt_CreateSymbolicLinkW _Not_supported_CreateSymbolicLinkW
#else // ^^^ _CRT_APP ^^^ // vvv !_CRT_APP vvv
#define __vcrt_CreateSymbolicLinkW CreateSymbolicLinkW
#endif // _CRT_APP

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

    [[nodiscard]] __std_win_error __stdcall _Translate_CreateFile_last_error(const HANDLE _Handle) {
        if (_Handle != INVALID_HANDLE_VALUE) {
            return __std_win_error::_Success;
        }

        return __std_win_error{GetLastError()};
    }

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
                _Symlink_file_name, _Target_file_name, _Flags | SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE)) {
            return __std_win_error::_Success;
        }

        DWORD _Last_error = GetLastError();
        if (_Last_error == ERROR_INVALID_PARAMETER) {
            if (__vcrt_CreateSymbolicLinkW(_Symlink_file_name, _Target_file_name, _Flags)) {
                return __std_win_error::_Success;
            }

            _Last_error = GetLastError();
        }

        return __std_win_error{_Last_error};
    }

    [[nodiscard]] __std_win_error __stdcall _Translate_not_found_to_success(const __std_win_error _Err) noexcept {
        if (__std_is_file_not_found(_Err)) {
            return __std_win_error::_Success;
        }

        return _Err;
    }

    [[nodiscard]] __std_win_error __stdcall _Get_last_write_time_by_handle(
        const HANDLE _Handle, long long* const _Last_write_filetime) {
        // read the last write time from _Handle and store it in _Last_write_filetime
        FILE_BASIC_INFO _Ex_info;
        if (GetFileInformationByHandleEx(_Handle, FileBasicInfo, &_Ex_info, sizeof(_Ex_info))) {
            *_Last_write_filetime = _Ex_info.LastWriteTime.QuadPart;
            return __std_win_error::_Success;
        }

        return __std_win_error{GetLastError()};
    }

    [[nodiscard]] __std_win_error __stdcall _Get_file_id_by_handle(
        const HANDLE _Handle, _Out_ FILE_ID_INFO* const _Id) noexcept {
        if (GetFileInformationByHandleEx(_Handle, FileIdInfo, _Id, sizeof(*_Id))) {
            // if we could get FILE_ID_INFO, use that as the source of truth
            return __std_win_error::_Success;
        }

        __std_win_error _Last_error{GetLastError()};

#ifndef _CRT_APP
        switch (_Last_error) {
        case __std_win_error::_Not_supported:
        case __std_win_error::_Invalid_parameter:
            break; // try more things
        default:
            return _Last_error; // real error, bail to the caller
        }

        // try GetFileInformationByHandle as a fallback
        BY_HANDLE_FILE_INFORMATION _Info;
        if (GetFileInformationByHandle(_Handle, &_Info)) {
            _Id->VolumeSerialNumber = _Info.dwVolumeSerialNumber;
            _CSTD memcpy(&_Id->FileId.Identifier[0], &_Info.nFileIndexHigh, 8); // copying from 2 consecutive DWORDs
            _CSTD memset(&_Id->FileId.Identifier[8], 0, 8);
            return __std_win_error::_Success;
        }

        _Last_error = __std_win_error{GetLastError()};
#endif // _CRT_APP

        return _Last_error;
    }

    [[nodiscard]] _Success_(return == __std_win_error::_Success) __std_win_error
        __stdcall _Set_delete_flag(_In_ __std_fs_file_handle _Handle) {

        // See minwinbase.h and WinBase.h.
        FILE_DISPOSITION_INFO_EX _Info_ex{FILE_DISPOSITION_FLAG_DELETE | FILE_DISPOSITION_FLAG_POSIX_SEMANTICS};
        if (SetFileInformationByHandle(
                reinterpret_cast<HANDLE>(_Handle), FileDispositionInfoEx, &_Info_ex, sizeof(_Info_ex))) {
            return __std_win_error::_Success;
        }

        const __std_win_error _Last_error{GetLastError()};
        switch (_Last_error) {
        case __std_win_error::_Invalid_parameter: // Older Windows versions
        case __std_win_error::_Invalid_function: // Windows 10 1607
        case __std_win_error::_Not_supported: // POSIX delete not supported by the file system
            break; // try non-POSIX delete below
        case __std_win_error::_Access_denied: // This might be due to the read-only bit, try to clear it and try again
        default:
            return _Last_error;
        }

        FILE_DISPOSITION_INFO _Info{/* .Delete= */ TRUE};
        if (SetFileInformationByHandle(reinterpret_cast<HANDLE>(_Handle), FileDispositionInfo, &_Info, sizeof(_Info))) {
            return __std_win_error::_Success;
        }

        return __std_win_error{GetLastError()};
    }

    [[nodiscard]] unsigned long long _Merge_to_ull(DWORD _High, DWORD _Low) noexcept {
        return (static_cast<unsigned long long>(_High) << 32) | static_cast<unsigned long long>(_Low);
    }
} // unnamed namespace

_EXTERN_C

[[nodiscard]] __std_ulong_and_error __stdcall __std_fs_get_full_path_name(_In_z_ const wchar_t* _Source,
    _In_ unsigned long _Target_size, _Out_writes_z_(_Target_size) wchar_t* _Target) noexcept { // calls GetFullPathNameW
    const auto _Result = GetFullPathNameW(_Source, _Target_size, _Target, nullptr);
    return {_Result, _Result == 0 ? __std_win_error{GetLastError()} : __std_win_error::_Success};
}

[[nodiscard]] __std_win_error __stdcall __std_fs_open_handle(_Out_ __std_fs_file_handle* const _Handle,
    _In_z_ const wchar_t* const _File_name, _In_ const __std_access_rights _Desired_access,
    _In_ const __std_fs_file_flags _Flags) noexcept { // calls CreateFile2 or CreateFileW
    const HANDLE _Result = __vcp_CreateFile(_File_name, static_cast<unsigned long>(_Desired_access),
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING,
        static_cast<unsigned long>(_Flags), nullptr);
    *_Handle             = static_cast<__std_fs_file_handle>(reinterpret_cast<intptr_t>(_Result));
    return _Translate_CreateFile_last_error(_Result);
}

void __stdcall __std_fs_close_handle(const __std_fs_file_handle _Handle) noexcept { // calls CloseHandle
    if (_Handle != __std_fs_file_handle::_Invalid && !CloseHandle(reinterpret_cast<HANDLE>(_Handle))) {
        terminate();
    }
}

[[nodiscard]] _Success_(return == __std_win_error::_Success) __std_win_error
    __stdcall __std_fs_get_file_attributes_by_handle(
        _In_ const __std_fs_file_handle _Handle, _Out_ unsigned long* const _File_attributes) noexcept {
    // read the attributes from _Handle and store it in _File_attributes
    const HANDLE _As_plain_handle = reinterpret_cast<HANDLE>(_Handle);

    FILE_BASIC_INFO _Ex_info;
    if (GetFileInformationByHandleEx(_As_plain_handle, FileBasicInfo, &_Ex_info, sizeof(_Ex_info))) {
        *_File_attributes = _Ex_info.FileAttributes;
        return __std_win_error::_Success;
    }

    return __std_win_error{GetLastError()};
}

[[nodiscard]] __std_ulong_and_error __stdcall __std_fs_get_final_path_name_by_handle(
    _In_ const __std_fs_file_handle _Handle, _Out_writes_z_(_Target_size) wchar_t* const _Target,
    _In_ const unsigned long _Target_size,
    _In_ const __std_fs_volume_name_kind _Flags) noexcept { // calls GetFinalPathNameByHandleW
    const auto _Result = GetFinalPathNameByHandleW(
        reinterpret_cast<HANDLE>(_Handle), _Target, _Target_size, static_cast<unsigned long>(_Flags));
    return {_Result, _Result == 0 ? __std_win_error{GetLastError()} : __std_win_error::_Success};
}

static_assert(sizeof(WIN32_FIND_DATAW) == sizeof(__std_fs_find_data));
static_assert(alignof(WIN32_FIND_DATAW) == alignof(__std_fs_find_data));

[[nodiscard]] __std_win_error __stdcall __std_fs_directory_iterator_open(_In_z_ const wchar_t* const _Path_spec,
    _Inout_ __std_fs_dir_handle* const _Handle, _Out_ __std_fs_find_data* const _Results) noexcept {
    __std_fs_directory_iterator_close(*_Handle);
    *_Handle = __std_fs_dir_handle{reinterpret_cast<intptr_t>(
        FindFirstFileExW(_Path_spec, FindExInfoBasic, _Results, FindExSearchNameMatch, nullptr, 0))};
    if (*_Handle != __std_fs_dir_handle::_Invalid) {
        return __std_win_error::_Success;
    }

    return __std_win_error{GetLastError()};
}

void __stdcall __std_fs_directory_iterator_close(_In_ const __std_fs_dir_handle _Handle) noexcept {
    if (_Handle != __std_fs_dir_handle::_Invalid && !FindClose(reinterpret_cast<HANDLE>(_Handle))) {
        terminate();
    }
}

[[nodiscard]] __std_win_error __stdcall __std_fs_directory_iterator_advance(
    _In_ const __std_fs_dir_handle _Handle, _Out_ __std_fs_find_data* const _Results) noexcept {
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

[[nodiscard]] __std_fs_convert_result __stdcall __std_fs_convert_narrow_to_wide(_In_ const __std_code_page _Code_page,
    _In_reads_(_Input_len) const char* const _Input_str, _In_ const int _Input_len,
    _Out_writes_opt_(_Output_len) wchar_t* const _Output_str, _In_ const int _Output_len) noexcept {
    const int _Len = MultiByteToWideChar(
        static_cast<unsigned int>(_Code_page), MB_ERR_INVALID_CHARS, _Input_str, _Input_len, _Output_str, _Output_len);
    return {_Len, _Len == 0 ? __std_win_error{GetLastError()} : __std_win_error::_Success};
}

[[nodiscard]] __std_fs_convert_result __stdcall __std_fs_convert_wide_to_narrow(_In_ const __std_code_page _Code_page,
    _In_reads_(_Input_len) const wchar_t* const _Input_str, _In_ const int _Input_len,
    _Out_writes_opt_(_Output_len) char* const _Output_str, _In_ const int _Output_len) noexcept {
    __std_fs_convert_result _Result;

    if (_Code_page == __std_code_page{CP_UTF8} || _Code_page == __std_code_page{54936}) {
        // For UTF-8 or GB18030, attempt to use WC_ERR_INVALID_CHARS. (These codepages can't use WC_NO_BEST_FIT_CHARS
        // below, and other codepages can't use WC_ERR_INVALID_CHARS.)
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

[[nodiscard]] __std_fs_convert_result __stdcall __std_fs_convert_wide_to_narrow_replace_chars(
    _In_ const __std_code_page _Code_page, _In_reads_(_Input_len) const wchar_t* const _Input_str,
    _In_ const int _Input_len, _Out_writes_opt_(_Output_len) char* const _Output_str,
    _In_ const int _Output_len) noexcept {
    __std_fs_convert_result _Result;

    _Result._Len = WideCharToMultiByte(static_cast<unsigned int>(_Code_page), WC_NO_BEST_FIT_CHARS, _Input_str,
        _Input_len, _Output_str, _Output_len, nullptr, nullptr);

    _Result._Err = _Result._Len == 0 ? __std_win_error{GetLastError()} : __std_win_error::_Success;

    // Some codepages don't support WC_NO_BEST_FIT_CHARS, fall back to default conversion.
    if (_Result._Err == __std_win_error{ERROR_INVALID_FLAGS}) {
        _Result._Len = WideCharToMultiByte(static_cast<unsigned int>(_Code_page), 0, _Input_str, _Input_len,
            _Output_str, _Output_len, nullptr, nullptr);

        _Result._Err = _Result._Len == 0 ? __std_win_error{GetLastError()} : __std_win_error::_Success;
    }

    return _Result;
}

[[nodiscard]] __std_fs_copy_file_result __stdcall __std_fs_copy_file(_In_z_ const wchar_t* const _Source,
    _In_z_ const wchar_t* const _Target, _In_ __std_fs_copy_options _Options) noexcept { // copy _Source to _Target
    _Options &= __std_fs_copy_options::_Existing_mask;
    if (_Options != __std_fs_copy_options::_Overwrite_existing) {
        const __std_fs_copy_file_result _First_try_result =
            __vcp_Copyfile(_Source, _Target, /* _Fail_if_exists = */ true);
        if (_First_try_result._Error != __std_win_error::_File_exists // successful copy or I/O error
            || _Options == __std_fs_copy_options::_None) { // caller requested fail if exists behavior
            return _First_try_result;
        }

        // At this point, the target exists, and we are `skip_existing` or `update_existing`.
        // To resolve either, we need to open handles to test `equivalent()` and `last_write_time()`.
        // We allow other programs to have these files open in read-only mode,
        // since that doesn't affect the last-write-check.
        // We also allow `FILE_SHARE_WRITE` when `skip_existing`, since that doesn't affect anything.
        {
            DWORD _Share_mode = FILE_SHARE_READ;
            if (_Options == __std_fs_copy_options::_Skip_existing) {
                _Share_mode |= FILE_SHARE_WRITE;
            }

            const _STD _Fs_file _Source_handle(
                __vcp_CreateFile(_Source, FILE_READ_ATTRIBUTES, _Share_mode, nullptr, OPEN_EXISTING, 0, nullptr));
            __std_win_error _Last_error = _Translate_CreateFile_last_error(_Source_handle._Get());
            if (_Last_error != __std_win_error::_Success) {
                return {false, _Last_error};
            }

            const _STD _Fs_file _Target_handle(
                __vcp_CreateFile(_Target, FILE_READ_ATTRIBUTES, _Share_mode, nullptr, OPEN_EXISTING, 0, nullptr));
            _Last_error = _Translate_CreateFile_last_error(_Target_handle._Get());
            if (_Last_error != __std_win_error::_Success) {
                return {false, _Last_error};
            }

            bool _Do_copy = false;
            if (_Options == __std_fs_copy_options::_Update_existing) {
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

                if (_Target_last_write_time < _Source_last_write_time) { // _Source is newer, so update_existing
                    _Do_copy = true;
                }
            }

            if (!_Do_copy) {
                // We only need to test `equivalent()` if we _aren't_ going to `CopyFileW()`,
                // since that call will fail with an `ERROR_SHARING_VIOLATION` anyways.
                FILE_ID_INFO _Source_id;
                _Last_error = _Get_file_id_by_handle(_Source_handle._Get(), &_Source_id);
                if (_Last_error != __std_win_error::_Success) {
                    return {false, _Last_error};
                }
                FILE_ID_INFO _Target_id;
                _Last_error = _Get_file_id_by_handle(_Target_handle._Get(), &_Target_id);
                if (_Last_error != __std_win_error::_Success) {
                    return {false, _Last_error};
                }

                if (_CSTD memcmp(&_Source_id, &_Target_id, sizeof(FILE_ID_INFO)) == 0) {
                    // the files are equivalent
                    return {false, __std_win_error::_Sharing_violation};
                }

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

_Success_(return == __std_win_error::_Success) __std_win_error
    __stdcall __std_fs_get_file_id(_Out_ __std_fs_file_id* const _Id, _In_z_ const wchar_t* const _Path) noexcept {
    __std_win_error _Last_error;
    const _STD _Fs_file _Handle(
        _Path, __std_access_rights::_File_read_attributes, __std_fs_file_flags::_Backup_semantics, &_Last_error);
    if (_Last_error != __std_win_error::_Success) {
        return _Last_error;
    }

    static_assert(sizeof(FILE_ID_INFO) == sizeof(__std_fs_file_id));
    static_assert(alignof(FILE_ID_INFO) == alignof(__std_fs_file_id));
    return _Get_file_id_by_handle(_Handle._Get(), reinterpret_cast<FILE_ID_INFO*>(_Id));
}

[[nodiscard]] __std_win_error __stdcall __std_fs_create_directory_symbolic_link(
    _In_z_ const wchar_t* const _Symlink_file_name, _In_z_ const wchar_t* const _Target_file_name) noexcept {
    return _Create_symlink(_Symlink_file_name, _Target_file_name, SYMBOLIC_LINK_FLAG_DIRECTORY);
}

[[nodiscard]] __std_win_error __stdcall __std_fs_create_hard_link(
    _In_z_ const wchar_t* const _File_name, _In_z_ const wchar_t* const _Existing_file_name) noexcept {
#if defined(_CRT_APP)
    (void) _File_name;
    (void) _Existing_file_name;
    return __std_win_error::_Not_supported;
#else // ^^^ defined(_CRT_APP) ^^^ // vvv !defined(_CRT_APP) vvv
    if (CreateHardLinkW(_File_name, _Existing_file_name, nullptr)) {
        return __std_win_error::_Success;
    }

    return __std_win_error{GetLastError()};
#endif // defined(_CRT_APP)
}

[[nodiscard]] __std_win_error __stdcall __std_fs_create_symbolic_link(
    _In_z_ const wchar_t* const _Symlink_file_name, _In_z_ const wchar_t* const _Target_file_name) noexcept {
    return _Create_symlink(_Symlink_file_name, _Target_file_name, 0);
}

[[nodiscard]] __std_win_error __stdcall __std_fs_read_reparse_data_buffer(_In_ const __std_fs_file_handle _Handle,
    _Out_writes_bytes_(_Buffer_size) void* const _Buffer, _In_ const unsigned long _Buffer_size) noexcept {
    unsigned long _Bytes_returned;
    if (DeviceIoControl(reinterpret_cast<HANDLE>(_Handle), FSCTL_GET_REPARSE_POINT, nullptr, 0, _Buffer, _Buffer_size,
            &_Bytes_returned, nullptr)) {
        return __std_win_error::_Success;
    }

    // If DeviceIoControl fails, _Bytes_returned is 0.
    return __std_win_error{GetLastError()};
}

[[nodiscard]] __std_win_error __stdcall __std_fs_write_reparse_data_buffer(
    _In_ const __std_fs_file_handle _Handle, _In_ const __std_fs_reparse_data_buffer* const _Buffer) noexcept {
    if (DeviceIoControl(reinterpret_cast<HANDLE>(_Handle), FSCTL_SET_REPARSE_POINT,
            const_cast<__std_fs_reparse_data_buffer*>(_Buffer), sizeof(_Buffer) + _Buffer->_Reparse_data_length,
            nullptr, 0, nullptr, nullptr)) {
        return __std_win_error::_Success;
    }

    // If DeviceIoControl fails, _Bytes_returned is 0.
    return __std_win_error{GetLastError()};
}

[[nodiscard]] bool __stdcall __std_fs_is_junction_from_reparse_data_buffer(
    _In_ const __std_fs_reparse_data_buffer* const _Buffer) noexcept {
    return _Buffer->_Reparse_tag == IO_REPARSE_TAG_MOUNT_POINT;
}

[[nodiscard]] _Success_(return == __std_win_error::_Success) __std_win_error
    __stdcall __std_fs_read_name_from_reparse_data_buffer(_In_ __std_fs_reparse_data_buffer* const _Buffer,
        _Out_ wchar_t** const _Offset, _Out_ unsigned short* const _Length) noexcept {
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
    } else if (_Buffer->_Reparse_tag == IO_REPARSE_TAG_MOUNT_POINT) {
        // junction
        auto& _Junction_buffer            = _Buffer->_Mount_point_reparse_buffer;
        const unsigned short _Temp_length = _Junction_buffer._Print_name_length / sizeof(wchar_t);

        if (_Temp_length == 0) {
            *_Length = _Junction_buffer._Substitute_name_length / sizeof(wchar_t);
            *_Offset = &_Junction_buffer._Path_buffer[_Junction_buffer._Substitute_name_offset / sizeof(wchar_t)];
        } else {
            *_Length = _Temp_length;
            *_Offset = &_Junction_buffer._Path_buffer[_Junction_buffer._Print_name_offset / sizeof(wchar_t)];
        }
    } else {
        return __std_win_error::_Reparse_tag_invalid;
    }

    return __std_win_error::_Success;
}

[[nodiscard]] __std_win_error __stdcall __std_fs_set_last_write_time(
    _In_ const long long _Last_write_filetime, _In_z_ const wchar_t* const _Path) noexcept {
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

[[nodiscard]] __std_fs_remove_result __stdcall __std_fs_remove(_In_z_ const wchar_t* const _Target) noexcept {
    // remove _Target without caring whether _Target is a file or directory
    __std_win_error _Last_error;
    bool _Able_to_change_attributes = false;

    constexpr auto _Flags = __std_fs_file_flags::_Backup_semantics | __std_fs_file_flags::_Open_reparse_point;
    _STD _Fs_file _Handle(_Target,
        __std_access_rights::_Delete | __std_access_rights::_File_read_attributes
            | __std_access_rights::_File_write_attributes,
        _Flags, &_Last_error);
    if (_Last_error == __std_win_error::_Success) {
        _Able_to_change_attributes = true;
    } else if (_Last_error == __std_win_error::_Access_denied) {
        // change the underlying HANDLE of _Handle
        _Last_error = __std_fs_open_handle(&_Handle._Raw, _Target, __std_access_rights::_Delete, _Flags);
        if (_Last_error != __std_win_error::_Success) {
            return {false, _Last_error};
        }
    } else {
        return {false, _Translate_not_found_to_success(_Last_error)};
    }

    // See minwinbase.h and WinBase.h.
    // Windows 10 1809 added support for FILE_DISPOSITION_FLAG_IGNORE_READONLY_ATTRIBUTE.
    FILE_DISPOSITION_INFO_EX _Info_ex{FILE_DISPOSITION_FLAG_DELETE | FILE_DISPOSITION_FLAG_POSIX_SEMANTICS
                                      | FILE_DISPOSITION_FLAG_IGNORE_READONLY_ATTRIBUTE};
    if (SetFileInformationByHandle(_Handle._Get(), FileDispositionInfoEx, &_Info_ex, sizeof(_Info_ex))) {
        return {true, __std_win_error::_Success};
    }

    _Last_error = __std_win_error{GetLastError()};
    switch (_Last_error) {
    // Windows versions older than 1809
    case __std_win_error::_Invalid_parameter:
    case __std_win_error::_Invalid_function:
    case __std_win_error::_Not_supported:
        break;
    default:
        // If we fail for reasons other than those mentioned above just return to the caller
        return {false, _Last_error};
    }

    // code path for versions older than 1809
    _Last_error = _Set_delete_flag(_Handle._Raw);
    if (_Last_error == __std_win_error::_Success) {
        return {true, __std_win_error::_Success};
    }

    if (_Last_error == __std_win_error::_Access_denied && _Able_to_change_attributes) {

        FILE_BASIC_INFO _Basic_info;
        if (!GetFileInformationByHandleEx(_Handle._Get(), FileBasicInfo, &_Basic_info, sizeof(_Basic_info))) {
            return {false, __std_win_error{GetLastError()}};
        }
        // check if FILE_ATTRIBUTE_READONLY is set
        if ((_Basic_info.FileAttributes & FILE_ATTRIBUTE_READONLY) != 0u) {
            // try to remove FILE_ATTRIBUTE_READONLY
            _Basic_info.FileAttributes ^= FILE_ATTRIBUTE_READONLY;
            if (!SetFileInformationByHandle(_Handle._Get(), FileBasicInfo, &_Basic_info, sizeof(_Basic_info))) {
                return {false, __std_win_error{GetLastError()}};
            }
            // removed FILE_ATTRIBUTE_READONLY, now try to set the delete flag again
            _Last_error = _Set_delete_flag(_Handle._Raw);
            if (_Last_error == __std_win_error::_Success) {
                return {true, __std_win_error::_Success};
            } else if (_Last_error == __std_win_error::_Access_denied) {
                // looks like we failed to set the delete flag, after clearing the FILE_ATTRIBUTE_READONLY flag
                // perform rollback
                _Basic_info.FileAttributes |= FILE_ATTRIBUTE_READONLY;
                if (!SetFileInformationByHandle(_Handle._Get(), FileBasicInfo, &_Basic_info, sizeof(_Basic_info))) {
                    return {false, __std_win_error{GetLastError()}};
                }
                return {false, _Last_error};
            }
        } else {
            return {false, _Last_error};
        }
    } else {
        return {false, _Last_error};
    }

    return {false, __std_win_error{GetLastError()}};
}

[[nodiscard]] __std_win_error __stdcall __std_fs_change_permissions(
    _In_z_ const wchar_t* const _Path, _In_ const bool _Follow_symlinks, _In_ const bool _Readonly) noexcept {
    const DWORD _Old_attributes = GetFileAttributesW(_Path);
    if (_Old_attributes == INVALID_FILE_ATTRIBUTES) {
        return __std_win_error{GetLastError()};
    }

    const DWORD _Readonly_test = _Readonly ? FILE_ATTRIBUTE_READONLY : 0;
    if ((_Old_attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0u && _Follow_symlinks) {
        __std_win_error _Err;
        _STD _Fs_file _Handle(_Path,
            __std_access_rights::_File_read_attributes | __std_access_rights::_File_write_attributes,
            __std_fs_file_flags::_Backup_semantics, &_Err);

        if (_Err != __std_win_error::_Success) {
            return _Err;
        }

        FILE_BASIC_INFO _Basic_info;
        if (!GetFileInformationByHandleEx(_Handle._Get(), FileBasicInfo, &_Basic_info, sizeof(_Basic_info))) {
            return __std_win_error{GetLastError()};
        }

        if ((_Basic_info.FileAttributes & FILE_ATTRIBUTE_READONLY) == _Readonly_test) { // nothing to do
            return __std_win_error::_Success;
        }

        _Basic_info.FileAttributes ^= FILE_ATTRIBUTE_READONLY;
        if (SetFileInformationByHandle(_Handle._Get(), FileBasicInfo, &_Basic_info, sizeof(_Basic_info))) {
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
    _In_z_ const wchar_t* const _Source, _In_z_ const wchar_t* const _Target) noexcept {
    if (MoveFileExW(_Source, _Target, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING)) {
        return __std_win_error::_Success;
    }

    return __std_win_error{GetLastError()};
}

[[nodiscard]] __std_win_error __stdcall __std_fs_resize_file(
    _In_z_ const wchar_t* const _Target, const uintmax_t _New_size) noexcept {
    __std_win_error _Err;
    const _STD _Fs_file _Handle(_Target, __std_access_rights::_File_generic_write, __std_fs_file_flags::_None, &_Err);
    if (_Err != __std_win_error::_Success) {
        return _Err;
    }

    FILE_END_OF_FILE_INFO _File_info;
    _File_info.EndOfFile.QuadPart = static_cast<LONGLONG>(_New_size);

    if (SetFileInformationByHandle(_Handle._Get(), FileEndOfFileInfo, &_File_info, sizeof(_File_info))) {
        return __std_win_error::_Success;
    }

    return __std_win_error{GetLastError()};
}

[[nodiscard]] __std_win_error __stdcall __std_fs_space(_In_z_ const wchar_t* const _Target,
    _Out_ uintmax_t* const _Available, _Out_ uintmax_t* const _Total_bytes,
    _Out_ uintmax_t* const _Free_bytes) noexcept {
    // get capacity information for the volume on which the file _Target resides
    static_assert(sizeof(uintmax_t) == sizeof(ULARGE_INTEGER) && alignof(uintmax_t) == alignof(ULARGE_INTEGER),
        "Size and alignment must match for reinterpret_cast<PULARGE_INTEGER>");
    const auto _Available_c   = reinterpret_cast<PULARGE_INTEGER>(_Available);
    const auto _Total_bytes_c = reinterpret_cast<PULARGE_INTEGER>(_Total_bytes);
    const auto _Free_bytes_c  = reinterpret_cast<PULARGE_INTEGER>(_Free_bytes);
    if (GetDiskFreeSpaceExW(_Target, _Available_c, _Total_bytes_c, _Free_bytes_c)) {
        return __std_win_error::_Success;
    }

    __std_win_error _Last_error{GetLastError()};
    _Available_c->QuadPart   = ~0ull;
    _Total_bytes_c->QuadPart = ~0ull;
    _Free_bytes_c->QuadPart  = ~0ull;
    if (_Last_error != __std_win_error::_Directory_name_is_invalid) {
        return _Last_error;
    }

    // Input could have been a file; canonicalize and remove the last component.
    // We use VOLUME_NAME_NT because it always has a mapping available and we don't care about the canonical path
    // being "ugly" due to needing the _Dos_to_nt_prefix.
    static constexpr wchar_t _Dos_to_nt_prefix[] = LR"(\\?\GLOBALROOT)";
    constexpr size_t _Dos_to_nt_prefix_count     = sizeof(_Dos_to_nt_prefix) / sizeof(wchar_t) - 1;
    __crt_unique_heap_ptr<wchar_t> _Buf;
    DWORD _Actual_length;
    {
        const _STD _Fs_file _Handle(
            _Target, __std_access_rights::_File_read_attributes, __std_fs_file_flags::_Backup_semantics, &_Last_error);
        if (_Last_error != __std_win_error::_Success) {
            return _Last_error;
        }

        DWORD _Buf_count = MAX_PATH;
        for (;;) {
            _Buf = _malloc_crt_t(wchar_t, _Buf_count);
            if (!_Buf) {
                return __std_win_error::_Not_enough_memory;
            }

            _Actual_length = GetFinalPathNameByHandleW(_Handle._Get(), _Buf.get() + _Dos_to_nt_prefix_count,
                _Buf_count - _Dos_to_nt_prefix_count, FILE_NAME_NORMALIZED | VOLUME_NAME_NT);
            if (_Actual_length == 0) {
                return __std_win_error{GetLastError()};
            }

            _Actual_length += _Dos_to_nt_prefix_count;
            if (_Actual_length <= _Buf_count) {
                break;
            }

            _Buf_count = _Actual_length;
        }
    } // close _Handle

    const auto _Ptr = _Buf.get();

    memcpy(_Ptr, _Dos_to_nt_prefix, _Dos_to_nt_prefix_count * sizeof(wchar_t));

    // insert null terminator at the last slash
    auto _Cursor = _Ptr + _Actual_length;
    do {
        --_Cursor; // cannot run off start because _Dos_to_nt_prefix contains a backslash
    } while (*_Cursor != L'\\');

    *_Cursor = L'\0';
    if (GetDiskFreeSpaceExW(_Ptr, _Available_c, _Total_bytes_c, _Free_bytes_c)) {
        return __std_win_error::_Success;
    }

    _Available_c->QuadPart   = ~0ull;
    _Total_bytes_c->QuadPart = ~0ull;
    _Free_bytes_c->QuadPart  = ~0ull;
    return __std_win_error{GetLastError()};
}

namespace {
    _Success_(return > 0 && return < nBufferLength) DWORD WINAPI
        _Stl_GetTempPath2W(_In_ DWORD nBufferLength, _Out_writes_to_opt_(nBufferLength, return +1) LPWSTR lpBuffer) {
        // See GH-3011: This is intentionally not attempting to cache the function pointer.
        // TRANSITION, ABI: This should use __crtGetTempPath2W after this code is moved into the STL's DLL.
        using _Fun_ptr = decltype(&::GetTempPath2W);

        const auto _Kernel32 = ::GetModuleHandleW(L"kernel32.dll");
        _Analysis_assume_(_Kernel32);
        _Fun_ptr _PfGetTempPath2W = reinterpret_cast<_Fun_ptr>(::GetProcAddress(_Kernel32, "GetTempPath2W"));
        if (!_PfGetTempPath2W) {
            _PfGetTempPath2W = &::GetTempPathW;
        }

        return _PfGetTempPath2W(nBufferLength, lpBuffer);
    }
} // unnamed namespace

[[nodiscard]] _Success_(return._Error == __std_win_error::_Success) __std_ulong_and_error
    __stdcall __std_fs_get_temp_path(_Out_writes_z_(__std_fs_temp_path_max) wchar_t* const _Target) noexcept {
    // calls GetTempPath2W if available (Win11+), else calls GetTempPathW
    // If getting the path failed, returns 0 size; otherwise, returns the size of the
    // expected directory. If the path could be resolved to an existing directory,
    // returns __std_win_error::_Success; otherwise, returns __std_win_error::_Max.
    const auto _Size = _Stl_GetTempPath2W(__std_fs_temp_path_max, _Target);
    if (_Size == 0) {
        return {0, __std_win_error{GetLastError()}};
    }

    // Effects: If exists(p) is false or is_directory(p) is false, an error is reported
    const DWORD _Attributes = GetFileAttributesW(_Target);
    if (_Attributes == INVALID_FILE_ATTRIBUTES || (_Attributes & FILE_ATTRIBUTE_DIRECTORY) == 0u) {
        return {_Size, __std_win_error::_Max};
    }

    if ((_Attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0u) {
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

[[nodiscard]] _Success_(return == __std_win_error::_Success) __std_win_error
    __stdcall __std_fs_get_stats(_In_z_ const wchar_t* const _Path, __std_fs_stats* const _Stats,
        _In_ __std_fs_stats_flags _Flags, _In_ const __std_fs_file_attr _Symlink_attribute_hint) noexcept {
    const bool _Follow_symlinks = _STD _Bitmask_includes_any(_Flags, __std_fs_stats_flags::_Follow_symlinks);
    _Flags &= ~__std_fs_stats_flags::_Follow_symlinks;
    if (_Follow_symlinks && _STD _Bitmask_includes_any(_Flags, __std_fs_stats_flags::_Reparse_tag)) {
        return __std_win_error::_Invalid_parameter;
    }

    if (_STD _Bitmask_includes_any(_Flags, __std_fs_stats_flags::_Attributes)
        && _Symlink_attribute_hint != __std_fs_file_attr::_Invalid
        && (!_STD _Bitmask_includes_any(_Symlink_attribute_hint, __std_fs_file_attr::_Reparse_point)
            || !_Follow_symlinks)) { // if the hint can't be a symlink, we already have the attributes
        _Flags &= ~__std_fs_stats_flags::_Attributes;
        _Stats->_Attributes = _Symlink_attribute_hint;
    }

    if (_Flags == __std_fs_stats_flags::_None) { // no more data to get, report success
        return __std_win_error::_Success;
    }

    constexpr auto _Get_file_attributes_data =
        __std_fs_stats_flags::_Attributes | __std_fs_stats_flags::_File_size | __std_fs_stats_flags::_Last_write_time;
    if (_STD _Bitmask_includes_any(_Flags, _Get_file_attributes_data)) {
        // caller wants something GetFileAttributesExW/FindFirstFileW might provide
        if (_Symlink_attribute_hint == __std_fs_file_attr::_Invalid
            || !_STD _Bitmask_includes_any(_Symlink_attribute_hint, __std_fs_file_attr::_Reparse_point)
            || !_Follow_symlinks) { // we might not be a symlink or not following symlinks, so
                                    // GetFileAttributesExW/FindFirstFileW would return the right answer

            WIN32_FILE_ATTRIBUTE_DATA _Data;
            if (!GetFileAttributesExW(_Path, GetFileExInfoStandard, &_Data)) {
                // In some cases, ERROR_SHARING_VIOLATION is returned from GetFileAttributesExW;
                // FindFirstFileW will work in those cases if we have read permissions on the directory.
                if (const __std_win_error _Last_error{GetLastError()};
                    _Last_error != __std_win_error::_Sharing_violation) {
                    return _Last_error;
                }

                // Note that FindFirstFileW does allow globbing characters and has extra behavior with them
                // that we don't want. However, GetFileAttributesExW would've failed with ERROR_INVALID_NAME
                // if there were any globbing characters in _Path.
                WIN32_FIND_DATAW _Find_data;
                {
                    HANDLE _Find_handle = FindFirstFileW(_Path, &_Find_data);
                    if (_Find_handle == INVALID_HANDLE_VALUE) {
                        return __std_win_error{GetLastError()};
                    }
                    FindClose(_Find_handle);
                }

                _Data.dwFileAttributes = _Find_data.dwFileAttributes;
                _Data.nFileSizeHigh    = _Find_data.nFileSizeHigh;
                _Data.nFileSizeLow     = _Find_data.nFileSizeLow;
                _Data.ftLastWriteTime  = _Find_data.ftLastWriteTime;
            }

            const __std_fs_file_attr _Attributes{_Data.dwFileAttributes};
            if (!_Follow_symlinks || !_STD _Bitmask_includes_any(_Attributes, __std_fs_file_attr::_Reparse_point)) {
                // if we aren't following symlinks or can't be a symlink, that data was useful, record
                _Stats->_Attributes      = _Attributes;
                _Stats->_File_size       = _Merge_to_ull(_Data.nFileSizeHigh, _Data.nFileSizeLow);
                _Stats->_Last_write_time = static_cast<long long>(
                    _Merge_to_ull(_Data.ftLastWriteTime.dwHighDateTime, _Data.ftLastWriteTime.dwLowDateTime));

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

    constexpr auto _Basic_info_data    = __std_fs_stats_flags::_Attributes | __std_fs_stats_flags::_Last_write_time;
    constexpr auto _Standard_info_data = __std_fs_stats_flags::_File_size | __std_fs_stats_flags::_Link_count;

    if (_STD _Bitmask_includes_any(_Flags, _Basic_info_data | __std_fs_stats_flags::_Reparse_tag)) {
        FILE_BASIC_INFO _Info;
        if (!GetFileInformationByHandleEx(_Handle._Get(), FileBasicInfo, &_Info, sizeof(_Info))) {
            return __std_win_error{GetLastError()};
        }

        _Stats->_Attributes      = __std_fs_file_attr{_Info.FileAttributes};
        _Stats->_Last_write_time = _Info.LastWriteTime.QuadPart;
        _Flags &= ~_Basic_info_data;
        if (_STD _Bitmask_includes_any(_Flags, __std_fs_stats_flags::_Reparse_tag)) {
            // Calling GetFileInformationByHandleEx with FileAttributeTagInfo fails on FAT file system with
            // ERROR_INVALID_PARAMETER. We avoid calling this for non-reparse-points.
            if ((_Info.FileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0u) {
                FILE_ATTRIBUTE_TAG_INFO _TagInfo;
                if (!GetFileInformationByHandleEx(_Handle._Get(), FileAttributeTagInfo, &_TagInfo, sizeof(_TagInfo))) {
                    return __std_win_error{GetLastError()};
                }

                _Stats->_Reparse_point_tag = __std_fs_reparse_tag{_TagInfo.ReparseTag};
            } else {
                _Stats->_Reparse_point_tag = __std_fs_reparse_tag::_None;
            }
            _Flags &= ~__std_fs_stats_flags::_Reparse_tag;
        }
    }

    if (_STD _Bitmask_includes_any(_Flags, _Standard_info_data)) {
        FILE_STANDARD_INFO _Info;
        if (!GetFileInformationByHandleEx(_Handle._Get(), FileStandardInfo, &_Info, sizeof(_Info))) {
            return __std_win_error{GetLastError()};
        }

        _Stats->_File_size  = _Info.EndOfFile.QuadPart;
        _Stats->_Link_count = _Info.NumberOfLinks;
        _Flags &= ~_Standard_info_data;
    }

    if (_Flags == __std_fs_stats_flags::_None) { // no more data to get, report success
        return __std_win_error::_Success;
    }

    // somehow the caller asked for more data than we yet know about, maybe new headers
    // targeting old separately compiled bits
    return __std_win_error::_Not_supported;
}

[[nodiscard]] __std_fs_create_directory_result __stdcall __std_fs_create_directory(
    _In_z_ const wchar_t* const _New_directory) noexcept {
    if (CreateDirectoryW(_New_directory, nullptr)) {
        return {true, __std_win_error::_Success};
    }

    __std_win_error _Last_error{GetLastError()};
    if (_Last_error == __std_win_error::_Already_exists) {
        __std_fs_stats _Stats;
        _Last_error = __std_fs_get_stats(
            _New_directory, &_Stats, __std_fs_stats_flags::_Attributes | __std_fs_stats_flags::_Follow_symlinks);
        if (_Last_error == __std_win_error::_Success
            && !_STD _Bitmask_includes_any(_Stats._Attributes, __std_fs_file_attr::_Directory)) {
            _Last_error = __std_win_error::_Already_exists;
        }
    }

    return {false, _Last_error};
}

// TRANSITION, ABI: __std_fs_create_directory_template() is preserved for binary compatibility
[[nodiscard]] __std_fs_create_directory_result __stdcall __std_fs_create_directory_template(
    _In_z_ const wchar_t* const _Template_directory, _In_z_ const wchar_t* const _New_directory) noexcept {
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

[[nodiscard]] _Success_(return._Error == __std_win_error::_Success) __std_ulong_and_error
    __stdcall __std_fs_get_current_path(
        _In_ const unsigned long _Target_size, _Out_writes_z_(_Target_size) wchar_t* const _Target) noexcept {
    // If getting the path failed, GetCurrentDirectoryW returns 0; otherwise, returns the size of the expected
    // directory.
    const auto _Size = GetCurrentDirectoryW(_Target_size, _Target);
    if (_Size != 0 && _Size <= _Target_size) {
        return {_Size, __std_win_error::_Success};
    }

    return {_Size, __std_win_error{GetLastError()}};
}

[[nodiscard]] __std_win_error __stdcall __std_fs_set_current_path(_In_z_ const wchar_t* const _Target) noexcept {
    // If setting the path failed, SetCurrentDirectoryW returns 0; otherwise returns non-zero.
    if (SetCurrentDirectoryW(_Target)) {
        return __std_win_error::_Success;
    }

    return __std_win_error{GetLastError()};
}

_END_EXTERN_C
