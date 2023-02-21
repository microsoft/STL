// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <compare>
#include <concepts>
#include <cstddef>
#include <memory>
#include <type_traits>

using namespace std;

struct MoveOnly {
    MoveOnly(int); // not defined
    MoveOnly(const MoveOnly&)            = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;
    MoveOnly(MoveOnly&&)                 = default;
    MoveOnly& operator=(MoveOnly&&)      = default;

    strong_ordering operator<=>(const MoveOnly&) const = default;
    strong_ordering operator<=>(int) const; // not defined
};

// Check three_way_comparable_with
static_assert(three_way_comparable_with<MoveOnly, int>);
static_assert(three_way_comparable_with<int, MoveOnly>);
static_assert(three_way_comparable_with<MoveOnly, int, strong_ordering>);
static_assert(three_way_comparable_with<int, MoveOnly, strong_ordering>);

// Check equality_comparable_with
static_assert(equality_comparable_with<MoveOnly, int>);
static_assert(equality_comparable_with<int, MoveOnly>);
static_assert(equality_comparable_with<unique_ptr<int>, nullptr_t>);
static_assert(equality_comparable_with<nullptr_t, unique_ptr<int>>);

// Check totally_ordered_with
static_assert(totally_ordered_with<MoveOnly, int>);
static_assert(totally_ordered_with<int, MoveOnly>);

// Check [diff.cpp20.concepts]
// clang-format off
template <class T, class U>
    requires equality_comparable_with<T, U>
bool attempted_equals(const T&, const U&); // not defined

template <class T, class U>
    requires common_reference_with<const remove_reference_t<T>&, const remove_reference_t<U>&>
bool attempted_equals(const T&, const U&); // not defined
// clang-format on

template <class T>
constexpr bool check_diff_cpp20_concepts = !requires(T p) { attempted_equals(p, nullptr); };

static_assert(check_diff_cpp20_concepts<shared_ptr<int>>);
