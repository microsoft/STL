// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <functional>
#include <memory>
#include <new>
#include <unordered_set>

using namespace std;

// Defend against regression of GH-2789, which bug resulted in deallocating
// a nullptr when unordered_meow creation fails due to OOM on the first call
// to `_Hash_vec::_Assign_grow`. (This test relies on counting the number of
// allocations to inject a failure in the right spot, so it's inherently
// fragile.)

#if _ITERATOR_DEBUG_LEVEL != 0
static size_t alloc_count = 3;
#else
static size_t alloc_count = 1;
#endif

template <class T>
struct countdown_alloc {
    using value_type = T;

    countdown_alloc() = default;
    template <class U>
    countdown_alloc(const countdown_alloc<U>&) {}

    T* allocate(const size_t n) {
        if (n == 0) { // allocate never returns nullptr...
            return allocator<T>{}.allocate(1);
        } else if (alloc_count > 0) {
            --alloc_count;
            return allocator<T>{}.allocate(n);
        }

        throw bad_alloc{};
    }

    void deallocate(T* const p, const size_t n) {
        assert(p != nullptr); // ... so deallocate need not tolerate nullptr.
        if (n == 0) {
            allocator<T>{}.deallocate(p, 1);
        } else {
            allocator<T>{}.deallocate(p, n);
        }
    }

    template <class B>
    friend bool operator==(const countdown_alloc&, const countdown_alloc<B>&) noexcept {
        return true;
    }
    template <class B>
    friend bool operator!=(const countdown_alloc&, const countdown_alloc<B>&) noexcept {
        return false;
    }
};

int main() {
    try {
        (void) unordered_set<int, hash<int>, equal_to<int>, countdown_alloc<int>>{};
        assert(false);
    } catch (const bad_alloc&) {
    }
}
