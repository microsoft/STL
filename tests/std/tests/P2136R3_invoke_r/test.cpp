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
    auto v = invoke_r<long int>(square, 3);
    assert(v == 9L);
    assert((is_same_v<decltype(v), long int>) );

#ifdef __cpp_noexcept_function_type
    static_assert(!noexcept(invoke_r<int>(square, 3)));
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
