// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <concepts>
#include <cstddef>
#include <functional>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

namespace ranges = std::ranges;

template <class>
inline constexpr bool always_false = false;

namespace detail {
    static constexpr bool permissive() {
        return false;
    }

    template <class>
    struct DependentBase {
        static constexpr bool permissive() {
            return true;
        }
    };

    template <class T>
    struct Derived : DependentBase<T> {
        static constexpr bool test() {
            return permissive();
        }
    };
} // namespace detail
constexpr bool is_permissive = detail::Derived<int>::test();

template <bool>
struct borrowed { // borrowed<true> is a borrowed_range; borrowed<false> is not
    int* begin() const;
    int* end() const;
};

template <>
inline constexpr bool std::ranges::enable_borrowed_range<borrowed<true>> = true;

#ifndef __clang__ // TRANSITION, LLVM-45213
inline
#endif
    constexpr auto get_first = [](auto&& x) -> auto&& {
    return static_cast<decltype(x)>(x).first;
};

#ifndef __clang__ // TRANSITION, LLVM-45213
inline
#endif
    constexpr auto get_second = [](auto&& x) -> auto&& {
    return static_cast<decltype(x)>(x).second;
};

template <class T>
class move_only_range : public ranges::view_base {
    // Adapts a contiguous range into a move-only view with move-only iterators
private:
    using U = std::span<T>;
    U elements;
    mutable bool begin_called = false;

    class iterator;
    class sentinel;

public:
    constexpr explicit move_only_range(U x) : elements{x} {}

    constexpr move_only_range(move_only_range&& that)
        : elements{std::exchange(that.elements, {})}, begin_called{that.begin_called} {}

    constexpr move_only_range& operator=(move_only_range&& that) {
        elements     = std::exchange(that.elements, {});
        begin_called = that.begin_called;
        return *this;
    }

    constexpr iterator begin() const {
        assert(!std::exchange(begin_called, true));
        return iterator{elements.begin()};
    }

    constexpr sentinel end() const {
        return sentinel{elements.end()};
    }
};

template <ranges::contiguous_range R>
move_only_range(R&) -> move_only_range<std::remove_reference_t<ranges::range_reference_t<R>>>;

template <class T>
class move_only_range<T>::iterator {
private:
    friend sentinel;
    ranges::iterator_t<std::span<T>> pos;

public:
    using iterator_concept  = std::input_iterator_tag;
    using iterator_category = std::output_iterator_tag;
    using value_type        = std::remove_cv_t<T>;
    using difference_type   = std::ptrdiff_t;
    using pointer           = void;
    using reference         = T&;

    iterator() = default;
    constexpr explicit iterator(ranges::iterator_t<U> p) : pos{p} {}
    constexpr iterator(iterator&& that) : pos{std::exchange(that.pos, {})} {}

    constexpr iterator& operator=(iterator&& that) {
        pos = std::exchange(that.pos, {});
        return *this;
    }

    constexpr ranges::iterator_t<U> base() const {
        return pos;
    }

    constexpr T& operator*() const {
        return *pos;
    }
    constexpr iterator& operator++() {
        ++pos;
        return *this;
    }
    constexpr void operator++(int) {
        ++pos;
    }
};

template <class T>
class move_only_range<T>::sentinel {
private:
    ranges::iterator_t<U> pos;

public:
    sentinel() = default;
    constexpr explicit sentinel(ranges::iterator_t<U> p) : pos{p} {}

    constexpr ranges::iterator_t<U> base() const {
        return pos;
    }

    constexpr bool operator==(iterator const& that) const {
        return pos == that.pos;
    }
};

template <class T>
inline constexpr bool ranges::enable_borrowed_range<::move_only_range<T>> = true;

struct boolish {
    operator bool() const {
        return true;
    }

    boolish operator!() const {
        return *this;
    }
};

template <class Category, class ValueType, bool Sized = false>
struct test_iterator {
    template <class T>
    static constexpr bool exactly = std::is_same_v<T, Category>;
    template <class T>
    static constexpr bool at_least = std::derived_from<Category, T>;

    struct reference {
        operator ValueType() const requires at_least<std::input_iterator_tag> {
            return {};
        }
        void operator=(ValueType const&) const {}
    };

