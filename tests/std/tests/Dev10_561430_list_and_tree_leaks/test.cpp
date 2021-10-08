// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <deque>
#include <forward_list>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <regex>
#include <set>
#include <stddef.h>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#ifndef _M_CEE
#include <future>
#endif // _M_CEE

int g_mallocs = 0;

// Also test DevDiv-483844 and DevDiv-781187, minimal allocator requirements.
template <typename T>
struct Mallocator {
    typedef T value_type;

    Mallocator()                  = default;
    Mallocator(const Mallocator&) = default;

    template <typename U>
    Mallocator(const Mallocator<U>&) {}

    Mallocator& operator=(const Mallocator&) = delete;

    bool operator==(const Mallocator&) const {
        return true;
    }
    bool operator!=(const Mallocator&) const {
        return false;
    }

    T* allocate(const size_t n) const {
        ++g_mallocs;

        if (n == 0) {
            return nullptr;
        }

        assert(n <= static_cast<size_t>(-1) / sizeof(T));

        void* const pv = malloc(n * sizeof(T));

        assert(pv != nullptr);

        return static_cast<T*>(pv);
    }

    void deallocate(T* const p, size_t) const {
        assert(p != nullptr);

        --g_mallocs;

        free(p);
    }
};

template <typename C>
void test() {
    assert(g_mallocs == 0);

    {
        C c1;
        C c2(c1);

        assert(g_mallocs >= 0);
    }

    assert(g_mallocs == 0);
}

void test_virtual_allocator();

int main() {
    using namespace std;

    test<deque<int, Mallocator<int>>>();
    test<forward_list<int, Mallocator<int>>>();
    test<list<int, Mallocator<int>>>();
    test<vector<int, Mallocator<int>>>();

    test<vector<bool, Mallocator<bool>>>();

    test<map<int, int, less<int>, Mallocator<pair<const int, int>>>>();
    test<multimap<int, int, less<int>, Mallocator<pair<const int, int>>>>();
    test<set<int, less<int>, Mallocator<int>>>();
    test<multiset<int, less<int>, Mallocator<int>>>();

    test<unordered_map<int, int, hash<int>, equal_to<int>, Mallocator<pair<const int, int>>>>();
    test<unordered_multimap<int, int, hash<int>, equal_to<int>, Mallocator<pair<const int, int>>>>();
    test<unordered_set<int, hash<int>, equal_to<int>, Mallocator<int>>>();
    test<unordered_multiset<int, hash<int>, equal_to<int>, Mallocator<int>>>();

    test<basic_string<char, char_traits<char>, Mallocator<char>>>();

    test<match_results<const char*, Mallocator<csub_match>>>();

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
    {
        function<int(int)> f1(allocator_arg, Mallocator<int>(), [](int n) { return n * 2; });
        function<int(int)> f2(f1);
    }
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

    { shared_ptr<int> sp(new int(1729), default_delete<int>(), Mallocator<int>()); }

    { shared_ptr<int> sp = allocate_shared<int>(Mallocator<int>(), 1729); }

#ifndef _M_CEE
    {
        promise<int> p(allocator_arg, Mallocator<int>());

        future<int> f = p.get_future();

        p.set_value(1729);

        assert(f.get() == 1729);
    }

    {
        promise<int&> p(allocator_arg, Mallocator<int>());

        future<int&> f = p.get_future();

        int n = 4096;

        p.set_value(n);

        assert(&f.get() == &n);
    }

    {
        promise<void> p(allocator_arg, Mallocator<int>());

        future<void> f = p.get_future();

        p.set_value();

        f.get();
    }

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
    {
        packaged_task<int()> pt(allocator_arg, Mallocator<int>(), [] { return 1234; });

        future<int> f = pt.get_future();

        pt();

        assert(f.get() == 1234);
    }

    {
        int n = 4096;

        packaged_task<int&()> pt(allocator_arg, Mallocator<int>(), [&n]() -> int& { return n; });

        future<int&> f = pt.get_future();

        pt();

        assert(&f.get() == &n);
    }

    {
        packaged_task<void()> pt(allocator_arg, Mallocator<int>(), [] {});

        future<void> f = pt.get_future();

        pt();

        f.get();
    }
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT
#endif // _M_CEE

    assert(g_mallocs == 0);

    test_virtual_allocator();
}


// Also test DevDiv-819467 "<memory>: Custom allocator with virtual max_size function causes infinite recursion".

template <typename T>
struct WeirdAllocator {
    typedef T value_type;

    WeirdAllocator() {}
    template <typename U>
    WeirdAllocator(const WeirdAllocator<U>&) {}

    virtual bool operator==(const WeirdAllocator&) const {
        return true;
    }
    virtual bool operator!=(const WeirdAllocator&) const {
        return false;
    }

    virtual T* allocate(size_t n) {
        return std::allocator<T>().allocate(n);
    }

    virtual void deallocate(T* p, size_t n) {
        std::allocator<T>().deallocate(p, n);
    }


    virtual T* allocate(size_t n, const void*) {
        return std::allocator<T>().allocate(n);
    }

    virtual size_t max_size() const {
        return static_cast<size_t>(-1) / sizeof(T);
    }

    virtual WeirdAllocator select_on_container_copy_construction() const {
        return *this;
    }
};

void test_virtual_allocator() {
    std::vector<int, WeirdAllocator<int>> v;

    v.push_back(1729);

    assert(v[0] == 1729);
}
