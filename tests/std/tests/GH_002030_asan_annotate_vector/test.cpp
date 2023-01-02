// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// REQUIRES: x64 || x86

#include <cassert>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#pragma warning(disable : 4984) // 'if constexpr' is a C++17 language extension

#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++17-extensions" // constexpr if is a C++17 extension
#endif // __clang__

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

#ifndef __SANITIZE_ADDRESS__
#if defined(__clang__) && defined(__has_feature)
#if __has_feature(address_sanitizer)
#define __SANITIZE_ADDRESS__
#endif
#endif
#endif

#ifdef __SANITIZE_ADDRESS__
extern "C" {
void* __sanitizer_contiguous_container_find_bad_address(const void* beg, const void* mid, const void* end) noexcept;
void __asan_describe_address(void*) noexcept;
}
#endif // ASan instrumentation enabled

struct non_trivial_can_throw {
    non_trivial_can_throw() {
        ++i;
        if (i == 0) {
            throw i;
        }
    }

    non_trivial_can_throw(const non_trivial_can_throw&) {
        ++i;
        if (i == 0) {
            throw i;
        }
    }

    non_trivial_can_throw& operator=(const non_trivial_can_throw&) {
        return *this;
    }

    unsigned int i = 0;
};

struct non_trivial_cannot_throw {
    non_trivial_cannot_throw() noexcept {
        ++i;
    }

    unsigned int i = 0;
};

struct throw_on_construction {
    throw_on_construction() {
        throw 0;
    }

    explicit throw_on_construction(bool should_throw) {
        if (should_throw) {
            throw 0;
        }
    }

    throw_on_construction(const throw_on_construction&) {
        throw 0;
    }

    [[noreturn]] throw_on_construction& operator=(const throw_on_construction&) {
        throw 0;
    }
};

struct throw_on_copy {
    throw_on_copy() = default;

    throw_on_copy(const throw_on_copy&) {
        throw 0;
    }

    throw_on_copy(throw_on_copy&&) {}

    [[noreturn]] throw_on_copy& operator=(const throw_on_copy&) {
        throw 0;
    }
};

template <class T, int N>
class input_iterator_tester {
private:
    T data[N] = {};

public:
    class iterator {
    private:
        T* curr;

    public:
        using iterator_category = input_iterator_tag;
        using value_type        = T;
        using difference_type   = ptrdiff_t;
        using pointer           = void;
        using reference         = T&;

        explicit iterator(T* start) : curr(start) {}

        reference operator*() const {
            return *curr;
        }

        iterator& operator++() {
            ++curr;
            return *this;
        }

        iterator operator++(int) {
            auto tmp = *this;
            ++curr;
            return tmp;
        }

        bool operator==(const iterator& that) const {
            return curr == that.curr;
        }

        bool operator!=(const iterator& that) const {
            return !(*this == that);
        }
    };

    iterator begin() {
        return iterator(data);
    }

    iterator end() {
        return iterator(data + N);
    }
};

template <class T, class Alloc>
bool verify_vector(vector<T, Alloc>& vec) {
#ifdef __SANITIZE_ADDRESS__
    const void* buffer  = vec.data();
    const void* buf_end = vec.data() + vec.capacity();
    _Asan_aligned_pointers aligned;

    if constexpr ((_Container_allocation_minimum_asan_alignment<vector<T, Alloc>>) >= 8) {
        aligned = {buffer, buf_end};
    } else {
        aligned = _Get_asan_aligned_first_end(buffer, buf_end);
        if (aligned._First == aligned._End) {
            return true;
        }
    }

    const void* const mid       = vec.data() + vec.size();
    const void* const fixed_mid = aligned._Clamp_to_end(mid);

    void* bad_address = __sanitizer_contiguous_container_find_bad_address(aligned._First, fixed_mid, aligned._End);
    if (bad_address == nullptr) {
        return true;
    }

    if (bad_address < mid) {
        cout << bad_address << " was marked as poisoned when it should not be." << endl;
    } else {
        cout << bad_address << " was not marked as poisoned when it should be." << endl;
    }
    cout << "Vector State:" << endl;
    cout << "  begin:         " << buffer << endl;
    cout << "  aligned begin: " << aligned._First << endl;
    cout << "  last:          " << mid << endl;
    cout << "  aligned_last:  " << fixed_mid << endl;
    cout << "  end:           " << buf_end << endl;
    cout << "  aligned_end:   " << aligned._End << endl;
    __asan_describe_address(bad_address);

    return false;
#else // ^^^ ASan instrumentation enabled / ASan instrumentation disabled vvv
    (void) vec;
    return true;
#endif // Asan instrumentation disabled
}

