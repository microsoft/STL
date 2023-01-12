// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <compare>
#include <concepts>
#include <memory>

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
bool attempted_equals(const T&, const U& u);

template <class T, class U>
    requires common_reference_with<const remove_reference_t<T>&, const remove_reference_t<U>&>
bool attempted_equals(const T& t, const U& u);
// clang-format on

template <class T>
bool test(T val) {
    return attempted_equals(val, nullptr); // ill-formed; previously well-formed
}

template <class T>
constexpr bool diff_cpp20_concepts = requires(T val) { test(val); };

#if _HAS_CXX23
static_assert(!diff_cpp20_concepts<unique_ptr<int>>);
#else // ^^^ C++23 / C++20 vvv
static_assert(diff_cpp20_concepts<unique_ptr<int>>);
#endif // C++20

int main() {} // COMPILE-ONLY
