// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <latch>
#include <thread>

using namespace std::chrono_literals;

void test(const bool release_wait) {
    std::latch latch(5);

    std::thread t1([&] { latch.wait(); });

    std::thread t2([&] { latch.arrive_and_wait(2); });

    latch.count_down();

    if (release_wait) {
        latch.arrive_and_wait(2);
    } else {
        latch.count_down(2);
    }

    t1.join();
    t2.join();
}

int main() {
    static_assert(std::latch::max() >= 5, "latch should support some number of count downs");

    test(true);
    test(false);
}
