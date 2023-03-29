// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// filesys.cpp -- <experimental/filesystem> implementation
// (see filesystem.cpp for C++17 <filesystem> implementation)

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <yvals.h>

#include <cstring>
#include <direct.h>
#include <experimental/filesystem>
#include <io.h>

#include <Windows.h>

#ifdef _M_CEE_PURE
#define __crtGetTempPath2W(BufferLength, Buffer) GetTempPathW(BufferLength, Buffer)
#else // vvv !defined(_M_CEE_PURE) vvv
extern "C" _Success_(return > 0 && return < BufferLength) DWORD
    __stdcall __crtGetTempPath2W(_In_ DWORD BufferLength, _Out_writes_to_opt_(BufferLength, return +1) LPWSTR Buffer);
#endif // ^^^ !defined(_M_CEE_PURE) ^^^

_FS_BEGIN
static file_type _Map_mode(int _Mode) { // map Windows file attributes to file_status
    constexpr int _File_attribute_regular =
        FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_COMPRESSED | FILE_ATTRIBUTE_ENCRYPTED | FILE_ATTRIBUTE_HIDDEN
        | FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED | FILE_ATTRIBUTE_OFFLINE | FILE_ATTRIBUTE_READONLY
        | FILE_ATTRIBUTE_SPARSE_FILE | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY;

    if ((_Mode & FILE_ATTRIBUTE_DIRECTORY) != 0) {
        return file_type::directory;
    } else if ((_Mode & _File_attribute_regular) != 0) {
        return file_type::regular;
    } else {
        return file_type::unknown;
    }
}

_FS_DLL void __CLRCALL_PURE_OR_CDECL _Close_dir(void* _Handle) { // close a directory
    FindClose(_Handle);
}

static wchar_t* _Strcpy(wchar_t (&_Dest)[_MAX_FILESYS_NAME], const wchar_t* _Src) { // copy an NTCTS
    ::wcscpy_s(_Dest, _MAX_FILESYS_NAME, _Src);
    return _Dest;
}

static HANDLE _FilesysOpenFile(const wchar_t* _Fname, DWORD _Desired_access, DWORD _Flags) {
#ifdef _CRT_APP
    CREATEFILE2_EXTENDED_PARAMETERS _Create_file_parameters = {};
    _Create_file_parameters.dwSize                          = sizeof(_Create_file_parameters);
    _Create_file_parameters.dwFileFlags                     = _Flags;

    return CreateFile2(_Fname, _Desired_access, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, OPEN_EXISTING,
        &_Create_file_parameters);
#else // _CRT_APP
    return CreateFileW(_Fname, _Desired_access, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
        OPEN_EXISTING, _Flags, nullptr);
#endif // _CRT_APP
}

_FS_DLL wchar_t* __CLRCALL_PURE_OR_CDECL _Read_dir(
    wchar_t (&_Dest)[_MAX_FILESYS_NAME], void* _Handle, file_type& _Ftype) { // read a directory entry
    WIN32_FIND_DATAW _Dentry;

    while (FindNextFileW(_Handle, &_Dentry)) {
        if (_Dentry.cFileName[0] != L'.'
            || (_Dentry.cFileName[1] != L'\0'
                && (_Dentry.cFileName[1] != L'.'
                    || _Dentry.cFileName[2] != L'\0'))) { // not "." or "..", get file type and return name
            _Ftype = _Map_mode(_Dentry.dwFileAttributes);
            return _Strcpy(_Dest, &_Dentry.cFileName[0]);
        }
    }

    _Ftype = file_type::unknown;
    return _Strcpy(_Dest, L"");
}

static unsigned int _Filesys_code_page() { // determine appropriate code page
#if defined(_ONECORE)
    return CP_ACP;
#else // defined(_ONECORE)
    if (AreFileApisANSI()) {
        return CP_ACP;
    } else {
        return CP_OEMCP;
    }
#endif // defined(_ONECORE)
}

_FS_DLL int __CLRCALL_PURE_OR_CDECL _To_wide(const char* _Bsrc, wchar_t* _Wdest) {
    // return nonzero on success
    return MultiByteToWideChar(_Filesys_code_page(), 0, _Bsrc, -1, _Wdest, _MAX_FILESYS_NAME);
}

