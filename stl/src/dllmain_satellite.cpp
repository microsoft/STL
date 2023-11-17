// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Windows.h>

// DllMain for 'satellite' DLLs which don't need TLS.

extern "C" BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID) noexcept {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        if (!DisableThreadLibraryCalls(hModule)) {
            return FALSE;
        }
    }

    return TRUE;
}
