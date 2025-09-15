// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#if defined(__clang__) && defined(_M_ARM64EC) // TRANSITION, LLVM-158341
int main() {}
#else // ^^^ workaround / no workaround vvv

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <forward_list>
#include <generator>
#include <memory>
#include <memory_resource>
#include <new>
#include <ranges>
#include <stdexcept>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

#include "test_generator_support.hpp"

#pragma warning(disable : 28251) // Inconsistent annotation for 'new': this instance has no annotations.

using namespace std;

template <class Traits, class ValueType, class ReferenceType, class RvalueReferenceType>
consteval bool static_checks() {
    using G = Traits::generator;
    static_assert(derived_from<G, ranges::view_interface<G>>);

    // Specializations of generator are move-only input-and-no-stronger views
    static_assert(ranges::view<G>);
    static_assert(ranges::input_range<G>);
    static_assert(!ranges::forward_range<G>);

    static_assert(!copy_constructible<G>);
    static_assert(!is_copy_assignable_v<G>);

    static_assert(is_nothrow_destructible_v<G>);
    static_assert(is_nothrow_move_constructible_v<G>);
    static_assert(is_nothrow_move_assignable_v<G>);

    // Verify the generator's associated types
    static_assert(same_as<ranges::range_value_t<G>, ValueType>);
    static_assert(same_as<ranges::range_difference_t<G>, ptrdiff_t>);
    static_assert(same_as<ranges::range_reference_t<G>, ReferenceType>);
    static_assert(same_as<ranges::range_rvalue_reference_t<G>, RvalueReferenceType>);
    static_assert(same_as<typename G::yielded, typename Traits::yielded>);

    // Verify end
    static_assert(same_as<default_sentinel_t, ranges::sentinel_t<G>>);
    static_assert(same_as<default_sentinel_t, decltype(declval<const G&>().end())>);
    static_assert(noexcept(declval<G&>().end()));
    static_assert(noexcept(declval<const G&>().end()));

    // iterator properties are verified in P2502R2_generator_iterator
    // promise properties are verified in P2502R2_generator_promise

    // Non-portable size check
    static_assert(sizeof(G) == sizeof(void*));

    return true;
}

static_assert(static_checks<gen_traits<int>, int, int&&, int&&>());
static_assert(static_checks<gen_traits<const int&>, int, const int&, const int&&>());
static_assert(static_checks<gen_traits<int&&>, int, int&&, int&&>());
static_assert(static_checks<gen_traits<int&>, int, int&, int&&>());

static_assert(static_checks<gen_traits<int, int>, int, int, int>());
static_assert(static_checks<gen_traits<const int&, int>, int, const int&, const int&&>());
static_assert(static_checks<gen_traits<int&&, int>, int, int&&, int&&>());
static_assert(static_checks<gen_traits<int&, int>, int, int&, int&&>());

template <class Traits, class ValueType, class ReferenceType, class RvalueReferenceType, class R, class V, class A>
void test_one(generator<R, V, A> g0, invocable<generator<R, V, A>> auto adaptor, ranges::input_range auto&& expected)
    requires ranges::input_range<decltype(adaptor(move(g0)))>
{
    static_assert(same_as<generator<R, V, A>, typename Traits::generator>);
    static_assert(static_checks<Traits, ValueType, ReferenceType, RvalueReferenceType>());

    auto g1 = move(g0);
    auto i  = ranges::cbegin(expected);
    for (auto&& x : adaptor(move(g1))) {
        assert(i != ranges::cend(expected));
        assert(*i == x);
        ++i;
        // Verify iterator stays valid after move assignment
        ranges::swap(g0, g1);
    }
    assert(i == ranges::cend(expected));
}

template <class Traits, class ValueType, class ReferenceType, class RvalueReferenceType, class R, class V, class A,
    ranges::input_range Ex>
void test_one(generator<R, V, A> g0, Ex&& expected) {
    return test_one<Traits, ValueType, ReferenceType, RvalueReferenceType>(move(g0), identity{}, forward<Ex>(expected));
}

// Some simple end-to-end tests, mostly from the Working Draft or P2502R2
generator<int> ints(int start = 0) {
    for (;;) {
        co_yield start++;
    }
}

template <ranges::input_range Rng1, ranges::input_range Rng2>
generator<tuple<ranges::range_reference_t<Rng1>, ranges::range_reference_t<Rng2>>,
    tuple<ranges::range_value_t<Rng1>, ranges::range_value_t<Rng2>>>
    co_zip(Rng1 r1, Rng2 r2) {
    auto it1        = ranges::begin(r1);
    auto it2        = ranges::begin(r2);
    const auto end1 = ranges::end(r1);
    const auto end2 = ranges::end(r2);
    for (; it1 != end1 && it2 != end2; ++it1, ++it2) {
        co_yield {*it1, *it2};
    }
}