    // output iterator operations
    test_iterator()                = default;
    test_iterator(test_iterator&&) = default;
    test_iterator& operator=(test_iterator&&) = default;

    reference operator*() const {
        return {};
    }
    ValueType& operator*() const requires at_least<std::contiguous_iterator_tag> {
        static ValueType value{};
        return value;
    }

    friend boolish operator==(test_iterator const&, std::default_sentinel_t const&) {
        return {};
    }
    friend boolish operator==(std::default_sentinel_t const&, test_iterator const&) {
        return {};
    }
    friend boolish operator!=(test_iterator const&, std::default_sentinel_t const&) {
        return {};
    }
    friend boolish operator!=(std::default_sentinel_t const&, test_iterator const&) {
        return {};
    }

    test_iterator& operator++() & {
        return *this;
    }
    test_iterator operator++(int) & {
        return {};
    }

    auto operator--() & {
        STATIC_ASSERT(always_false<Category>);
    }
    auto operator--(int) & {
        STATIC_ASSERT(always_false<Category>);
    }

    friend void iter_move(test_iterator const&) {
        STATIC_ASSERT(always_false<Category>);
    }
    friend void iter_swap(test_iterator const&, test_iterator const&) {
        STATIC_ASSERT(always_false<Category>);
    }
    void operator<(test_iterator const&) const {
        STATIC_ASSERT(always_false<Category>);
    }
    void operator>(test_iterator const&) const {
        STATIC_ASSERT(always_false<Category>);
    }
    void operator<=(test_iterator const&) const {
        STATIC_ASSERT(always_false<Category>);
    }
    void operator>=(test_iterator const&) const {
        STATIC_ASSERT(always_false<Category>);
    }

    void operator&() const {
        STATIC_ASSERT(always_false<Category>);
    }
    template <class T>
    friend void operator,(test_iterator const&, T&&) {
        STATIC_ASSERT(always_false<Category>);
    }

    // input iterator operations:
    void operator++(int) & requires exactly<std::input_iterator_tag> {}
    friend ValueType iter_move(test_iterator const&) requires at_least<std::input_iterator_tag> {
        return {};
    }
    friend void iter_swap(test_iterator const&, test_iterator const&) requires at_least<std::input_iterator_tag> {}

    // forward iterator operations:
    test_iterator(test_iterator const&) requires at_least<std::forward_iterator_tag> = default;
    test_iterator& operator=(test_iterator const&) requires at_least<std::forward_iterator_tag> = default;
    test_iterator operator++(int) & requires at_least<std::forward_iterator_tag> {}
    boolish operator==(test_iterator const&) const requires at_least<std::forward_iterator_tag> {
        return {};
    }
    boolish operator!=(test_iterator const&) const requires at_least<std::forward_iterator_tag> {
        return {};
    }

    // bidirectional iterator operations:
    test_iterator& operator--() & requires at_least<std::bidirectional_iterator_tag> {
        return *this;
    }
    test_iterator operator--(int) & requires at_least<std::bidirectional_iterator_tag> {}

    // random-access iterator operations:
    boolish operator<(test_iterator const&) const requires at_least<std::random_access_iterator_tag> {
        return {};
    }
    boolish operator>(test_iterator const&) const requires at_least<std::random_access_iterator_tag> {
        return {};
    }
    boolish operator<=(test_iterator const&) const requires at_least<std::random_access_iterator_tag> {
        return {};
    }
    boolish operator>=(test_iterator const&) const requires at_least<std::random_access_iterator_tag> {
        return {};
    }
    decltype(auto) operator[](std::ptrdiff_t) const& requires at_least<std::random_access_iterator_tag> {
        return **this;
    }
    test_iterator& operator+=(std::ptrdiff_t) & requires at_least<std::random_access_iterator_tag> {
        return *this;
    }
    test_iterator& operator-=(std::ptrdiff_t) & requires at_least<std::random_access_iterator_tag> {
        return *this;
    }
    test_iterator operator+(std::ptrdiff_t) const requires at_least<std::random_access_iterator_tag> {
        return *this;
    }
    friend test_iterator operator+(
        std::ptrdiff_t, test_iterator const& i) requires at_least<std::random_access_iterator_tag> {
        return i;
    }
    test_iterator operator-(std::ptrdiff_t) const requires at_least<std::random_access_iterator_tag> {
        return *this;
    }