// Note: This class does not satisfy all the allocator requirements but is sufficient for this test.
template <class T, class Pocma, class Stateless>
struct custom_test_allocator {
    using value_type                             = T;
    using propagate_on_container_move_assignment = Pocma;
    using is_always_equal                        = Stateless;
};

template <class T1, class T2, class Pocma, class Stateless>
constexpr bool operator==(
    const custom_test_allocator<T1, Pocma, Stateless>&, const custom_test_allocator<T2, Pocma, Stateless>&) noexcept {
    return Stateless::value;
}

template <class T1, class T2, class Pocma, class Stateless>
constexpr bool operator!=(
    const custom_test_allocator<T1, Pocma, Stateless>&, const custom_test_allocator<T2, Pocma, Stateless>&) noexcept {
    return !Stateless::value;
}

template <class T, class Pocma = true_type, class Stateless = true_type>
struct aligned_allocator : custom_test_allocator<T, Pocma, Stateless> {
    static constexpr size_t _Minimum_asan_allocation_alignment = 8;

    aligned_allocator() = default;
    template <class U>
    constexpr aligned_allocator(const aligned_allocator<U, Pocma, Stateless>&) noexcept {}

    T* allocate(size_t n) {
        return new T[n];
    }

    void deallocate(T* p, size_t) noexcept {
        delete[] p;
    }
};
STATIC_ASSERT(_Container_allocation_minimum_asan_alignment<vector<char, aligned_allocator<char>>> == 8);

template <class T, class Pocma = true_type, class Stateless = true_type>
struct explicit_allocator : custom_test_allocator<T, Pocma, Stateless> {
    static constexpr size_t _Minimum_asan_allocation_alignment = alignof(T);

    explicit_allocator() = default;
    template <class U>
    constexpr explicit_allocator(const explicit_allocator<U, Pocma, Stateless>&) noexcept {}

    T* allocate(size_t n) {
        T* mem = new T[n + 1];
        return mem + 1;
    }

    void deallocate(T* p, size_t) noexcept {
        delete[] (p - 1);
    }
};
STATIC_ASSERT(_Container_allocation_minimum_asan_alignment<vector<char, explicit_allocator<char>>> == 1);
STATIC_ASSERT(_Container_allocation_minimum_asan_alignment<vector<wchar_t, explicit_allocator<wchar_t>>> == 2);

template <class T, class Pocma = true_type, class Stateless = true_type>
struct implicit_allocator : custom_test_allocator<T, Pocma, Stateless> {
    implicit_allocator() = default;
    template <class U>
    constexpr implicit_allocator(const implicit_allocator<U, Pocma, Stateless>&) noexcept {}

    T* allocate(size_t n) {
        T* mem = new T[n + 1];
        return mem + 1;
    }

    void deallocate(T* p, size_t) noexcept {
        delete[] (p - 1);
    }
};
STATIC_ASSERT(_Container_allocation_minimum_asan_alignment<vector<char, implicit_allocator<char>>> == 1);
STATIC_ASSERT(_Container_allocation_minimum_asan_alignment<vector<wchar_t, implicit_allocator<wchar_t>>> == 2);

template <class Alloc>
void test_push_pop() {
    using T = typename Alloc::value_type;

    vector<T, Alloc> v;
    assert(verify_vector(v));

    v.push_back(T());
    assert(verify_vector(v));

    v.pop_back();
    assert(verify_vector(v));
}

template <class Alloc, int Size = 1024, int Stride = 128>
void test_reserve_shrink() {
    using T = typename Alloc::value_type;

    vector<T, Alloc> v;
    assert(verify_vector(v));

    v.reserve(Size);
    assert(verify_vector(v));

    for (int i = 0; i < Size; i += Stride) {
        for (int j = 0; j < Stride && j + i < Size; ++j) {
            v.push_back(T());
        }

        assert(verify_vector(v));
    }

    v.push_back(T());
    assert(verify_vector(v));

    for (int i = 0; i < Size; i += Stride) {
        for (int j = 0; j < Stride && j + i < Size; ++j) {
            v.pop_back();
        }

        v.shrink_to_fit();
        assert(verify_vector(v));
    }

    v.pop_back();
    assert(verify_vector(v));
    v.shrink_to_fit();
    assert(verify_vector(v));
}

template <class Alloc>
void test_emplace_pop() {
    using T = typename Alloc::value_type;

    vector<T, Alloc> v;
    assert(verify_vector(v));

    v.emplace_back(T());
    assert(verify_vector(v));

    v.emplace(v.begin(), T());
    assert(verify_vector(v));

    v.emplace(v.end(), T());
    assert(verify_vector(v));

    v.pop_back();
    assert(verify_vector(v));
}

