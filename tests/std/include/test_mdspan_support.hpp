// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <concepts>
#include <utility>

enum class IsNothrow : bool { no, yes };

template <class Int, IsNothrow Nothrow = IsNothrow::yes>
struct ConvertibleToInt {
    constexpr operator Int() const noexcept(std::to_underlying(Nothrow)) {
        return Int{1};
    }
};

struct NonConvertibleToAnything {};

template <class T>
constexpr void check_implicit_conversion(T); // not defined

// clang-format off
template <class T, class... Args>
concept NotImplicitlyConstructibleFrom =
    std::constructible_from<T, Args...>
    && !requires(Args&&... args) { check_implicit_conversion<T>({std::forward<Args>(args)...}); };
// clang-format on
