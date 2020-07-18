// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// system_error message mapping

#include <system_error>

#include <Windows.h>

// TRANSITION, MSBuild
// MSBuild has a hard requirement against including the same file in both a DLL and its import lib, so we include
// the import lib .cpp here to make those functions available for internal use by other parts of our DLL.
#ifdef _DLL
#include "syserror_import_lib.cpp"
#endif

_STD_BEGIN
struct _Win_errtab_t { // maps Windows error to Posix error
    int _Windows;
    errc _Posix;
};

static constexpr _Win_errtab_t _Win_errtab[] = {
    // table of Windows/Posix pairs
    {ERROR_ACCESS_DENIED, errc::permission_denied},
    {ERROR_ALREADY_EXISTS, errc::file_exists},
    {ERROR_BAD_NETPATH, errc::no_such_file_or_directory},
    {ERROR_BAD_UNIT, errc::no_such_device},
    {ERROR_BROKEN_PIPE, errc::broken_pipe},
    {ERROR_BUFFER_OVERFLOW, errc::filename_too_long},
    {ERROR_BUSY, errc::device_or_resource_busy},
    {ERROR_BUSY_DRIVE, errc::device_or_resource_busy},
    {ERROR_CANNOT_MAKE, errc::permission_denied},
    {ERROR_CANTOPEN, errc::io_error},
    {ERROR_CANTREAD, errc::io_error},
    {ERROR_CANTWRITE, errc::io_error},
    {ERROR_CURRENT_DIRECTORY, errc::permission_denied},
    {ERROR_DEV_NOT_EXIST, errc::no_such_device},
    {ERROR_DEVICE_IN_USE, errc::device_or_resource_busy},
    {ERROR_DIR_NOT_EMPTY, errc::directory_not_empty},
    {ERROR_DIRECTORY, errc::invalid_argument},
    {ERROR_DISK_FULL, errc::no_space_on_device},
    {ERROR_FILE_EXISTS, errc::file_exists},
    {ERROR_FILE_NOT_FOUND, errc::no_such_file_or_directory},
    {ERROR_HANDLE_DISK_FULL, errc::no_space_on_device},
    {ERROR_INVALID_ACCESS, errc::permission_denied},
    {ERROR_INVALID_DRIVE, errc::no_such_device},
    {ERROR_INVALID_FUNCTION, errc::function_not_supported},
    {ERROR_INVALID_HANDLE, errc::invalid_argument},
    {ERROR_INVALID_NAME, errc::no_such_file_or_directory},
    {ERROR_INVALID_PARAMETER, errc::invalid_argument},
    {ERROR_LOCK_VIOLATION, errc::no_lock_available},
    {ERROR_LOCKED, errc::no_lock_available},
    {ERROR_NEGATIVE_SEEK, errc::invalid_argument},
    {ERROR_NOACCESS, errc::permission_denied},
    {ERROR_NOT_ENOUGH_MEMORY, errc::not_enough_memory},
    {ERROR_NOT_READY, errc::resource_unavailable_try_again},
    {ERROR_NOT_SAME_DEVICE, errc::cross_device_link},
    {ERROR_NOT_SUPPORTED, errc::not_supported},
    {ERROR_OPEN_FAILED, errc::io_error},
    {ERROR_OPEN_FILES, errc::device_or_resource_busy},
    {ERROR_OPERATION_ABORTED, errc::operation_canceled},
    {ERROR_OUTOFMEMORY, errc::not_enough_memory},
    {ERROR_PATH_NOT_FOUND, errc::no_such_file_or_directory},
    {ERROR_READ_FAULT, errc::io_error},
    {ERROR_REPARSE_TAG_INVALID, errc::invalid_argument},
    {ERROR_RETRY, errc::resource_unavailable_try_again},
    {ERROR_SEEK, errc::io_error},
    {ERROR_SHARING_VIOLATION, errc::permission_denied},
    {ERROR_TOO_MANY_OPEN_FILES, errc::too_many_files_open},
    {ERROR_WRITE_FAULT, errc::io_error},
    {ERROR_WRITE_PROTECT, errc::permission_denied},
    {WSAEACCES, errc::permission_denied},
    {WSAEADDRINUSE, errc::address_in_use},
    {WSAEADDRNOTAVAIL, errc::address_not_available},
    {WSAEAFNOSUPPORT, errc::address_family_not_supported},
    {WSAEALREADY, errc::connection_already_in_progress},
    {WSAEBADF, errc::bad_file_descriptor},
    {WSAECONNABORTED, errc::connection_aborted},
    {WSAECONNREFUSED, errc::connection_refused},
    {WSAECONNRESET, errc::connection_reset},
    {WSAEDESTADDRREQ, errc::destination_address_required},
    {WSAEFAULT, errc::bad_address},
    {WSAEHOSTUNREACH, errc::host_unreachable},
    {WSAEINPROGRESS, errc::operation_in_progress},
    {WSAEINTR, errc::interrupted},
    {WSAEINVAL, errc::invalid_argument},
    {WSAEISCONN, errc::already_connected},
    {WSAEMFILE, errc::too_many_files_open},
    {WSAEMSGSIZE, errc::message_size},
    {WSAENAMETOOLONG, errc::filename_too_long},
    {WSAENETDOWN, errc::network_down},
    {WSAENETRESET, errc::network_reset},
    {WSAENETUNREACH, errc::network_unreachable},
    {WSAENOBUFS, errc::no_buffer_space},
    {WSAENOPROTOOPT, errc::no_protocol_option},
    {WSAENOTCONN, errc::not_connected},
    {WSAENOTSOCK, errc::not_a_socket},
    {WSAEOPNOTSUPP, errc::operation_not_supported},
    {WSAEPROTONOSUPPORT, errc::protocol_not_supported},
    {WSAEPROTOTYPE, errc::wrong_protocol_type},
    {WSAETIMEDOUT, errc::timed_out},
    {WSAEWOULDBLOCK, errc::operation_would_block},
};