template <class Alloc>
void test_move_assign() {
    using T = typename Alloc::value_type;

    vector<T, Alloc> v1;
    vector<T, Alloc> v2;
    assert(verify_vector(v1));
    assert(verify_vector(v2));

    v1.push_back(T());
    assert(verify_vector(v1));

    v2 = move(v1);
    assert(verify_vector(v1));
    assert(verify_vector(v2));

    vector<T, Alloc> v3;
    vector<T, Alloc> v4;
    assert(verify_vector(v3));
    assert(verify_vector(v4));

    v3.reserve(v3.capacity() + 1);
    assert(verify_vector(v3));
    v3.resize(v3.capacity() + 1, T());
    assert(verify_vector(v3));
    v3.reserve(v3.capacity() + 1);
    assert(verify_vector(v3));
    v4.resize(v3.capacity(), T());
    assert(verify_vector(v4));
    v3 = move(v4);
    assert(verify_vector(v3));
    assert(verify_vector(v4));
}

template <class Alloc>
void test_copy_assign() {
    using T = typename Alloc::value_type;

    vector<T, Alloc> v1;
    vector<T, Alloc> v2;
    assert(verify_vector(v1));
    assert(verify_vector(v2));

    v1.push_back(T());
    assert(verify_vector(v1));

    v2 = v1;
    assert(verify_vector(v1));
    assert(verify_vector(v2));
}

template <class Alloc, int N = 128>
void test_constructors() {
    using T  = typename Alloc::value_type;
    Alloc al = Alloc();

    vector<T, Alloc> v1;
    vector<T, Alloc> v2(al);
    vector<T, Alloc> v3(N, T());
    vector<T, Alloc> v4(N);
    assert(verify_vector(v1));
    assert(verify_vector(v2));
    assert(verify_vector(v3));
    assert(verify_vector(v4));

    vector<T, Alloc> v5(v3.begin(), v3.end());
    vector<T, Alloc> v6(v3);
    vector<T, Alloc> v7(v3, al);
    assert(verify_vector(v5));
    assert(verify_vector(v6));
    assert(verify_vector(v7));

    vector<T, Alloc> v8(move(v3));
    vector<T, Alloc> v9(move(v4), al);
    assert(verify_vector(v8));
    assert(verify_vector(v9));

    vector<T, Alloc> v10({T(), T()});
    assert(verify_vector(v10));
}

template <class Alloc, int N = 128>
void test_insert_n() {
    using T = typename Alloc::value_type;

    vector<T, Alloc> v(1);

    v.insert(v.begin(), N, T());
    assert(verify_vector(v));
    v.insert(v.end(), N, T());
    assert(verify_vector(v));
    v.insert(v.begin() + N, N, T());
    assert(verify_vector(v));
}

template <class Alloc, int N = 128>
void test_insert_range() {
    using T = typename Alloc::value_type;

    vector<T, Alloc> v1(1);
    vector<T, Alloc> v2(N);
    input_iterator_tester<T, N> t;

    v1.insert(v1.begin(), v2.begin(), v2.end());
    assert(verify_vector(v1));
    v1.insert(v1.end(), v2.begin(), v2.end());
    assert(verify_vector(v1));
    v1.insert(v1.begin() + N, v2.begin(), v2.end());
    assert(verify_vector(v1));

    v1.insert(v1.begin(), t.begin(), t.end());
    assert(verify_vector(v1));
    v1.insert(v1.end(), t.begin(), t.end());
    assert(verify_vector(v1));
    v1.insert(v1.begin() + N, t.begin(), t.end());
    assert(verify_vector(v1));
}

template <class Alloc, int N = 128>
void test_assign() {
    using T = typename Alloc::value_type;

    vector<T, Alloc> v1(1);
    vector<T, Alloc> v2(N + 1);
    vector<T, Alloc> v3(N + 2);
    input_iterator_tester<T, N + 2> t1;
    input_iterator_tester<T, N + 3> t2;

    v1.assign(N, T());
    assert(verify_vector(v1));
    v1.assign(v2.begin(), v2.end());
    assert(verify_vector(v1));
    v1.assign(v3.begin(), v3.end());
    assert(verify_vector(v1));
    v1.reserve(v1.size() + 1);
    assert(verify_vector(v1));
    vector<T, Alloc> larger(v1.capacity());
    v1.assign(larger.begin(), larger.end());
    assert(verify_vector(v1));

    v1.assign(t1.begin(), t1.end());
    assert(verify_vector(v1));
    v1.assign(t2.begin(), t2.end());
    assert(verify_vector(v1));
    v1.assign(t1.begin(), t1.end());
    assert(verify_vector(v1));

    v1.assign(v3.begin(), v3.end());
    assert(verify_vector(v1));
    v1.assign(v2.begin(), v2.end());
    assert(verify_vector(v1));

    v1.assign(N, T());
    assert(verify_vector(v1));
    v1.reserve(v1.size() + 1);
    assert(verify_vector(v1));
    v1.assign(v1.capacity(), T());
    assert(verify_vector(v1));

    vector<T, Alloc> v4;
    v4.assign({T()});
    assert(verify_vector(v4));
    v4.assign({T(), T()});
    assert(verify_vector(v4));
    v4.assign({T()});
    assert(verify_vector(v4));
}

