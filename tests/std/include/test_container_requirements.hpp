// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <algorithm>
#include <cassert>
#include <compare>
#include <format>
#include <iterator>
#include <print>
#include <type_traits>
#include <utility>

// Extracted common functionality from flat_map and flat_set tests.
// May be extended for other containers if needed.

template <class T>
void assert_container_requirements(const T& s) {
    T m = s;
    assert(m == s);

    static_assert(std::is_same_v<decltype(m = s), T&>);
    static_assert(std::is_same_v<decltype(m = std::move(m)), T&>);
    static_assert(std::is_same_v<decltype(m.begin()), typename T::iterator>);
    static_assert(std::is_same_v<decltype(m.end()), typename T::iterator>);
    static_assert(std::is_same_v<decltype(s.begin()), typename T::const_iterator>);
    static_assert(std::is_same_v<decltype(s.end()), typename T::const_iterator>);
    static_assert(std::is_same_v<decltype(m.cbegin()), typename T::const_iterator>);
    static_assert(std::is_same_v<decltype(m.cend()), typename T::const_iterator>);
    static_assert(std::is_same_v<decltype(s.cbegin()), typename T::const_iterator>);
    static_assert(std::is_same_v<decltype(s.cend()), typename T::const_iterator>);
    static_assert(std::is_convertible_v<typename T::iterator, typename T::const_iterator>);
    static_assert(std::is_same_v<decltype(m.begin() <=> m.end()), std::strong_ordering>);
    static_assert(std::is_same_v<decltype(s.size()), typename T::size_type>);
    static_assert(std::is_same_v<decltype(s.max_size()), typename T::size_type>);
    static_assert(std::is_same_v<decltype(*m.begin()), typename T::reference>
                  || std::is_same_v<decltype(*m.begin()), typename T::const_reference>);
    static_assert(std::is_same_v<decltype(*m.cbegin()), typename T::const_reference>);

    T my_moved = std::move(m);
    assert(!(my_moved != s));

    T my_empty{};
    assert(my_empty.empty());

    T non_empty = s;
    my_empty.swap(non_empty);
    assert(non_empty.empty());
    assert(my_empty == s);

    std::swap(my_empty, non_empty);
    assert(my_empty.empty());
    assert(non_empty == s);

    assert(s.cbegin() <= s.cend());
    assert((s.cbegin() < s.cend()) == !s.empty());

    assert(m.begin() <= m.end());
    assert((m.begin() < m.end()) == !m.empty());

    assert(static_cast<T::size_type>(s.cend() - s.cbegin()) == s.size());
}

template <class T>
void assert_reversible_container_requirements(const T& s) {
    static_assert(std::is_same_v<std::reverse_iterator<typename T::iterator>, typename T::reverse_iterator>);
    static_assert(
        std::is_same_v<std::reverse_iterator<typename T::const_iterator>, typename T::const_reverse_iterator>);
    static_assert(std::is_same_v<decltype(T{}.rbegin()), typename T::reverse_iterator>);
    static_assert(std::is_same_v<decltype(T{}.rend()), typename T::reverse_iterator>);
    static_assert(std::is_same_v<decltype(s.rbegin()), typename T::const_reverse_iterator>);
    static_assert(std::is_same_v<decltype(s.rend()), typename T::const_reverse_iterator>);
    static_assert(std::is_same_v<decltype(T{}.crbegin()), typename T::const_reverse_iterator>);
    static_assert(std::is_same_v<decltype(T{}.crend()), typename T::const_reverse_iterator>);
    static_assert(std::is_same_v<decltype(s.crbegin()), typename T::const_reverse_iterator>);
    static_assert(std::is_same_v<decltype(s.crend()), typename T::const_reverse_iterator>);
    static_assert(std::is_convertible_v<typename T::reverse_iterator, typename T::const_reverse_iterator>);
}

template <class T>
concept map_has_nothrow_swappable_containers = requires {
    typename T::key_container_type;
    typename T::mapped_container_type;
    requires std::is_nothrow_swappable_v<typename T::key_container_type>;
    requires std::is_nothrow_swappable_v<typename T::mapped_container_type>;
};

