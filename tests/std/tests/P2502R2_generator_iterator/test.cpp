// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#if defined(__clang__) && defined(_M_ARM64EC) // TRANSITION, LLVM-158341
int main() {}
#else // ^^^ workaround / no workaround vvv

#include <cassert>
#include <concepts>
#include <cstddef>
#include <generator>
#include <memory>
#include <memory_resource>
#include <ranges>
#include <string>
#include <type_traits>
#include <utility>

#include "test_generator_support.hpp"

using namespace std;

template <class Ref, class V, class Alloc>
generator<Ref, V, Alloc> generate_zero() {
    co_return;
}

template <class Ref, class V, class Alloc, class ValueType = gen_value_t<Ref, V>>
    requires default_initializable<ValueType>
          && (same_as<remove_cvref_t<Ref>, ValueType> || constructible_from<remove_cvref_t<Ref>, ValueType&>)
generator<Ref, V, Alloc> generate_one() {
    if constexpr (same_as<remove_cvref_t<Ref>, ValueType>) {
        // non-proxy reference case
        if constexpr (is_reference_v<Ref>) {
            remove_reference_t<Ref> val{};
            co_yield static_cast<Ref>(val);
        } else {
            co_yield ValueType{};
        }
    } else {
        ValueType val{};
        // proxy reference case
        if constexpr (is_reference_v<Ref>) {
            // yielding a non-prvalue proxy reference is super weird, but not forbidden
            remove_reference_t<Ref> ref{val};
            co_yield static_cast<Ref>(ref);
        } else {
            co_yield Ref{val};
        }
    }
}

template <class Ref, class V, class Alloc>
generator<Ref, V, Alloc> generate_one_recursively() {
    co_yield ranges::elements_of{generate_zero<Ref, V, Alloc>()};
    co_yield ranges::elements_of{generate_one<Ref, V, Alloc>()};
    co_yield ranges::elements_of{generate_zero<Ref, V, Alloc>()};
}

template <class Ref, class V = void, class Alloc = void>
void test_one() {
    using Gen  = generator<Ref, V, Alloc>;
    using Iter = ranges::iterator_t<Gen>;
    static_assert(input_iterator<Iter>);
    static_assert(sizeof(Iter) == sizeof(void*)); // NB: implementation defined

    // Test member types
    static_assert(same_as<typename Iter::value_type, gen_value_t<Ref, V>>);
    static_assert(same_as<typename Iter::difference_type, ptrdiff_t>);

    // Test copying functions
    static_assert(!is_copy_constructible_v<Iter>);
    static_assert(!is_copy_assignable_v<Iter>);

    { // Test move constructor
        Gen g  = generate_zero<Ref, V, Alloc>();
        Iter i = g.begin();
        Iter j = move(i);
        assert(j == default_sentinel);

        static_assert(is_nothrow_move_constructible_v<Iter>);
    }

    { // Test move assignment operator
        Gen g1 = generate_one<Ref, V, Alloc>();
        Iter i = g1.begin();
        Gen g2 = generate_zero<Ref, V, Alloc>();
        Iter j = g2.begin();

        same_as<Iter&> decltype(auto) k = (i = move(j));
        assert(&k == &i);
        assert(k == default_sentinel);
        static_assert(is_nothrow_move_assignable_v<Iter>);
    }

    { // Test indirection
        auto g = generate_one<Ref, V, Alloc>();
        auto i = g.begin();

        same_as<gen_reference_t<Ref, V>> decltype(auto) r = *i;

        using ValueType = gen_value_t<Ref, V>;
        if constexpr (default_initializable<ValueType> && equality_comparable<ValueType>) {
            assert(r == ValueType{});
        }
    }

    { // Test pre-incrementation
        auto g = generate_one_recursively<Ref, V, Alloc>();
        auto i = g.begin();

        same_as<Iter&> decltype(auto) i_ref = ++i;
        assert(&i_ref == &i);
        assert(i_ref == default_sentinel);
    }

    { // Test post-incrementation
        auto g = generate_one_recursively<Ref, V, Alloc>();
        auto i = g.begin();
        i++;
        assert(i == default_sentinel);

        static_assert(is_void_v<decltype(i++)>);
    }

    { // Test equal operator
        auto g1 = generate_one<Ref, V, Alloc>();
        auto i  = g1.begin();
        auto g2 = generate_zero<Ref, V, Alloc>();
        auto j  = g2.begin();

        same_as<bool> decltype(auto) b1 = i == default_sentinel;
        assert(!b1);

        same_as<bool> decltype(auto) b2 = default_sentinel == j;
        assert(b2);

        same_as<bool> decltype(auto) b3 = i != default_sentinel;
        assert(b3);

        same_as<bool> decltype(auto) b4 = default_sentinel != j;
        assert(!b4);
    }
}

template <class Ref, class V = void>
void test_with_allocator() {
    test_one<Ref, V>();
    test_one<Ref, V, allocator<void>>();
    test_one<Ref, V, pmr::polymorphic_allocator<void>>();
    test_one<Ref, V, StatelessAlloc<void>>();
}

template <class T>
void test_with_type() {
    test_with_allocator<T>();
    test_with_allocator<T&>();
    test_with_allocator<const T&>();
    test_with_allocator<T&&>();
    test_with_allocator<const T&&>();

    test_with_allocator<Proxy<T>, T>();
    test_with_allocator<Proxy<T>&, T>();
    test_with_allocator<const Proxy<T>&, T>();
    test_with_allocator<Proxy<T>&&, T>();
    test_with_allocator<const Proxy<T>&&, T>();
}

int main() {
    test_with_type<int>();
    test_with_type<float>();
    test_with_type<string>();
    test_with_type<MoveOnly>();
}

#endif // ^^^ no workaround ^^^
