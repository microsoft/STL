// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <utility>

using namespace std;
using ranges::subrange;

template <class T, class U>
concept CanCommonType = requires { typename common_type<T, U>::type; };

template <class T, class U, class Expected>
    requires CanCommonType<T, U>
inline constexpr bool test_common_type =
    same_as<common_type_t<T, U>, Expected> && same_as<common_type_t<U, T>, Expected>;

struct Sentinel {
    bool operator==(const auto&) const; // not defined
    operator unreachable_sentinel_t() const; // not defined
};

template <>
struct std::common_type<Sentinel, unreachable_sentinel_t> {
    using type = unreachable_sentinel_t;
};

template <>
struct std::common_type<unreachable_sentinel_t, Sentinel> {
    using type = unreachable_sentinel_t;
};

static_assert(test_common_type<Sentinel, unreachable_sentinel_t, unreachable_sentinel_t>);

// Test common_type<tuple, tuple>
static_assert(test_common_type<tuple<>, tuple<>, tuple<>>);
static_assert(test_common_type<tuple<char>, tuple<int>, tuple<int>>);
static_assert(test_common_type<tuple<int&>, tuple<const int&>, tuple<int>>);
static_assert(test_common_type<tuple<int&>, tuple<int&&>, tuple<int>>);
static_assert(test_common_type<tuple<short, long, long>, tuple<long, int, short>, tuple<long, long, long>>);

// Test common_reference<cvref tuple, cvref tuple>
static_assert(test_common_type<const tuple<>&, tuple<>&&, tuple<>>);
static_assert(test_common_type<tuple<char>&, const tuple<int>&&, tuple<int>>);
static_assert(test_common_type<tuple<int&>&, tuple<const int&>, tuple<int>>);
static_assert(test_common_type<volatile tuple<int&>&, tuple<int&&>, tuple<int>>);

// Test common_type<tuple, tuple-like>, tuple-like is pair
static_assert(test_common_type<tuple<int, int>, pair<int, int>, tuple<int, int>>);
static_assert(test_common_type<tuple<int, long>, pair<short, long long>, tuple<int, long long>>);
static_assert(test_common_type<tuple<int&, long&&>, pair<const int&, const long&&>, tuple<int, long>>);
static_assert(test_common_type<tuple<short&, const volatile long&&>, pair<volatile int, long>, tuple<int, long>>);

// Test common_type<cvref tuple, cvref tuple-like>, tuple-like is pair
static_assert(test_common_type<const tuple<int, int>&&, volatile pair<int, int>&, tuple<int, int>>);
static_assert(test_common_type<volatile tuple<int, long>&&, const pair<short, long long>&&, tuple<int, long long>>);

// Test common_type<tuple, tuple-like>, tuple-like is array
static_assert(test_common_type<tuple<>, array<int, 0>, tuple<>>);
static_assert(test_common_type<tuple<int>, array<int, 1>, tuple<int>>);
static_assert(test_common_type<tuple<int, long>, array<int, 2>, tuple<int, long>>);
static_assert(test_common_type<tuple<short, int, long>, array<int, 3>, tuple<int, int, long>>);
static_assert(test_common_type<tuple<short&, int&&, const volatile long>, array<int, 3>, tuple<int, int, long>>);

// Test common_type<cvref tuple, cvref tuple-like>, tuple-like is array
static_assert(test_common_type<const tuple<>&&, volatile array<int, 0>&, tuple<>>);
static_assert(test_common_type<const tuple<int>&, volatile array<int, 1>&&, tuple<int>>);

// Test common_type<tuple, tuple-like>, tuple-like is ranges::subrange
static_assert(test_common_type<tuple<int*, int*>, subrange<int*, int*>, tuple<int*, int*>>);
static_assert(test_common_type<tuple<int*, const int*>, subrange<const int*, int*>, tuple<const int*, const int*>>);
static_assert(test_common_type<tuple<int*, Sentinel>, subrange<int*, unreachable_sentinel_t>,
    tuple<int*, unreachable_sentinel_t>>);
static_assert(test_common_type<tuple<const volatile int*, Sentinel>, subrange<int*, unreachable_sentinel_t>,
    tuple<const volatile int*, unreachable_sentinel_t>>);

// Test common_type<cvref tuple, cvref tuple-like>, tuple-like is ranges::subrange
static_assert(test_common_type<const tuple<int*, int*>&, volatile subrange<int*, int*>&&, tuple<int*, int*>>);
static_assert(test_common_type<volatile tuple<int*, Sentinel>&, const subrange<int*, unreachable_sentinel_t>&&,
    tuple<int*, unreachable_sentinel_t>>);

// Test invalid common_types
static_assert(!CanCommonType<tuple<int>, array<int, 2>>);
static_assert(!CanCommonType<tuple<int>, subrange<int*, int*>>);
static_assert(!CanCommonType<tuple<int>, pair<int, int>>);
