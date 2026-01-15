// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstddef>
#include <deque>
#include <flat_map>
#include <functional>
#include <memory>
#include <type_traits>
#include <vector>

using namespace std;

// Test MSVC STL-specific SCARY-ness

template <class T>
struct MyAllocator {
    using value_type = T;

    MyAllocator() = default;
    template <class U>
    MyAllocator(const MyAllocator<U>&) noexcept {}

    T* allocate(size_t n) {
        return allocator<T>{}.allocate(n);
    }

    void deallocate(T* p, size_t n) noexcept {
        allocator<T>{}.deallocate(p, n);
    }

    template <class U>
    friend constexpr bool operator==(const MyAllocator&, const MyAllocator<U>&) noexcept {
        return true;
    }
};

template <bool>
struct flat_map_unique_if_impl;
template <>
struct flat_map_unique_if_impl<true> {
    template <class Key, class Mapped, class Comp, class KeyCont, class MappedCont>
    using type = flat_map<Key, Mapped, Comp, KeyCont, MappedCont>;
};
template <>
struct flat_map_unique_if_impl<false> {
    template <class Key, class Mapped, class Comp, class KeyCont, class MappedCont>
    using type = flat_multimap<Key, Mapped, Comp, KeyCont, MappedCont>;
};

template <bool IsUnique, class Key, class Mapped, class Comp, class KeyCont, class MappedCont>
using flat_map_unique_if = flat_map_unique_if_impl<IsUnique>::template type<Key, Mapped, Comp, KeyCont, MappedCont>;

template <class A, class B>
constexpr bool has_different_nested_types = !is_same_v<typename A::value_compare, typename B::value_compare>
                                         && !is_same_v<typename A::containers, typename B::containers>;

template <bool IsUnique, class Comparator, class Alloc1, class Alloc2>
void test_scary_ness_one() {
    using Iter = flat_map<int, int>::iterator;
    using OtherIter =
        flat_map_unique_if<IsUnique, int, int, Comparator, vector<int, Alloc1>, vector<int, Alloc2>>::iterator;
    static_assert(is_same_v<Iter, OtherIter>);

    using ConstIter = flat_map<int, int>::const_iterator;
    using OtherConstIter =
        flat_map_unique_if<IsUnique, int, int, Comparator, vector<int, Alloc1>, vector<int, Alloc2>>::const_iterator;
    static_assert(is_same_v<ConstIter, OtherConstIter>);

    using Cont       = flat_map_unique_if<IsUnique, int, int, Comparator, vector<int, Alloc1>, vector<int, Alloc2>>;
    using OtherCont1 = flat_map_unique_if<!IsUnique, int, int, Comparator, vector<int, Alloc1>, vector<int, Alloc2>>;
    using OtherCont2 = flat_map_unique_if<IsUnique, int, int, Comparator, deque<int>, deque<int>>;
    using OtherCont3 = flat_map_unique_if<IsUnique, int, int, Comparator, vector<int, Alloc1>, deque<int, Alloc2>>;
    using OtherCont4 = flat_map_unique_if<IsUnique, int, int, Comparator, deque<int, Alloc1>, vector<int, Alloc2>>;
    static_assert(has_different_nested_types<Cont, OtherCont1>);
    static_assert(has_different_nested_types<Cont, OtherCont2>);
    static_assert(has_different_nested_types<Cont, OtherCont3>);
    static_assert(has_different_nested_types<Cont, OtherCont4>);
}

void test_scary_ness() {
    test_scary_ness_one<true, greater<int>, allocator<int>, allocator<int>>();
    test_scary_ness_one<true, greater<int>, allocator<int>, MyAllocator<int>>();
    test_scary_ness_one<true, greater<int>, MyAllocator<int>, allocator<int>>();

    test_scary_ness_one<true, less<>, allocator<int>, allocator<int>>();
    test_scary_ness_one<true, less<>, allocator<int>, MyAllocator<int>>();
    test_scary_ness_one<true, less<>, MyAllocator<int>, allocator<int>>();

    test_scary_ness_one<true, greater<>, allocator<int>, allocator<int>>();
    test_scary_ness_one<true, greater<>, allocator<int>, MyAllocator<int>>();
    test_scary_ness_one<true, greater<>, MyAllocator<int>, allocator<int>>();

    test_scary_ness_one<false, greater<int>, allocator<int>, allocator<int>>();
    test_scary_ness_one<false, greater<int>, allocator<int>, MyAllocator<int>>();
    test_scary_ness_one<false, greater<int>, MyAllocator<int>, allocator<int>>();

    test_scary_ness_one<false, less<>, allocator<int>, allocator<int>>();
    test_scary_ness_one<false, less<>, allocator<int>, MyAllocator<int>>();
    test_scary_ness_one<false, less<>, MyAllocator<int>, allocator<int>>();

    test_scary_ness_one<false, greater<>, allocator<int>, allocator<int>>();
    test_scary_ness_one<false, greater<>, allocator<int>, MyAllocator<int>>();
    test_scary_ness_one<false, greater<>, MyAllocator<int>, allocator<int>>();
}
