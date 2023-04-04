// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <utility>

using namespace std;

#define NOEXCEPT(...)     static_assert(noexcept(__VA_ARGS__), #__VA_ARGS__ " should be noexcept")
#define NOT_NOEXCEPT(...) static_assert(!noexcept(__VA_ARGS__), #__VA_ARGS__ " should NOT be noexcept")

struct throwing_move_ctor {
    throwing_move_ctor(int) noexcept {}
    throwing_move_ctor(throwing_move_ctor&&) noexcept(false) {}
};

struct throwing_move_assign {
    throwing_move_assign(int) noexcept {}
    throwing_move_assign(throwing_move_assign&&) noexcept {}
    throwing_move_assign& operator=(throwing_move_assign&&) noexcept(false) {
        return *this;
    }
};

struct throwing_assign_sometimes {
    throwing_assign_sometimes(int) noexcept {}
    throwing_assign_sometimes(throwing_assign_sometimes&&) noexcept {}
    throwing_assign_sometimes& operator=(int&) noexcept(false) {
        return *this;
    }
    throwing_assign_sometimes& operator=(int&&) noexcept(false) {
        return *this;
    }
    throwing_assign_sometimes& operator=(double&) noexcept(false) {
        return *this;
    }
    throwing_assign_sometimes& operator=(double&&) noexcept {
        return *this;
    }
};

NOEXCEPT(exchange(declval<int&>(), 1729));
NOT_NOEXCEPT(exchange(declval<throwing_move_ctor&>(), throwing_move_ctor{1729}));
NOT_NOEXCEPT(exchange(declval<throwing_move_assign&>(), throwing_move_assign{1729}));
NOT_NOEXCEPT(exchange(declval<throwing_assign_sometimes&>(), declval<int&>()));
NOT_NOEXCEPT(exchange(declval<throwing_assign_sometimes&>(), declval<int>()));
NOT_NOEXCEPT(exchange(declval<throwing_assign_sometimes&>(), declval<double&>()));
NOEXCEPT(exchange(declval<throwing_assign_sometimes&>(), declval<double>()));