_FS_DLL int __CLRCALL_PURE_OR_CDECL _To_byte(const wchar_t* _Wsrc, char* _Bdest) {
    // return nonzero on success
    return WideCharToMultiByte(_Filesys_code_page(), 0, _Wsrc, -1, _Bdest, _MAX_FILESYS_NAME, nullptr, nullptr);
}

_FS_DLL void* __CLRCALL_PURE_OR_CDECL _Open_dir(
    wchar_t (&_Dest)[_MAX_FILESYS_NAME], const wchar_t* _Dirname, int& _Errno, file_type& _Ftype) {
    // open a directory for reading
    WIN32_FIND_DATAW _Dentry;
    wstring _Wildname(_Dirname);
    if (!_Wildname.empty()) {
        _Wildname.append(L"\\*");
    }

    void* _Handle =
        FindFirstFileExW(_Wildname.c_str(), FindExInfoStandard, &_Dentry, FindExSearchNameMatch, nullptr, 0);
    if (_Handle == INVALID_HANDLE_VALUE) { // report failure
        _Errno = ERROR_BAD_PATHNAME;
        *_Dest = L'\0';
        return nullptr;
    }

    // success, get first directory entry
    _Errno = 0;
    if (_Dentry.cFileName[0] == L'.'
        && (_Dentry.cFileName[1] == L'\0'
            || _Dentry.cFileName[1] == L'.' && _Dentry.cFileName[2] == L'\0')) { // skip "." and ".."
        _Read_dir(_Dest, _Handle, _Ftype);
        if (_Dest[0] == L'\0') {
            // no entries, release handle
            _Close_dir(_Handle);
            return nullptr;
        }
        return _Handle;
    }

    // get file type and return handle
    _Strcpy(_Dest, _Dentry.cFileName);
    _Ftype = _Map_mode(_Dentry.dwFileAttributes);
    return _Handle;
}

_FS_DLL bool __CLRCALL_PURE_OR_CDECL _Current_get(wchar_t (&_Dest)[_MAX_FILESYS_NAME]) {
    // get current working directory
    _Strcpy(_Dest, L"");
#ifdef _CRT_APP
    return false; // no support
#else // _CRT_APP
    return _wgetcwd(_Dest, _MAX_FILESYS_NAME) != nullptr;
#endif // _CRT_APP
}

_FS_DLL bool __CLRCALL_PURE_OR_CDECL _Current_set(const wchar_t* _Dirname) {
    // set current working directory
#ifdef _CRT_APP
    (void) _Dirname;
    return false; // no support
#else // _CRT_APP
    return _wchdir(_Dirname) == 0;
#endif // _CRT_APP
}

_FS_DLL wchar_t* __CLRCALL_PURE_OR_CDECL _Symlink_get(wchar_t (&_Dest)[_MAX_FILESYS_NAME], const wchar_t*) {
    // get symlink -- DUMMY
    _Dest[0] = L'\0';
    return _Dest;
}

_FS_DLL wchar_t* __CLRCALL_PURE_OR_CDECL _Temp_get(wchar_t (&_Dest)[_MAX_FILESYS_NAME]) {
    // get temp directory
    wchar_t _Dentry[MAX_PATH];
    return _Strcpy(_Dest, __crtGetTempPath2W(MAX_PATH, _Dentry) != 0 ? _Dentry : L".");
}

_FS_DLL int __CLRCALL_PURE_OR_CDECL _Make_dir(const wchar_t* _Fname, const wchar_t*) {
    // make a new directory (ignore attributes)
    if (CreateDirectoryW(_Fname, nullptr)) {
        return 1;
    } else if (GetLastError() == ERROR_ALREADY_EXISTS) {
        return 0;
    } else {
        return -1;
    }
}

_FS_DLL bool __CLRCALL_PURE_OR_CDECL _Remove_dir(const wchar_t* _Fname) { // remove a directory
    return _wrmdir(_Fname) != -1;
}

