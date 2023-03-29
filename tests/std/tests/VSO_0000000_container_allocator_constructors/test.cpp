// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <forward_list>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <list>
#include <map>
#include <new>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

#pragma warning(disable : 28251) // Inconsistent annotation for 'new': this instance has no annotations.

// Break std::allocator by making calling new fail

void* operator new(size_t) {
    abort();
}

void* operator new(size_t, const nothrow_t&) noexcept {
    abort();
}

void operator delete(void*) noexcept {
    abort();
}

void operator delete(void*, const nothrow_t&) noexcept {
    abort();
}

void* operator new[](size_t) {
    abort();
}

void* operator new[](size_t, const nothrow_t&) noexcept {
    abort();
}

void operator delete[](void*) noexcept {
    abort();
}

void operator delete[](void*, const nothrow_t&) noexcept {
    abort();
}

struct ConstructT {};

constexpr ConstructT C{};

template <typename T>
struct Mallocator {
    using value_type = T;

    Mallocator() = delete;
    explicit Mallocator(const ConstructT&) {}
    Mallocator(const Mallocator&) = default;

    template <typename U>
    Mallocator(const Mallocator<U>&) {}

    value_type* allocate(const size_t n) {
        auto res = static_cast<value_type*>(calloc(n, sizeof(T)));
        if (res == nullptr) {
            throw bad_alloc();
        }

        return res;
    }

    void deallocate(value_type* p, size_t) {
        free(p);
    }
};

template <class T, class U>
bool operator==(const Mallocator<T>&, const Mallocator<U>&) {
    return true;
}
template <class T, class U>
bool operator!=(const Mallocator<T>&, const Mallocator<U>&) {
    return false;
}

int main() {
    deque<int, Mallocator<int>>(42, Mallocator<int>(C));
    forward_list<int, Mallocator<int>>(42, Mallocator<int>(C));
    list<int, Mallocator<int>>(42, Mallocator<int>(C));
    vector<int, Mallocator<int>>(42, Mallocator<int>(C));
    vector<bool, Mallocator<bool>>(42, Mallocator<bool>(C));

    const pair<int, int> map_input[]                           = {{42, 1729}, {1, 2}, {3, 4}};
    const initializer_list<pair<const int, int>> map_init_list = {{42, 1729}, {1, 2}, {3, 4}}; // lifetime extended
    using assoc_alloc_t                                        = Mallocator<pair<const int, int>>;
    using map_t                                                = map<int, int, less<>, assoc_alloc_t>;
    map_t(begin(map_input), end(map_input), assoc_alloc_t(C));
    map_t(map_init_list, assoc_alloc_t(C));
    using mmap_t = multimap<int, int, less<>, assoc_alloc_t>;
    mmap_t(begin(map_input), end(map_input), assoc_alloc_t(C));
    mmap_t(map_init_list, assoc_alloc_t(C));

    const int set_input[]                     = {42, 1729, 1, 2, 3, 4};
    const initializer_list<int> set_init_list = {42, 1729, 1, 2, 3, 4};
    using set_t                               = set<int, less<>, Mallocator<int>>;
    set_t(begin(set_input), end(set_input), Mallocator<int>(C));
    set_t(set_init_list, Mallocator<int>(C));
    using mset_t = multiset<int, less<>, Mallocator<int>>;
    mset_t(begin(set_input), end(set_input), Mallocator<int>(C));
    mset_t(set_init_list, Mallocator<int>(C));


    using unordered_map_t = unordered_map<int, int, hash<int>, equal_to<>, assoc_alloc_t>;
    unordered_map_t(2, assoc_alloc_t(C));
    unordered_map_t(2, hash<int>(), assoc_alloc_t(C));
    unordered_map_t(begin(map_input), end(map_input), 2, assoc_alloc_t(C));
    unordered_map_t(begin(map_input), end(map_input), 2, hash<int>(), assoc_alloc_t(C));
    unordered_map_t(map_init_list, 2, assoc_alloc_t(C));
    unordered_map_t(map_init_list, 2, hash<int>(), assoc_alloc_t(C));
    using unordered_mmap_t = unordered_multimap<int, int, hash<int>, equal_to<>, assoc_alloc_t>;
    unordered_mmap_t(2, assoc_alloc_t(C));
    unordered_mmap_t(2, hash<int>(), assoc_alloc_t(C));
    unordered_mmap_t(begin(map_input), end(map_input), 2, assoc_alloc_t(C));
    unordered_mmap_t(begin(map_input), end(map_input), 2, hash<int>(), assoc_alloc_t(C));
    unordered_mmap_t(map_init_list, 2, assoc_alloc_t(C));
    unordered_mmap_t(map_init_list, 2, hash<int>(), assoc_alloc_t(C));

    using unordered_set_t = unordered_set<int, hash<int>, equal_to<>, Mallocator<int>>;
    unordered_set_t(2, Mallocator<int>(C));
    unordered_set_t(2, hash<int>(), Mallocator<int>(C));
    unordered_set_t(begin(set_input), end(set_input), 2, Mallocator<int>(C));
    unordered_set_t(begin(set_input), end(set_input), 2, hash<int>(), Mallocator<int>(C));
    unordered_set_t(set_init_list, 2, Mallocator<int>(C));
    unordered_set_t(set_init_list, 2, hash<int>(), Mallocator<int>(C));
    using unordered_mset_t = unordered_multiset<int, hash<int>, equal_to<>, Mallocator<int>>;
    unordered_mset_t(2, Mallocator<int>(C));
    unordered_mset_t(2, hash<int>(), Mallocator<int>(C));
    unordered_mset_t(begin(set_input), end(set_input), 2, Mallocator<int>(C));
    unordered_mset_t(begin(set_input), end(set_input), 2, hash<int>(), Mallocator<int>(C));
    unordered_mset_t(set_init_list, 2, Mallocator<int>(C));
    unordered_mset_t(set_init_list, 2, hash<int>(), Mallocator<int>(C));
}
