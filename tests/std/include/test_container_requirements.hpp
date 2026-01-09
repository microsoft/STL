// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <iterator>
#include <print>
#include <type_traits>

using namespace std;

// Extracted common functionality from flat_map and flat_set tests.
// May be extended for other containers if needed

template <class T>
void assert_container_requirements(const T& s) {
    T m = s;
    assert(m == s);

    static_assert(is_same_v<decltype(m = s), T&>);
    static_assert(is_same_v<decltype(m = std::move(m)), T&>);
    static_assert(is_same_v<decltype(m.begin()), typename T::iterator>);
    static_assert(is_same_v<decltype(m.end()), typename T::iterator>);
    static_assert(is_same_v<decltype(s.cbegin()), typename T::const_iterator>);
    static_assert(is_same_v<decltype(m.cend()), typename T::const_iterator>);
    static_assert(is_convertible_v<typename T::iterator, typename T::const_iterator>);
    static_assert(is_same_v<decltype(m.begin() <=> m.end()), strong_ordering>);
    static_assert(is_same_v<decltype(s.size()), typename T::size_type>);
    static_assert(is_same_v<decltype(s.max_size()), typename T::size_type>);
    static_assert(is_same_v<decltype(*m.begin()), const typename T::value_type&>);
    static_assert(is_same_v<decltype(*m.cbegin()), const typename T::value_type&>);

    T my_moved = std::move(m);
    assert(!(my_moved != s));

    T empty{};
    assert(empty.empty());

    T non_empty = s;
    empty.swap(non_empty);
    assert(non_empty.empty());
    assert(empty == s);

    std::swap(empty, non_empty);
    assert(empty.empty());
    assert(non_empty == s);

    assert(s.cbegin() <= s.cend());
    assert(s.cbegin() < s.cend() || s.empty());

    assert(m.begin() <= m.end());
    assert(m.begin() < m.end() || m.empty());

    assert(static_cast<typename T::size_type>(s.cend() - s.cbegin()) == s.size());
}

template <class T>
void assert_reversible_container_requirements(const T& s) {
    static_assert(is_same_v<reverse_iterator<typename T::iterator>, typename T::reverse_iterator>);
    static_assert(is_same_v<reverse_iterator<typename T::const_iterator>, typename T::const_reverse_iterator>);
    static_assert(is_same_v<decltype(T{}.rbegin()), typename T::reverse_iterator>);
    static_assert(is_same_v<decltype(T{}.rend()), typename T::reverse_iterator>);
    static_assert(is_same_v<decltype(s.rbegin()), typename T::const_reverse_iterator>);
    static_assert(is_same_v<decltype(s.rend()), typename T::const_reverse_iterator>);
    static_assert(is_same_v<decltype(s.crbegin()), typename T::const_reverse_iterator>);
    static_assert(is_same_v<decltype(s.crend()), typename T::const_reverse_iterator>);
    static_assert(is_convertible_v<typename T::reverse_iterator, typename T::const_reverse_iterator>);
}

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

    if constexpr (!is_const_v<T>) {
        constexpr bool is_noexcept =
            is_nothrow_swappable_v<typename T::container_type> && is_nothrow_swappable_v<typename T::key_compare>;
        static_assert(noexcept(s.swap(s)) == is_noexcept);
        static_assert(noexcept(ranges::swap(s, s)) == is_noexcept); // using ADL-swap
        static_assert(noexcept(s.clear()));
    }
}

template <bool ExpectedUnique, class T>
void assert_is_sorted_maybe_unique(const T& s) {
    const auto val_comp = s.value_comp();
    auto begin_it       = s.cbegin();
    const auto end_it   = s.cend();

    // internal check by the container itself
    assert(s._Is_sorted_and_unique());

    // external check observable by the user
    assert(is_sorted(begin_it, end_it, val_comp));
    if constexpr (ExpectedUnique) {
        if (!s.empty()) {
            for (auto prev_it = begin_it, it = prev_it + 1; it != end_it; ++prev_it, ++it) {
                if (!val_comp(*prev_it, *it)) {
                    println("Container {} is not sorted-maybe-unique", s);
                    assert(false);
                }
            }
        }
    }
}


template <class T>
void assert_set_requirements() {
    using iterator       = T::iterator;
    using const_iterator = T::const_iterator;
    using key_type       = T::key_type;
    using value_type     = T::value_type;

    static_assert(same_as<std::const_iterator<const_iterator>, const_iterator>);
    static_assert(is_convertible_v<iterator, const_iterator>);

    // additionally:
    static_assert(is_same_v<key_type, value_type>);
    static_assert(same_as<std::const_iterator<iterator>, iterator>);
    static_assert(is_convertible_v<const_iterator, iterator>);
}