_FS_DLL file_type __CLRCALL_PURE_OR_CDECL _Stat(const wchar_t* _Fname, perms* _Pmode) { // get file status
    WIN32_FILE_ATTRIBUTE_DATA _Data;

    if (GetFileAttributesExW(_Fname, GetFileExInfoStandard, &_Data)) {
        // get file type and return permissions
        if (_Pmode != nullptr) {
            constexpr perms _Write_perms    = perms::owner_write | perms::group_write | perms::others_write;
            constexpr perms _Readonly_perms = perms::all & ~_Write_perms;

            *_Pmode = (_Data.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0u ? _Readonly_perms : perms::all;
        }

        return _Map_mode(_Data.dwFileAttributes);
    }

    // invalid, get error code
    switch (GetLastError()) {
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
    case ERROR_INVALID_NAME:
    case ERROR_INVALID_DRIVE:
    case ERROR_INVALID_PARAMETER:
    case ERROR_BAD_NETPATH:
    case ERROR_BAD_PATHNAME:
        return file_type::not_found;
    default:
        return file_type::unknown;
    }
}

_FS_DLL file_type __CLRCALL_PURE_OR_CDECL _Lstat(const wchar_t* _Fname, perms* _Pmode) {
    // get symlink file status
    return _Stat(_Fname, _Pmode); // symlink not supported
}

_FS_DLL uintmax_t __CLRCALL_PURE_OR_CDECL _Hard_links(const wchar_t* _Fname) {
    // get hard link count
    HANDLE _Handle = _FilesysOpenFile(_Fname, FILE_READ_ATTRIBUTES, FILE_FLAG_BACKUP_SEMANTICS);

    if (_Handle == INVALID_HANDLE_VALUE) {
        return static_cast<uintmax_t>(-1);
    }

#ifdef _CRT_APP
    FILE_STANDARD_INFO _Info = {0};

    // get file info
    const auto _Ok = GetFileInformationByHandleEx(_Handle, FileStandardInfo, &_Info, sizeof(_Info));
    CloseHandle(_Handle);
    return _Ok ? _Info.NumberOfLinks : static_cast<uintmax_t>(-1);
#else // _CRT_APP
    BY_HANDLE_FILE_INFORMATION _Info = {0};

    // get file info
    const auto _Ok = GetFileInformationByHandle(_Handle, &_Info);
    CloseHandle(_Handle);
    return _Ok ? _Info.nNumberOfLinks : static_cast<uintmax_t>(-1);
#endif // _CRT_APP
}

_FS_DLL uintmax_t __CLRCALL_PURE_OR_CDECL _File_size(const wchar_t* _Fname) { // get file size
    WIN32_FILE_ATTRIBUTE_DATA _Data;

    if (GetFileAttributesExW(_Fname, GetFileExInfoStandard, &_Data)) {
        return static_cast<uintmax_t>(_Data.nFileSizeHigh) << 32 | _Data.nFileSizeLow;
    } else {
        return static_cast<uintmax_t>(-1);
    }
}

// 3 centuries with 24 leap years each:
// 1600 is excluded, 1700/1800 are not leap years
// 1 partial century with 17 leap years:
// 1900 is not a leap year
// 1904 is leap year number 1
// 1908 is leap year number 2
// 1968 is leap year number 17

constexpr uint64_t _Win_ticks_per_second = 10000000ULL;
constexpr uint64_t _Win_ticks_from_epoch = ((1970 - 1601) * 365 + 3 * 24 + 17) * 86400ULL * _Win_ticks_per_second;

_FS_DLL int64_t __CLRCALL_PURE_OR_CDECL _Last_write_time(const wchar_t* _Fname) { // get last write time
    WIN32_FILE_ATTRIBUTE_DATA _Data;

    if (!GetFileAttributesExW(_Fname, GetFileExInfoStandard, &_Data)) {
        return -1;
    }

    // success, convert time
    uint64_t _Wtime =
        static_cast<uint64_t>(_Data.ftLastWriteTime.dwHighDateTime) << 32 | _Data.ftLastWriteTime.dwLowDateTime;
    return static_cast<int64_t>(_Wtime - _Win_ticks_from_epoch);
}

_FS_DLL int __CLRCALL_PURE_OR_CDECL _Set_last_write_time(const wchar_t* _Fname, int64_t _When) {
    // set last write time
    HANDLE _Handle = _FilesysOpenFile(_Fname, FILE_WRITE_ATTRIBUTES, FILE_FLAG_BACKUP_SEMANTICS);

    if (_Handle == INVALID_HANDLE_VALUE) {
        return 0;
    }

    // convert to FILETIME and set
    uint64_t _Wtime = static_cast<uint64_t>(_When) + _Win_ticks_from_epoch;
    FILETIME _Ft;
    _Ft.dwLowDateTime  = static_cast<DWORD>(_Wtime & 0xFFFFFFFFUL);
    _Ft.dwHighDateTime = static_cast<DWORD>(_Wtime >> 32);
    int _Result        = SetFileTime(_Handle, nullptr, nullptr, &_Ft);
    CloseHandle(_Handle);
    return _Result;
}

_FS_DLL space_info __CLRCALL_PURE_OR_CDECL _Statvfs(const wchar_t* _Fname) {
    // get space information for volume
    space_info _Ans  = {static_cast<uintmax_t>(-1), static_cast<uintmax_t>(-1), static_cast<uintmax_t>(-1)};
    wstring _Devname = _Fname;

    if (_Devname.empty() || _Devname.back() != L'/' && _Devname.back() != L'\\') {
        _Devname.push_back(L'/');
    }

    _ULARGE_INTEGER _Available;
    _ULARGE_INTEGER _Capacity;
    _ULARGE_INTEGER _Free;

    if (GetDiskFreeSpaceExW(_Devname.c_str(), &_Available, &_Capacity, &_Free)) { // convert values
        _Ans.capacity  = _Capacity.QuadPart;
        _Ans.free      = _Free.QuadPart;
        _Ans.available = _Available.QuadPart;
    }
    return _Ans;
}

_FS_DLL int __CLRCALL_PURE_OR_CDECL _Equivalent(
    const wchar_t* _Fname1, const wchar_t* _Fname2) { // test for equivalent file names
#ifdef _CRT_APP
    _FILE_ID_INFO _Info1 = {0};
    _FILE_ID_INFO _Info2 = {0};
    bool _Ok1            = false;
    bool _Ok2            = false;

    HANDLE _Handle = _FilesysOpenFile(_Fname1, FILE_READ_ATTRIBUTES, FILE_FLAG_BACKUP_SEMANTICS);
    if (_Handle != INVALID_HANDLE_VALUE) { // get file1 info
        _Ok1 = GetFileInformationByHandleEx(_Handle, FileIdInfo, &_Info1, sizeof(_Info1)) != 0;
        CloseHandle(_Handle);
    }

    _Handle = _FilesysOpenFile(_Fname2, FILE_READ_ATTRIBUTES, FILE_FLAG_BACKUP_SEMANTICS);
    if (_Handle != INVALID_HANDLE_VALUE) { // get file2 info
        _Ok2 = GetFileInformationByHandleEx(_Handle, FileIdInfo, &_Info2, sizeof(_Info2)) != 0;
        CloseHandle(_Handle);
    }

    if (!_Ok1 && !_Ok2) {
        return -1;
    } else if (!_Ok1 || !_Ok2) {
        return 0;
    } else { // test existing files for equivalence
        return memcmp(&_Info1, &_Info2, sizeof(_FILE_ID_INFO)) == 0 ? 1 : 0;
    }
#else // _CRT_APP
    BY_HANDLE_FILE_INFORMATION _Info1 = {0};
    BY_HANDLE_FILE_INFORMATION _Info2 = {0};
    bool _Ok1                         = false;
    bool _Ok2                         = false;

    HANDLE _Handle = _FilesysOpenFile(_Fname1, FILE_READ_ATTRIBUTES, FILE_FLAG_BACKUP_SEMANTICS);
    if (_Handle != INVALID_HANDLE_VALUE) { // get file1 info
        _Ok1 = GetFileInformationByHandle(_Handle, &_Info1) != 0;
        CloseHandle(_Handle);
    }

    _Handle = _FilesysOpenFile(_Fname2, FILE_READ_ATTRIBUTES, FILE_FLAG_BACKUP_SEMANTICS);
    if (_Handle != INVALID_HANDLE_VALUE) { // get file2 info
        _Ok2 = GetFileInformationByHandle(_Handle, &_Info2) != 0;
        CloseHandle(_Handle);
    }

    if (!_Ok1 && !_Ok2) {
        return -1;
    } else if (!_Ok1 || !_Ok2) {
        return 0;
    } else { // test existing files for equivalence
        return _Info1.dwVolumeSerialNumber == _Info2.dwVolumeSerialNumber
                    && _Info1.nFileIndexHigh == _Info2.nFileIndexHigh && _Info1.nFileIndexLow == _Info2.nFileIndexLow
                 ? 1
                 : 0;
    }
#endif // _CRT_APP
}

_FS_DLL int __CLRCALL_PURE_OR_CDECL _Link(const wchar_t* _Fname1, const wchar_t* _Fname2) {
    // link _Fname2 to _Fname1
#ifdef _CRT_APP
    (void) _Fname1;
    (void) _Fname2;
    return errno = EDOM; // hardlinks not supported
#else // _CRT_APP
    return CreateHardLinkW(_Fname2, _Fname1, nullptr) ? 0 : GetLastError();
#endif // _CRT_APP
}

_FS_DLL int __CLRCALL_PURE_OR_CDECL _Symlink(const wchar_t* _Fname1, const wchar_t* _Fname2) {
    // link _Fname2 to _Fname1
#ifdef _CRT_APP
    (void) _Fname1;
    (void) _Fname2;
    return errno = EDOM; // symlinks not supported
#else // _CRT_APP
    return CreateSymbolicLinkW(_Fname2, _Fname1, 0) ? 0 : GetLastError();
#endif // _CRT_APP
}

_FS_DLL int __CLRCALL_PURE_OR_CDECL _Rename(const wchar_t* _Fname1, const wchar_t* _Fname2) {
    // rename _Fname1 as _Fname2
    return _wrename(_Fname1, _Fname2) == 0 ? 0 : GetLastError();
}

_FS_DLL int __CLRCALL_PURE_OR_CDECL _Resize(const wchar_t* _Fname, uintmax_t _Newsize) { // change file size

    HANDLE _Handle = _FilesysOpenFile(_Fname, FILE_GENERIC_WRITE, 0);

    if (_Handle == INVALID_HANDLE_VALUE) {
        return GetLastError();
    }

    FILE_END_OF_FILE_INFO _File_info;
    _File_info.EndOfFile.QuadPart = static_cast<LONGLONG>(_Newsize);

    const auto _Ok = SetFileInformationByHandle(_Handle, FileEndOfFileInfo, &_File_info, sizeof(_File_info));

    CloseHandle(_Handle);
    return _Ok ? 0 : GetLastError();
}

_FS_DLL int __CLRCALL_PURE_OR_CDECL _Unlink(const wchar_t* _Fname) { // unlink _Fname
    return _wremove(_Fname) == 0 ? 0 : GetLastError();
}

_FS_DLL int __CLRCALL_PURE_OR_CDECL _Copy_file(const wchar_t* _Fname1, const wchar_t* _Fname2) {
    // copy _Fname1 to _Fname2
#if defined(_ONECORE)
    COPYFILE2_EXTENDED_PARAMETERS _Params = {0};
    _Params.dwSize                        = sizeof(COPYFILE2_EXTENDED_PARAMETERS);
    _Params.dwCopyFlags                   = 0;

    const HRESULT _Copy_result = CopyFile2(_Fname1, _Fname2, &_Params);
    if (SUCCEEDED(_Copy_result)) {
        return 0;
    }

    // take lower bits to undo HRESULT_FROM_WIN32
    return _Copy_result & 0x0000FFFFU;
#else // defined(_ONECORE)
    return CopyFileW(_Fname1, _Fname2, 0) ? 0 : GetLastError();
#endif // defined(_ONECORE)
}

_FS_DLL int __CLRCALL_PURE_OR_CDECL _Chmod(const wchar_t* _Fname, perms _Newmode) {
    // change file mode to _Newmode
    WIN32_FILE_ATTRIBUTE_DATA _Data;

    if (!GetFileAttributesExW(_Fname, GetFileExInfoStandard, &_Data)) {
        return -1;
    }

    // got mode, alter readonly bit
    DWORD _Oldmode = _Data.dwFileAttributes;
    DWORD _Mode    = _Oldmode;

    constexpr perms _Write_perms = perms::owner_write | perms::group_write | perms::others_write;

    if ((_Newmode & _Write_perms) == perms::none) {
        _Mode |= FILE_ATTRIBUTE_READONLY;
    } else {
        _Mode &= ~FILE_ATTRIBUTE_READONLY;
    }

    return _Mode == _Oldmode || SetFileAttributesW(_Fname, _Mode) ? 0 : -1;
}
_FS_END
