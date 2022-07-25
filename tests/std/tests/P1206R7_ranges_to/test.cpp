// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <deque>
#include <format>
#include <forward_list>
#include <iostream>
#include <list>
#include <map>
#include <ranges>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>

using namespace std::literals;

namespace ranges = std::ranges;

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

struct sequence_instantiator {
    template <template <class...> class C>
    struct deduce_container_impl {
        template <class... Args>
        using apply = C<Args...>;
    };
    template <>
    struct deduce_container_impl<std::basic_string> {
        template <class T, class... Args>
        using apply = std::basic_string<T, std::char_traits<T>, Args...>;
    };

    template <template <class...> class C, class T, class... Args>
    using deduce_container = typename deduce_container_impl<C>::template apply<T, Args...>;

    static constexpr auto meow = "meow"sv;

    template <template <class...> class C>
    static constexpr bool test_copy_move() {
        const auto c = ranges::to<C<char>>(meow);

        // validate the "direct" construction path with some copies and moves
        {
            std::same_as<C<char>> auto c0 = ranges::to<C<char>>(ranges::to<C<char>>(meow));
            assert(c0 == c);
        }
        {
            std::same_as<C<char>> auto c1 = ranges::to<C<char>>(c);
            assert(c1 == c);
        }
        {
            std::same_as<C<char>> auto c2 = ranges::to<C>(ranges::to<C<char>>(meow));
            assert(c2 == c);
        }
        {
            std::same_as<C<char>> auto c3 = ranges::to<C>(c);
            assert(c3 == c);
        }
        {
            std::same_as<C<char>> auto c4 = ranges::to<C<char>>(meow) | ranges::to<C<char>>();
            assert(c4 == c);
        }
        {
            std::same_as<C<char>> auto c5 = c | ranges::to<C<char>>();
            assert(c5 == c);
        }
        {
            std::same_as<C<char>> auto c6 = ranges::to<C<char>>(meow) | ranges::to<C>();
            assert(c6 == c);
        }
        {
            std::same_as<C<char>> auto c7 = c | ranges::to<C>();
            assert(c7 == c);
        }

        return true;
    }

    template <ranges::input_range R, template <class...> class C>
    static constexpr bool test_sequence() {
        // validate from_range construction and deduction guides
        {
            std::same_as<C<char>> auto c0 = ranges::to<C<char>>(R{meow});
            assert(ranges::equal(c0, meow));
        }
        {
            std::same_as<C<char>> auto c1 = ranges::to<C>(R{meow});
            assert(ranges::equal(c1, meow));
        }
        {
            std::same_as<C<char>> auto c2 = R{meow} | ranges::to<C<char>>();
            assert(ranges::equal(c2, meow));
        }
        {
            std::same_as<C<char>> auto c3 = R{meow} | ranges::to<C>();
            assert(ranges::equal(c3, meow));
        }

        // validate from_range construction and deduction guides with an extra argument
        using Alloc = myalloc<char>;
        using T     = deduce_container<C, char, Alloc>;

        {
            std::same_as<T> auto c4 = ranges::to<T>(R{meow}, Alloc{13});
            assert(c4.get_allocator().state == 13);
            assert(ranges::equal(c4, meow));
        }
        {
            std::same_as<T> auto c5 = ranges::to<C>(R{meow}, Alloc{13});
            assert(c5.get_allocator().state == 13);
            assert(ranges::equal(c5, meow));
        }
        {
            std::same_as<T> auto c6 = R{meow} | ranges::to<T>(Alloc{13});
            assert(c6.get_allocator().state == 13);
            assert(ranges::equal(c6, meow));
        }
        {
            std::same_as<T> auto c7 = R{meow} | ranges::to<C>(Alloc{13});
            assert(c7.get_allocator().state == 13);
            assert(ranges::equal(c7, meow));
        }

        return true;
    }

    template <ranges::input_range R>
    static void call() {
        test_sequence<R, std::basic_string>();
        test_sequence<R, std::deque>();
        test_sequence<R, std::forward_list>();
        test_sequence<R, std::list>();
        test_sequence<R, std::vector>();

#ifdef __cpp_lib_constexpr_string
        static_assert(test_sequence<R, std::basic_string>());
#endif // __cpp_lib_constexpr_string
#ifdef __cpp_lib_constexpr_vector
        static_assert(test_sequence<R, std::vector>());
#endif // __cpp_lib_constexpr_vector
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
        }
        ();

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

struct secret_key_t {
    explicit secret_key_t() = default;
};
inline constexpr secret_key_t secret_key;

struct reservable {
    using value_type = int;

    std::size_t cap_      = 0;
    std::size_t reserved_ = 0;
    std::size_t size_     = 0;

    constexpr reservable(secret_key_t) {}

    constexpr std::size_t capacity() const {
        return cap_;
    }
    constexpr std::size_t max_size() const {
        return ~std::size_t{};
    }
    constexpr std::size_t size() const {
        return size_;
    }
    constexpr void reserve(const std::size_t n) {
        if (n > cap_) {
            assert(reserved_ == 0);
            reserved_ = n;
            cap_      = n;
        }
    }

