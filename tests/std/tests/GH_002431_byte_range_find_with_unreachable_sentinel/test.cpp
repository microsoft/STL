// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <isa_availability.h>
#include <ranges>

#include <Windows.h>

using namespace std;

template <class T>
void test_impl(void* sv, void* ev) {
    const auto s = reinterpret_cast<T*>(sv);
    const auto e = reinterpret_cast<T*>(ev);
    ranges::fill(s, e, '0');
    const auto m = e - 3;
    m[0]         = '1';
    m[1]         = '2';
    m[2]         = '3';

    for (int start = -66; start <= 0; ++start) {
        assert(ranges::find(m + start, unreachable_sentinel, '1') == m);
        assert(ranges::find(m + start, unreachable_sentinel, '2') == m + 1);
        assert(ranges::find(m + start, unreachable_sentinel, '3') == m + 2);
    }
}

#if defined(_M_IX86) || defined(_M_X64)
extern "C" long __isa_enabled;

void disable_instructions(ISA_AVAILABILITY isa) {
    __isa_enabled &= ~(1UL << static_cast<unsigned long>(isa));
}
#endif // defined(_M_IX86) || defined(_M_X64)

void test_all_element_sizes(void* p, size_t page) {
    test_impl<char>(p, reinterpret_cast<char*>(p) + page);
    test_impl<short>(p, reinterpret_cast<char*>(p) + page);
    test_impl<long>(p, reinterpret_cast<char*>(p) + page);
    test_impl<long long>(p, reinterpret_cast<char*>(p) + page);
}

int main() {
    SYSTEM_INFO si = {};
    GetSystemInfo(&si);

    const size_t page  = si.dwPageSize;
    const size_t alloc = (max) (page * 2, size_t{si.dwAllocationGranularity});

    void* p = VirtualAlloc(nullptr, alloc, MEM_RESERVE, PAGE_NOACCESS);
    assert(p != nullptr);
    void* p2 = VirtualAlloc(p, page, MEM_COMMIT, PAGE_READWRITE);
    assert(p2 != nullptr);

    test_all_element_sizes(p, page);
#if defined(_M_IX86) || defined(_M_X64)
    disable_instructions(__ISA_AVAILABLE_AVX2);
    test_all_element_sizes(p, page);
    disable_instructions(__ISA_AVAILABLE_SSE42);
    test_all_element_sizes(p, page);
#endif // defined(_M_IX86) || defined(_M_X64)
#if defined(_M_IX86)
    disable_instructions(__ISA_AVAILABLE_SSE2);
    test_all_element_sizes(p, page);
#endif // defined(_M_IX86)

    VirtualFree(p, 0, MEM_RELEASE);
}
