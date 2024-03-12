// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <forward_list>
#include <generator>
#include <random>
#include <ranges>
#include <sstream>
#include <type_traits>
#include <utility>
#include <vector>

namespace ranges = std::ranges;

template <class G, class V, class R, class RR>
constexpr bool static_checks() {
    static_assert(ranges::input_range<G>);
    static_assert(ranges::view<G>);
    static_assert(!ranges::forward_range<G>);
    static_assert(!ranges::borrowed_range<G>);
    static_assert(!ranges::common_range<G>);

    static_assert(std::same_as<ranges::range_value_t<G>, V>);
    static_assert(std::same_as<ranges::range_difference_t<G>, std::ptrdiff_t>);
    static_assert(std::same_as<ranges::range_reference_t<G>, R>);
    static_assert(std::same_as<ranges::range_rvalue_reference_t<G>, RR>);

    // Non-portable size checks
    static_assert(sizeof(G) == sizeof(void*));
    static_assert(sizeof(typename G::promise_type) == 3 * sizeof(void*));
    static_assert(sizeof(ranges::iterator_t<G>) == sizeof(void*));

    return true;
}

static_assert(static_checks<std::generator<int>, int, int&&, int&&>());
static_assert(static_checks<std::generator<const int&>, int, const int&, const int&&>());
static_assert(static_checks<std::generator<int&&>, int, int&&, int&&>());
static_assert(static_checks<std::generator<int&>, int, int&, int&&>());
static_assert(static_checks<std::generator<int, int>, int, int, int>());

// From the proposal:
std::generator<int> iota(int start = 0) {
    while (true) {
        co_yield start;
        ++start;
    }
}

void f(std::ostream& os) {
    os << '"';
    for (auto i : iota() | std::views::take(3)) {
        os << i << ' '; // prints "0 1 2 "
    }
    os << "\"\n";
}

