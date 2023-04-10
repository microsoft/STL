// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// REQUIRES: debug_CRT

#define _SILENCE_CXX23_ALIGNED_UNION_DEPRECATION_WARNING

#include <cassert>
#include <crtdbg.h>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <new>
#include <type_traits>
#include <vector>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

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


void test(const int num) {
    bool reached_switch = false;

    try {
        aligned_union_t<1, function<void()>> bits;
        STATIC_ASSERT(sizeof(bits) >= sizeof(function<void()>));
        void* pv = &bits;
        memset(pv, 0xCC, sizeof(bits));

        vector<int> v(10, 1729);
        long long a = 0, b = 0, c = 0, d = 0, e = 0;
        auto big_lambda = [v, a, b, c, d, e] {
            (void) v;
            (void) a;
            (void) b;
            (void) c;
            (void) d;
            (void) e;
        };

        function<void()> src_function(big_lambda);

        reached_switch = true;

        switch (num) {
        case 0:
            // Verify that std::function construction doesn't crash when it can't allocate memory.
            g_allocations_remaining = 0;
            ::new (pv) function<void()>(big_lambda);
            break;

        case 1:
            // Verify that std::function construction doesn't leak when it can allocate memory, but the target functor's
            // copy ctor throws.
            g_allocations_remaining = 1;
            ::new (pv) function<void()>(big_lambda);
            break;

        case 2:
            // Verify that std::function copy construction doesn't crash when it can't allocate memory.
            g_allocations_remaining = 0;
            ::new (pv) function<void()>(src_function);
            break;

        case 3:
            // Verify that std::function copy construction doesn't leak when it can allocate memory, but the target
            // functor's copy ctor throws.
            g_allocations_remaining = 1;
            ::new (pv) function<void()>(src_function);
            break;

        default:
            abort();
        }
    } catch (const bad_alloc&) {
        if (!reached_switch) {
            abort();
        }

        g_allocations_remaining = 1000000;

        if (_CrtDumpMemoryLeaks()) {
            abort();
        }

        return;
    }

    abort();
}

int main() {
    for (int num = 0; num < 4; ++num) {
        test(num);
    }
}
