// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <latch>
#include <semaphore>
#include <thread>


using namespace std::chrono_literals;


void test_counting_semaphore_count(const std::chrono::milliseconds delay_duration) {
    std::latch start{4};

    std::counting_semaphore<4> semaphore{2};
    std::atomic<int> v{0};

    auto thread_function = [&] {
        start.arrive_and_wait();
        for (int i = 0; i < 3; ++i) {
            semaphore.acquire();
            v.fetch_add(1);
        }
    };

    std::thread t1{thread_function};
    std::thread t2{thread_function};
    std::thread t3{thread_function};

    start.arrive_and_wait();

    std::this_thread::sleep_for(delay_duration);

    assert(v.load() == 2);

    semaphore.release();

    std::this_thread::sleep_for(delay_duration);

    assert(v.load() == 3);

    semaphore.release(4);

    std::this_thread::sleep_for(delay_duration);

    assert(v.load() == 7);

    semaphore.release(4);

    std::this_thread::sleep_for(delay_duration);

    assert(v.load() == 9);

    t1.join();
    t2.join();
    t3.join();
}

void test_binary_semaphore_count(const std::chrono::milliseconds delay_duration) {
    std::latch start{3};

    std::binary_semaphore semaphore{1};

    std::atomic<int> v{0};

    auto thread_function = [&] {
        start.arrive_and_wait();
        for (int i = 0; i < 2; ++i) {
            semaphore.acquire();
            v.fetch_add(1);
        }
    };

    std::thread t1{thread_function};
    std::thread t2{thread_function};

    start.arrive_and_wait();

    std::this_thread::sleep_for(delay_duration);
    assert(v.load() == 1);

    semaphore.release();
    std::this_thread::sleep_for(delay_duration);
    assert(v.load() == 2);

    semaphore.release();
    std::this_thread::sleep_for(delay_duration);
    assert(v.load() == 3);

    semaphore.release();
    std::this_thread::sleep_for(delay_duration);
    assert(v.load() == 4);

    semaphore.release();
    std::this_thread::sleep_for(delay_duration);
    assert(v.load() == 4);

    t1.join();
    t2.join();
}

template <class Semaphore>
void test_semaphore_wait_for(const std::chrono::milliseconds delay_duration) {
    std::latch start{2};

    Semaphore semaphore{0};

    std::thread t([&] {
        start.arrive_and_wait();

        assert(semaphore.try_acquire_for(delay_duration));
        assert(!semaphore.try_acquire_for(delay_duration * 16));
    });

    start.arrive_and_wait();

    semaphore.release();

    std::this_thread::sleep_for(delay_duration * 4);

    t.join();
}

template <class Semaphore>
void test_semaphore_wait_until(const std::chrono::milliseconds delay_duration) {
    std::latch start{2};

    Semaphore semaphore{0};

    std::thread t([&] {
        start.arrive_and_wait();

        assert(semaphore.try_acquire_until(std::chrono::steady_clock::now() + delay_duration));
        assert(!semaphore.try_acquire_until(std::chrono::steady_clock::now() + delay_duration * 8));
    });

    start.arrive_and_wait();

    semaphore.release();

    std::this_thread::sleep_for(delay_duration * 4);

    t.join();
}

#include <iostream>

int main() {
    constexpr auto max = std::numeric_limits<std::ptrdiff_t>::max();

    static_assert(std::counting_semaphore<max>::max() >= max, "semahpore should support some number of count downs");
    static_assert(std::counting_semaphore<5>::max() >= 5, "semahpore should support some number of count downs");
    static_assert(std::binary_semaphore::max() >= 1, "semahpore should support some number of count downs");

    constexpr auto delay_duration = std::chrono::milliseconds(200);

    test_counting_semaphore_count(delay_duration);
    test_binary_semaphore_count(delay_duration);

    test_semaphore_wait_for<std::counting_semaphore<>>(delay_duration);
    test_semaphore_wait_until<std::counting_semaphore<>>(delay_duration);
    test_semaphore_wait_for<std::binary_semaphore>(delay_duration);
    test_semaphore_wait_until<std::binary_semaphore>(delay_duration);
}
