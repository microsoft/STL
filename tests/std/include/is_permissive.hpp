// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <yvals_core.h>

namespace detail {
    constexpr bool permissive() {
        return false;
    }

    template <class>
    struct PermissiveTestBase {
        static constexpr bool permissive() {
            return true;
        }
    };

    template <class T>
    struct PermissiveTest : PermissiveTestBase<T> {
        static constexpr bool test() {
            return permissive();
        }
    };
} // namespace detail

template <class T>
constexpr bool is_permissive_v = detail::PermissiveTest<T>::test();

_INLINE_VAR constexpr bool is_permissive = is_permissive_v<int>;
