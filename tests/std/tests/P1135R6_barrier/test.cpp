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

void test_with_functor() {
    std::atomic<int> c{0};
    int called_times = 0;

    struct Functor {
        void operator()() {
            switch (++(*called_times)) {
            case 1:
                assert(c->load(std::memory_order_relaxed) == 0);
                break;
            case 2:
                assert(c->load(std::memory_order_relaxed) == 2);
                break;
            case 3:
                assert(c->load(std::memory_order_relaxed) == 4);
                break;
            case 4:
                assert(c->load(std::memory_order_relaxed) == 6);
                break;
            case 5:
                assert(c->load(std::memory_order_relaxed) == 7);
                break;
            default:
                assert(false);
                break;
            }
        }

        int* called_times;
        std::atomic<int>* c;
    } f = {&called_times, &c};

    std::barrier barrier(2, f);

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
    assert(called_times == 5);
}

int main() {
    static_assert(std::barrier<>::max() >= 5, "latch should support some number of arrivals");

    test();
    test_with_functor();
}
