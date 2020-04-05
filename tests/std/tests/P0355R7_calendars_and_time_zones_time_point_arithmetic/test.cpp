// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>

using namespace std::chrono;

constexpr bool test() {
    steady_clock::time_point tp1;
    auto tp2 = tp1++;
    assert(tp1.time_since_epoch().count() == 1);
    assert(tp2.time_since_epoch().count() == 0);
    tp2 = ++tp1;
    assert(tp1.time_since_epoch().count() == 2);
    assert(tp2.time_since_epoch().count() == 2);

    tp2 = tp1--;
    assert(tp1.time_since_epoch().count() == 1);
    assert(tp2.time_since_epoch().count() == 2);
    tp2 = --tp1;
    assert(tp1.time_since_epoch().count() == 0);
    assert(tp2.time_since_epoch().count() == 0);

    return true;
}

int main() {
    test();
    static_assert(test());
}
