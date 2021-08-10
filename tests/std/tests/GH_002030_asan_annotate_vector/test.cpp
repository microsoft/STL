// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// REQUIRES: asan, x64 || x86

#include <cassert>
#include <list>
#include <memory>
#include <type_traits>
#include <vector>

using namespace std;

extern "C" int __sanitizer_verify_contiguous_container(const void* beg, const void* mid, const void* end);

template <class T, class Alloc>
bool verify_vector(vector<T, Alloc>& vec) {
    size_t buffer_size  = vec.capacity() * sizeof(T);
    void* buffer        = static_cast<void*>(vec.data());
    void* aligned_start = align(8, 1, buffer, buffer_size);

    if (!aligned_start) {
        return true;
    }

    void* mid = static_cast<void*>(vec.data() + vec.size());
    mid       = mid > aligned_start ? mid : aligned_start;

    return __sanitizer_verify_contiguous_container(aligned_start, mid, vec.data() + vec.capacity()) != 0;
}

template <class T, class Pocma, class Stateless>
struct custom_test_allocator {
    using value_type                             = T;
    using propagate_on_container_move_assignment = Pocma;
    using is_always_equal                        = Stateless;
};

template <class T, class Pocma, class Stateless>
constexpr bool operator==(const custom_test_allocator<T, Pocma, Stateless>& lhs,
    const custom_test_allocator<T, Pocma, Stateless>& rhs) noexcept {
    return Stateless::value || (&lhs == &rhs);
}

template <class T, class Pocma, class Stateless>
constexpr bool operator!=(const custom_test_allocator<T, Pocma, Stateless>& lhs,
    const custom_test_allocator<T, Pocma, Stateless>& rhs) noexcept {
    return !(lhs == rhs);
}

template <class T, class Pocma = true_type, class Stateless = true_type>
struct aligned_allocator : public custom_test_allocator<T, Pocma, Stateless> {
    static constexpr size_t _Minimum_allocation_alignment = 8;

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

template <class T, class Pocma = true_type, class Stateless = true_type>
struct explicit_allocator : public custom_test_allocator<T, Pocma, Stateless> {
    static constexpr size_t _Minimum_allocation_alignment = alignof(T);

    explicit_allocator() = default;
    template <class U>
    constexpr explicit_allocator(const explicit_allocator<U, Pocma, Stateless>&) noexcept {}

    T* allocate(size_t n) {
        T* mem = new T[n + 1];
        return mem + 1;
    }

    void deallocate(T* p, size_t) noexcept {
        delete[](p - 1);
    }
};

template <class T, class Pocma = true_type, class Stateless = true_type>
struct implicit_allocator : public custom_test_allocator<T, Pocma, Stateless> {
    implicit_allocator() = default;
    template <class U>
    constexpr implicit_allocator(const implicit_allocator<U, Pocma, Stateless>&) noexcept {}

    T* allocate(size_t n) {
        T* mem = new T[n + 1];
        return mem + 1;
    }

    void deallocate(T* p, size_t) noexcept {
        delete[](p - 1);
    }
};

template <class Alloc>
void test_case_push_pop() {
    using T = typename Alloc::value_type;

    vector<T, Alloc> v;
    assert(verify_vector(v));

    v.push_back(T());
    assert(verify_vector(v));

    v.pop_back();
    assert(verify_vector(v));
}

template <class Alloc, int Size = 1024, int Stride = 128>
void test_case_reserve_shrink() {
    using T = typename Alloc::value_type;

    vector<T, Alloc> v;
    assert(verify_vector(v));

    v.reserve(Size);
    assert(verify_vector(v));

    for (int i = 0; i < Size; i += Stride) {
        for (int j = i; j < Stride && j + i < Size; ++j) {
            v.push_back(T());
        }

        assert(verify_vector(v));
    }

    v.push_back(T());
    assert(verify_vector(v));

    for (int i = 0; i < Size; i += Stride) {
        for (int j = i; j < Stride && j + i < Size; ++j) {
            v.pop_back();
        }

        v.shrink_to_fit();
        assert(verify_vector(v));
    }

    v.pop_back();
    v.shrink_to_fit();
    assert(verify_vector(v));
}

template <class Alloc>
void test_case_emplace_pop() {
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
void test_case_move_assign() {
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
}

template <class Alloc>
void test_case_copy_assign() {
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
void test_case_constructors() {
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
void test_case_insert_n() {
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
void test_case_insert_range() {
    using T = typename Alloc::value_type;

    vector<T, Alloc> v1(1);
    vector<T, Alloc> v2(N);
    list<T> l(N);

    v1.insert(v1.begin(), v2.begin(), v2.end());
    assert(verify_vector(v1));
    v1.insert(v1.end(), v2.begin(), v2.end());
    assert(verify_vector(v1));
    v1.insert(v1.begin() + N, v2.begin(), v2.end());
    assert(verify_vector(v1));

    v1.insert(v1.begin(), l.begin(), l.end());
    assert(verify_vector(v1));
    v1.insert(v1.end(), l.begin(), l.end());
    assert(verify_vector(v1));
    v1.insert(v1.begin() + N, l.begin(), l.end());
    assert(verify_vector(v1));
}

template <class Alloc, int N = 128>
void test_case_assign() {
    using T = typename Alloc::value_type;

    vector<T, Alloc> v1(1);
    vector<T, Alloc> v2(N + 1);
    vector<T, Alloc> v3(N + 2);
    list<T> l1(N + 2);
    list<T> l2(N + 3);

    v1.assign(N, T());
    assert(verify_vector(v1));
    v1.assign(v2.begin(), v2.end());
    assert(verify_vector(v1));
    v1.assign(v3.begin(), v3.end());
    assert(verify_vector(v1));
    v1.assign(l1.begin(), l1.end());
    assert(verify_vector(v1));
    v1.assign(l2.begin(), l2.end());
    assert(verify_vector(v1));
    v1.assign(l1.begin(), l1.end());
    assert(verify_vector(v1));
    v1.assign(v3.begin(), v3.end());
    assert(verify_vector(v1));
    v1.assign(v2.begin(), v2.end());
    assert(verify_vector(v1));
    v1.assign(N, T());
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
void test_case_resize() {
    using T = typename Alloc::value_type;

    vector<T, Alloc> v;
    v.resize(N, T());
    assert(verify_vector(v));
    v.resize(1, T());
    assert(verify_vector(v));
}

template <class Alloc>
void run_tests() {
    test_case_push_pop<Alloc>();
    test_case_reserve_shrink<Alloc>();
    test_case_emplace_pop<Alloc>();
    test_case_move_assign<Alloc>();
    test_case_copy_assign<Alloc>();
    test_case_constructors<Alloc>();
    test_case_insert_n<Alloc>();
    test_case_insert_range<Alloc>();
    test_case_assign<Alloc>();
    test_case_resize<Alloc>();
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
    run_allocator_matrix<char>();
    run_allocator_matrix<int>();
    run_allocator_matrix<double>();

    // TODO: Test a type that throws.
}
