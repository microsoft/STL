// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <ranges>
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>

template <class T>
struct myalloc {
    using value_type = T;

    int state = 42;

    myalloc() = default;
    constexpr explicit myalloc(int i) noexcept : state{i} {}
    template <class U>
    constexpr myalloc(const myalloc<U>& that) noexcept : state{that.state} {}

    [[nodiscard]] constexpr T* allocate(std::size_t n) const {
        return std::allocator<T>{}.allocate(n);
    }

    constexpr void deallocate(T* const p, const std::size_t n) const noexcept {
        std::allocator<T>{}.deallocate(p, n);
    }

    template <class U>
    constexpr bool operator==(const myalloc<U>& that) const noexcept {
        return state == that.state;
    }
};

struct settish_instantiator {
    static constexpr int some_ints[]   = {0, 1, 1, 1, 2, 3, 4, 5, 5, 6, 7};
    static constexpr int unique_ints[] = {0, 1, 2, 3, 4, 5, 6, 7};

    enum class is_multi : bool { no, yes };

    template <template <class...> class C>
    struct deduce_container_impl; // not defined
    template <>
    struct deduce_container_impl<std::set> {
        template <class Key, class... Args>
        using apply = std::set<Key, std::less<Key>, Args...>;
    };
    template <>
    struct deduce_container_impl<std::multiset> {
        template <class Key, class... Args>
        using apply = std::multiset<Key, std::less<Key>, Args...>;
    };
    template <>
    struct deduce_container_impl<std::unordered_set> {
        template <class Key, class... Args>
        using apply = std::unordered_set<Key, std::hash<Key>, std::equal_to<Key>, Args...>;
    };
    template <>
    struct deduce_container_impl<std::unordered_multiset> {
        template <class Key, class... Args>
        using apply = std::unordered_multiset<Key, std::hash<Key>, std::equal_to<Key>, Args...>;
    };

    template <template <class...> class C, class Key, class... Args>
    using deduce_container = typename deduce_container_impl<C>::template apply<Key, Args...>;

    template <template <class...> class C>
    static void test_copy_move() {
        using Container = C<int>;
        const auto c    = ranges::to<Container>(some_ints);

        // validate the "direct" construction path with some copies and moves
        {
            std::same_as<Container> auto c0 = ranges::to<Container>(ranges::to<Container>(some_ints));
            assert(c0 == c);
        }
        {
            std::same_as<Container> auto c1 = ranges::to<Container>(c);
            assert(c1 == c);
        }
        {
            std::same_as<Container> auto c2 = ranges::to<C>(ranges::to<Container>(some_ints));
            assert(c2 == c);
        }
        {
            std::same_as<Container> auto c3 = ranges::to<C>(c);
            assert(c3 == c);
        }
        {
            std::same_as<Container> auto c4 = ranges::to<Container>(some_ints) | ranges::to<Container>();
            assert(c4 == c);
        }
        {
            std::same_as<Container> auto c5 = c | ranges::to<Container>();
            assert(c5 == c);
        }
        {
            std::same_as<Container> auto c6 = ranges::to<Container>(some_ints) | ranges::to<C>();
            assert(c6 == c);
        }
        {
            std::same_as<Container> auto c7 = c | ranges::to<C>();
            assert(c7 == c);
        }
    }

    template <ranges::input_range R, template <class...> class C, is_multi Multi>
    static void test_settish() {
        auto& expected = []() -> auto& {
            if constexpr (Multi == is_multi::yes) {
                return some_ints;
            } else {
                return unique_ints;
            }
        }();

        using Container = C<int>;
        {
            std::same_as<Container> auto c0 = ranges::to<Container>(R{some_ints});
            assert(ranges::is_permutation(c0, expected));
        }
        {
            std::same_as<Container> auto c1 = ranges::to<C>(R{some_ints});
            assert(ranges::is_permutation(c1, expected));
        }
        {
            std::same_as<Container> auto c2 = R{some_ints} | ranges::to<Container>();
            assert(ranges::is_permutation(c2, expected));
        }
        {
            std::same_as<Container> auto c3 = R{some_ints} | ranges::to<C>();
            assert(ranges::is_permutation(c3, expected));
        }

        using Alloc = myalloc<int>;
        using T     = deduce_container<C, int, Alloc>;

        {
            std::same_as<T> auto c4 = ranges::to<T>(R{some_ints}, Alloc{13});
            assert(c4.get_allocator().state == 13);
            assert(ranges::is_permutation(c4, expected));
        }
        {
            std::same_as<T> auto c5 = ranges::to<C>(R{some_ints}, Alloc{13});
            assert(c5.get_allocator().state == 13);
            assert(ranges::is_permutation(c5, expected));
        }
        {
            std::same_as<T> auto c6 = R{some_ints} | ranges::to<T>(Alloc{13});
            assert(c6.get_allocator().state == 13);
            assert(ranges::is_permutation(c6, expected));
        }
        {
            std::same_as<T> auto c7 = R{some_ints} | ranges::to<C>(Alloc{13});
            assert(c7.get_allocator().state == 13);
            assert(ranges::is_permutation(c7, expected));
        }
    }

    template <ranges::input_range R>
    static void call() {
        test_settish<R, std::set, is_multi::no>();
        test_settish<R, std::multiset, is_multi::yes>();
        test_settish<R, std::unordered_set, is_multi::no>();
        test_settish<R, std::unordered_multiset, is_multi::yes>();
    }
};

int main() {
    test_in<settish_instantiator, const int>();
    settish_instantiator::test_copy_move<std::set>();
    settish_instantiator::test_copy_move<std::multiset>();
    settish_instantiator::test_copy_move<std::unordered_set>();
    settish_instantiator::test_copy_move<std::unordered_multiset>();
}
