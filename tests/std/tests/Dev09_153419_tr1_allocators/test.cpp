// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// This defines a malloc()-based allocator named Mallocator, instrumented
// to count the number of calls to malloc() and free().  It also replaces
// the global allocation and deallocation functions, instrumenting them to
// count the number of calls to new and delete.
//
// Then it exercises shared_ptr and function's new allocator-taking
// interfaces, making sure that the Mallocator is actually used.
//
// Big is 7 * 64 bits to disable the Small Functor Optimization.

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <memory>
#include <new>
#include <vector>

using namespace std;

#pragma warning(disable : 28251) // Inconsistent annotation for 'new': this instance has no annotations.

int g_mallocs    = 0;
int g_frees      = 0;
int g_constructs = 0;
int g_destroys   = 0;

template <typename T>
struct Mallocator {
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    template <typename U>
    struct rebind {
        typedef Mallocator<U> other;
    };

    T* address(T& t) const {
        return &t;
    }

    const T* address(const T& t) const {
        return &t;
    }

    T* allocate(const size_t n) const {
        ++g_mallocs;

        if (n == 0) {
            return nullptr;
        }

        return static_cast<T*>(malloc(n * sizeof(T)));
    }

    template <typename U>
    T* allocate(const size_t n, const U*) const {
        return allocate(n);
    }

    void deallocate(T* const p, size_t) const {
        ++g_frees;

        free(p);
    }

    size_t max_size() const {
        return 1000000;
    }

    bool operator==(const Mallocator&) const {
        return true;
    }

    bool operator!=(const Mallocator&) const {
        return false;
    }

    Mallocator() {}

    template <typename U>
    Mallocator(const Mallocator<U>&) {}

    template <typename... U>
    void construct(T* const p, const U&... u) const {
        ++g_constructs;

        new (static_cast<void*>(p)) T(u...);
    }

    void destroy(T* const p) const {
        ++g_destroys;

        p->~T();
    }
};

int g_news    = 0;
int g_deletes = 0;

void* operator new(size_t size) {
    void* const p = ::operator new(size, nothrow);

    if (p) {
        return p;
    } else {
        throw bad_alloc();
    }
}

void* operator new(size_t size, const nothrow_t&) noexcept {
    ++g_news;

    void* const result = malloc(size == 0 ? 1 : size);
    if (result) {
        memset(result, 0xDD, size);
    }

    return result;
}

void operator delete(void* ptr) noexcept {
    ::operator delete(ptr, nothrow);
}

void operator delete(void* ptr, const nothrow_t&) noexcept {
    if (ptr) {
        ++g_deletes;

        free(ptr);
    }
}

void* operator new[](size_t size) {
    return ::operator new(size);
}

void* operator new[](size_t size, const nothrow_t&) noexcept {
    return ::operator new(size, nothrow);
}

void operator delete[](void* ptr) noexcept {
    ::operator delete(ptr);
}

void operator delete[](void* ptr, const nothrow_t&) noexcept {
    ::operator delete(ptr, nothrow);
}

void custom_deleter(int* const p) {
    delete p;
}

void test(const int mallocs, const int frees, const int news, const int deletes, const int line) {
    if (g_mallocs != mallocs) {
        printf("FAIL: On line %d, g_mallocs is %d but should be %d.\n", line, g_mallocs, mallocs);
        abort();
    }

    if (g_frees != frees) {
        printf("FAIL: On line %d, g_frees is %d but should be %d.\n", line, g_frees, frees);
        abort();
    }

    if (g_news != news) {
        printf("FAIL: On line %d, g_news is %d but should be %d.\n", line, g_news, news);
        abort();
    }

    if (g_deletes != deletes) {
        printf("FAIL: On line %d, g_deletes is %d but should be %d.\n", line, g_deletes, deletes);
        abort();
    }
}

void test_constructors(const int constructs, const int destroys, const int line) {
    if (g_constructs != constructs) {
        printf("FAIL: On line %d, g_constructs is %d but should be %d.\n", line, g_constructs, constructs);
        abort();
    }

    if (g_destroys != destroys) {
        printf("FAIL: On line %d, g_destroys is %d but should be %d.\n", line, g_destroys, destroys);
        abort();
    }
}

#define TEST(A, B, C, D)        test(A, B, C, D, __LINE__)
#define TEST_CONSTRUCTORS(A, B) test_constructors(A, B, __LINE__)

void reset_counters() {
    g_mallocs = g_frees = g_constructs = g_destroys = g_news = g_deletes = 0;
    TEST(0, 0, 0, 0);
    TEST_CONSTRUCTORS(0, 0);
}

typedef unsigned long long ull_t;

struct Big {
    Big(const ull_t a, const ull_t b, const ull_t c, const ull_t d)
        : m_a(a), m_b(b), m_c(c), m_d(d), m_e(0), m_f(0), m_g(0) {}