template <ranges::input_range Rng1, ranges::input_range Rng2>
std::generator<std::tuple<ranges::range_reference_t<Rng1>, ranges::range_reference_t<Rng2>>,
    std::tuple<ranges::range_value_t<Rng1>, ranges::range_value_t<Rng2>>>
    zip(Rng1 r1, Rng2 r2) {
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
std::generator<Reference, int> meow(const int hi) {
    for (int i = 0; i < hi; ++i) {
        co_yield i;
    }
}

template <ranges::input_range R>
void dump(std::ostream& os, R&& r) {
    os << '{';
    bool first = true;
    for (auto&& e : r) {
        if (first) {
            first = false;
        } else {
            os << ", ";
        }
        os << e;
    }
    os << "}\n";
}

template <class T>
struct stateless_alloc {
    using value_type = T;

    stateless_alloc() = default;

    template <class U>
    constexpr stateless_alloc(const stateless_alloc<U>&) noexcept {}

    T* allocate(const std::size_t n) {
        void* vp;
        if constexpr (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
            vp = ::_aligned_malloc(n * sizeof(T), alignof(T));
        } else {
            vp = std::malloc(n * sizeof(T));
        }

        if (vp) {
            return static_cast<T*>(vp);
        }

        throw std::bad_alloc{};
    }

    void deallocate(void* const vp, [[maybe_unused]] const std::size_t n) noexcept {
        if constexpr (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
            ::_aligned_free(vp);
        } else {
            std::free(vp);
        }
    }

    template <class U>
    constexpr bool operator==(const stateless_alloc<U>&) noexcept {
        return true;
    }
};
static_assert(std::default_initializable<stateless_alloc<int>>);

template <class T>
struct stateful_alloc {
    using value_type = T;

    int domain;

    explicit stateful_alloc(int dom) noexcept : domain{dom} {}

    template <class U>
    constexpr stateful_alloc(const stateful_alloc<U>& that) noexcept : domain{that.domain} {}

    T* allocate(const std::size_t n) {
        void* vp;
        if constexpr (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
            vp = ::_aligned_malloc(n * sizeof(T), alignof(T));
        } else {
            vp = std::malloc(n * sizeof(T));
        }

        if (vp) {
            return static_cast<T*>(vp);
        }

        throw std::bad_alloc{};
    }

    void deallocate(void* const vp, [[maybe_unused]] const std::size_t n) noexcept {
        if constexpr (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
            ::_aligned_free(vp);
        } else {
            std::free(vp);
        }
    }

    template <class U>
    constexpr bool operator==(const stateful_alloc<U>& that) noexcept {
        return this->domain == that.domain;
    }
};
static_assert(!std::default_initializable<stateful_alloc<int>>);

void static_allocator_test() {
    {
        auto g = [](const int hi) -> std::generator<int, int, stateless_alloc<char>> {
            constexpr std::size_t n = 64;
            int some_ints[n];
            for (int i = 0; i < hi; ++i) {
                co_yield some_ints[i % n] = i;
            }
        };

        assert(ranges::equal(g(1024), ranges::views::iota(0, 1024)));
    }

    {
        auto g = [](std::allocator_arg_t, stateless_alloc<int>,
                     const int hi) -> std::generator<int, int, stateless_alloc<char>> {
            constexpr std::size_t n = 64;
            int some_ints[n];
            for (int i = 0; i < hi; ++i) {
                co_yield some_ints[i % n] = i;
            }
        };

        assert(ranges::equal(g(std::allocator_arg, {}, 1024), ranges::views::iota(0, 1024)));
    }

#ifndef __EDG__ // TRANSITION, VSO-1951821
    {
        auto g = [](std::allocator_arg_t, stateful_alloc<int>,
                     const int hi) -> std::generator<int, int, stateful_alloc<char>> {
            constexpr std::size_t n = 64;
            int some_ints[n];
            for (int i = 0; i < hi; ++i) {
                co_yield some_ints[i % n] = i;
            }
        };

        assert(ranges::equal(g(std::allocator_arg, stateful_alloc<int>{42}, 1024), ranges::views::iota(0, 1024)));
    }
#endif // ^^^ no workaround ^^^
}

void dynamic_allocator_test() {
    auto g = [](std::allocator_arg_t, const auto&, const int hi) -> std::generator<int> {
        constexpr std::size_t n = 64;
        int some_ints[n];
        for (int i = 0; i < hi; ++i) {
            co_yield some_ints[i % n] = i;
        }
    };

    assert(ranges::equal(g(std::allocator_arg, std::allocator<float>{}, 1024), ranges::views::iota(0, 1024)));
    assert(ranges::equal(g(std::allocator_arg, stateless_alloc<float>{}, 1024), ranges::views::iota(0, 1024)));
#ifndef __EDG__ // TRANSITION, VSO-1951821
    assert(ranges::equal(g(std::allocator_arg, stateful_alloc<float>{1729}, 1024), ranges::views::iota(0, 1024)));
#endif // ^^^ no workaround ^^^
}

void zip_example() {
    int length = 0;
    for (auto x : zip(std::array{1, 2, 3}, std::vector{10, 20, 30, 40, 50})) {
        static_assert(std::same_as<decltype(x), std::tuple<int&, int&>>);
        assert(std::get<0>(x) * 10 == std::get<1>(x));
        ++length;
    }
    assert(length == 3);
}

#if !(defined(__clang__) && defined(_M_IX86)) // TRANSITION, LLVM-56507
std::generator<int> iota_repeater(const int hi, const int depth) {
    if (depth > 0) {
        co_yield ranges::elements_of(iota_repeater(hi, depth - 1));
        co_yield ranges::elements_of(iota_repeater(hi, depth - 1));
    } else {
        co_yield ranges::elements_of(meow<int>(hi));
    }
}

void recursive_test() {
    struct some_error {};

    static constexpr auto might_throw = []() -> std::generator<int> {
        co_yield 0;
        throw some_error{};
    };

    static constexpr auto nested_ints = []() -> std::generator<int> {
        try {
            co_yield ranges::elements_of(might_throw());
        } catch (const some_error&) {
        }
        co_yield 1;
    };

    assert(ranges::equal(iota_repeater(3, 2), std::array{0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2}));
    assert(ranges::equal(nested_ints(), std::array{0, 1}));
}

void arbitrary_range_test() {
    auto yield_arbitrary_ranges = []() -> std::generator<const int&> {
        co_yield ranges::elements_of(std::vector<int>{40, 30, 20, 10});
        co_yield ranges::elements_of(ranges::views::iota(0, 4));
        std::forward_list<int> fl{500, 400, 300};
        co_yield ranges::elements_of(fl);
    };

    assert(ranges::equal(yield_arbitrary_ranges(), std::array{40, 30, 20, 10, 0, 1, 2, 3, 500, 400, 300}));
}

#ifndef _M_CEE // TRANSITION, VSO-1659496
template <class T>
struct holder {
    T t;
};

struct incomplete;

void adl_proof_test() {
    using validator  = holder<incomplete>*;
    auto yield_range = []() -> std::generator<validator> {
        co_yield ranges::elements_of(
            ranges::views::repeat(nullptr, 42) | ranges::views::transform([](std::nullptr_t) { return validator{}; }));
    };

    using R = decltype(yield_range());
    static_assert(ranges::input_range<R>);

    using It = ranges::iterator_t<R>;
    static_assert(std::is_same_v<decltype(&std::declval<It&>()), It*>);

    using Promise = R::promise_type;
    static_assert(std::is_same_v<decltype(&std::declval<Promise&>()), Promise*>);

    std::size_t i = 0;
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
        std::stringstream ss;
        f(ss);
        assert(ss.str() == "\"0 1 2 \"\n");
    }
    assert(ranges::equal(meow(6), ranges::views::iota(0, 6)));

    {
        // test with mutable lvalue reference type
        auto r   = meow<int&>(32);
        auto pos = r.begin();
        for (int i = 0; i < 16; ++i, ++*pos, ++pos) {
            assert(pos != r.end());
            assert(*pos == 2 * i);
        }
        assert(pos == r.end());
    }

#if !(defined(__clang__) && defined(_M_IX86)) // TRANSITION, LLVM-56507
    {
        // test with mutable xvalue reference type
        auto woof = [](std::size_t size, std::size_t count) -> std::generator<std::vector<int>&&> {
            std::random_device rd{};
            std::uniform_int_distribution dist{0, 99};
            std::vector<int> vec;
            while (count-- > 0) {
                vec.resize(size);
                ranges::generate(vec, [&] { return dist(rd); });
                co_yield std::move(vec);
            }
            // test yielding lvalue
            vec.resize(size);
            ranges::generate(vec, [&] { return dist(rd); });
            const auto tmp = vec;
            co_yield vec;
            assert(tmp == vec);
        };

        constexpr size_t size = 16;
        auto r                = woof(size, 4);
        for (auto i = r.begin(); i != r.end(); ++i) {
            std::vector<int> vec = *i;
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
