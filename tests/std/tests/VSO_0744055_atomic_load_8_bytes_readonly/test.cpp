// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>

#pragma warning(push) // TRANSITION, OS-23694920
#pragma warning(disable : 4668) // 'MEOW' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
#include <Windows.h>
#pragma warning(pop)

int main() {
    // See DevCom-274938, where a customer wanted to read a std::atomic from a read-only memory-mapped file.
    // This was originally implemented with cmpxchg8b on x86 which raised a write access violation.

    // https://learn.microsoft.com/windows/win32/api/memoryapi/nf-memoryapi-createfilemappingw
    // "If hFile is INVALID_HANDLE_VALUE, [...] CreateFileMapping creates a file mapping object of a
    // specified size that is backed by the system paging file instead of by a file in the file system."
    // "The initial contents of the pages in a file mapping object
    // backed by the operating system paging file are 0 (zero)."

    constexpr DWORD bufferSize = 200;
    HANDLE hMapFile = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READONLY, 0, bufferSize, nullptr);
    assert(hMapFile);
    void* view = MapViewOfFileEx(hMapFile, FILE_MAP_READ, 0, 0, bufferSize, nullptr);
    assert(view);

    using Atom               = std::atomic<std::uint64_t>;
    std::size_t space        = bufferSize;
    const auto adjusted_view = std::align(alignof(Atom), sizeof(Atom), view, space);
    assert(adjusted_view == view);

    // Formally UB, but we want std::atomic loads to work from read-only memory regions:
    const auto a = reinterpret_cast<Atom*>(view);

    // In the original repro, a->load() failed with an access violation.
    assert(a->load() == 0);

    UnmapViewOfFile(view);
    CloseHandle(hMapFile);
}
