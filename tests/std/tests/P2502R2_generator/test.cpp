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
#include <ostream>
#include <random>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <test_generator_support.hpp>

using namespace std;

template <class G, class V, class R, class RR>
consteval bool static_checks() {
    static_assert(ranges::input_range<G>);
    static_assert(ranges::view<G>);
    static_assert(!ranges::forward_range<G>);
    static_assert(!ranges::borrowed_range<G>);
    static_assert(!ranges::common_range<G>);

    static_assert(same_as<ranges::range_value_t<G>, V>);
    static_assert(same_as<ranges::range_difference_t<G>, ptrdiff_t>);
    static_assert(same_as<ranges::range_reference_t<G>, R>);
    static_assert(same_as<ranges::range_rvalue_reference_t<G>, RR>);

    // Non-portable size checks
    static_assert(sizeof(G) == sizeof(void*));
    static_assert(sizeof(typename G::promise_type) == 2 * sizeof(void*));

    return true;
}

static_assert(static_checks<generator<int>, int, int&&, int&&>());
static_assert(static_checks<generator<const int&>, int, const int&, const int&&>());
static_assert(static_checks<generator<int&&>, int, int&&, int&&>());
static_assert(static_checks<generator<int&>, int, int&, int&&>());
static_assert(static_checks<generator<int, int>, int, int, int>());

// [coroutine.generator.overview] Example 1:
generator<int> ints(int start = 0) {
    while (true) {
        co_yield start++;
    }
}

void f(ostream& os) {
    for (auto i : ints() | views::take(3)) {
        os << i << ' '; // prints '0 1 2 '
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

template <class T>
struct stateful_alloc {
    using value_type = T;

    int domain;

    explicit stateful_alloc(int dom) noexcept : domain{dom} {}

    template <class U>
    constexpr stateful_alloc(const stateful_alloc<U>& that) noexcept : domain{that.domain} {}

    T* allocate(const size_t n) {
        void* vp;
        if constexpr (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
            vp = ::_aligned_malloc(n * sizeof(T), alignof(T));
        } else {
            vp = malloc(n * sizeof(T));
        }

        if (vp) {
            return static_cast<T*>(vp);
        }

        throw bad_alloc{};
    }

    void deallocate(void* const vp, [[maybe_unused]] const size_t n) noexcept {
        if constexpr (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
            ::_aligned_free(vp);
        } else {
            free(vp);
        }
    }

    template <class U>
    constexpr bool operator==(const stateful_alloc<U>& that) noexcept {
        return this->domain == that.domain;
    }
};
static_assert(!default_initializable<stateful_alloc<int>>);

void static_allocator_test() {
    {
        auto g = [](const int hi) -> generator<int, int, StatelessAlloc<char>> {
            constexpr size_t n = 64;
            int some_ints[n];
            for (int i = 0; i < hi; ++i) {
                co_yield some_ints[i % n] = i;
            }
        };

        assert(ranges::equal(g(1024), views::iota(0, 1024)));
    }

    {
        auto g = [](allocator_arg_t, StatelessAlloc<int>, const int hi) -> generator<int, int, StatelessAlloc<char>> {
            constexpr size_t n = 64;
            int some_ints[n];
            for (int i = 0; i < hi; ++i) {
                co_yield some_ints[i % n] = i;
            }
        };

        assert(ranges::equal(g(allocator_arg, {}, 1024), views::iota(0, 1024)));
    }

#ifndef __EDG__ // TRANSITION, VSO-1951821
    {
        auto g = [](allocator_arg_t, stateful_alloc<int>, const int hi) -> generator<int, int, stateful_alloc<char>> {
            constexpr size_t n = 64;
            int some_ints[n];
            for (int i = 0; i < hi; ++i) {
                co_yield some_ints[i % n] = i;
            }
        };

        assert(ranges::equal(g(allocator_arg, stateful_alloc<int>{42}, 1024), views::iota(0, 1024)));
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

void dynamic_allocator_test() {
    auto g = [](allocator_arg_t, const auto&, const int hi) -> generator<int> {
        constexpr size_t n = 64;
        int some_ints[n];
        for (int i = 0; i < hi; ++i) {
            co_yield some_ints[i % n] = i;
        }
    };

    assert(ranges::equal(g(allocator_arg, allocator<float>{}, 1024), views::iota(0, 1024)));
    assert(ranges::equal(g(allocator_arg, StatelessAlloc<float>{}, 1024), views::iota(0, 1024)));
#ifndef __EDG__ // TRANSITION, VSO-1951821
    assert(ranges::equal(g(allocator_arg, stateful_alloc<float>{1729}, 1024), views::iota(0, 1024)));
#endif // ^^^ no workaround ^^^
    pmr::synchronized_pool_resource pool;
    assert(ranges::equal(g(allocator_arg, pmr::polymorphic_allocator<int>{&pool}, 1024), views::iota(0, 1024)));
}

void zip_example() {
    int length = 0;
    for (auto x : co_zip(array{1, 2, 3}, vector{10, 20, 30, 40, 50})) {
        static_assert(same_as<decltype(x), tuple<int&, int&>>);
        assert(get<0>(x) * 10 == get<1>(x));
        ++length;
    }
    assert(length == 3);
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

    assert(ranges::equal(iota_repeater(3, 2), array{0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2}));
    assert(ranges::equal(nested_ints(), array{0, 1}));
}

void arbitrary_range_test() {
    auto yield_arbitrary_ranges = []() -> generator<const int&> {
        co_yield ranges::elements_of(vector<int>{40, 30, 20, 10});
        co_yield ranges::elements_of(views::iota(0, 4));
        forward_list<int> fl{500, 400, 300};
        co_yield ranges::elements_of(fl);
    };

    assert(ranges::equal(yield_arbitrary_ranges(), array{40, 30, 20, 10, 0, 1, 2, 3, 500, 400, 300}));
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

int main() {
    {
        stringstream ss;
        f(ss);
        assert(ss.str() == "0 1 2 ");
    }
    assert(ranges::equal(co_upto(6), views::iota(0, 6)));

    { // Test with mutable lvalue reference type
        auto r   = co_upto<int&>(32);
        auto pos = r.begin();
        for (int i = 0; i < 16; ++i, ++*pos, ++pos) {
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
            }

            // Test yielding lvalue
            vec.resize(size);
            ranges::generate(vec, [&] { return dist(rd); });
            const auto tmp = vec;
            co_yield vec;
            assert(tmp == vec);
        };

        constexpr size_t size = 16;
        auto r                = woof(size, 4);
        for (auto i = r.begin(); i != r.end(); ++i) {
            vector<int> vec = *i;
            assert(vec.size() == size);
            assert((*i).empty());
        }
    }
#endif // ^^^ no workaround ^^^

    static_allocator_test();
    dynamic_allocator_test();

    zip_example();
#if !(defined(__clang__) && defined(_M_IX86)) // TRANSITION, LLVM-56507
    recursive_test();
    arbitrary_range_test();

#ifndef _M_CEE // TRANSITION, VSO-1659496
    adl_proof_test();
#endif // ^^^ no workaround ^^^
#endif // ^^^ no workaround ^^^
}
