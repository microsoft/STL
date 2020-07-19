// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <latch>
#include <thread>

using namespace std::chrono_literals;

void test(const bool release_wait) {
    std::latch latch(5);

    std::atomic<int> c{0};

    std::thread t1([&] {
        latch.wait();
        c.fetch_add(1, std::memory_order_relaxed);
    });

    std::thread t2([&] {
        latch.arrive_and_wait(2);
        c.fetch_add(1, std::memory_order_relaxed);
    });

    latch.count_down();

    std::this_thread::sleep_for(200ms);

    assert(c.load(std::memory_order_relaxed) == 0);

    if (release_wait) {
        latch.arrive_and_wait(2);
    } else {
        latch.count_down(2);
    }

    std::this_thread::sleep_for(200ms);

    assert(c.load(std::memory_order_relaxed) == 2);

    t1.join();
    t2.join();
}

int main() {
    static_assert(std::latch::max() > 0);

    test(true);
    test(false);
}
