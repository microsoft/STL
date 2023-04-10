// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <deque>
#include <forward_list>
#include <list>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

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

        static_assert(test_sequence<R, std::basic_string>());
        static_assert(test_sequence<R, std::vector>());
    }
};

int main() {
    test_in<sequence_instantiator, const char>();

    sequence_instantiator::test_copy_move<std::basic_string>();
    sequence_instantiator::test_copy_move<std::deque>();
    sequence_instantiator::test_copy_move<std::forward_list>();
    sequence_instantiator::test_copy_move<std::list>();
    sequence_instantiator::test_copy_move<std::vector>();
    static_assert(sequence_instantiator::test_copy_move<std::basic_string>());
    static_assert(sequence_instantiator::test_copy_move<std::vector>());
}