    // sized_sentinel_for operations:
    std::ptrdiff_t operator-(test_iterator const&) const requires Sized || at_least<std::random_access_iterator_tag> {
        return 42;
    }
    friend std::ptrdiff_t operator-(std::default_sentinel_t, test_iterator const&) requires Sized {
        return 42;
    }
    friend std::ptrdiff_t operator-(test_iterator const&, std::default_sentinel_t) requires Sized {
        return -42;
    }
};

template <class Category, class ValueType, bool Sized>
struct std::iterator_traits<::test_iterator<Category, ValueType, Sized>> {
    using iterator_concept  = Category;
    using iterator_category = Category; // TRANSITION, LWG-3289
    using value_type        = ValueType;
    using difference_type   = ptrdiff_t;
    using pointer           = void;
    using reference         = iter_reference_t<::test_iterator<Category, ValueType, Sized>>;
};

template <class ValueType, bool Sized>
struct std::pointer_traits<::test_iterator<std::contiguous_iterator_tag, ValueType, Sized>> {
    using pointer         = ::test_iterator<contiguous_iterator_tag, ValueType, Sized>;
    using element_type    = ValueType;
    using difference_type = ptrdiff_t;

    [[nodiscard]] static constexpr element_type* to_address(pointer) noexcept {
        return nullptr;
    }
};

STATIC_ASSERT(std::output_iterator<test_iterator<std::output_iterator_tag, int, false>, int>);
STATIC_ASSERT(std::input_iterator<test_iterator<std::input_iterator_tag, int, false>>);
STATIC_ASSERT(std::forward_iterator<test_iterator<std::forward_iterator_tag, int, false>>);
STATIC_ASSERT(std::bidirectional_iterator<test_iterator<std::bidirectional_iterator_tag, int, false>>);
STATIC_ASSERT(std::random_access_iterator<test_iterator<std::random_access_iterator_tag, int, false>>);
STATIC_ASSERT(std::contiguous_iterator<test_iterator<std::contiguous_iterator_tag, int, false>>);

STATIC_ASSERT(std::output_iterator<test_iterator<std::output_iterator_tag, int, true>, int>);
STATIC_ASSERT(std::input_iterator<test_iterator<std::input_iterator_tag, int, true>>);
STATIC_ASSERT(std::forward_iterator<test_iterator<std::forward_iterator_tag, int, true>>);
STATIC_ASSERT(std::bidirectional_iterator<test_iterator<std::bidirectional_iterator_tag, int, true>>);
STATIC_ASSERT(std::random_access_iterator<test_iterator<std::random_access_iterator_tag, int, true>>);
STATIC_ASSERT(std::contiguous_iterator<test_iterator<std::contiguous_iterator_tag, int, true>>);

STATIC_ASSERT(std::sized_sentinel_for<std::default_sentinel_t, test_iterator<std::output_iterator_tag, int, true>>);
STATIC_ASSERT(std::sized_sentinel_for<std::default_sentinel_t, test_iterator<std::input_iterator_tag, int, true>>);
STATIC_ASSERT(std::sized_sentinel_for<std::default_sentinel_t, test_iterator<std::forward_iterator_tag, int, true>>);
STATIC_ASSERT(
    std::sized_sentinel_for<std::default_sentinel_t, test_iterator<std::bidirectional_iterator_tag, int, true>>);
STATIC_ASSERT(
    std::sized_sentinel_for<std::default_sentinel_t, test_iterator<std::random_access_iterator_tag, int, true>>);
STATIC_ASSERT(std::sized_sentinel_for<std::default_sentinel_t, test_iterator<std::contiguous_iterator_tag, int, true>>);

STATIC_ASSERT(std::sized_sentinel_for<test_iterator<std::forward_iterator_tag, int, true>,
    test_iterator<std::forward_iterator_tag, int, true>>);
STATIC_ASSERT(std::sized_sentinel_for<test_iterator<std::bidirectional_iterator_tag, int, true>,
    test_iterator<std::bidirectional_iterator_tag, int, true>>);
STATIC_ASSERT(std::sized_sentinel_for<test_iterator<std::random_access_iterator_tag, int, true>,
    test_iterator<std::random_access_iterator_tag, int, true>>);
STATIC_ASSERT(std::sized_sentinel_for<test_iterator<std::contiguous_iterator_tag, int, true>,
    test_iterator<std::contiguous_iterator_tag, int, true>>);

