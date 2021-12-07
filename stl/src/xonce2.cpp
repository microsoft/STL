// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <yvals_core.h>

#include <synchapi.h>

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

// Provides forwarders for InitOnceBeginInitialize and InitOnceComplete for
// environments that can't use /ALTERNATENAME.

_EXTERN_C

int __stdcall __std_init_once_begin_initialize_fwd(
    void** _LpInitOnce, unsigned long _DwFlags, int* _FPending, void** _LpContext) noexcept {
    return InitOnceBeginInitialize(reinterpret_cast<LPINIT_ONCE>(_LpInitOnce), _DwFlags, _FPending, _LpContext);
}

int __stdcall __std_init_once_complete_fwd(void** _LpInitOnce, unsigned long _DwFlags, void* _LpContext) noexcept {
    return InitOnceComplete(reinterpret_cast<LPINIT_ONCE>(_LpInitOnce), _DwFlags, _LpContext);
}

void __stdcall __std_init_once_link_alternate_names() noexcept {}

#if defined(_M_ARM64EC)
#error ARM64EC is not expected here
#elif defined(_M_HYBRID)
#error CHPE is not expected here
#elif defined(_M_IX86)
#pragma comment(linker, "/ALTERNATENAME:__imp____std_init_once_begin_initialize@16=__imp__InitOnceBeginInitialize@16")
#pragma comment(linker, "/ALTERNATENAME:__imp____std_init_once_complete@12=__imp__InitOnceComplete@12")
#elif defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64)
#pragma comment(linker, "/ALTERNATENAME:__imp___std_init_once_begin_initialize=__imp_InitOnceBeginInitialize")
#pragma comment(linker, "/ALTERNATENAME:__imp___std_init_once_complete=__imp_InitOnceComplete")
#else // ^^^ known architecture / unknown architecture vvv
#error Unknown architecture
#endif // ^^^ unknown architecture ^^^

_END_EXTERN_C
