// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <utility>

constexpr int test_impl(int arg) {
    switch (arg) {
    case 1:
        return 'x';

    case 2:
        return 'y';

    default:
        std::unreachable();
    }
}

constexpr bool test() {
    assert(test_impl(1) == 'x');
    assert(test_impl(2) == 'y');
    return true;
}

int main() {
    test();
    static_assert(test());
    static_assert(noexcept(std::unreachable())); // strengthened
}
