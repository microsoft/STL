// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <forward_list>
#include <generator>
#include <memory>
#include <memory_resource>
#include <new>
#include <random>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "test_generator_support.hpp"

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

// Some simple end-to-end tests, mostly from the Working Draft or P2502R2
generator<int> ints(int start = 0) {
    while (true) {
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

// Not from the proposal:
template <class Reference = const int&>
generator<Reference, int> co_upto(const int hi) {
    for (int i = 0; i < hi; ++i) {
        co_yield i;
    }
}

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

        test_one<gen_traits<int, int, StatelessAlloc<char>>, int, int, int>(
            g(1024), std::identity{}, views::iota(0, 1024));
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
            g(allocator_arg, {}, 1024), std::identity{}, views::iota(0, 1024));
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
            g(allocator_arg, StatefulAlloc<int>{42}, 1024), std::identity{}, views::iota(0, 1024));
    }
#endif // ^^^ no workaround ^^^
    {
        auto g = [](allocator_arg_t, pmr::polymorphic_allocator<int>, const int hi) -> pmr::generator<int, int> {
            constexpr size_t n = 64;
            int some_ints[n];
            for (int i = 0; i < hi; ++i) {
                co_yield some_ints[i % n] = i;
            }
        };

        static_assert(is_same_v<pmr::generator<int, int>, generator<int, int, pmr::polymorphic_allocator<>>>);
        assert(ranges::equal(g(allocator_arg, pmr::polymorphic_allocator<int>{}, 1024), views::iota(0, 1024)));
    }
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

    test_one<gen_traits<int>, int, int&&, int&&>(
        g(allocator_arg, allocator<float>{}, 1024), std::identity{}, views::iota(0, 1024));
    test_one<gen_traits<int>, int, int&&, int&&>(
        g(allocator_arg, StatelessAlloc<float>{}, 1024), std::identity{}, views::iota(0, 1024));
#ifndef __EDG__ // TRANSITION, VSO-1951821
    test_one<gen_traits<int>, int, int&&, int&&>(
        g(allocator_arg, StatefulAlloc<float>{1729}, 1024), std::identity{}, views::iota(0, 1024));
#endif // ^^^ no workaround ^^^
    pmr::synchronized_pool_resource pool;
    test_one<gen_traits<int>, int, int&&, int&&>(
        g(allocator_arg, pmr::polymorphic_allocator<>{&pool}, 1024), std::identity{}, views::iota(0, 1024));
}

void zip_example() {
    using V = tuple<int, int>;
    using R = tuple<int&, int&>;

    auto g0 = co_zip(array{1, 2, 3}, vector{10, 20, 30, 40, 50});
    test_one<gen_traits<R, V>, V, R, R>(move(g0), std::identity{}, array{tuple{1, 10}, tuple{2, 20}, tuple{3, 30}});

    g0 = co_zip(array{3, 2, 1}, vector{10, 20, 30, 40, 50});
    test_one<gen_traits<R, V>, V, R, R>(move(g0), std::identity{}, array{tuple{3, 10}, tuple{2, 20}, tuple{1, 30}});
}

#if !(defined(__clang__) && defined(_M_IX86)) // TRANSITION, LLVM-56507
generator<int> iota_repeater(const int hi, const int depth) {
    if (depth > 0) {
        co_yield ranges::elements_of(iota_repeater(hi, depth - 1));
        co_yield ranges::elements_of(iota_repeater(hi, depth - 1));
    } else {
        co_yield ranges::elements_of(co_upto<int>(hi));
    }
}

void recursive_test() {
    static constexpr auto might_throw = []() -> generator<int> {
        co_yield 0;
        throw runtime_error{"error"};
    };

    static constexpr auto nested_ints = []() -> generator<int> {
        try {
            co_yield ranges::elements_of(might_throw());
        } catch (const runtime_error& e) {
            assert(e.what() == "error"sv);
        }
        co_yield 1;
    };

    test_one<gen_traits<int>, int, int&&, int&&>(
        iota_repeater(3, 2), std::identity{}, array{0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2});
    test_one<gen_traits<int>, int, int&&, int&&>(nested_ints(), std::identity{}, array{0, 1});
}

void arbitrary_range_test() {
    auto yield_arbitrary_ranges = []() -> generator<const int&> {
        co_yield ranges::elements_of(vector<int>{40, 30, 20, 10});
        co_yield ranges::elements_of(views::iota(0, 4));
        forward_list<int> fl{500, 400, 300};
        co_yield ranges::elements_of(fl);
    };

    test_one<gen_traits<const int&>, int, const int&, const int&&>(
        yield_arbitrary_ranges(), std::identity{}, array{40, 30, 20, 10, 0, 1, 2, 3, 500, 400, 300});
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
    static_assert(is_same_v<decltype(&declval<It&>()), It*>);

    using Promise = R::promise_type;
    static_assert(is_same_v<decltype(&declval<Promise&>()), Promise*>);

    size_t i = 0;
    for (const auto elem : yield_range()) {
        ++i;
        assert(elem == nullptr);
    }
    assert(i == 42);
}
#endif // ^^^ no workaround ^^^
#endif // ^^^ no workaround ^^^

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

#if !(defined(__clang__) && defined(_M_IX86)) // TRANSITION, LLVM-56507
    { // Test with mutable xvalue reference type
        auto woof = [](size_t size, size_t count) -> generator<vector<int>&&> {
            random_device rd{};
            uniform_int_distribution dist{0, 99};
            vector<int> vec;
            while (count-- > 0) {
                vec.resize(size);
                ranges::generate(vec, [&] { return dist(rd); });
                co_yield move(vec);
                assert(vec.empty()); // when we yield an rvalue, the caller moves from it
            }

            // Test yielding lvalue
            vec.resize(size);
            ranges::generate(vec, [&] { return dist(rd); });
            const auto tmp = vec;
            co_yield vec;
            assert(tmp == vec); // when we yield an lvalue, the caller moves from a copy
        };

        constexpr size_t size = 16;
        auto r                = woof(size, 4);
        for (auto i = r.begin(); i != r.end(); ++i) {
            vector<int> vec = *i;
            assert(vec.size() == size);
        }
    }
#endif // ^^^ no workaround ^^^
}

int main() {
    // End-to-end tests
    test_one<gen_traits<int>, int, int&&, int&&>(ints(), views::take(3), array{0, 1, 2});
    assert(ranges::equal(co_upto(6), views::iota(0, 6)));
    static_allocator_test();
    dynamic_allocator_test();

    zip_example();
    test_weird_reference_types();
#if !(defined(__clang__) && defined(_M_IX86)) // TRANSITION, LLVM-56507
    recursive_test();
    arbitrary_range_test();

#ifndef _M_CEE // TRANSITION, VSO-1659496
    adl_proof_test();
#endif // ^^^ no workaround ^^^
#endif // ^^^ no workaround ^^^
}
