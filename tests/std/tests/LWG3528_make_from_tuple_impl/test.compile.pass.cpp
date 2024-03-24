//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//===----------------------------------------------------------------------===//

#include <cstdint>
#include <tuple>
#include <type_traits>

struct A {
    int a;
};
struct B : public A {};

struct C {
    C(const B&) {}
};

enum class D {
    ONE,
    TWO,
};

template <class T, class Tuple>
auto can_make_from_tuple(T&&, Tuple&& t) -> decltype(std::make_from_tuple<T>(t), uint8_t()) {
    return 0;
}
template <class T, class Tuple>
uint32_t can_make_from_tuple(...) {
    return 0;
}

template <class T, class Tuple>
inline constexpr bool has_make_from_tuple =
    std::is_same_v<decltype(can_make_from_tuple<T, Tuple>(T{}, Tuple{})), uint8_t>;

template <class T, class Tuple>
auto can_make_from_tuple_impl(T&&, Tuple&& t)
    -> decltype(std::_Make_from_tuple_impl<T>(std::forward<Tuple>(t),
                    std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{}),
        uint8_t()) {
    return 0;
}
template <class T, class Tuple>
uint32_t can_make_from_tuple_impl(...) {
    return 0;
}

template <class T, class Tuple>
inline constexpr bool has_make_from_tuple_impl =
    std::is_same_v<decltype(can_make_from_tuple_impl<T, Tuple>(T{}, Tuple{})), uint8_t>;

// Test std::make_from_tuple constraints.

// reinterpret_cast
static_assert(!has_make_from_tuple<int*, std::tuple<A*>>);
static_assert(has_make_from_tuple<A*, std::tuple<A*>>);

// const_cast
static_assert(!has_make_from_tuple<char*, std::tuple<const char*>>);
static_assert(!has_make_from_tuple<volatile char*, std::tuple<const volatile char*>>);
static_assert(has_make_from_tuple<volatile char*, std::tuple<volatile char*>>);
static_assert(has_make_from_tuple<char*, std::tuple<char*>>);
static_assert(has_make_from_tuple<const char*, std::tuple<char*>>);
static_assert(has_make_from_tuple<const volatile char*, std::tuple<volatile char*>>);

// static_cast
static_assert(!has_make_from_tuple<int, std::tuple<D>>);
static_assert(!has_make_from_tuple<D, std::tuple<int>>);
static_assert(has_make_from_tuple<long, std::tuple<int>>);
static_assert(has_make_from_tuple<double, std::tuple<float>>);
static_assert(has_make_from_tuple<float, std::tuple<double>>);

// Test std::__Make_from_tuple_impl constraints.

// reinterpret_cast
static_assert(!has_make_from_tuple_impl<int*, std::tuple<A*>>);
static_assert(has_make_from_tuple_impl<A*, std::tuple<A*>>);

// const_cast
static_assert(!has_make_from_tuple_impl<char*, std::tuple<const char*>>);
static_assert(!has_make_from_tuple_impl<volatile char*, std::tuple<const volatile char*>>);
static_assert(has_make_from_tuple_impl<volatile char*, std::tuple<volatile char*>>);
static_assert(has_make_from_tuple_impl<char*, std::tuple<char*>>);
static_assert(has_make_from_tuple_impl<const char*, std::tuple<char*>>);
static_assert(has_make_from_tuple_impl<const volatile char*, std::tuple<volatile char*>>);

// static_cast
static_assert(!has_make_from_tuple_impl<int, std::tuple<D>>);
static_assert(!has_make_from_tuple_impl<D, std::tuple<int>>);
static_assert(has_make_from_tuple_impl<long, std::tuple<int>>);
static_assert(has_make_from_tuple_impl<double, std::tuple<float>>);
static_assert(has_make_from_tuple_impl<float, std::tuple<double>>);

int main() {
    return 0;
}
