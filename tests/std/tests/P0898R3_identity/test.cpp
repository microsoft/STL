// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <functional>
#include <vector>

using namespace std;

struct S {
    S()                    = delete;
    S(const S&)            = delete;
    S& operator=(const S&) = delete;
};

template <typename T>
using test = T::is_transparent;

int main() {
    static_assert(identity{}(42) == 42);
    static_assert(identity{}(4.2) == 4.2);

    constexpr identity i;
    assert(i(42) == 42);
    assert(i(4.2) == 4.2);
    static_assert(i(42) == 42);
    static_assert(i(4.2) == 4.2);

    static_assert(noexcept(identity{}(42)));
    static_assert(noexcept(identity{}(4.2)));

    vector<int> v = {42, 42, 42};
    assert(identity{}(v) == v);
    assert(i(v) == v);
    assert(&i(v) == &v);
    assert(noexcept(identity{}(v)));

    static_assert(is_same_v<S&, decltype(identity{}(declval<S&>()))>);
    static_assert(is_same_v<const S&, decltype(identity{}(declval<const S&>()))>);
    static_assert(is_same_v<volatile S&, decltype(identity{}(declval<volatile S&>()))>);
    static_assert(is_same_v<const volatile S&, decltype(identity{}(declval<const volatile S&>()))>);
    static_assert(is_same_v<S&&, decltype(identity{}(declval<S>()))>);
    static_assert(is_same_v<const S&&, decltype(identity{}(declval<const S>()))>);
    static_assert(is_same_v<volatile S&&, decltype(identity{}(declval<volatile S>()))>);
    static_assert(is_same_v<const volatile S&&, decltype(identity{}(declval<const volatile S>()))>);

    static_assert(is_same_v<test<identity>, test<identity>>);
}
