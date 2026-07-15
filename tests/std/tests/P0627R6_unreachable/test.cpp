// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <utility> // Only include <utility> to test compilation of doom function without additional includes.

constexpr char test_impl(const int arg) {
    switch (arg) {
    case 1:
        return 'a';

    case 2:
        return 'z';

    default:
        std::unreachable();
    }
}

constexpr bool test() {
    return test_impl(1) == 'a' && test_impl(2) == 'z';
}

int main() {
    if (!test()) {
        return 1;
    }
    static_assert(test());
    static_assert(noexcept(std::unreachable())); // strengthened
}
