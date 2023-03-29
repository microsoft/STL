// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <string>
#include <tuple>
#include <utility>

int main() {
    using tuple_t = std::tuple<int, std::string>;

    const tuple_t one(1, "one");
    const tuple_t two(2, "two");

    tuple_t x = one;
    tuple_t y = two;

    assert(x == one && y == two);

    x.swap(y);

    assert(y == one && x == two);

    std::swap(x, y);

    assert(x == one && y == two);
}
