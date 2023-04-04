// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Test DevDiv-920385 "<list>: list::sort shouldn't default-construct allocators".

#define _HAS_DEPRECATED_ALLOCATOR_MEMBERS 1
#define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <forward_list>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <new>
#include <numeric>
#include <random>
#include <regex>
#include <set>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#ifndef _M_CEE_PURE
#include <future>
#endif // _M_CEE_PURE

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <typename T>
struct MyAlloc {
    size_t m_offset;

    typedef T value_type;

    explicit MyAlloc(const size_t offset) : m_offset(offset) {}

    template <typename U>
    MyAlloc(const MyAlloc<U>& other) : m_offset(other.m_offset) {}

    template <typename U>
    bool operator==(const MyAlloc<U>& other) const {
        return m_offset == other.m_offset;
    }

    template <typename U>
    bool operator!=(const MyAlloc<U>& other) const {
        return m_offset != other.m_offset;
    }

    T* allocate(const size_t n) {
        if (n == 0) {
            return nullptr;
        }

        // Production code should check for integer overflow.
        void* const pv = malloc((n + m_offset) * sizeof(T));

        if (!pv) {
            throw bad_alloc();
        }

        memset(pv, 0xAB, (n + m_offset) * sizeof(T));

        return static_cast<T*>(pv) + m_offset;
    }

    void deallocate(T* const p, size_t) {
        if (p) {
            free(p - m_offset);
        }
    }
};

void test_1119194();
void test_1184701();
void test_LWG_2455();
void test_allocator_construct_const();

