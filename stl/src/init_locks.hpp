// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <yvals.h>

_STD_BEGIN

class _CRTIMP2_PURE_IMPORT _Init_locks { // initialize mutexes
public:
#ifdef _M_CEE_PURE
    __CLR_OR_THIS_CALL _Init_locks() noexcept {
        _Init_locks_ctor(this);
    }

    __CLR_OR_THIS_CALL ~_Init_locks() noexcept {
        _Init_locks_dtor(this);
    }
#else // ^^^ defined(_M_CEE_PURE) / !defined(_M_CEE_PURE) vvv
    __thiscall _Init_locks() noexcept;
    __thiscall ~_Init_locks() noexcept;
#endif // ^^^ !defined(_M_CEE_PURE) ^^^

private:
    static void __cdecl _Init_locks_ctor(_Init_locks*) noexcept;
    static void __cdecl _Init_locks_dtor(_Init_locks*) noexcept;
};

_STD_END
