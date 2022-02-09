// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <utility>

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
    assert(test_impl(1) == 'a');
    assert(test_impl(2) == 'z');
    return true;
}

int main() {
    test();
    static_assert(test());
    static_assert(noexcept(std::unreachable())); // strengthened
}