template <class Category, class ValueType, bool Sized = false, bool Common = false>
struct test_range {
    using I = test_iterator<Category, ValueType, Sized>;
    using S = std::conditional_t<Common && std::derived_from<Category, std::forward_iterator_tag>, I,
        std::default_sentinel_t>;

    I begin() const {
        return {};
    }

    S end() const {
        return {};
    }

    std::ptrdiff_t size() const requires Sized {
        return 42;
    }

    ValueType* data() const requires std::derived_from<Category, std::contiguous_iterator_tag> {
        return nullptr;
    }

    void operator&() const {
        STATIC_ASSERT(always_false<Category>);
    }
    template <class T>
    friend void operator,(test_range const&, T&&) {
        STATIC_ASSERT(always_false<Category>);
    }
};

STATIC_ASSERT(ranges::output_range<test_range<std::output_iterator_tag, int, false>, int>);
STATIC_ASSERT(ranges::input_range<test_range<std::input_iterator_tag, int, false>>);
STATIC_ASSERT(ranges::forward_range<test_range<std::forward_iterator_tag, int, false>>);
STATIC_ASSERT(ranges::bidirectional_range<test_range<std::bidirectional_iterator_tag, int, false>>);
STATIC_ASSERT(ranges::random_access_range<test_range<std::random_access_iterator_tag, int, false>>);
STATIC_ASSERT(ranges::contiguous_range<test_range<std::contiguous_iterator_tag, int, false>>);

STATIC_ASSERT(ranges::output_range<test_range<std::output_iterator_tag, int, true>, int>);
STATIC_ASSERT(ranges::input_range<test_range<std::input_iterator_tag, int, true>>);
STATIC_ASSERT(ranges::forward_range<test_range<std::forward_iterator_tag, int, true>>);
STATIC_ASSERT(ranges::bidirectional_range<test_range<std::bidirectional_iterator_tag, int, true>>);
STATIC_ASSERT(ranges::random_access_range<test_range<std::random_access_iterator_tag, int, true>>);
STATIC_ASSERT(ranges::contiguous_range<test_range<std::contiguous_iterator_tag, int, true>>);
STATIC_ASSERT(ranges::sized_range<test_range<std::output_iterator_tag, int, true>>);
STATIC_ASSERT(ranges::sized_range<test_range<std::input_iterator_tag, int, true>>);
STATIC_ASSERT(ranges::sized_range<test_range<std::forward_iterator_tag, int, true>>);
STATIC_ASSERT(ranges::sized_range<test_range<std::bidirectional_iterator_tag, int, true>>);
STATIC_ASSERT(ranges::sized_range<test_range<std::random_access_iterator_tag, int, true>>);
STATIC_ASSERT(ranges::sized_range<test_range<std::contiguous_iterator_tag, int, true>>);

STATIC_ASSERT(ranges::forward_range<test_range<std::forward_iterator_tag, int, true, true>>);
STATIC_ASSERT(ranges::bidirectional_range<test_range<std::bidirectional_iterator_tag, int, true, true>>);
STATIC_ASSERT(ranges::random_access_range<test_range<std::random_access_iterator_tag, int, true, true>>);
STATIC_ASSERT(ranges::contiguous_range<test_range<std::contiguous_iterator_tag, int, true, true>>);
STATIC_ASSERT(ranges::sized_range<test_range<std::forward_iterator_tag, int, true, true>>);
STATIC_ASSERT(ranges::sized_range<test_range<std::bidirectional_iterator_tag, int, true, true>>);
STATIC_ASSERT(ranges::sized_range<test_range<std::random_access_iterator_tag, int, true, true>>);
STATIC_ASSERT(ranges::sized_range<test_range<std::contiguous_iterator_tag, int, true, true>>);

template <int>
struct unique_tag {};

template <class I, int Tag = 0>
using ProjectionFor = unique_tag<Tag> (*)(std::iter_common_reference_t<I>);

template <class I>
using UnaryPredicateFor = boolish (*)(std::iter_common_reference_t<I>);