template <class Alloc, int N = 128>
void test_resize() {
    using T = typename Alloc::value_type;

    vector<T, Alloc> v;
    v.resize(N, T());
    assert(verify_vector(v));
    v.resize(1, T());
    assert(verify_vector(v));
    v.resize(v.capacity(), T());
    assert(verify_vector(v));
    v.resize(v.size() - 1, T());
    assert(verify_vector(v));
    v.resize(v.capacity() + N, T());
    assert(verify_vector(v));
}

void test_push_back_throw() {
    {
        vector<throw_on_construction> v;
        v.reserve(1);

        throw_on_construction t(false);
        try {
            v.push_back(t);
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }
    {
        vector<throw_on_construction> v;

        throw_on_construction t(false);
        try {
            v.push_back(t);
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }
    {
        vector<throw_on_construction> v;
        v.reserve(1);

        try {
            v.push_back(throw_on_construction(false));
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }
    {
        vector<throw_on_construction> v;

        try {
            v.push_back(throw_on_construction(false));
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }
}

void test_emplace_back_throw() {
    {
        vector<throw_on_construction> v;
        v.reserve(1);

        try {
            v.emplace_back(true);
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }
    {
        vector<throw_on_construction> v;

        try {
            v.emplace_back(true);
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }
}

void test_insert_range_throw() {
    {
        vector<throw_on_construction> v;

        v.reserve(4);
        v.emplace_back(false);
        v.emplace_back(false);

        try {
            v.insert(v.begin(), {throw_on_construction(false), throw_on_construction(false)});
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }

    {
        vector<throw_on_construction> v;

        v.reserve(2);
        v.emplace_back(false);
        v.emplace_back(false);

        try {
            v.insert(v.begin(), {throw_on_construction(false), throw_on_construction(false)});
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }

    {
        vector<throw_on_construction> v;

        v.reserve(2);

        try {
            v.insert(v.end(), {throw_on_construction(false), throw_on_construction(false)});
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }

    {
        vector<throw_on_construction> v;

        try {
            v.insert(v.end(), {throw_on_construction(false), throw_on_construction(false)});
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }
}

void test_insert_throw() {
    {
        vector<throw_on_construction> v;

        v.reserve(3);
        v.emplace_back(false);
        v.emplace_back(false);

        try {
            v.insert(v.begin(), throw_on_construction(false));
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }

    {
        vector<throw_on_construction> v;

        v.reserve(2);
        v.emplace_back(false);
        v.emplace_back(false);

        try {
            v.insert(v.begin(), throw_on_construction(false));
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }

    {
        vector<throw_on_construction> v;

        v.reserve(1);

        try {
            v.insert(v.end(), throw_on_construction(false));
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }

    {
        vector<throw_on_construction> v;

        try {
            v.insert(v.end(), throw_on_construction(false));
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }
}

void test_emplace_throw() {
    {
        vector<throw_on_construction> v;

        v.reserve(3);
        v.emplace_back(false);
        v.emplace_back(false);

        try {
            v.emplace(v.begin(), false);
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }

    {
        vector<throw_on_construction> v;

        v.reserve(2);
        v.emplace_back(false);
        v.emplace_back(false);

        try {
            v.emplace(v.begin(), true);
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }

    {
        vector<throw_on_construction> v;

        v.reserve(1);

        try {
            v.emplace(v.end(), true);
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }

    {
        vector<throw_on_construction> v;

        try {
            v.emplace(v.end(), true);
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }
}

void test_resize_throw() {
    {
        vector<throw_on_construction> v;

        v.reserve(2);
        v.emplace_back(false);

        try {
            v.resize(2);
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }

    {
        vector<throw_on_construction> v;

        v.reserve(1);
        v.emplace_back(false);

        try {
            v.resize(2);
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }

    {
        vector<throw_on_copy> v;

        v.reserve(2);
        v.push_back(throw_on_copy());

        try {
            v.resize(2, throw_on_copy());
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }

    {
        vector<throw_on_copy> v;

        v.reserve(1);
        v.push_back(throw_on_copy());

        try {
            v.resize(2, throw_on_copy());
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }
}

void test_insert_n_throw() {
    {
        vector<throw_on_construction> v;

        v.reserve(2);
        v.emplace_back(false);

        try {
            v.insert(v.begin(), 2, throw_on_construction(false));
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }

    {
        vector<throw_on_construction> v;

        v.reserve(2);
        v.emplace_back(false);

        try {
            v.insert(v.end(), 2, throw_on_construction(false));
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }

    {
        vector<throw_on_construction> v;

        v.reserve(1);
        v.emplace_back(false);

        try {
            v.insert(v.begin(), 2, throw_on_construction(false));
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }

    {
        vector<throw_on_construction> v;

        v.reserve(1);
        v.emplace_back(false);

        try {
            v.insert(v.end(), 2, throw_on_construction(false));
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }

    {
        vector<throw_on_copy> v;

        v.reserve(2);
        v.push_back(throw_on_copy());

        try {
            v.insert(v.begin(), 2, throw_on_copy());
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }

    {
        vector<throw_on_copy> v;

        v.reserve(2);
        v.push_back(throw_on_copy());

        try {
            v.insert(v.end(), 2, throw_on_copy());
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }

    {
        vector<throw_on_copy> v;

        v.reserve(1);
        v.push_back(throw_on_copy());

        try {
            v.insert(v.begin(), 2, throw_on_copy());
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }

    {
        vector<throw_on_copy> v;

        v.reserve(1);
        v.push_back(throw_on_copy());

        try {
            v.insert(v.end(), 2, throw_on_copy());
            assert(false);
        } catch (int) {
            assert(verify_vector(v));
        }
    }
}

template <class Alloc>
void test_clear() {
    using T = typename Alloc::value_type;

    vector<T, Alloc> v;
    v.push_back(T());
    v.assign(v.capacity() + 1, T());
    assert(verify_vector(v));
    v.clear();
    assert(verify_vector(v));
    v.clear();
    assert(verify_vector(v));
}

template <class Alloc>
void test_empty() {
    using T = typename Alloc::value_type;

    vector<T, Alloc> v1;
    v1.clear();
    v1.resize(0);
    v1.shrink_to_fit();
    v1.assign(0, T());
    v1.assign({});
    v1 = {};

    vector<T, Alloc> v2;
    v1.assign(v2.begin(), v2.end());

    input_iterator_tester<T, 11> in;
    auto e = in.end();
    v1.assign(e, e); // empty range of input iterators

    vector<T, Alloc> v3;
    v1 = v3;
    v3 = move(v1);
}

template <class Alloc>
void run_tests() {
    test_push_pop<Alloc>();
    test_reserve_shrink<Alloc>();
    test_emplace_pop<Alloc>();
    test_move_assign<Alloc>();
    test_copy_assign<Alloc>();
    test_constructors<Alloc>();
    test_insert_n<Alloc>();
    test_insert_range<Alloc>();
    test_assign<Alloc>();
    test_resize<Alloc>();
    test_clear<Alloc>();
    test_empty<Alloc>();
}

template <class T, template <class, class, class> class AllocT>
void run_custom_allocator_matrix() {
    run_tests<AllocT<T, true_type, true_type>>();
    run_tests<AllocT<T, true_type, false_type>>();
    run_tests<AllocT<T, false_type, true_type>>();
    run_tests<AllocT<T, false_type, false_type>>();
}

template <class T>
void run_allocator_matrix() {
    run_tests<allocator<T>>();
    run_custom_allocator_matrix<T, aligned_allocator>();
    run_custom_allocator_matrix<T, explicit_allocator>();
    run_custom_allocator_matrix<T, implicit_allocator>();
}

int main() {
    // Do some work even when we aren't instrumented
    run_allocator_matrix<char>();
#ifdef __SANITIZE_ADDRESS__
    run_allocator_matrix<int>();
    run_allocator_matrix<double>();
    run_allocator_matrix<non_trivial_can_throw>();
    run_allocator_matrix<non_trivial_cannot_throw>();

// TRANSITION, LLVM-35365
#ifndef __clang__
    test_push_back_throw();
    test_emplace_back_throw();
    test_insert_range_throw();
    test_insert_throw();
    test_emplace_throw();
    test_resize_throw();
    test_insert_n_throw();
#endif // !__clang__
#endif // ASan instrumentation enabled
}
