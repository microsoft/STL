// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <vector>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

// No STL containers support only input_iterator_tag (just stuff like istream_iterator).
// input_iterator_container is a range that only supports input iterators
// with added unchecked/rechecked functionality to ensure we traverse those code paths
namespace std_testing {

    template <typename T>
    struct const_unchecked_input_iterator {
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = const T*;
        using reference         = const T&;
        using iterator_category = std::input_iterator_tag;
        using my_iter           = const_unchecked_input_iterator;

        explicit const_unchecked_input_iterator(const T* val) : m_val(const_cast<T*>(val)) {}
        const_unchecked_input_iterator(const const_unchecked_input_iterator&)            = default;
        const_unchecked_input_iterator(const_unchecked_input_iterator&&)                 = default;
        const_unchecked_input_iterator& operator=(const const_unchecked_input_iterator&) = default;
        const_unchecked_input_iterator& operator=(const_unchecked_input_iterator&&)      = default;

        reference operator*() const {
            return *m_val;
        }

        pointer operator->() const {
            return m_val;
        }

        my_iter& operator++() {
            ++m_val;
            return *this;
        }

        void operator++(int) = delete; // avoid postincrement

        bool operator==(const my_iter& right) const {
            return m_val == right.m_val;
        }

        bool operator!=(const my_iter& right) const {
            return !(*this == right);
        }

        T* m_val;

    protected:
        explicit const_unchecked_input_iterator(T* val) : m_val(val) {}
    };

    template <typename T>
    struct unchecked_input_iterator : const_unchecked_input_iterator<T> {
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;
        using iterator_category = std::input_iterator_tag;
        using my_iter           = unchecked_input_iterator;

        explicit unchecked_input_iterator(T* val) : const_unchecked_input_iterator<T>(val) {}
        unchecked_input_iterator(const unchecked_input_iterator&)            = default;
        unchecked_input_iterator(unchecked_input_iterator&&)                 = default;
        unchecked_input_iterator& operator=(const unchecked_input_iterator&) = default;
        unchecked_input_iterator& operator=(unchecked_input_iterator&&)      = default;

        reference operator*() const {
            return *this->m_val;
        }

        pointer operator->() const {
            return this->m_val;
        }

        my_iter& operator++() {
            ++this->m_val;
            return *this;
        }

        void operator++(int) = delete; // avoid postincrement
    };

    template <typename T>
    struct const_checked_input_iterator : const_unchecked_input_iterator<T> {
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = const T*;
        using reference         = const T&;
        using iterator_category = std::input_iterator_tag;
        using my_iter           = const_checked_input_iterator;

        explicit const_checked_input_iterator(const T* val) : const_unchecked_input_iterator<T>(val) {}
        const_checked_input_iterator(const const_checked_input_iterator&)            = default;
        const_checked_input_iterator(const_checked_input_iterator&&)                 = default;
        const_checked_input_iterator& operator=(const const_checked_input_iterator&) = default;
        const_checked_input_iterator& operator=(const_checked_input_iterator&&)      = default;

        void _Seek_to(const_unchecked_input_iterator<T> right) {
            this->m_val = right.m_val;
        }

        const_unchecked_input_iterator<T> _Unwrapped() const {
            return const_unchecked_input_iterator<T>(this->m_val);
        }

        static constexpr bool _Unwrap_when_unverified = _ITERATOR_DEBUG_LEVEL == 0;

        my_iter& operator++() {
            ++this->m_val;
            return *this;
        }

        void operator++(int) = delete; // avoid postincrement
    };

    template <typename T>
    struct checked_input_iterator : unchecked_input_iterator<T> {
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;
        using iterator_category = std::input_iterator_tag;
        using my_iter           = checked_input_iterator;

        explicit checked_input_iterator(T* val) : unchecked_input_iterator<T>(val) {}
        checked_input_iterator(const checked_input_iterator&)            = default;
        checked_input_iterator(checked_input_iterator&&)                 = default;
        checked_input_iterator& operator=(const checked_input_iterator&) = default;
        checked_input_iterator& operator=(checked_input_iterator&&)      = default;

        void _Seek_to(unchecked_input_iterator<T> right) {
            this->m_val = right.m_val;
        }

        unchecked_input_iterator<T> _Unwrapped() const {
            return unchecked_input_iterator<T>(this->m_val);
        }

        static constexpr bool _Unwrap_when_unverified = _ITERATOR_DEBUG_LEVEL == 0;

        my_iter& operator++() {
            ++this->m_val;
            return *this;
        }

        void operator++(int) = delete; // avoid postincrement
    };

    template <typename T>
    struct input_iterator_container {
        using impl_value = std::conditional_t<std::is_same_v<T, bool>, char, T>; // avoid vector<bool> nonsense
        STATIC_ASSERT(sizeof(impl_value) == sizeof(T));
        STATIC_ASSERT(alignof(impl_value) == alignof(T));

        using iterator       = checked_input_iterator<T>;
        using const_iterator = const_checked_input_iterator<T>;

        input_iterator_container() = default;

        template <typename InIt>
        input_iterator_container(InIt first, InIt last) : m_val(first, last) {}

        input_iterator_container(std::initializer_list<T> init_list)
            : input_iterator_container(init_list.begin(), init_list.end()) {}

        input_iterator_container(const input_iterator_container&)            = default;
        input_iterator_container(input_iterator_container&&)                 = default;
        input_iterator_container& operator=(const input_iterator_container&) = default;
        input_iterator_container& operator=(input_iterator_container&&)      = default;

        const_iterator begin() const {
            return const_iterator{reinterpret_cast<const T*>(m_val.data())};
        }
        const_iterator end() const {
            return const_iterator{reinterpret_cast<const T*>(m_val.data()) + m_val.size()};
        }
        iterator begin() {
            return iterator{reinterpret_cast<T*>(m_val.data())};
        }
        iterator end() {
            return iterator{reinterpret_cast<T*>(m_val.data()) + m_val.size()};
        }

        const_iterator cbegin() const {
            return begin();
        }
        const_iterator cend() const {
            return end();
        }

        std::vector<impl_value> m_val;
    };
} // namespace std_testing

#undef STATIC_ASSERT