    ull_t operator()() const {
        return m_a * m_b + m_c * m_d;
    }

    ull_t m_a;
    ull_t m_b;
    ull_t m_c;
    ull_t m_d;
    ull_t m_e;
    ull_t m_f;
    ull_t m_g;
};

ull_t func() {
    return 0x12345678DEADBEEFULL;
}

int main() {
    // Ignore any allocations that were performed during CRT startup.
    reset_counters();

    {
        int* const raw = new int(47);

        TEST(0, 0, 1, 0);

        shared_ptr<int> sp(raw, custom_deleter, Mallocator<double>());

        TEST(1, 0, 1, 0);

        weak_ptr<int> wp(sp);

        TEST(1, 0, 1, 0);

        sp.reset();

        TEST(1, 0, 1, 1);

        wp.reset();

        TEST(1, 1, 1, 1);
    }

    reset_counters();

    {
        int* const raw = new int(47);

        TEST(0, 0, 1, 0);

        shared_ptr<int> sp;

        TEST(0, 0, 1, 0);

        sp.reset(raw, custom_deleter, Mallocator<double>());

        TEST(1, 0, 1, 0);

        weak_ptr<int> wp(sp);

        TEST(1, 0, 1, 0);

        sp.reset();

        TEST(1, 0, 1, 1);

        wp.reset();

        TEST(1, 1, 1, 1);
    }

    reset_counters();

    {
        shared_ptr<int> sp;

        TEST(0, 0, 0, 0);

        sp = allocate_shared<int>(Mallocator<double>(), 1729);

        TEST(1, 0, 0, 0);

        weak_ptr<int> wp(sp);

        TEST(1, 0, 0, 0);

        sp.reset();

        TEST(1, 0, 0, 0);

        wp.reset();

        TEST(1, 1, 0, 0);
    }

    reset_counters();

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
    {
        function<ull_t()> f(allocator_arg, Mallocator<double>(), Big(10, 20, 30, 40));

        TEST(1, 0, 0, 0);

        f = nullptr;

        TEST(1, 1, 0, 0);
    }

    reset_counters();
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

    {
        function<ull_t()> f = func;

        TEST(0, 0, 0, 0);

        f = nullptr;

        TEST(0, 0, 0, 0);

        f = func;

        TEST(0, 0, 0, 0);

        f = nullptr;

        TEST(0, 0, 0, 0);
    }

    reset_counters();

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
    {
        function<ull_t()> f(allocator_arg, Mallocator<double>(), func);

        TEST(0, 0, 0, 0);

        f = nullptr;

        TEST(0, 0, 0, 0);
    }

    reset_counters();
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

    {
        Big b(10, 20, 30, 40);

        function<ull_t()> f = ref(b);

        TEST(0, 0, 0, 0);

        f = nullptr;

        TEST(0, 0, 0, 0);

        f = ref(b);

        TEST(0, 0, 0, 0);

        f = nullptr;

        TEST(0, 0, 0, 0);
    }

    reset_counters();

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
    {
        Big b(10, 20, 30, 40);

        function<ull_t()> f(allocator_arg, Mallocator<double>(), ref(b));

        TEST(0, 0, 0, 0);

        f = nullptr;

        TEST(0, 0, 0, 0);
    }

    reset_counters();
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

    {
        // Test Dev10-531321 "function: tr1::function memory leak".

        Big b(10, 20, 30, 40);
        function<ull_t()> f;

        TEST(0, 0, 0, 0);

        f = b;
        TEST(0, 0, 1, 0);
        f = b;
        TEST(0, 0, 2, 1);
        f = nullptr;
        TEST(0, 0, 2, 2);
    }

    reset_counters();

    {
        // Test (indirectly) _Uninitialized_fill_n with non-default allocator
        vector<char, Mallocator<char>> vec(3, static_cast<char>(-2));
        TEST_CONSTRUCTORS(3, 0);
        for (char c : vec) {
            assert(c == static_cast<char>(-2));
        }

        // Test (indirectly) _Uninitialized_copy with non-default allocator
        vector<char, Mallocator<char>> vecCopy(vec);
        TEST_CONSTRUCTORS(6, 0);

        // Test (indirectly) _Uninitialized_move with non-default allocator
        vec.reserve(100);
        TEST_CONSTRUCTORS(9, 3);

        // Test (indirectly) _Destroy_range with non-default allocator
        vec.clear();
        TEST_CONSTRUCTORS(9, 6);
    }

    reset_counters();

    {
        // Test (indirectly) _Uninitialized_default_fill_n with non-default allocator
        vector<char, Mallocator<char>> vec(3);
        TEST_CONSTRUCTORS(3, 0);
        if (any_of(vec.begin(), vec.end(), [](char x) { return x != '\0'; })) {
            puts("vector didn't clear its contents");
            abort();
        }

        vec.clear();

        TEST_CONSTRUCTORS(3, 3);
    }

    reset_counters();
}
