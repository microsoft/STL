// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <cassert>
#include <cstdio>

#include <Windows.h>

int main() {
    // A customer wanted to read a std::atomic from a read-only memory-mapped file.
    // Before MSVC 19.24 this was implemented with cmpxchg8b on x86 which raised a write access violation. See
    // DevCom-274938
    constexpr DWORD bufferSize = 200;
    HANDLE hMapFile = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READONLY, 0, bufferSize, nullptr);
    assert(hMapFile);
    const auto view = static_cast<unsigned char*>(MapViewOfFileEx(hMapFile, FILE_MAP_READ, 0, 0, bufferSize, nullptr));
    assert(view);

    // Formally UB, but we want std::atomic loads to work from read-only memory regions:
    const auto a = reinterpret_cast<std::atomic<unsigned long long>*>(view);
    // In the original repro, a->load() fails with an access violation
    printf("atomic load: %llu, should be 0\n", a->load());
    UnmapViewOfFile(view);
    CloseHandle(hMapFile);
}