    constexpr void push_back(int) {
        if (++size_ > cap_) {
            assert(false);
        }
    }

    int* begin(); // not defined
    int* end(); // not defined
};

constexpr bool test_reservable() {
    int some_ints[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    {
        std::same_as<reservable> auto r = some_ints | ranges::to<reservable>(secret_key);
        assert(r.size_ == ranges::size(some_ints));
        assert(r.cap_ == ranges::size(some_ints));
        assert(r.reserved_ == ranges::size(some_ints));
    }

    return true;
}

constexpr bool test_common_constructible() {
    struct common_constructible {
        using value_type = int;

        constexpr common_constructible(const int* const first, const int* const last, secret_key_t)
            : first_{first}, last_{last} {}

        const int* begin() const; // not defined
        const int* end() const; // not defined

        const int* first_;
        const int* last_;
    };

    int some_ints[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    {
        std::same_as<common_constructible> auto c0 = ranges::to<common_constructible>(some_ints, secret_key);
        assert(c0.first_ == ranges::begin(some_ints));
        assert(c0.last_ == ranges::end(some_ints));
    }
    {
        std::same_as<common_constructible> auto c1 = some_ints | ranges::to<common_constructible>(secret_key);
        assert(c1.first_ == ranges::begin(some_ints));
        assert(c1.last_ == ranges::end(some_ints));
    }

    return true;
}

constexpr bool test_nested_range() {
    int some_int_ervals[3][2][2] = {{{0, 1}, {2, 3}}, {{4, 5}, {6, 7}}, {{8, 9}, {10, 11}}};

    using C = std::vector<std::vector<std::vector<int>>>;

    {
        std::same_as<C> auto c0 = ranges::to<C>(some_int_ervals);
        assert(c0.size() == 3);
        assert(c0[0].size() == 2);
        assert(c0[0][0].size() == 2);
        assert(c0[0][0][0] == 0);
        assert(c0[0][0][1] == 1);
        assert(c0[0][1][0] == 2);
        assert(c0[0][1][1] == 3);
        assert(c0[1][0][0] == 4);
        assert(c0[1][0][1] == 5);
        assert(c0[1][1][0] == 6);
        assert(c0[1][1][1] == 7);
        assert(c0[2][0][0] == 8);
        assert(c0[2][0][1] == 9);
        assert(c0[2][1][0] == 10);
        assert(c0[2][1][1] == 11);
    }
    {
        std::same_as<C> auto c0 = some_int_ervals | ranges::to<C>();
        assert(c0.size() == 3);
        assert(c0[0].size() == 2);
        assert(c0[0][0].size() == 2);
        assert(c0[0][0][0] == 0);
        assert(c0[0][0][1] == 1);
        assert(c0[0][1][0] == 2);
        assert(c0[0][1][1] == 3);
        assert(c0[1][0][0] == 4);
        assert(c0[1][0][1] == 5);
        assert(c0[1][1][0] == 6);
        assert(c0[1][1][1] == 7);
        assert(c0[2][0][0] == 8);
        assert(c0[2][0][1] == 9);
        assert(c0[2][1][0] == 10);
        assert(c0[2][1][1] == 11);
    }

    return true;
}

int main() {
    test_in<sequence_instantiator, const char>();
    test_in<mappish_instantiator, const std::pair<int, std::string_view>>();
    test_in<settish_instantiator, const int>();

    sequence_instantiator::test_copy_move<std::basic_string>();
    sequence_instantiator::test_copy_move<std::deque>();
    sequence_instantiator::test_copy_move<std::forward_list>();
    sequence_instantiator::test_copy_move<std::list>();
    sequence_instantiator::test_copy_move<std::vector>();
#ifdef __cpp_lib_constexpr_string
    static_assert(sequence_instantiator::test_copy_move<std::basic_string>());
#endif // __cpp_lib_constexpr_string
#ifdef __cpp_lib_constexpr_vector
    static_assert(sequence_instantiator::test_copy_move<std::vector>());
#endif // __cpp_lib_constexpr_vector

    mappish_instantiator::test_copy_move<std::map>();
    mappish_instantiator::test_copy_move<std::multimap>();
    mappish_instantiator::test_copy_move<std::unordered_map>();
    mappish_instantiator::test_copy_move<std::unordered_multimap>();

    settish_instantiator::test_copy_move<std::set>();
    settish_instantiator::test_copy_move<std::multiset>();
    settish_instantiator::test_copy_move<std::unordered_set>();
    settish_instantiator::test_copy_move<std::unordered_multiset>();

    test_reservable();
    static_assert(test_reservable());

    test_common_constructible();
    static_assert(test_common_constructible());

    test_nested_range();
#if defined(__cpp_lib_constexpr_vector) && (defined(__clang__) || defined(__EDG__))
    static_assert(test_nested_range());
#endif // defined(__cpp_lib_constexpr_vector) && (defined(__clang__) || defined(__EDG__))
}
