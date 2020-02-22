// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <new>
#include <set>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

map<size_t, int> g_map;

size_t g_next_id = 0;

template <typename T>
class MyAllocator {
public:
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    template <typename U>
    struct rebind {
        typedef MyAllocator<U> other;
    };

    T* address(T& r) const {
        return &r;
    };

    const T* address(const T& s) const {
        return &s;
    }

    T* allocate(size_t n) const {
        ++g_map[m_id];

        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    template <typename U>
    T* allocate(size_t n, const U*) const {
        return allocate(n);
    }

    void deallocate(T* p, size_t) const {
        --g_map[m_id];

        ::operator delete(p);
    }

    size_t max_size() const {
        return 1000000;
    }

    bool operator==(const MyAllocator& other) const {
        return m_id == other.m_id;
    }

    bool operator!=(const MyAllocator& other) const {
        return m_id != other.m_id;
    }

    MyAllocator() : m_id(g_next_id++) {}
    MyAllocator(const MyAllocator& right) : m_id(right.m_id) {}
    template <typename U>
    MyAllocator(const MyAllocator<U>& right) : m_id(right.m_id) {}

    void construct(T* p, const T& t) const {
        new (static_cast<void*>(p)) T(t);
    }

    void destroy(T* p) const {
        p->~T();
    }

    MyAllocator& operator=(const MyAllocator&);

    size_t m_id;
};

void dump_map() {
    for (auto&& entry : g_map) {
        printf("ID: %zu | allocate - deallocate: %i\n", entry.first, entry.second);
    }
}

int main() {
    {
        vector<double, MyAllocator<double>> v;
        deque<double, MyAllocator<double>> d;
        list<double, MyAllocator<double>> l;
        forward_list<double, MyAllocator<double>> fl;
        set<double, less<double>, MyAllocator<double>> s;
        multiset<double, less<double>, MyAllocator<double>> ms;
        map<float, double, less<float>, MyAllocator<pair<const float, double>>> m;
        multimap<float, double, less<float>, MyAllocator<pair<const float, double>>> mm;
        unordered_set<int, hash<int>, equal_to<int>, MyAllocator<int>> us;
        unordered_multiset<int, hash<int>, equal_to<int>, MyAllocator<int>> ums;
        unordered_map<int, int, hash<int>, equal_to<int>, MyAllocator<pair<const int, int>>> um;
        unordered_multimap<int, int, hash<int>, equal_to<int>, MyAllocator<pair<const int, int>>> umm;
    }

    for (auto&& entry : g_map) {
        if (entry.second != 0) {
            dump_map();
            abort();
        }
    }
}