int main() {
    MyAlloc<int> alloc(7);

    // LWG-2466 "allocator_traits::max_size default behavior is incorrect"
#ifdef _WIN64
    assert(allocator_traits<MyAlloc<int>>::max_size(alloc) == 0x3FFFFFFFFFFFFFFFULL);
#else
    assert(allocator_traits<MyAlloc<int>>::max_size(alloc) == 0x3FFFFFFFUL);
#endif

    for (size_t k = 0; k < 8; ++k) {
        vector<int> v(k);
        iota(v.begin(), v.end(), 0);

        do {
            list<int, MyAlloc<int>> l(v.begin(), v.end(), alloc);
            l.sort();
            assert(is_sorted(l.begin(), l.end()));

            forward_list<int, MyAlloc<int>> fl(v.begin(), v.end(), alloc);
            fl.sort();
            assert(is_sorted(fl.begin(), fl.end()));

        } while (next_permutation(v.begin(), v.end()));
    }

    {
        mt19937 urng(1729);

        vector<int> v;

        for (int i = 0; i < 11213; ++i) {
            v.push_back(static_cast<int>(urng() & 0xFF));
        }

        for (int k = 0; k < 250; ++k) {
            list<int, MyAlloc<int>> l(v.begin(), v.begin() + k, alloc);
            l.sort();
            assert(is_sorted(l.begin(), l.end()));

            forward_list<int, MyAlloc<int>> fl(v.begin(), v.begin() + k, alloc);
            fl.sort();
            assert(is_sorted(fl.begin(), fl.end()));
        }

        list<int, MyAlloc<int>> l(v.begin(), v.end(), alloc);
        l.sort();
        assert(is_sorted(l.begin(), l.end()));

        forward_list<int, MyAlloc<int>> fl(v.begin(), v.end(), alloc);
        fl.sort();
        assert(is_sorted(fl.begin(), fl.end()));
    }

    // Also test that sort is stable (which also stresses merge for list)
    {
        const auto cmp_first = [](const pair<unsigned int, int>& lhs, const pair<unsigned int, int>& rhs) {
            return lhs.first < rhs.first;
        };

        mt19937 urng(1729);
        list<pair<unsigned int, int>> l;
        for (int i = 0; i < 1000; ++i) {
            l.emplace_back(urng(), 0);
        }

        auto it = l.begin();
        for (int i = 0; i < 1000; ++i, ++it) {
            l.emplace_back(it->first, 1);
        }

        forward_list<pair<unsigned int, int>> fl(l.begin(), l.end());

        l.sort(cmp_first);
        assert(is_sorted(l.begin(), l.end()));

        fl.sort(cmp_first);
        assert(is_sorted(fl.begin(), fl.end()));
    }

    // Also test for iterator stability bug introduced touching merge; we must not change end() in a merge
    {
        list<int> l{1, 2, 3, 4};
        list<int> target;
        auto i = target.end();
        target.merge(l);
        --i;
        assert(*i == 4);
    }

    {
        typedef MyAlloc<int> IntAlloc;
        typedef MyAlloc<pair<const int, int>> PairAlloc;

        typedef less<> Lt;
        typedef hash<int> Hf;
        typedef equal_to<int> Eq;
        Lt lt;
        Hf hf;
        Eq eq;

        initializer_list<int> il                   = {11, 22, 33};
        initializer_list<pair<const int, int>> ilp = {{10, 11}, {20, 22}, {30, 33}};

        deque<int, IntAlloc> d(10, 1729, alloc);
        forward_list<int, IntAlloc> fl(10, 1729, alloc);
        list<int, IntAlloc> l(10, 1729, alloc);
        vector<int, IntAlloc> v(10, 1729, alloc);

        vector<bool, MyAlloc<bool>> vb(10, true, alloc);

        map<int, int, Lt, PairAlloc> m(ilp, lt, alloc);
        multimap<int, int, Lt, PairAlloc> mm(ilp, lt, alloc);
        set<int, Lt, IntAlloc> s(il, lt, alloc);
        multiset<int, Lt, IntAlloc> ms(il, lt, alloc);

        unordered_map<int, int, Hf, Eq, PairAlloc> um(ilp, 100, hf, eq, alloc);
        unordered_multimap<int, int, Hf, Eq, PairAlloc> umm(ilp, 100, hf, eq, alloc);
        unordered_set<int, Hf, Eq, IntAlloc> us(il, 100, hf, eq, alloc);
        unordered_multiset<int, Hf, Eq, IntAlloc> ums(il, 100, hf, eq, alloc);

        basic_string<char, char_traits<char>, MyAlloc<char>> str("meow", alloc);
        // also test VSO-274112 "basic_string mishandles allocators / requires defaultconstructible"
        forward_list<char> strInput{'h', 'i', '!'};
        str.assign(strInput.begin(), strInput.end());
        str.append(strInput.begin(), strInput.end());
        str.insert(str.begin() + 1, strInput.begin(), strInput.end());
        str.replace(str.begin() + 1, str.begin() + 2, strInput.begin(), strInput.end());

        match_results<const char*, MyAlloc<csub_match>> mat(alloc);
        assert(regex_match("meow", mat, regex("m..w")));
    }

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
    {
        function<int(int)> f1(allocator_arg, alloc, [](int n) { return n * 2; });
        function<int(int)> f2(f1);
    }
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

    {
        shared_ptr<int> sp1(new int(1729), default_delete<int>(), alloc);
        shared_ptr<int> sp2 = allocate_shared<int>(alloc, 1729);
    }

#ifndef _M_CEE_PURE
    {
        promise<int> p(allocator_arg, alloc);
        future<int> f = p.get_future();
        p.set_value(1729);
        assert(f.get() == 1729);
    }

    {
        promise<int&> p(allocator_arg, alloc);
        future<int&> f = p.get_future();
        int n          = 4096;
        p.set_value(n);
        assert(&f.get() == &n);
    }

    {
        promise<void> p(allocator_arg, alloc);
        future<void> f = p.get_future();
        p.set_value();
        f.get();
    }

#if _HAS_FUNCTION_ALLOCATOR_SUPPORT
    {
        packaged_task<int()> pt(allocator_arg, alloc, [] { return 1234; });
        future<int> f = pt.get_future();
        pt();
        assert(f.get() == 1234);
    }

    {
        int n = 4096;
        packaged_task<int&()> pt(allocator_arg, alloc, [&n]() -> int& { return n; });
        future<int&> f = pt.get_future();
        pt();
        assert(&f.get() == &n);
    }

    {
        packaged_task<void()> pt(allocator_arg, alloc, [] {});
        future<void> f = pt.get_future();
        pt();
        f.get();
    }
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT
#endif // _M_CEE_PURE

    test_1119194();
    test_1184701();
    test_LWG_2455();
#if !_HAS_CXX20 // support removed in C++20 by P0619R4
    test_allocator_construct_const();
#endif // !_HAS_CXX20
}

// Also test DevDiv-1119194 "The STL should handle allocators that aren't assignable".

template <typename T>
struct NoProp {
    typedef T value_type;

    NoProp() noexcept {}
    NoProp(const NoProp&) noexcept {}
    template <typename U>
    NoProp(const NoProp<U>&) noexcept {}

    NoProp& operator=(const NoProp&) = delete; // Important!

    template <typename U>
    bool operator==(const NoProp<U>&) const noexcept {
        return true;
    }
    template <typename U>
    bool operator!=(const NoProp<U>&) const noexcept {
        return false;
    }

    T* allocate(const size_t n) const {
        return allocator<T>().allocate(n);
    }

    void deallocate(T* const p, const size_t n) const noexcept {
        allocator<T>().deallocate(p, n);
    }
};

template <typename Container>
void test_swap_copy_move() {
    Container a;
    Container b;

    a.swap(b);
    a = b;
    a = move(b);
}

