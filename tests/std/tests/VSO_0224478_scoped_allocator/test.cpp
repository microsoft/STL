// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <memory>
#include <new>
#include <scoped_allocator>
#include <tuple>
#include <utility>
#include <vector>

using namespace std;

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template <typename T>
struct VSO_129349_alloc : allocator<T> {
    template <typename U>
    struct rebind {
        using other = VSO_129349_alloc<U>;
    };

    template <typename U>
    VSO_129349_alloc(const U&) {}
};

template <typename T>
allocator<T> VSO_129349_make_alloc() {
    return allocator<T>();
}

void test_case_VSO_129349_non_default_constructible_allocators() {
    scoped_allocator_adaptor<VSO_129349_alloc<char>> a00(VSO_129349_make_alloc<char>());
}

void test_case_VSO_184838_scoped_allocator_assignment_conversions() {
    allocator<vector<int>> vec_alloc{};
    allocator<int> int_alloc{};
    using my_vector_saa = scoped_allocator_adaptor<allocator<vector<int>>, allocator<int>>;

    my_vector_saa saa1(move(vec_alloc), move(int_alloc));

    using my_double_saa = scoped_allocator_adaptor<allocator<vector<double>>, allocator<int>>;
    my_double_saa saa2(saa1);
    saa2 = move(saa1);

    vector<int> vec1{};
    vector<int> vec2{};
    using my_pair = pair<vector<int>, vector<int>>;
    auto pair_ptr = static_cast<my_pair*>(malloc(sizeof(my_pair)));
    saa1.construct(pair_ptr, move(vec1), move(vec2));
    saa1.destroy(pair_ptr);
    free(pair_ptr);
}

void test_case_VSO_224478_allow_stacking() {
    vector<int, scoped_allocator_adaptor<scoped_allocator_adaptor<allocator<int>>>> vec;
    vec.push_back(42);
}

int g_min_alloc_construct_count = 0;

template <typename T>
struct min_alloc {
    using value_type = T;

    min_alloc() = default;
    template <typename U>
    min_alloc(const min_alloc<U>&) {}
    min_alloc(const min_alloc&)            = default;
    min_alloc& operator=(const min_alloc&) = default;

    value_type* allocate(size_t n) {
        return allocator<T>().allocate(n);
    }

    void deallocate(value_type* p, size_t n) {
        allocator<T>().deallocate(p, n);
    }

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        ++g_min_alloc_construct_count;
        ::new (static_cast<void*>(p)) U(forward<Args>(args)...);
    }
};

template <typename T, typename U>
bool operator==(const min_alloc<T>&, const min_alloc<U>&) {
    return true;
}

template <typename T, typename U>
bool operator!=(const min_alloc<T>&, const min_alloc<U>&) {
    return false;
}

void test_case_VSO_224478_piecewise_construct_calls_allocator_construct() {
    scoped_allocator_adaptor<min_alloc<pair<int, int>>> sa;
    const auto ptr = sa.allocate(1);
    sa.construct(ptr, piecewise_construct, tuple<int>(42), tuple<int>(1729));
    assert(g_min_alloc_construct_count == 1);
    sa.destroy(ptr);
    sa.deallocate(ptr, 1);
}

struct lwg_2586 {
    using allocator_type = allocator<lwg_2586>;
    lwg_2586(allocator_arg_t, allocator_type&&) {}
    lwg_2586(const allocator_type&) {}
};

void test_case_LWG_2586() {
    // LWG-2586: "Wrong value category used in scoped_allocator_adaptor::construct()"
    scoped_allocator_adaptor<allocator<lwg_2586>> sa;
    const auto ptr = sa.allocate(1);
    sa.construct(ptr);
    sa.destroy(ptr);
    sa.deallocate(ptr, 1);
}

void test_case_move_rebind_one_alloc() {
    scoped_allocator_adaptor<allocator<int>> sa;
    scoped_allocator_adaptor<allocator<double>> target(move(sa));
}

namespace test_LWG_2782 {
    // LWG-2782: "scoped_allocator_adaptor constructors must be constrained"
    // Verify prerequisites
    STATIC_ASSERT(!is_constructible_v<allocator<int>, void*>);
    STATIC_ASSERT(!is_constructible_v<allocator<int>, min_alloc<int>>);
    STATIC_ASSERT(!is_constructible_v<allocator<int>, const min_alloc<int>&>);

    using SA  = scoped_allocator_adaptor<allocator<int>>;
    using SAA = scoped_allocator_adaptor<allocator<int>, allocator<int>>;
    using SM  = scoped_allocator_adaptor<min_alloc<int>>;
    using SMA = scoped_allocator_adaptor<min_alloc<int>, allocator<int>>;

    // Actual tests
    STATIC_ASSERT(is_constructible_v<SA, allocator<float>>);
    STATIC_ASSERT(!is_constructible_v<SA, void*>);
    STATIC_ASSERT(is_constructible_v<SAA, allocator<float>, allocator<float>>);
    STATIC_ASSERT(!is_constructible_v<SAA, void*, allocator<int>>);

    STATIC_ASSERT(is_constructible_v<SA, const scoped_allocator_adaptor<allocator<float>>&>);
    STATIC_ASSERT(!is_constructible_v<SA, const SM&>);
    STATIC_ASSERT(is_constructible_v<SAA, const scoped_allocator_adaptor<allocator<float>, allocator<int>>&>);
    STATIC_ASSERT(!is_constructible_v<SAA, const SMA&>);

    STATIC_ASSERT(is_constructible_v<SA, scoped_allocator_adaptor<allocator<float>>>);
    STATIC_ASSERT(!is_constructible_v<SA, SM>);
    STATIC_ASSERT(is_constructible_v<SAA, scoped_allocator_adaptor<allocator<float>, allocator<int>>>);
    STATIC_ASSERT(!is_constructible_v<SAA, SMA>);
} // namespace test_LWG_2782

int main() {
    test_case_VSO_129349_non_default_constructible_allocators();
    test_case_VSO_184838_scoped_allocator_assignment_conversions();
    test_case_VSO_224478_allow_stacking();
    test_case_VSO_224478_piecewise_construct_calls_allocator_construct();
    test_case_LWG_2586();
    test_case_move_rebind_one_alloc();
}
