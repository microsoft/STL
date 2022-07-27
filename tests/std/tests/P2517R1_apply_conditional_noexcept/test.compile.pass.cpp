// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <string>
#include <tuple>
#include <type_traits>

using namespace std;

struct NoexceptIf {
    template <bool b, class... Ts>
    void operator()(std::bool_constant<b>, Ts...) const noexcept(b);
};

constexpr static NoexceptIf noexcept_if;

static_assert(noexcept(apply(noexcept_if, forward_as_tuple(true_type{}))));
static_assert(!noexcept(apply(noexcept_if, forward_as_tuple(false_type{}))));

static_assert(noexcept(apply(noexcept_if, forward_as_tuple(true_type{}, 0))));
static_assert(noexcept(apply(noexcept_if, forward_as_tuple(true_type{}, string{}))));
static_assert(!noexcept(apply(noexcept_if, forward_as_tuple(true_type{}, static_cast<const string&>(string{})))));
