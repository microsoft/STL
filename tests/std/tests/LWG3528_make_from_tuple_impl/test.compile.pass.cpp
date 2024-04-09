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

// derived from libc++'s test files:
// * std/utilities/tuple/tuple.tuple/tuple.apply/make_from_tuple.pass.cpp

#include <array>
#include <tuple>
#include <type_traits>
#include <utility>

struct A {
    int a;
};

enum class D {
    one,
    two,
};

template <class T, class Tuple, class = void>
constexpr bool has_make_from_tuple = false;

template <class T, class Tuple>
constexpr bool has_make_from_tuple<T, Tuple, std::void_t<decltype(std::make_from_tuple<T>(std::declval<Tuple>()))>> =
    true;

// Test std::make_from_tuple.

// reinterpret_cast, std::tuple<T>
static_assert(!has_make_from_tuple<int*, std::tuple<A*>>);
static_assert(has_make_from_tuple<A*, std::tuple<A*>>);

// reinterpret_cast, std::array<T, 1>
static_assert(!has_make_from_tuple<int*, std::array<A*, 1>>);
static_assert(has_make_from_tuple<A*, std::array<A*, 1>>);

// const_cast, std::tuple<T>
static_assert(!has_make_from_tuple<char*, std::tuple<const char*>>);
static_assert(!has_make_from_tuple<volatile char*, std::tuple<const volatile char*>>);
static_assert(has_make_from_tuple<volatile char*, std::tuple<volatile char*>>);
static_assert(has_make_from_tuple<char*, std::tuple<char*>>);
static_assert(has_make_from_tuple<const char*, std::tuple<char*>>);
static_assert(has_make_from_tuple<const volatile char*, std::tuple<volatile char*>>);

// const_cast, std::array<T, 1>
static_assert(!has_make_from_tuple<char*, std::array<const char*, 1>>);
static_assert(!has_make_from_tuple<volatile char*, std::array<const volatile char*, 1>>);
static_assert(has_make_from_tuple<volatile char*, std::array<volatile char*, 1>>);
static_assert(has_make_from_tuple<char*, std::array<char*, 1>>);
static_assert(has_make_from_tuple<const char*, std::array<char*, 1>>);
static_assert(has_make_from_tuple<const volatile char*, std::array<volatile char*, 1>>);

// static_cast, std::tuple<T>
static_assert(!has_make_from_tuple<int, std::tuple<D>>);
static_assert(!has_make_from_tuple<D, std::tuple<int>>);
static_assert(has_make_from_tuple<long, std::tuple<int>>);
static_assert(has_make_from_tuple<double, std::tuple<float>>);
static_assert(has_make_from_tuple<float, std::tuple<double>>);

// static_cast, std::array<T, 1>
static_assert(!has_make_from_tuple<int, std::array<D, 1>>);
static_assert(!has_make_from_tuple<D, std::array<int, 1>>);
static_assert(has_make_from_tuple<long, std::array<int, 1>>);
static_assert(has_make_from_tuple<double, std::array<float, 1>>);
static_assert(has_make_from_tuple<float, std::array<double, 1>>);
