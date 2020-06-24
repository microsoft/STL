// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#include <algorithm>
#include <cstddef>
#include <forward_list>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <list>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <input_iterator.hpp>

// lets INSTANTIATE macro be used for types that aren't default constructible.
// Doesn't actually instantiate the type if not default constructible,
// but it will inspect the type, which is the true purpose.
template <typename T, bool = std::is_default_constructible_v<T>>
struct instantiate_helper;

template <typename T>
struct instantiate_helper<T, true> {
    using type = T;
};

template <typename T>
struct instantiate_helper<T, false> {
    using type = int;
};

#define USE_VALUE(...)          \
    do {                        \
        auto val = __VA_ARGS__; \
        (void) val;             \
    } while (0)
#define INSTANTIATE(...)                                      \
    do {                                                      \
        typename instantiate_helper<__VA_ARGS__>::type val{}; \
        (void) val;                                           \
    } while (0)
#define TRAIT_V(TRAIT_NAME, ...) USE_VALUE(TRAIT_NAME##_v<__VA_ARGS__>)

// need to prefer copy constructor over default constructor so iterator comparison is valid.
template <typename T>
std::enable_if_t<std::is_copy_constructible_v<T>, T> construct_another(const T& value) {
    return value;
}

template <typename T>
std::enable_if_t<!std::is_copy_constructible_v<T>, T> construct_another(const T&) {
    return T{};
}

template <typename T, typename U>
void equality_test(T&& lhs, U&& rhs) {
    USE_VALUE(lhs == rhs);
    USE_VALUE(lhs != rhs);
}

template <typename T>
void equality_test(T value) {
    T another = construct_another(value);
    equality_test(value, another);
}

template <typename T, typename U>
void comparable_test(T&& lhs, U&& rhs) {
    equality_test(lhs, rhs);
    USE_VALUE(lhs < rhs);
    USE_VALUE(lhs > rhs);
    USE_VALUE(lhs <= rhs);
    USE_VALUE(lhs >= rhs);
}

template <typename T>
void comparable_test(T value) {
    T another = construct_another(value);
    comparable_test(value, another);
}

template <typename T>
void swap_test(const T& value) {
    T lhs = construct_another(value);
    T rhs = construct_another(value);
    std::swap(lhs, rhs);
}

template <typename T>
void hash_test(T&& value) {
    auto hasher = std::hash<std::decay_t<T>>();
    (void) hasher(std::forward<T>(value));
}

template <typename T>
void hash_test() {
    T value{};
    auto hasher = std::hash<T>();
    (void) hasher(value);
}

template <typename Container>
void fwd_iterators_test() {
    using ValueType = typename Container::value_type;
    Container c{static_cast<ValueType>(1), static_cast<ValueType>(2), static_cast<ValueType>(3),
        static_cast<ValueType>(4), static_cast<ValueType>(5), static_cast<ValueType>(6), static_cast<ValueType>(7),
        static_cast<ValueType>(8), static_cast<ValueType>(9), static_cast<ValueType>(10)};

    auto it  = begin(c);
    auto it2 = it;

    INSTANTIATE(std::iterator_traits<decltype(it)>);
    (void) next(it);
    advance(it2, 5);
    (void) distance(it, it2);

    USE_VALUE(make_move_iterator(it));
    swap_test(it);
}

template <typename Container>
void bidi_iterators_test() {
    using ValueType = typename Container::value_type;
    Container c{static_cast<ValueType>(1), static_cast<ValueType>(2), static_cast<ValueType>(3),
        static_cast<ValueType>(4), static_cast<ValueType>(5), static_cast<ValueType>(6), static_cast<ValueType>(7),
        static_cast<ValueType>(8), static_cast<ValueType>(9), static_cast<ValueType>(10)};
    auto it = begin(c);
    equality_test(it);
    auto rit = end(c);
    equality_test(rit);
    (void) prev(next(it));
}

template <typename T>
void random_access_iterator_test(T value) {
    typename std::iterator_traits<T>::difference_type diff{};
    USE_VALUE(diff + value);
    USE_VALUE(value - value);
    comparable_test(value);
    ++value;
    value++;
    --value;
    value--;
}

template <typename Container>
void random_iterators_test() {
    using ValueType = typename Container::value_type;
    Container c{static_cast<ValueType>(1), static_cast<ValueType>(2), static_cast<ValueType>(3),
        static_cast<ValueType>(4), static_cast<ValueType>(5), static_cast<ValueType>(6), static_cast<ValueType>(7),
        static_cast<ValueType>(8), static_cast<ValueType>(9), static_cast<ValueType>(10)};
    random_access_iterator_test(c.begin());
    random_access_iterator_test(c.rbegin());
}

template <typename T>
auto get_all_iterator_types_for(T value) {
    using value_type = typename T::value_type;

    return std::make_tuple(std_testing::input_iterator_container<value_type>(begin(value), end(value)),
        std::forward_list<value_type>(begin(value), end(value)), std::list<value_type>(begin(value), end(value)),
        std::vector<value_type>(begin(value), end(value)));
}
