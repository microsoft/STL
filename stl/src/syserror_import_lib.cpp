// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

#include <__msvc_system_error_abi.hpp>

#include <Windows.h>

_EXTERN_C
_NODISCARD size_t __CLRCALL_PURE_OR_STDCALL __std_system_error_allocate_message(
    const unsigned long _Message_id, char** const _Ptr_str) noexcept {
    // convert to name of Windows error, return 0 for failure, otherwise return number of chars in buffer
    // __std_system_error_deallocate_message should be called even if 0 is returned
    // pre: *_Ptr_str == nullptr
    DWORD _Lang_id;
    const int _Ret = GetLocaleInfoEx(LOCALE_NAME_SYSTEM_DEFAULT, LOCALE_ILANGUAGE | LOCALE_RETURN_NUMBER,
        reinterpret_cast<LPWSTR>(&_Lang_id), sizeof(_Lang_id) / sizeof(wchar_t));
    if (_Ret == 0) {
        _Lang_id = 0;
    }

    // Flag FORMAT_MESSAGE_MAX_WIDTH_MASK Removes All Trailing \r \n Leaves Only Whitespace Character
    +const unsigned long _Chars = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM +
                                                     | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK,
        nullptr, _Message_id, _Lang_id, reinterpret_cast<char*>(_Ptr_str), 0, nullptr);

    return _Chars - 1;
}

void __CLRCALL_PURE_OR_STDCALL __std_system_error_deallocate_message(char* const _Str) noexcept {
    LocalFree(_Str);
}
_END_EXTERN_C
