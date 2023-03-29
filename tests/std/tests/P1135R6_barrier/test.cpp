// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <barrier>
#include <cassert>
#include <thread>
#include <utility>

void test() {
    std::barrier b(2);

    std::atomic<int> c{0};

    std::thread t1([&] {
        for (int i = 0; i < 5; ++i) {
            auto token = b.arrive();
            b.wait(std::move(token));
            c.fetch_add(1, std::memory_order_relaxed);
        }
    });

    std::thread t2([&] {
        for (int i = 0; i < 3; ++i) {
            b.arrive_and_wait();
            c.fetch_add(1, std::memory_order_relaxed);
        }
        b.arrive_and_drop();
    });

    t1.join();
    t2.join();

    assert(c.load(std::memory_order_relaxed) == 8);
}

void test_with_functor() {
    std::atomic<int> c{0};
    std::atomic<int> called_times{0};

    struct Functor {
        void operator()() noexcept {
            switch (called_times->fetch_add(1, std::memory_order_relaxed) + 1) {
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

        std::atomic<int>* called_times;
        std::atomic<int>* c;
    } f = {&called_times, &c};

    std::barrier b(2, f);

    std::thread t1([&] {
        for (int i = 0; i < 5; ++i) {
            auto token = b.arrive();
            b.wait(std::move(token));
            c.fetch_add(1, std::memory_order_relaxed);
        }
    });

    std::thread t2([&] {
        for (int i = 0; i < 3; ++i) {
            b.arrive_and_wait();
            c.fetch_add(1, std::memory_order_relaxed);
        }
        b.arrive_and_drop();
    });

    t1.join();
    t2.join();

    assert(c.load(std::memory_order_relaxed) == 8);
    assert(called_times.load(std::memory_order_relaxed) == 5);
}


void test_token() {
    std::atomic<int> called_times{0};

    auto f = [&]() noexcept { called_times.fetch_add(1, std::memory_order_relaxed); };

    std::barrier b(2, f);
    auto t1 = b.arrive();
    auto t2 = std::move(t1);

    assert(called_times.load(std::memory_order_relaxed) == 0);
    auto t3 = b.arrive();
    auto t4 = std::move(t3);

    assert(called_times.load(std::memory_order_relaxed) == 1);
    b.wait(std::move(t4));
    assert(called_times.load(std::memory_order_relaxed) == 1);
    b.wait(std::move(t2));
    assert(called_times.load(std::memory_order_relaxed) == 1);
}

void barrier_callback_function() noexcept {}

void test_functor_types() {
    struct f1 {
        void operator()() noexcept {}

        f1(int, int, int) {}

        f1(f1&&) noexcept   = default;
        f1& operator=(f1&&) = delete;
    };
    std::barrier b1{1, f1{0, 0, 0}};
    b1.arrive_and_wait();

    std::barrier b2{1, barrier_callback_function};
    b2.arrive_and_wait();

    std::barrier b3{1, []() noexcept {}};
    b3.arrive_and_wait();
}

int main() {
    static_assert(std::barrier<>::max() >= 5, "barrier should support some number of arrivals");

    test();
    test_with_functor();
    test_token();
    test_functor_types();
}
