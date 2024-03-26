// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <coroutine>
#include <cstddef>
#include <cstdint>
#include <forward_list>
#include <generator>
#include <list>
#include <memory>
#include <memory_resource>
#include <new>
#include <ranges>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "range_algorithm_support.hpp"

using namespace std;

template <class T, class AlwaysEqual = true_type, signed_integral DifferenceType = ptrdiff_t>
class TestAllocator : public allocator<T> {
public:
    using value_type      = T;
    using is_always_equal = AlwaysEqual;
    using difference_type = DifferenceType;
    using size_type       = make_unsigned_t<difference_type>;

    TestAllocator() = default;

    template <class U>
    TestAllocator(const TestAllocator<U, AlwaysEqual, DifferenceType>&) {}

    T* allocate(const size_type s) {
        return static_cast<T*>(::operator new(static_cast<size_t>(s * sizeof(T)), align_val_t{alignof(T)}));
    }

    void deallocate(T* const p, size_type s) {
        ::operator delete(p, s * sizeof(T), align_val_t{alignof(T)});
    }

    operator pmr::polymorphic_allocator<void>() const {
        return {};
    }

    bool operator==(const TestAllocator&) const = default;
};

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

struct MoveOnly {
    MoveOnly(const MoveOnly&)            = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;
    MoveOnly(MoveOnly&&)                 = default;
    MoveOnly& operator=(MoveOnly&&)      = default;
};

static_assert(movable<MoveOnly>);
static_assert(!copyable<MoveOnly>);

struct Immovable {
    Immovable(Immovable&&)            = delete;
    Immovable& operator=(Immovable&&) = delete;
};

static_assert(!movable<Immovable>);

template <class T>
struct Proxy {
    Proxy(const T&); // not defined
};

template <class Gen, class Range>
    requires convertible_to<ranges::range_reference_t<Range&>, typename Gen::yielded>
void test_yield_elements_of_range(typename Gen::promise_type& p) {
    using Alloc = generator_allocator<Gen>::type;

    {
        using Awaitable = decltype(p.yield_value(ranges::elements_of{declval<Range&>()}));
        static_assert(convertible_to<decltype(declval<Awaitable&>().await_ready()), bool>);
    }

    if constexpr (!is_void_v<Alloc>) {
        using Awaitable = decltype(p.yield_value(ranges::elements_of{declval<Range&>(), declval<Alloc&>()}));
        static_assert(convertible_to<decltype(declval<Awaitable&>().await_ready()), bool>);
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
        assert(reinterpret_cast<intptr_t>(mem) % __STDCPP_DEFAULT_NEW_ALIGNMENT__ == 0);
        p.operator delete(mem, size);
    }

    // Test 'operator new(size_t, allocator_arg_t, const Alloc2&, const Args&...)'
    constexpr bool has_op_new2 = HasOperatorNew<Promise, size_t, allocator_arg_t, const Alloc2&, int, int>;
    static_assert(has_op_new2 == (same_as<Alloc, void> || convertible_to<const Alloc2&, Alloc>) );
    if constexpr (has_op_new2) {
        const size_t size = __STDCPP_DEFAULT_NEW_ALIGNMENT__;
        void* const mem   = p.operator new(size, allocator_arg, alloc2, 0, 0);
        assert(reinterpret_cast<intptr_t>(mem) % __STDCPP_DEFAULT_NEW_ALIGNMENT__ == 0);
        p.operator delete(mem, size);
    }

    // Test 'operator new(size_t, const This&, allocator_arg_t, const Alloc2&, const Args&...)'
    struct S {};
    constexpr bool has_op_new3 = HasOperatorNew<Promise, size_t, const S&, allocator_arg_t, const Alloc2&, int, int>;
    static_assert(has_op_new3 == (same_as<Alloc, void> || convertible_to<const Alloc2&, Alloc>) );
    if constexpr (has_op_new3) {
        const size_t size = __STDCPP_DEFAULT_NEW_ALIGNMENT__;
        const S s;
        void* const mem = p.operator new(size, s, allocator_arg, alloc2, 0, 0);
        assert(reinterpret_cast<intptr_t>(mem) % __STDCPP_DEFAULT_NEW_ALIGNMENT__ == 0);
        p.operator delete(mem, size);
    }
}

template <class Ref, class V = void, class Alloc = void>
void test_one() {
    using Gen     = generator<Ref, V, Alloc>;
    using Promise = Gen::promise_type;
    using Yielded = Gen::yielded;
    static_assert(semiregular<Promise>);

    Promise p;

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
                      == is_nothrow_constructible_v<remove_cvref_t<Yielded>,
                          const remove_reference_t<Yielded>&>); // strengthened
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
    if constexpr (convertible_to<ValTy&, Yielded>) { // Test 'yield_value(ranges::elements_of<range>)'
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
    test_operator_new<Gen, TestAllocator<void, false_type, int>>(p);
    if constexpr (same_as<Alloc, void>) {
        test_operator_new<Gen, TestAllocator<void>>(p);
        test_operator_new<Gen, TestAllocator<void, false_type>>(p);
        test_operator_new<Gen, TestAllocator<void, true_type, int>>(p);
    }
}

template <class Ref, class V = void>
void test_with_allocator() {
    test_one<Ref, V>();
    test_one<Ref, V, allocator<void>>();
    test_one<Ref, V, pmr::polymorphic_allocator<void>>();
    test_one<Ref, V, TestAllocator<void>>();
    test_one<Ref, V, TestAllocator<void, false_type>>();
    test_one<Ref, V, TestAllocator<void, true_type, int>>();
    test_one<Ref, V, TestAllocator<void, false_type, int>>();
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
    test_with_type<Immovable>();
    test_with_allocator<vector<bool>::reference, bool>();
}