_CRTIMP2_PURE int __CLRCALL_PURE_OR_CDECL _Winerror_map(int _Errcode) {
    // convert Windows error to Posix error if possible, otherwise 0
    for (const auto& _Entry : _Win_errtab) {
        if (_Entry._Windows == _Errcode) {
            return static_cast<int>(_Entry._Posix);
        }
    }

    return 0;
}

// TRANSITION, ABI: _Winerror_message() is preserved for binary compatibility
_CRTIMP2_PURE unsigned long __CLRCALL_PURE_OR_CDECL _Winerror_message(
    unsigned long _Message_id, char* _Narrow, unsigned long _Size) {
    // convert to name of Windows error, return 0 for failure, otherwise return number of chars written
    // pre: _Size < INT_MAX
    const unsigned long _Chars = FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, _Message_id, 0, _Narrow, _Size, nullptr);

    return static_cast<unsigned long>(_CSTD __std_get_string_size_without_trailing_whitespace(_Narrow, _Chars));
}

struct _Sys_errtab_t { // maps error_code to NTBS
    errc _Errcode;
    const char* _Name;
};

static constexpr _Sys_errtab_t _Sys_errtab[] = {
    // table of Posix code/name pairs
    {errc::address_family_not_supported, "address family not supported"},
    {errc::address_in_use, "address in use"},
    {errc::address_not_available, "address not available"},
    {errc::already_connected, "already connected"},
    {errc::argument_list_too_long, "argument list too long"},
    {errc::argument_out_of_domain, "argument out of domain"},
    {errc::bad_address, "bad address"},
    {errc::bad_file_descriptor, "bad file descriptor"},
    {errc::bad_message, "bad message"},
    {errc::broken_pipe, "broken pipe"},
    {errc::connection_aborted, "connection aborted"},
    {errc::connection_already_in_progress, "connection already in progress"},
    {errc::connection_refused, "connection refused"},
    {errc::connection_reset, "connection reset"},
    {errc::cross_device_link, "cross device link"},
    {errc::destination_address_required, "destination address required"},
    {errc::device_or_resource_busy, "device or resource busy"},
    {errc::directory_not_empty, "directory not empty"},
    {errc::executable_format_error, "executable format error"},
    {errc::file_exists, "file exists"},
    {errc::file_too_large, "file too large"},
    {errc::filename_too_long, "filename too long"},
    {errc::function_not_supported, "function not supported"},
    {errc::host_unreachable, "host unreachable"},
    {errc::identifier_removed, "identifier removed"},
    {errc::illegal_byte_sequence, "illegal byte sequence"},
    {errc::inappropriate_io_control_operation, "inappropriate io control operation"},
    {errc::interrupted, "interrupted"},
    {errc::invalid_argument, "invalid argument"},
    {errc::invalid_seek, "invalid seek"},
    {errc::io_error, "io error"},
    {errc::is_a_directory, "is a directory"},
    {errc::message_size, "message size"},
    {errc::network_down, "network down"},
    {errc::network_reset, "network reset"},
    {errc::network_unreachable, "network unreachable"},
    {errc::no_buffer_space, "no buffer space"},
    {errc::no_child_process, "no child process"},
    {errc::no_link, "no link"},
    {errc::no_lock_available, "no lock available"},
    {errc::no_message_available, "no message available"},
    {errc::no_message, "no message"},
    {errc::no_protocol_option, "no protocol option"},
    {errc::no_space_on_device, "no space on device"},
    {errc::no_stream_resources, "no stream resources"},
    {errc::no_such_device_or_address, "no such device or address"},
    {errc::no_such_device, "no such device"},
    {errc::no_such_file_or_directory, "no such file or directory"},
    {errc::no_such_process, "no such process"},
    {errc::not_a_directory, "not a directory"},
    {errc::not_a_socket, "not a socket"},
    {errc::not_a_stream, "not a stream"},
    {errc::not_connected, "not connected"},
    {errc::not_enough_memory, "not enough memory"},
    {errc::not_supported, "not supported"},
    {errc::operation_canceled, "operation canceled"},
    {errc::operation_in_progress, "operation in progress"},
    {errc::operation_not_permitted, "operation not permitted"},
    {errc::operation_not_supported, "operation not supported"},
    {errc::operation_would_block, "operation would block"},
    {errc::owner_dead, "owner dead"},
    {errc::permission_denied, "permission denied"},
    {errc::protocol_error, "protocol error"},
    {errc::protocol_not_supported, "protocol not supported"},
    {errc::read_only_file_system, "read only file system"},
    {errc::resource_deadlock_would_occur, "resource deadlock would occur"},
    {errc::resource_unavailable_try_again, "resource unavailable try again"},
    {errc::result_out_of_range, "result out of range"},
    {errc::state_not_recoverable, "state not recoverable"},
    {errc::stream_timeout, "stream timeout"},
    {errc::text_file_busy, "text file busy"},
    {errc::timed_out, "timed out"},
    {errc::too_many_files_open_in_system, "too many files open in system"},
    {errc::too_many_files_open, "too many files open"},
    {errc::too_many_links, "too many links"},
    {errc::too_many_symbolic_link_levels, "too many symbolic link levels"},
    {errc::value_too_large, "value too large"},
    {errc::wrong_protocol_type, "wrong protocol type"},
};

_CRTIMP2_PURE const char* __CLRCALL_PURE_OR_CDECL _Syserror_map(int _Errcode) { // convert to name of generic error
    for (const auto& _Entry : _Sys_errtab) {
        if (static_cast<int>(_Entry._Errcode) == _Errcode) {
            return _Entry._Name;
        }
    }

    return "unknown error";
}
_STD_END
