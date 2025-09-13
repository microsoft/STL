// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#if defined(__clang__) && defined(_M_ARM64EC) // TRANSITION, LLVM-158341
int main() {}
#else // ^^^ workaround / no workaround vvv

#include <cassert>
#include <coroutine>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <forward_list>
#include <generator>
#include <list>
#include <memory>
#include <memory_resource>
#include <new>
#include <ranges>
#include <span>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "range_algorithm_support.hpp"
#include "test_generator_support.hpp"

using namespace std;

#pragma warning(disable : 28251) // Inconsistent annotation for 'new[]': this instance has no annotations.

void* operator new[](size_t) {
    abort();
}

void operator delete[](void*) noexcept {
    abort();
}

template <class Promise, class... Args>
concept HasOperatorNew = requires(Args&&... args) {
    { Promise::operator new(forward<Args>(args)...) } -> same_as<void*>;
};

template <class Gen>
struct generator_allocator {};

template <class Ref, class V, class Alloc>
struct generator_allocator<generator<Ref, V, Alloc>> {
    using type = Alloc;
};

template <class Gen, class Range>
    requires convertible_to<ranges::range_reference_t<Range&>, typename Gen::yielded>
void test_yield_elements_of_range(typename Gen::promise_type& p) {
    using Alloc = generator_allocator<Gen>::type;

    {
        using Awaitable = decltype(p.yield_value(ranges::elements_of{declval<Range&>()}));
        static_assert(same_as<decltype(declval<Awaitable&>().await_ready()), bool>);
    }

    if constexpr (!is_void_v<Alloc>) {
        using Awaitable = decltype(p.yield_value(ranges::elements_of{declval<Range&>(), declval<Alloc&>()}));
        static_assert(same_as<decltype(declval<Awaitable&>().await_ready()), bool>);
    }
}

template <class Gen, class Alloc2>
void test_operator_new(typename Gen::promise_type& p, const Alloc2& alloc2 = {}) {
    using Promise = Gen::promise_type;
    using Alloc   = generator_allocator<Gen>::type;

    // Test 'operator new(size_t)'
    constexpr bool has_op_new1 = HasOperatorNew<Promise, size_t>;
    static_assert(has_op_new1 == (same_as<Alloc, void> || default_initializable<Alloc>) );
    if constexpr (has_op_new1) {
        const size_t size = __STDCPP_DEFAULT_NEW_ALIGNMENT__;
        void* const mem   = p.operator new(size);
        assert(reinterpret_cast<uintptr_t>(mem) % __STDCPP_DEFAULT_NEW_ALIGNMENT__ == 0);
        p.operator delete(mem, size);
    }

    // Test 'operator new(size_t, allocator_arg_t, const Alloc2&, const Args&...)'
    // This operator new is unconstrained.
    if constexpr (same_as<Alloc, void> || convertible_to<const Alloc2&, Alloc>) {
        const size_t size = __STDCPP_DEFAULT_NEW_ALIGNMENT__;
        void* const mem   = p.operator new(size, allocator_arg, alloc2, 0, 0);
        assert(reinterpret_cast<uintptr_t>(mem) % __STDCPP_DEFAULT_NEW_ALIGNMENT__ == 0);
        p.operator delete(mem, size);
    }

    // Test 'operator new(size_t, const This&, allocator_arg_t, const Alloc2&, const Args&...)'
    // This operator new is unconstrained.
    struct S {};
    if constexpr (same_as<Alloc, void> || convertible_to<const Alloc2&, Alloc>) {
        const size_t size = __STDCPP_DEFAULT_NEW_ALIGNMENT__;
        const S s;
        void* const mem = p.operator new(size, s, allocator_arg, alloc2, 0, 0);
        assert(reinterpret_cast<uintptr_t>(mem) % __STDCPP_DEFAULT_NEW_ALIGNMENT__ == 0);
        p.operator delete(mem, size);
    }
}

