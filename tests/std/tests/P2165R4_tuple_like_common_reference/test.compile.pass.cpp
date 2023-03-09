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
concept CanCommonReference = requires { typename common_reference<T, U>::type; };

template <class T, class U, class Expected>
    requires CanCommonReference<T, U>
constexpr bool test_common_reference =
    same_as<common_reference_t<T, U>, Expected> && same_as<common_reference_t<U, T>, Expected>;

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

static_assert(test_common_reference<Sentinel, unreachable_sentinel_t, unreachable_sentinel_t>);

// Test common_reference<tuple, tuple>
static_assert(test_common_reference<tuple<>, tuple<>, tuple<>>);
static_assert(test_common_reference<tuple<char&&>, tuple<const int&>, tuple<int>>);
static_assert(test_common_reference<tuple<int&>, tuple<const int&>, tuple<const int&>>);
static_assert(test_common_reference<tuple<int&>, tuple<int&&>, tuple<const int&>>);
static_assert(test_common_reference<tuple<short, int, long>, tuple<long, int, short>, tuple<long, int, long>>);

// Test common_reference<cvref tuple, cvref tuple>
static_assert(test_common_reference<tuple<>&, tuple<>&&, const tuple<>&>);
static_assert(
    test_common_reference<const tuple<int, long>&, volatile tuple<int, long>&, const volatile tuple<int, long>&>);
static_assert(test_common_reference<tuple<int&, const int&&, volatile int&>&,
    const tuple<const int&, const int, const int&>, tuple<const int&, int, const volatile int&>>);

// Test common_reference<tuple, tuple-like>, tuple-like is pair
static_assert(test_common_reference<tuple<int, int>, pair<int, int>, tuple<int, int>>);
static_assert(test_common_reference<tuple<int, long>, pair<short, long long>, tuple<int, long long>>);
static_assert(
    test_common_reference<tuple<int&, long&&>, pair<const int&, const long&&>, tuple<const int&, const long&&>>);
static_assert(test_common_reference<tuple<short&, const volatile long&&>, pair<volatile int, long>, tuple<int, long>>);

// Test common_reference<cvref tuple, cvref tuple-like>, tuple-like is pair
static_assert(test_common_reference<tuple<int, int>&, pair<int, int>&, tuple<int&, int&>>);
static_assert(test_common_reference<const tuple<int, int>&, const pair<int, int>&, tuple<const int&, const int&>>);
static_assert(
    test_common_reference<tuple<int&, long&&>&, const pair<const int&, const long&&>&, tuple<const int&, const long&>>);
static_assert(test_common_reference<const tuple<short&, const volatile long&&>, const pair<volatile int, long>&,
    tuple<int, long>>);

// Test common_reference<tuple, tuple-like>, tuple-like is array
static_assert(test_common_reference<tuple<>, array<int, 0>, tuple<>>);
static_assert(test_common_reference<tuple<int>, array<int, 1>, tuple<int>>);
static_assert(test_common_reference<tuple<int, long>, array<int, 2>, tuple<int, long>>);
static_assert(test_common_reference<tuple<short, int&&, const long>, array<int, 3>, tuple<int, int, long>>);
static_assert(test_common_reference<tuple<short&, int&&, const volatile long>, array<int, 3>, tuple<int, int, long>>);

// Test common_reference<cvref tuple, cvref tuple-like>, tuple-like is array
static_assert(test_common_reference<const tuple<>&, array<int, 0>, tuple<>>);
static_assert(test_common_reference<tuple<int>&, array<int, 1>&, tuple<int&>>);
static_assert(test_common_reference<const tuple<int, long>&&, array<int, 2>&, tuple<const int&, long>>);

// Test common_reference<tuple, tuple-like>, tuple-like is ranges::subrange
static_assert(test_common_reference<tuple<int*, int*>, subrange<int*, int*>, tuple<int*, int*>>);
static_assert(
    test_common_reference<tuple<int*, const int*>, subrange<const int*, int*>, tuple<const int*, const int*>>);
static_assert(test_common_reference<tuple<int*, Sentinel>, subrange<int*, unreachable_sentinel_t>,
    tuple<int*, unreachable_sentinel_t>>);
static_assert(test_common_reference<tuple<const volatile int*, Sentinel>, subrange<int*, unreachable_sentinel_t>,
    tuple<const volatile int*, unreachable_sentinel_t>>);

// Test common_reference<cvref tuple, cvref tuple-like>, tuple-like is ranges::subrange
static_assert(test_common_reference<const tuple<int*, int*>&&, const subrange<int*, int*>, tuple<int*, int*>>);
static_assert(
    test_common_reference<const tuple<int*, const int*>&, subrange<const int*, int*>, tuple<const int*, const int*>>);

// Test invalid common_references
static_assert(!CanCommonReference<tuple<int>, array<int, 2>>);
static_assert(!CanCommonReference<tuple<int>, subrange<int*, int*>>);
static_assert(!CanCommonReference<tuple<int>, pair<int, int>>);
