// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <yvals_core.h>

#include <cstdlib>
#include <synchapi.h>

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

// Provides forwarders for InitOnceBeginInitialize and InitOnceComplete for
// environments that can't use /ALTERNATENAME.
// They were originally specific to /clr but are now used in other scenarios.

_EXTERN_C

int __stdcall __std_init_once_begin_initialize_clr(
    void** _LpInitOnce, unsigned long _DwFlags, int* _FPending, void** _LpContext) noexcept {
    return InitOnceBeginInitialize(reinterpret_cast<LPINIT_ONCE>(_LpInitOnce), _DwFlags, _FPending, _LpContext);
}

int __stdcall __std_init_once_complete_clr(void** _LpInitOnce, unsigned long _DwFlags, void* _LpContext) noexcept {
    return InitOnceComplete(reinterpret_cast<LPINIT_ONCE>(_LpInitOnce), _DwFlags, _LpContext);
}

[[noreturn]] void __stdcall __std_init_once_link_alternate_names_and_abort() noexcept {
    _CSTD abort();
}

#if defined(_M_ARM64EC) || defined(_M_HYBRID)
// <mutex> uses the forwarder fallbacks for ARM64EC and CHPE.
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
