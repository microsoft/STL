// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "awint.h"
#include <internal_shared.h>

#include <Windows.h>

#if defined(_CRT_APP)
// free static resource used by causality
extern "C" void __cdecl __crtCleanupCausalityStaticFactories();
#endif

extern "C" BOOL APIENTRY DllMain(HMODULE /* hModule */, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_DETACH) {
#if defined(_CRT_APP)
        if (lpReserved == nullptr) { // only when the process is not terminating
            __crtCleanupCausalityStaticFactories();
        }
#else
        (void) (lpReserved);
#endif
    }
    return TRUE;
}
