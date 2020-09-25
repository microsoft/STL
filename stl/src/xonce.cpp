// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Execute_once function

#include <mutex>

#include "awint.hpp"

_STD_BEGIN
// TRANSITION, ABI
_CRTIMP2_PURE int __CLRCALL_PURE_OR_CDECL _Execute_once(
    once_flag& _Flag, _Execute_once_fp_t _Callback, void* _Pv) noexcept { // wrap Win32 InitOnceExecuteOnce()
    static_assert(sizeof(_Flag._Opaque) == sizeof(INIT_ONCE), "invalid size");

    // _Execute_once_fp_t and PINIT_ONCE_FN differ in type signature, therefore
    // we introduce _Xfg_trampoline which has PINIT_ONCE_FN's type signature and
    // calls into _Callback as an _Execute_once_fp_t for XFG compatibility.

    PINIT_ONCE_FN _Xfg_trampoline = [](PINIT_ONCE _InitOnce, PVOID _Parameter, PVOID* _Context) {
        const auto _Callback = reinterpret_cast<_Execute_once_fp_t>(_Context);
        return static_cast<BOOL>(_Callback(_InitOnce, _Parameter, nullptr));
    };

    return __crtInitOnceExecuteOnce(
        reinterpret_cast<PINIT_ONCE>(&_Flag._Opaque), _Xfg_trampoline, _Pv, reinterpret_cast<PVOID*>(_Callback));
}

[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL
    _XGetLastError() { // throw system_error containing GetLastError()
    error_code _Code(static_cast<int>(GetLastError()), _STD system_category());
    _THROW(system_error(_Code));
}
_STD_END
