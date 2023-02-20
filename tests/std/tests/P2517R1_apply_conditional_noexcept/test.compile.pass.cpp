// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

using namespace std;

struct NoexceptIf {
    template <bool B, class... Ts>
    void operator()(bool_constant<B>, Ts...) const noexcept(B);
};

static constexpr NoexceptIf noexcept_if;

static_assert(noexcept(apply(noexcept_if, forward_as_tuple(true_type{}))));
static_assert(!noexcept(apply(noexcept_if, forward_as_tuple(false_type{}))));

static_assert(noexcept(apply(noexcept_if, forward_as_tuple(true_type{}, 0))));
static_assert(noexcept(apply(noexcept_if, forward_as_tuple(true_type{}, string{}))));
static_assert(!noexcept(apply(noexcept_if, forward_as_tuple(true_type{}, declval<const string&>()))));
