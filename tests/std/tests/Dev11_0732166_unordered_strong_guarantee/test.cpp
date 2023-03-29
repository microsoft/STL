// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <map>
#include <new>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

#pragma warning(disable : 28251) // Inconsistent annotation for 'new': this instance has no annotations.

int g_allocations_remaining = 1000000;

void* operator new(size_t size, const nothrow_t&) noexcept {
    if (g_allocations_remaining == 0) {
        return nullptr;
    }

    --g_allocations_remaining;

    return malloc(size == 0 ? 1 : size);
}

void* operator new(size_t size) {
    void* const p = ::operator new(size, nothrow);

    if (p) {
        return p;
    } else {
        throw bad_alloc();
    }
}

void operator delete(void* ptr, const nothrow_t&) noexcept {
    free(ptr);
}

void operator delete(void* ptr) noexcept {
    ::operator delete(ptr, nothrow);
}

void* operator new[](size_t size, const nothrow_t&) noexcept {
    return ::operator new(size, nothrow);
}

void* operator new[](size_t size) {
    return ::operator new(size);
}

void operator delete[](void* ptr, const nothrow_t&) noexcept {
    ::operator delete(ptr, nothrow);
}

void operator delete[](void* ptr) noexcept {
    ::operator delete(ptr);
}

int main() {
    {
        // Basic tests.
        const vector<int> v = {10, 32, 20, 22, 11, 32, 21, 31, 20, 32, 31, 30, 12, 31, 30, 21, 22, 30};

        const vector<pair<int, int>> vp = {{10, -40}, {32, -41}, {20, -42}, {22, -43}, {11, -44}, {32, -45}, {21, -46},
            {31, -47}, {20, -48}, {32, -49}, {31, -50}, {30, -51}, {12, -52}, {31, -53}, {30, -54}, {21, -55},
            {22, -56}, {30, -57}};

        const set<int> s(v.begin(), v.end());
        const multiset<int> ms(v.begin(), v.end());
        const map<int, int> m(vp.begin(), vp.end());
        const multimap<int, int> mm(vp.begin(), vp.end());

        const unordered_set<int> us(v.begin(), v.end());
        const unordered_multiset<int> ums(v.begin(), v.end());
        const unordered_map<int, int> um(vp.begin(), vp.end());
        const unordered_multimap<int, int> umm(vp.begin(), vp.end());

        assert(is_permutation(s.begin(), s.end(), us.begin(), us.end()));
        assert(is_permutation(ms.begin(), ms.end(), ums.begin(), ums.end()));
        assert(is_permutation(m.begin(), m.end(), um.begin(), um.end()));
        assert(is_permutation(mm.begin(), mm.end(), umm.begin(), umm.end()));
    }

    // N3936 23.2.5.1 [unord.req.except]/2:
    // For unordered associative containers, if an exception is thrown by any
    // operation other than the container's hash function from within an insert
    // or emplace function inserting a single element, the insertion has no effect.

    // /4:
    // For unordered associative containers, if an exception is thrown from
    // within a rehash() function other than by the container's hash function
    // or comparison function, the rehash() function has no effect.

    for (int k = 0; k < 5; ++k) {
        g_allocations_remaining = 1000000;

        unordered_set<int> us = {10, 20, 30, 40, 50, 60, 70, 80};

        const vector<int> v(us.begin(), us.end());

        try {
            switch (k) {
            case 0:
                g_allocations_remaining = 0;
                us.insert(90);
                break;
            case 1:
                g_allocations_remaining = 0;
                us.insert(1729);
                break;
            case 2:
                g_allocations_remaining = 1;
                us.insert(90);
                break;
            case 3:
                g_allocations_remaining = 1;
                us.insert(1729);
                break;
            case 4:
                g_allocations_remaining = 0;
                us.rehash(1024);
                break;
            default:
                break;
            }

            abort();
        } catch (const bad_alloc&) {
            // Good.
        }

        assert(equal(us.begin(), us.end(), v.begin(), v.end()));

        for (int n = 10; n <= 80; n += 10) {
            const auto i = us.find(n);

            assert(i != us.end() && *i == n);
        }

        assert(us.find(90) == us.end());
        assert(us.find(1729) == us.end());
    }
}
