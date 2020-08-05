// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <latch>
#include <thread>

void test(const bool release_wait) {
    std::latch l(5);

    std::thread t1([&] { l.wait(); });

    std::thread t2([&] { l.arrive_and_wait(2); });

    l.count_down();

    if (release_wait) {
        l.arrive_and_wait(2);
    } else {
        l.count_down(2);
    }

    t1.join();
    t2.join();
}

int main() {
    static_assert(std::latch::max() >= 5, "latch should support some number of count downs");

    test(true);
    test(false);
}
