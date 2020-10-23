// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// _Execute_once function

#include <mutex>

#include "awint.hpp"

_STD_BEGIN

struct _Xfg_trampoline_parameter {
    void* _Pv;
    _Execute_once_fp_t _Callback;
};

// TRANSITION, ABI
_CRTIMP2_PURE int __CLRCALL_PURE_OR_CDECL _Execute_once(
    once_flag& _Flag, _Execute_once_fp_t _Callback, void* _Pv) noexcept { // wrap Win32 InitOnceExecuteOnce()
    static_assert(sizeof(_Flag._Opaque) == sizeof(INIT_ONCE), "invalid size");

    // _Execute_once_fp_t and PINIT_ONCE_FN differ in type signature, therefore
    // we introduce _Xfg_trampoline which has PINIT_ONCE_FN's type signature and
    // calls into _Callback as an _Execute_once_fp_t for XFG compatibility.

    _Xfg_trampoline_parameter _Trampoline_parameter = {_Pv, _Callback};

    PINIT_ONCE_FN _Xfg_trampoline = [](PINIT_ONCE _InitOnce, PVOID _Parameter, PVOID* _Context) {
        const auto _Trampoline_parameter = static_cast<_Xfg_trampoline_parameter*>(_Parameter);
        return static_cast<BOOL>(_Trampoline_parameter->_Callback(_InitOnce, _Trampoline_parameter->_Pv, _Context));
    };

    return InitOnceExecuteOnce(
        reinterpret_cast<PINIT_ONCE>(&_Flag._Opaque), _Xfg_trampoline, &_Trampoline_parameter, nullptr);
}

[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL
    _XGetLastError() { // throw system_error containing GetLastError()
    error_code _Code(static_cast<int>(GetLastError()), _STD system_category());
    _THROW(system_error(_Code));
}
_STD_END