template <class T>
concept set_has_nothrow_swappable_containers = requires {
    typename T::container_type;
    requires std::is_nothrow_swappable_v<typename T::container_type>;
};

template <class T>
concept has_nothrow_swappable_containers =
    set_has_nothrow_swappable_containers<T> || map_has_nothrow_swappable_containers<T>;

template <class T>
void assert_noexcept_requirements(T& s) {
    static_assert(noexcept(s.begin()));
    static_assert(noexcept(s.end()));
    static_assert(noexcept(s.cbegin()));
    static_assert(noexcept(s.cend()));
    static_assert(noexcept(s.rbegin()));
    static_assert(noexcept(s.rend()));
    static_assert(noexcept(s.crbegin()));
    static_assert(noexcept(s.crend()));

    static_assert(noexcept(s.empty()));
    static_assert(noexcept(s.size()));
    static_assert(noexcept(s.max_size()));

    if constexpr (!std::is_const_v<T>) {
        constexpr bool is_noexcept =
            has_nothrow_swappable_containers<T> && std::is_nothrow_swappable_v<typename T::key_compare>;
        static_assert(noexcept(s.swap(s)) == is_noexcept);
        static_assert(noexcept(std::ranges::swap(s, s)) == is_noexcept); // using ADL-swap
        static_assert(noexcept(s.clear()));
    }
}

template <bool ExpectedUnique, class T>
void assert_is_sorted_maybe_unique(const T& s) {
    const auto val_comp = s.value_comp();
    const auto begin_it = s.cbegin();
    const auto end_it   = s.cend();

    // internal check by the container itself
    assert(s._Is_sorted_and_unique());

    // external check observable by the user
    assert(std::is_sorted(begin_it, end_it, val_comp));
    if constexpr (ExpectedUnique) {
        const auto not_comp  = [&](const auto& left, const auto& right) { return !val_comp(left, right); };
        const bool is_unique = std::adjacent_find(begin_it, end_it, not_comp) == end_it;

        if constexpr (std::formattable<T, char>) {
            if (!is_unique) {
                std::println("Container {} is not unique", s);
            }
        }
        assert(is_unique);
    }
}

template <class T>
void assert_set_requirements() {
    using iterator        = T::iterator;
    using const_iterator  = T::const_iterator;
    using key_type        = T::key_type;
    using value_type      = T::value_type;
    using reference       = T::reference;
    using const_reference = T::const_reference;

    static_assert(std::same_as<std::const_iterator<const_iterator>, const_iterator>);
    static_assert(std::is_convertible_v<iterator, const_iterator>);

    // additionally:
    static_assert(std::is_same_v<key_type, value_type>);
    static_assert(std::same_as<std::const_iterator<iterator>, iterator>);
    static_assert(std::is_convertible_v<const_iterator, iterator>);
    static_assert(std::is_same_v<decltype(*std::declval<T>().begin()), const value_type&>);
    static_assert(std::is_same_v<std::remove_cvref_t<reference>, value_type>);
    static_assert(std::is_same_v<const_reference, const value_type&>);
}

template <class T>
void assert_map_requirements() {
    using iterator       = T::iterator;
    using const_iterator = T::const_iterator;
    using key_type       = T::key_type;
    using value_type     = T::value_type;
    using mapped_type    = T::mapped_type;

    static_assert(std::same_as<std::const_iterator<const_iterator>, const_iterator>);
    static_assert(std::is_convertible_v<iterator, const_iterator>);

    // additionally:
    static_assert(std::is_same_v<value_type, std::pair<key_type, mapped_type>>);
}

template <class T>
void assert_three_way_comparability() {
    using value_type = T::value_type;
    if constexpr (std::three_way_comparable<value_type>) {
        static_assert(std::three_way_comparable<T>);
    }
}

class key_comparer {
private:
    static const auto& extract_key(const auto& obj) {
        if constexpr (requires { obj.key; }) {
            return obj.key;
        } else {
            return obj;
        }
    }

public:
    bool operator()(const auto& lhs, const auto& rhs) const {
        return extract_key(lhs) < extract_key(rhs);
    }

    using is_transparent = int;
};