template <class Ref, class V, class Alloc, bool TestingIncomplete>
void test_one() {
    using Gen     = generator<Ref, V, Alloc>;
    using Promise = Gen::promise_type;
    using Yielded = Gen::yielded;
    static_assert(semiregular<Promise>);

    Promise p;

    // Test that operator& for promise_type resolves to the built-in version and doesn't involve ADL
    static_assert(same_as<decltype(&p), Promise*>);
    assert(&p == addressof(p));

    // Test 'get_return_object'
    static_assert(same_as<decltype(p.get_return_object()), Gen>);
    static_assert(noexcept(p.get_return_object()));

    // Test 'initial_suspend'
    static_assert(same_as<decltype(p.initial_suspend()), suspend_always>);
    static_assert(same_as<decltype(as_const(p).initial_suspend()), suspend_always>);
    static_assert(noexcept(p.initial_suspend()));
    static_assert(noexcept(as_const(p).initial_suspend()));

    // Test 'final_suspend'
    using FinalAwaitable = decltype(p.final_suspend());
    static_assert(convertible_to<decltype(declval<FinalAwaitable&>().await_ready()), bool>);
    static_assert(noexcept(p.final_suspend()));

    // Test 'yield_value(yielded)'
    static_assert(same_as<decltype(p.yield_value(declval<Yielded>())), suspend_always>);
    static_assert(noexcept(p.yield_value(declval<Yielded>())));

    // Test 'yield_value(const remove_reference_t<yielded>&)'
    if constexpr (is_rvalue_reference_v<Yielded>
                  && constructible_from<remove_cvref_t<Yielded>, const remove_reference_t<Yielded>&>) {
        using Lval      = const remove_reference_t<Yielded>&;
        using Awaitable = decltype(p.yield_value(declval<Lval>()));

        static_assert(convertible_to<decltype(declval<Awaitable&>().await_ready()), bool>);
        static_assert(is_void_v<decltype(declval<Awaitable&>().await_resume())>);
        static_assert(noexcept(p.yield_value(declval<Lval>()))
                      == is_nothrow_constructible_v<remove_cvref_t<Yielded>, Lval>); // strengthened
    }

    { // Test 'yield_value(elements_of<generator>)'
        {
            using Awaitable = decltype(p.yield_value(ranges::elements_of{declval<Gen&>()}));
            static_assert(convertible_to<decltype(declval<Awaitable&>().await_ready()), bool>);
        }

        if constexpr (!is_void_v<Alloc>) {
            using Awaitable = decltype(p.yield_value(ranges::elements_of{declval<Gen&>(), declval<Alloc&>()}));
            static_assert(convertible_to<decltype(declval<Awaitable&>().await_ready()), bool>);
        }
    }

    using ValTy = conditional_t<is_void_v<V>, remove_cvref_t<Ref>, V>;
    if constexpr (!TestingIncomplete && convertible_to<ValTy&, Yielded>) {
        // Test 'yield_value(ranges::elements_of<range>)'
        test_yield_elements_of_range<Gen, vector<ValTy>>(p);
        test_yield_elements_of_range<Gen, list<ValTy>>(p);
        test_yield_elements_of_range<Gen, forward_list<ValTy>>(p);
        test_yield_elements_of_range<Gen,
            test::range<input_iterator_tag, ValTy, test::Sized::no, test::CanDifference::no, test::Common::no,
                test::CanCompare::no, test::ProxyRef::no>>(p);
    }

    // Test 'await_transform'
    static_assert(!requires(Promise& p) { p.await_transform(); });

    // Test 'return_void'
    static_assert(is_void_v<decltype(p.return_void())>);
    static_assert(is_void_v<decltype(as_const(p).return_void())>);
    static_assert(noexcept(p.return_void()));
    static_assert(noexcept(as_const(p).return_void()));

    // Test 'unhandled_exception'
    static_assert(is_void_v<decltype(p.unhandled_exception())>);

    // Test 'operator new(size_t, ARGS...)'
    test_operator_new<Gen, allocator<void>>(p);
    test_operator_new<Gen, pmr::polymorphic_allocator<void>>(p);
    test_operator_new<Gen, StatelessAlloc<void, false_type, int>>(p);
    if constexpr (same_as<Alloc, void>) {
        test_operator_new<Gen, StatelessAlloc<void>>(p);
        test_operator_new<Gen, StatelessAlloc<void, false_type>>(p);
        test_operator_new<Gen, StatelessAlloc<void, true_type, int>>(p);
    }

    // Non-portable size check
    static_assert(sizeof(Promise) == 2 * sizeof(void*));
}

template <class Ref, class V, bool TestingIncomplete = false>
void test_with_allocator() {
    test_one<Ref, V, void, TestingIncomplete>();
    test_one<Ref, V, allocator<void>, TestingIncomplete>();
    test_one<Ref, V, pmr::polymorphic_allocator<void>, TestingIncomplete>();
    test_one<Ref, V, StatelessAlloc<void>, TestingIncomplete>();
    test_one<Ref, V, StatelessAlloc<void, false_type>, TestingIncomplete>();
    test_one<Ref, V, StatelessAlloc<void, true_type, int>, TestingIncomplete>();
    test_one<Ref, V, StatelessAlloc<void, false_type, int>, TestingIncomplete>();
}

template <class T, bool TestingIncomplete = false>
void test_with_type() {
    test_with_allocator<T, void, TestingIncomplete>();
    test_with_allocator<T&, void, TestingIncomplete>();
    test_with_allocator<const T&, void, TestingIncomplete>();
    test_with_allocator<T&&, void, TestingIncomplete>();
    test_with_allocator<const T&&, void, TestingIncomplete>();

    test_with_allocator<Proxy<T>, T, TestingIncomplete>();
    test_with_allocator<Proxy<T>&, T, TestingIncomplete>();
    test_with_allocator<const Proxy<T>&, T, TestingIncomplete>();
    test_with_allocator<Proxy<T>&&, T, TestingIncomplete>();
    test_with_allocator<const Proxy<T>&&, T, TestingIncomplete>();
}

#ifndef _M_CEE // TRANSITION, VSO-1659496
template <class T>
struct Holder {
    T t;
};

struct Incomplete;
#endif // ^^^ no workaround ^^^

// Also test LWG-4119:
// "generator::promise_type::yield_value(ranges::elements_of<R, Alloc>)'s nested generator may be ill-formed"
generator<span<int>> test_lwg_4119() { // COMPILE-ONLY
    vector<vector<int>> v;
    co_yield ranges::elements_of(v);
}

int main() {
    test_with_type<int>();
    test_with_type<float>();
    test_with_type<string>();
    test_with_type<MoveOnly>();
    test_with_type<Immovable>();
    test_with_allocator<vector<bool>::reference, bool>();
#ifndef _M_CEE // TRANSITION, VSO-1659496
    test_with_type<Holder<Incomplete>*, true>();
#endif // ^^^ no workaround ^^^
}

#endif // ^^^ no workaround ^^^
