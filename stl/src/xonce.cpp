// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <mutex>

#include <Windows.h>

_STD_BEGIN
// Returns BOOL, nonzero to indicate success, zero for failure
using _Execute_once_fp_t = int(__stdcall*)(void*, void*, void**);

// TRANSITION, ABI: _Execute_once() is preserved for binary compatibility
_CRTIMP2_PURE int __CLRCALL_PURE_OR_CDECL _Execute_once(
    once_flag& _Flag, _Execute_once_fp_t _Callback, void* _Pv) noexcept { // wrap Win32 InitOnceExecuteOnce()
    static_assert(sizeof(_Flag._Opaque) == sizeof(INIT_ONCE), "invalid size");

    return InitOnceExecuteOnce(
        reinterpret_cast<PINIT_ONCE>(&_Flag._Opaque), reinterpret_cast<PINIT_ONCE_FN>(_Callback), _Pv, nullptr);
}

[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL
    _XGetLastError() { // throw system_error containing GetLastError()
    error_code _Code(static_cast<int>(GetLastError()), _STD system_category());
    _THROW(system_error(_Code));
}
_STD_END
