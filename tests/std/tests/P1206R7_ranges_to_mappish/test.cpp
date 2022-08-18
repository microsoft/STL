// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <map>
#include <ranges>
#include <string_view>
#include <unordered_map>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std::literals;

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

struct mappish_instantiator {
    static constexpr std::pair<int, std::string_view> some_pairs[]   = {{0, "0"}, {1, "1a"}, {1, "1b"}, {1, "1c"},
          {2, "2"}, {3, "3"}, {4, "4"}, {5, "5a"}, {5, "5b"}, {6, "6"}, {7, "7"}};
    static constexpr std::pair<int, std::string_view> unique_pairs[] = {
        {0, "0"}, {1, "1a"}, {2, "2"}, {3, "3"}, {4, "4"}, {5, "5a"}, {6, "6"}, {7, "7"}};

    enum class is_multi : bool { no, yes };

    static constexpr auto any_pair_eq = [](auto x, auto y) { return x.first == y.first && x.second == y.second; };

    template <template <class...> class C>
    struct deduce_container_impl; // not defined
    template <>
    struct deduce_container_impl<std::map> {
        template <class Key, class Val, class... Args>
        using apply = std::map<Key, Val, std::less<Key>, Args...>;
    };
    template <>
    struct deduce_container_impl<std::multimap> {
        template <class Key, class Val, class... Args>
        using apply = std::multimap<Key, Val, std::less<Key>, Args...>;
    };
    template <>
    struct deduce_container_impl<std::unordered_map> {
        template <class Key, class Val, class... Args>
        using apply = std::unordered_map<Key, Val, std::hash<Key>, std::equal_to<Key>, Args...>;
    };
    template <>
    struct deduce_container_impl<std::unordered_multimap> {
        template <class Key, class Val, class... Args>
        using apply = std::unordered_multimap<Key, Val, std::hash<Key>, std::equal_to<Key>, Args...>;
    };

    template <template <class...> class C, class Key, class Val, class... Args>
    using deduce_container = typename deduce_container_impl<C>::template apply<Key, Val, Args...>;

    template <template <class...> class C>
    static void test_copy_move() {
        using Container = C<int, std::string_view>;
        const auto c    = ranges::to<Container>(some_pairs);

        // validate the "direct" construction path with some copies and moves
        {
            std::same_as<Container> auto c0 = ranges::to<Container>(ranges::to<Container>(some_pairs));
            assert(c0 == c);
        }
        {
            std::same_as<Container> auto c1 = ranges::to<Container>(c);
            assert(c1 == c);
        }
        {
            std::same_as<Container> auto c2 = ranges::to<C>(ranges::to<Container>(some_pairs));
            assert(c2 == c);
        }
        {
            std::same_as<Container> auto c3 = ranges::to<C>(c);
            assert(c3 == c);
        }
        {
            std::same_as<Container> auto c4 = ranges::to<Container>(some_pairs) | ranges::to<Container>();
            assert(c4 == c);
        }
        {
            std::same_as<Container> auto c5 = c | ranges::to<Container>();
            assert(c5 == c);
        }
        {
            std::same_as<Container> auto c6 = ranges::to<Container>(some_pairs) | ranges::to<C>();
            assert(c6 == c);
        }
        {
            std::same_as<Container> auto c7 = c | ranges::to<C>();
            assert(c7 == c);
        }
    }

    template <ranges::input_range R, template <class...> class C, is_multi Multi>
    static void test_mappish() {
        auto& expected = []() -> auto& {
            if constexpr (Multi == is_multi::yes) {
                return some_pairs;
            } else {
                return unique_pairs;
            }
        }
        ();

        using Container = C<int, std::string_view>;
        {
            std::same_as<Container> auto c0 = ranges::to<Container>(R{some_pairs});
            assert(ranges::is_permutation(c0, expected, any_pair_eq));
        }
        {
            std::same_as<Container> auto c1 = ranges::to<C>(R{some_pairs});
            assert(ranges::is_permutation(c1, expected, any_pair_eq));
        }
        {
            std::same_as<Container> auto c2 = R{some_pairs} | ranges::to<Container>();
            assert(ranges::is_permutation(c2, expected, any_pair_eq));
        }
        {
            std::same_as<Container> auto c3 = R{some_pairs} | ranges::to<C>();
            assert(ranges::is_permutation(c3, expected, any_pair_eq));
        }

        using Alloc = myalloc<std::pair<const int, std::string_view>>;
        using T     = deduce_container<C, int, std::string_view, Alloc>;

        {
            std::same_as<T> auto c4 = ranges::to<T>(R{some_pairs}, Alloc{13});
            assert(c4.get_allocator().state == 13);
            assert(ranges::is_permutation(c4, expected, any_pair_eq));
        }
        {
            std::same_as<T> auto c5 = ranges::to<C>(R{some_pairs}, Alloc{13});
            assert(c5.get_allocator().state == 13);
            assert(ranges::is_permutation(c5, expected, any_pair_eq));
        }
        {
            std::same_as<T> auto c6 = R{some_pairs} | ranges::to<T>(Alloc{13});
            assert(c6.get_allocator().state == 13);
            assert(ranges::is_permutation(c6, expected, any_pair_eq));
        }
        {
            std::same_as<T> auto c7 = R{some_pairs} | ranges::to<C>(Alloc{13});
            assert(c7.get_allocator().state == 13);
            assert(ranges::is_permutation(c7, expected, any_pair_eq));
        }
    }

    template <ranges::input_range R>
    static void call() {
        if constexpr (std::same_as<std::remove_cvref_t<ranges::range_reference_t<R>>,
                          std::pair<int, std::string_view>>) {
            test_mappish<R, std::map, is_multi::no>();
            test_mappish<R, std::multimap, is_multi::yes>();
            test_mappish<R, std::unordered_map, is_multi::no>();
            test_mappish<R, std::unordered_multimap, is_multi::yes>();
        }
    }
};

int main() {
    test_in<mappish_instantiator, const std::pair<int, std::string_view>>();
    mappish_instantiator::test_copy_move<std::map>();
    mappish_instantiator::test_copy_move<std::multimap>();
    mappish_instantiator::test_copy_move<std::unordered_map>();
    mappish_instantiator::test_copy_move<std::unordered_multimap>();
}
