// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <ranges>
#include <utility>

#include <Windows.h>

using namespace std;

template <class T>
void test_impl(void* sv, void* ev) {
    const auto s = reinterpret_cast<T*>(sv);
    const auto e = reinterpret_cast<T*>(ev);
    ranges::fill(s, e, '0');
    const auto m     = e - 3;
    m[0]             = '1';
    m[1]             = '2';
    m[2]             = '3';
    const auto bingo = ranges::find(m, unreachable_sentinel, '3');
    assert(bingo == e - 1);

    const auto bingo2 = ranges::find(m - 16, unreachable_sentinel, '2');
    assert(bingo2 == e - 2);

    const auto bingo3 = ranges::find(m - 32, unreachable_sentinel, '1');
    assert(bingo3 == e - 3);

    const auto bingo4 = ranges::find(m - 64, unreachable_sentinel, '3');
    assert(bingo4 == e - 1);
}

int main() {
    SYSTEM_INFO si = {};
    GetSystemInfo(&si);

    const size_t page  = si.dwPageSize;
    const size_t alloc = max<size_t>(page * 2, si.dwAllocationGranularity);

    void* p = VirtualAlloc(nullptr, alloc, MEM_RESERVE, PAGE_NOACCESS);
    assert(p != nullptr);
    void* p2 = VirtualAlloc(p, page, MEM_COMMIT, PAGE_READWRITE);
    assert(p2 != nullptr);

    test_impl<char>(p2, reinterpret_cast<char*>(p2) + page);
    test_impl<short>(p2, reinterpret_cast<char*>(p2) + page);
    test_impl<long>(p2, reinterpret_cast<char*>(p2) + page);
    test_impl<long long>(p2, reinterpret_cast<char*>(p2) + page);

    VirtualFree(p, 0, MEM_RELEASE);
}