void zip_example() {
    using V = tuple<int, int>;
    using R = tuple<int&, int&>;

    auto g0 = co_zip(array{1, 2, 3}, vector{10, 20, 30, 40, 50});
    test_one<gen_traits<R, V>, V, R, R>(move(g0), array{tuple{1, 10}, tuple{2, 20}, tuple{3, 30}});

    g0 = co_zip(array{3, 2, 1}, vector{10, 20, 30, 40, 50});
    test_one<gen_traits<R, V>, V, R, R>(move(g0), array{tuple{3, 10}, tuple{2, 20}, tuple{1, 30}});
}

template <class Reference = const int&>
generator<Reference, int> co_upto(const int hi) {
    assert(hi >= 0);
    for (int i = 0; i < hi; ++i) {
        co_yield i;
    }
}

void test_weird_reference_types() {
    constexpr int n = 32;
    { // Test mutable lvalue reference type
        auto r   = co_upto<int&>(n);
        auto pos = r.begin();
        for (int i = 0; i < n / 2; ++i, ++*pos, ++pos) {
            assert(pos != r.end());
            assert(*pos == 2 * i);
        }
        assert(pos == r.end());
    }

    { // Test with mutable rvalue reference type
        constexpr size_t segment_size = 16;
        auto woof                     = []() -> generator<vector<int>&&> {
            vector<int> vec(segment_size);

            co_yield vec; // When we yield an lvalue...
            assert(vec.size() == segment_size); // ... the caller moves from a copy.

            co_yield move(vec); // When we yield an rvalue...
            assert(vec.size() == 0); // ... the caller moves from it.
        };

        for (auto vec : woof()) { // Intentionally by value
            assert(vec.size() == segment_size);
        }
    }
}

generator<int> iota_repeater(const int hi, const int depth) {
    if (depth > 0) {
        co_yield ranges::elements_of(iota_repeater(hi, depth - 1));
        co_yield ranges::elements_of(iota_repeater(hi, depth - 1));
    } else {
        co_yield ranges::elements_of(co_upto<int>(hi));
    }
}

void recursive_test() {
    auto might_throw = []() -> generator<int> {
        co_yield 0;
        throw runtime_error{"error"};
    };

    auto nested_ints = [=]() -> generator<int> {
        try {
            co_yield ranges::elements_of(might_throw());
        } catch (const runtime_error& e) {
            assert(e.what() == "error"sv);
        }
        co_yield 1;
    };

    test_one<gen_traits<int>, int, int&&, int&&>(iota_repeater(3, 2), array{0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2});
    test_one<gen_traits<int>, int, int&&, int&&>(nested_ints(), array{0, 1});
}

void arbitrary_range_test() {
    auto yield_arbitrary_ranges = []() -> generator<const int&> {
        co_yield ranges::elements_of(vector<int>{40, 30, 20, 10});
        co_yield ranges::elements_of(views::iota(0, 4));
        forward_list<int> fl{500, 400, 300};
        co_yield ranges::elements_of(fl);
    };

    test_one<gen_traits<const int&>, int, const int&, const int&&>(
        yield_arbitrary_ranges(), array{40, 30, 20, 10, 0, 1, 2, 3, 500, 400, 300});
}

#ifndef _M_CEE // TRANSITION, VSO-1659496
template <class T>
struct holder {
    T t;
};

struct incomplete;

void adl_proof_test() {
    using validator  = holder<incomplete>*;
    auto yield_range = []() -> generator<validator> {
        co_yield ranges::elements_of(
            views::repeat(nullptr, 42) | views::transform([](nullptr_t) { return validator{}; }));
    };

    using R = decltype(yield_range());
    static_assert(ranges::input_range<R>);

    using It = ranges::iterator_t<R>;
    static_assert(same_as<decltype(&declval<It&>()), It*>);

    using Promise = R::promise_type;
    static_assert(same_as<decltype(&declval<Promise&>()), Promise*>);

    size_t i = 0;
    for (const auto elem : yield_range()) {
        ++i;
        assert(elem == nullptr);
    }
    assert(i == 42);
}
#endif // ^^^ no workaround ^^^

// Verify behavior with unerased allocator types
void static_allocator_test() {
    {
        auto g = [](const int hi) -> generator<int, int, StatelessAlloc<char>> {
            constexpr size_t n = 64;
            int some_ints[n];
            for (int i = 0; i < hi; ++i) {
                co_yield some_ints[i % n] = i;
            }
        };

        test_one<gen_traits<int, int, StatelessAlloc<char>>, int, int, int>(g(1024), views::iota(0, 1024));
    }

    {
        auto g = [](allocator_arg_t, StatelessAlloc<int>, const int hi) -> generator<int, int, StatelessAlloc<char>> {
            constexpr size_t n = 64;
            int some_ints[n];
            for (int i = 0; i < hi; ++i) {
                co_yield some_ints[i % n] = i;
            }
        };

        test_one<gen_traits<int, int, StatelessAlloc<char>>, int, int, int>(
            g(allocator_arg, {}, 1024), views::iota(0, 1024));
    }

#ifndef __EDG__ // TRANSITION, VSO-1951821
    {
        auto g = [](allocator_arg_t, StatefulAlloc<int>, const int hi) -> generator<int, int, StatefulAlloc<char>> {
            constexpr size_t n = 64;
            int some_ints[n];
            for (int i = 0; i < hi; ++i) {
                co_yield some_ints[i % n] = i;
            }
        };

        test_one<gen_traits<int, int, StatefulAlloc<char>>, int, int, int>(
            g(allocator_arg, StatefulAlloc<int>{42}, 1024), views::iota(0, 1024));
    }
#endif // ^^^ no workaround ^^^
}

