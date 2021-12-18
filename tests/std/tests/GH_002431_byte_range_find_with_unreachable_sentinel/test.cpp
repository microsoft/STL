// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <ranges>
#include <utility>

#include <Windows.h>

using namespace std;

void* volatile discard;

int main() {
    SYSTEM_INFO si = {};
    GetSystemInfo(&si);

    const size_t page  = si.dwPageSize;
    const size_t alloc = max<size_t>(page * 2, si.dwAllocationGranularity);

    void* p  = VirtualAlloc(nullptr, alloc, MEM_RESERVE, PAGE_NOACCESS);
    void* p2 = VirtualAlloc(p, page, MEM_COMMIT, PAGE_READWRITE);

    const auto p3 = reinterpret_cast<char*>(p2) + page - 3;
    p3[0]         = '1';
    p3[1]         = '2';
    p3[2]         = '3';

    const auto bingo = ranges::find(p3, std::unreachable_sentinel, '3');
    assert(bingo == p3 + 2);

    const auto p4 = p3 - 32;

    const auto bingo2 = ranges::find(p4, std::unreachable_sentinel, '2');
    assert(bingo2 == p3 + 1);
}
