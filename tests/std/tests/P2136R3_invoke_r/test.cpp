// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <functional>
#include <type_traits>

using namespace std;

constexpr int square(int n) {
    return n * n;
}
constexpr int square_noexcept(int n) noexcept {
    return n * n;
}

struct Thing {
    int n = 0;
    constexpr int& foo() {
        return n;
    }
};

constexpr bool is_rvalue(const int&) {
    return false;
}
constexpr bool is_rvalue(int&&) {
    return true;
}

constexpr bool test_invoke_r() {
    auto v1 = invoke_r<long int>(square, 3);
    assert(v1 == 9L);
    assert((is_same_v<decltype(v1), long int>) );

    auto v2 = invoke_r<double>([]() -> int { return 5; });
    assert(v2 == 5);
    assert((is_same_v<decltype(v2), double>) );

    static_assert(is_void_v<decltype(invoke_r<void>(square, 1))>);
    invoke_r<void>([] { return 1; }); // no nodiscard warning

#ifdef __cpp_noexcept_function_type
    static_assert(!noexcept(invoke_r<int>(square, 3))); // fails /permissive
    static_assert(noexcept(invoke_r<int>(square_noexcept, 3)));
#endif

    assert(!is_rvalue(invoke(&Thing::foo, Thing{})));
    assert(is_rvalue(invoke_r<int>(&Thing::foo, Thing{})));

    return true;
}

int main() {
    test_invoke_r();
    static_assert(test_invoke_r());
}