void test_1119194() {
    using AB = NoProp<bool>;
    using AC = NoProp<char>;
    using AI = NoProp<int>;
    using AP = NoProp<pair<const int, int>>;
    using L  = less<int>;
    using H  = hash<int>;
    using E  = equal_to<int>;

    test_swap_copy_move<deque<int, AI>>();
    test_swap_copy_move<forward_list<int, AI>>();
    test_swap_copy_move<list<int, AI>>();
    test_swap_copy_move<vector<int, AI>>();
    test_swap_copy_move<vector<bool, AB>>();

    test_swap_copy_move<map<int, int, L, AP>>();
    test_swap_copy_move<multimap<int, int, L, AP>>();
    test_swap_copy_move<set<int, L, AI>>();
    test_swap_copy_move<multiset<int, L, AI>>();

    test_swap_copy_move<unordered_map<int, int, H, E, AP>>();
    test_swap_copy_move<unordered_multimap<int, int, H, E, AP>>();
    test_swap_copy_move<unordered_set<int, H, E, AI>>();
    test_swap_copy_move<unordered_multiset<int, H, E, AI>>();

    test_swap_copy_move<basic_string<char, char_traits<char>, AC>>();
}

// Test DevDiv-1184701 "<memory>: We should handle construct/destroy returning non-void".

template <typename T>
struct NonVoid {
    typedef T value_type;
    NonVoid() noexcept {}
    template <typename U>
    NonVoid(const NonVoid<U>&) noexcept {}
    template <typename U>
    bool operator==(const NonVoid<U>&) const noexcept {
        return true;
    }
    template <typename U>
    bool operator!=(const NonVoid<U>&) const noexcept {
        return false;
    }

    T* allocate(const size_t n) const {
        return allocator<T>().allocate(n);
    }

    int deallocate(T* const p, const size_t n) const noexcept {
        allocator<T>().deallocate(p, n);
        return -9;
    }

    template <typename U, typename... Args>
    int construct(U* const p, Args&&... args) const {
        ::new (static_cast<void*>(p)) U(forward<Args>(args)...);
        return -99;
    }

    template <typename U>
    int destroy(U* const p) const noexcept {
        p->~U();
        return -999;
    }
};

void test_1184701() {
    const vector<int, NonVoid<int>> v = {11, 22, 33};

    assert(v[1] == 22);
}


// LWG-2455 "Allocator default construction should be allowed to throw"
bool g_true = true; // Silence warnings.

template <typename T>
struct ThrowingDefaultCtor {
    typedef T value_type;

    ThrowingDefaultCtor() {
        if (g_true) {
            throw 2015;
        }
    }

    ThrowingDefaultCtor(const ThrowingDefaultCtor&) noexcept {}
    template <typename U>
    ThrowingDefaultCtor(const ThrowingDefaultCtor<U>&) noexcept {}

    template <typename U>
    bool operator==(const ThrowingDefaultCtor<U>&) const noexcept {
        return true;
    }
    template <typename U>
    bool operator!=(const ThrowingDefaultCtor<U>&) const noexcept {
        return false;
    }

    T* allocate(const size_t n) const {
        return allocator<T>().allocate(n);
    }

    void deallocate(T* const p, const size_t n) const noexcept {
        allocator<T>().deallocate(p, n);
    }
};

void test_LWG_2455() {
    STATIC_ASSERT(is_nothrow_default_constructible_v<allocator<int>>);
    STATIC_ASSERT(is_nothrow_default_constructible_v<string>);
    STATIC_ASSERT(is_nothrow_default_constructible_v<vector<int>>);

    using ThrowingString = basic_string<char, char_traits<char>, ThrowingDefaultCtor<char>>;
    using ThrowingVector = vector<int, ThrowingDefaultCtor<int>>;

    STATIC_ASSERT(is_default_constructible_v<ThrowingDefaultCtor<int>>);
    STATIC_ASSERT(!is_nothrow_default_constructible_v<ThrowingDefaultCtor<int>>);
    STATIC_ASSERT(is_default_constructible_v<ThrowingString>);
    STATIC_ASSERT(!is_nothrow_default_constructible_v<ThrowingString>);
    STATIC_ASSERT(is_default_constructible_v<ThrowingVector>);
    STATIC_ASSERT(!is_nothrow_default_constructible_v<ThrowingVector>);

    try {
        ThrowingString str;
        abort();
    } catch (const int e) {
        assert(e == 2015);
    }

    try {
        ThrowingVector vec;
        abort();
    } catch (const int e) {
        assert(e == 2015);
    }
}

#if !_HAS_CXX20 // support removed in C++20 by P0619R4
void test_allocator_construct_const() {
    // Annex D actually requires the default allocator to const_cast here
    // See N4659 D.9 [depr.default.allocator]/6
    int example                  = 0;
    const int* const exampleCptr = &example;
    allocator<int> alloc;
    alloc.construct(&example, 42);
    assert(example == 42);
    allocator_traits<allocator<int>>::construct(alloc, exampleCptr, 1729);
    assert(example == 1729);
}
#endif // !_HAS_CXX20
