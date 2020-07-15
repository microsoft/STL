// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <internal_shared.h>

#include <Windows.h>

#include "awint.hpp"

#ifdef _CRT_APP
// free static resource used by causality
extern "C" void __cdecl __crtCleanupCausalityStaticFactories();
#endif // _CRT_APP

extern "C" BOOL APIENTRY DllMain(HMODULE /* hModule */, DWORD ul_reason_for_call, [[maybe_unused]] LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_DETACH) {
#ifdef _CRT_APP
        if (lpReserved == nullptr) { // only when the process is not terminating
            __crtCleanupCausalityStaticFactories();
        }
#endif // _CRT_APP
    }
    return TRUE;
}