// Verify behavior with erased allocator types
void dynamic_allocator_test() {
    auto g = [](allocator_arg_t, const auto&, const int hi) -> generator<int> {
        constexpr size_t n = 64;
        int some_ints[n];
        for (int i = 0; i < hi; ++i) {
            co_yield some_ints[i % n] = i;
        }
    };

    test_one<gen_traits<int>, int, int&&, int&&>(g(allocator_arg, allocator<float>{}, 1024), views::iota(0, 1024));
    test_one<gen_traits<int>, int, int&&, int&&>(g(allocator_arg, StatelessAlloc<float>{}, 1024), views::iota(0, 1024));
#ifndef __EDG__ // TRANSITION, VSO-1951821
    test_one<gen_traits<int>, int, int&&, int&&>(
        g(allocator_arg, StatefulAlloc<float>{1729}, 1024), views::iota(0, 1024));
#endif // ^^^ no workaround ^^^
    pmr::synchronized_pool_resource pool;
    test_one<gen_traits<int>, int, int&&, int&&>(
        g(allocator_arg, pmr::polymorphic_allocator<>{&pool}, 1024), views::iota(0, 1024));
}

static atomic<bool> allow_allocation{true};

void* operator new(const size_t n) {
    if (allow_allocation) {
        if (void* const result = malloc(n)) {
            return result;
        }
    }
    throw bad_alloc{};
}

void operator delete(void* const p) noexcept {
    free(p);
}

void operator delete(void* const p, size_t) noexcept {
    free(p);
}

void* operator new(const size_t n, const align_val_t al) {
    if (allow_allocation) {
        if (void* const result = ::_aligned_malloc(n, static_cast<size_t>(al))) {
            return result;
        }
    }
    throw bad_alloc{};
}

void operator delete(void* const p, align_val_t) noexcept {
    ::_aligned_free(p);
}

void operator delete(void* const p, size_t, align_val_t) noexcept {
    ::_aligned_free(p);
}

class malloc_resource final : public pmr::memory_resource {
private:
    void* do_allocate(size_t bytes, size_t align) override {
        assert(align <= __STDCPP_DEFAULT_NEW_ALIGNMENT__);
        if (bytes == 0) {
            bytes = 1;
        }

        if (void* const result = malloc(bytes)) {
            return result;
        }
        throw bad_alloc{};
    }

    void do_deallocate(void* ptr, size_t, size_t align) noexcept override {
        assert(align <= __STDCPP_DEFAULT_NEW_ALIGNMENT__);
        free(ptr);
    }

    bool do_is_equal(const memory_resource& that) const noexcept override {
        return typeid(malloc_resource) == typeid(that);
    }
};

void pmr_generator_test() {
    // Verify alias template
    static_assert(same_as<pmr::generator<int>, generator<int, void, pmr::polymorphic_allocator<>>>);
    static_assert(same_as<pmr::generator<int, int>, generator<int, int, pmr::polymorphic_allocator<>>>);
    static_assert(same_as<pmr::generator<const int&, int>, generator<const int&, int, pmr::polymorphic_allocator<>>>);

    // Simple end-to-end test
    malloc_resource mr{};
    auto g = [&mr](allocator_arg_t, pmr::polymorphic_allocator<> alloc, const int hi) -> pmr::generator<int, int> {
        assert(alloc.resource() == &mr);

        constexpr size_t n = 64;
        int some_ints[n];
        for (int i = 0; i < hi; ++i) {
            co_yield some_ints[i % n] = i;
        }
    };

    allow_allocation = false;
    test_one<gen_traits<int, int, pmr::polymorphic_allocator<>>, int, int, int>(
        g(allocator_arg, pmr::polymorphic_allocator<>{&mr}, 1024), views::iota(0, 1024));
    allow_allocation = true;
}

int main() {
    // End-to-end tests
    test_one<gen_traits<int>, int, int&&, int&&>(ints(), views::take(3), array{0, 1, 2});
    assert(ranges::equal(co_upto(6), views::iota(0, 6)));
    zip_example();
    test_weird_reference_types();
    recursive_test();
    arbitrary_range_test();

#ifndef _M_CEE // TRANSITION, VSO-1659496
    // Verify generation of a range of pointers-to-incomplete
    adl_proof_test();
#endif // ^^^ no workaround ^^^

    // Allocator tests
    static_allocator_test();
    dynamic_allocator_test();
    pmr_generator_test();
}

#endif // ^^^ no workaround ^^^