template <int Tag = 0>
using ProjectedUnaryPredicate = boolish (*)(unique_tag<Tag>);
template <class I2, int Tag1 = 0>
using HalfProjectedBinaryPredicateFor = boolish (*)(unique_tag<Tag1>, std::iter_common_reference_t<I2>);
template <int Tag1 = 0, int Tag2 = 0>
using ProjectedBinaryPredicate = boolish (*)(unique_tag<Tag1>, unique_tag<Tag2>);

template <class I1, class I2>
using BinaryPredicateFor = boolish (*)(std::iter_common_reference_t<I1>, std::iter_common_reference_t<I2>);

template <class Continuation>
struct with_output_iterators {
    template <class... Args>
    static void call() {
        Continuation::template call<Args..., test_iterator<std::output_iterator_tag, int>>();
        Continuation::template call<Args..., test_iterator<std::input_iterator_tag, int>>();
        Continuation::template call<Args..., test_iterator<std::forward_iterator_tag, int, false>>();
        Continuation::template call<Args..., test_iterator<std::forward_iterator_tag, int, true>>();
        Continuation::template call<Args..., test_iterator<std::bidirectional_iterator_tag, int, false>>();
        Continuation::template call<Args..., test_iterator<std::bidirectional_iterator_tag, int, true>>();
        Continuation::template call<Args..., test_iterator<std::random_access_iterator_tag, int>>();
        Continuation::template call<Args..., test_iterator<std::contiguous_iterator_tag, int>>();
    }
};

template <class Continuation>
struct with_input_ranges {
    template <class... Args>
    static void call() {
        Continuation::template call<Args..., test_range<std::input_iterator_tag, int, false>>();
        Continuation::template call<Args..., test_range<std::input_iterator_tag, int, true>>();

        Continuation::template call<Args..., test_range<std::forward_iterator_tag, int, false, false>>();
        Continuation::template call<Args..., test_range<std::forward_iterator_tag, int, false, true>>();
        Continuation::template call<Args..., test_range<std::forward_iterator_tag, int, true, false>>();
        Continuation::template call<Args..., test_range<std::forward_iterator_tag, int, true, true>>();

        Continuation::template call<Args..., test_range<std::bidirectional_iterator_tag, int, false, false>>();
        Continuation::template call<Args..., test_range<std::bidirectional_iterator_tag, int, false, true>>();
        Continuation::template call<Args..., test_range<std::bidirectional_iterator_tag, int, true, false>>();
        Continuation::template call<Args..., test_range<std::bidirectional_iterator_tag, int, true, true>>();

        Continuation::template call<Args..., test_range<std::random_access_iterator_tag, int, true, false>>();
        Continuation::template call<Args..., test_range<std::random_access_iterator_tag, int, true, true>>();

        Continuation::template call<Args..., test_range<std::contiguous_iterator_tag, int, true, false>>();
        Continuation::template call<Args..., test_range<std::contiguous_iterator_tag, int, true, true>>();
    }
};

template <class Continuation>
struct with_input_iterators {
    template <class... Args>
    static void call() {
        Continuation::template call<Args..., test_iterator<std::input_iterator_tag, int>>();
        Continuation::template call<Args..., test_iterator<std::forward_iterator_tag, int, false>>();
        Continuation::template call<Args..., test_iterator<std::forward_iterator_tag, int, true>>();
        Continuation::template call<Args..., test_iterator<std::bidirectional_iterator_tag, int, false>>();
        Continuation::template call<Args..., test_iterator<std::bidirectional_iterator_tag, int, true>>();
        Continuation::template call<Args..., test_iterator<std::random_access_iterator_tag, int>>();
        Continuation::template call<Args..., test_iterator<std::contiguous_iterator_tag, int>>();
    }
};

template <class Continuation>
struct with_difference {
    template <class Iterator>
    static void call() {
        Continuation::template call<Iterator, std::iter_difference_t<Iterator>>();
    }
};

template <class Instantiator>
void test_out() {
    with_output_iterators<Instantiator>::call();
}

template <class Instantiator>
void test_in() {
    with_input_ranges<Instantiator>::call();
}

template <class Instantiator>
void test_in_in() {
    with_input_ranges<with_input_ranges<Instantiator>>::call();
}

template <class Instantiator>
void test_in_out() {
    with_input_ranges<with_output_iterators<Instantiator>>::call();
}

template <class Instantiator>
void test_counted_out() {
    with_input_iterators<with_difference<with_output_iterators<Instantiator>>>::call();
}
