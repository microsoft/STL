// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <yvals_core.h>

#include <synchapi.h>

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

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
