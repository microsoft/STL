// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <concepts>
#include <iterator>

// Defend against regression of DevCom-10532126, in which several function templates used
// `_Iter_diff_t<meow>` as a parameter type instead of the specified
// `typename iterator_traits<meow>::difference_type`. The two are equivalent in C++17, but in C++20
// _Iter_diff_t<meow> becomes iter_difference_t<meow>. We thought the difference was not observable,
// but it interferes with concept overloading.

using std::iter_value_t, std::iterator_traits, std::same_as;
using std::next, std::prev, std::shift_left, std::shift_right;

struct meow {};

constexpr meow* nil = nullptr;

template <class I>
concept Meowerator = same_as<iter_value_t<I>, meow>;

template <Meowerator I>
void next(I, typename iterator_traits<I>::difference_type = 1) {}

template <Meowerator I>
void prev(I, typename iterator_traits<I>::difference_type = 1) {}

template <Meowerator I>
void shift_left(I, I, typename iterator_traits<I>::difference_type) {}
template <Meowerator I>
void shift_right(I, I, typename iterator_traits<I>::difference_type) {}
// Note that we intentionally do not test the ExecutionPolicy overloads of shift_meow. They are
// constrained via an unspecified mechanism to "not participate in overload resolution unless
// is_execution_policy_v<ExecutionPolicy> is true", effectively making concept overloading
// impossible (or at least non-portable).

static_assert(same_as<void, decltype(next(nil))>);
static_assert(same_as<void, decltype(next(nil, 42))>);

static_assert(same_as<void, decltype(prev(nil))>);
static_assert(same_as<void, decltype(prev(nil, 42))>);

static_assert(same_as<void, decltype(shift_left(nil, nil, 42))>);
static_assert(same_as<void, decltype(shift_right(nil, nil, 42))>);
