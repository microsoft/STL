// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <cassert>
#include <cstdio>

#pragma warning(push) // TRANSITION, OS-23694920
#pragma warning(disable : 4668) // 'MEOW' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
#include <Windows.h>
#pragma warning(pop)

int main() {
    // See DevCom-274938, where a customer wanted to read a std::atomic from a read-only memory-mapped file.
    // This was originally implemented with cmpxchg8b on x86 which raised a write access violation.
    constexpr DWORD bufferSize = 200;
    HANDLE hMapFile = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READONLY, 0, bufferSize, nullptr);
    assert(hMapFile);
    const auto view = static_cast<unsigned char*>(MapViewOfFileEx(hMapFile, FILE_MAP_READ, 0, 0, bufferSize, nullptr));
    assert(view);

    // Formally UB, but we want std::atomic loads to work from read-only memory regions:
    const auto a = reinterpret_cast<std::atomic<unsigned long long>*>(view);
    // In the original repro, a->load() failed with an access violation.
    printf("atomic load: %llu, should be 0\n", a->load());
    UnmapViewOfFile(view);
    CloseHandle(hMapFile);
}
