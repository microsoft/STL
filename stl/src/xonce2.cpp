// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <yvals_core.h>

#include <synchapi.h>

// This file is compiled into import library.
// Limitations apply to what can be included here; see import_library.txt

// Provides forwarders for InitOnceBeginInitialize and InitOnceComplete for
// environments that can't use aliasobj, like /clr.

_EXTERN_C

int __stdcall __std_init_once_begin_initialize_clr(
    void** _LpInitOnce, unsigned long _DwFlags, int* _FPending, void** _LpContext) noexcept {
    return InitOnceBeginInitialize(reinterpret_cast<LPINIT_ONCE>(_LpInitOnce), _DwFlags, _FPending, _LpContext);
}

int __stdcall __std_init_once_complete_clr(void** _LpInitOnce, unsigned long _DwFlags, void* _LpContext) noexcept {
    return InitOnceComplete(reinterpret_cast<LPINIT_ONCE>(_LpInitOnce), _DwFlags, _LpContext);
}

_END_EXTERN_C
