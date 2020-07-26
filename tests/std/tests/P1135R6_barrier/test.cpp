// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <barrier>
#include <cassert>
#include <chrono>
#include <thread>

void test() {
    std::barrier barrier(2);

    std::atomic<int> c{0};

    std::thread t1([&] {
        for (int i = 0; i < 5; i++) {
            auto token = barrier.arrive();
            barrier.wait(std::move(token));
            c.fetch_add(1, std::memory_order_relaxed);
        }
    });

    std::thread t2([&] {
        for (int i = 0; i < 3; i++) {
            barrier.arrive_and_wait();
            c.fetch_add(1, std::memory_order_relaxed);
        }
        barrier.arrive_and_drop();
    });

    t1.join();
    t2.join();

    assert(c.load(std::memory_order_relaxed) == 8);
}


int main() {
    test();
}
