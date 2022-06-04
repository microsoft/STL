// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <latch>
#include <limits>
#include <semaphore>
#include <thread>

using namespace std::chrono_literals;

void wait_and_expect(std::atomic<int>& v, const int val, const std::chrono::milliseconds delay_duration) {
#ifdef CAN_FAIL_ON_TIMING_ASSUMPTION
    std::this_thread::sleep_for(delay_duration);
    assert(v.load() == val);
#else // ^^^ CAN_FAIL_ON_TIMING_ASSUMPTION / !CAN_FAIL_ON_TIMING_ASSUMPTION vvv
    while (v.load() < val) {
        std::this_thread::sleep_for(delay_duration);
    }
    assert(v.load() == val);
#endif // ^^^ !CAN_FAIL_ON_TIMING_ASSUMPTION ^^^
}

void test_counting_semaphore_count(const std::chrono::milliseconds delay_duration) {
    std::latch start{4};

    std::counting_semaphore<4> s{2};
    std::atomic<int> v{0};

    auto thread_function = [&] {
        start.arrive_and_wait();
        for (int i = 0; i < 3; ++i) {
            s.acquire();
            v.fetch_add(1);
        }
    };

    std::thread t1{thread_function};
    std::thread t2{thread_function};
    std::thread t3{thread_function};

    start.arrive_and_wait();

    wait_and_expect(v, 2, delay_duration);

    s.release();

    wait_and_expect(v, 3, delay_duration);

    s.release(4);

    wait_and_expect(v, 7, delay_duration);

    s.release(4);

    wait_and_expect(v, 9, delay_duration);

    t1.join();
    t2.join();
    t3.join();
}

void test_binary_semaphore_count(const std::chrono::milliseconds delay_duration) {
    std::latch start{3};

    std::binary_semaphore s{1};

    std::atomic<int> v{0};

    auto thread_function = [&] {
        start.arrive_and_wait();
        for (int i = 0; i < 2; ++i) {
            s.acquire();
            v.fetch_add(1);
        }
    };

    std::thread t1{thread_function};
    std::thread t2{thread_function};

    start.arrive_and_wait();

    wait_and_expect(v, 1, delay_duration);

    s.release();
    wait_and_expect(v, 2, delay_duration);

    s.release();
    wait_and_expect(v, 3, delay_duration);

    s.release();
    wait_and_expect(v, 4, delay_duration);

    s.release();
    wait_and_expect(v, 4, delay_duration);

    t1.join();
    t2.join();
}

template <class Semaphore>
void test_semaphore_wait_for(const std::chrono::milliseconds delay_duration) {
    std::latch start{2};

    Semaphore s{0};

    std::thread t([&] {
        start.arrive_and_wait();

        assert(s.try_acquire_for(delay_duration));
        assert(!s.try_acquire_for(delay_duration * 16));
    });

    start.arrive_and_wait();

    s.release();

    std::this_thread::sleep_for(delay_duration * 4);

    t.join();
}

template <class Semaphore>
void test_semaphore_wait_until(const std::chrono::milliseconds delay_duration) {
    std::latch start{2};

    Semaphore s{0};

    std::thread t([&] {
        start.arrive_and_wait();

        assert(s.try_acquire_until(std::chrono::steady_clock::now() + delay_duration));
        assert(!s.try_acquire_until(std::chrono::steady_clock::now() + delay_duration * 8));
    });

    start.arrive_and_wait();

    s.release();

    std::this_thread::sleep_for(delay_duration * 4);

    t.join();
}

int main() {
    constexpr auto max = std::numeric_limits<std::ptrdiff_t>::max();

    static_assert(std::counting_semaphore<max>::max() >= max, "semaphore should support some number of count downs");
    static_assert(std::counting_semaphore<5>::max() >= 5, "semaphore should support some number of count downs");
    static_assert(std::binary_semaphore::max() >= 1, "semaphore should support some number of count downs");

    constexpr auto delay_duration = 200ms;

    test_counting_semaphore_count(delay_duration);
    test_binary_semaphore_count(delay_duration);

#ifdef CAN_FAIL_ON_TIMING_ASSUMPTION
    test_semaphore_wait_for<std::counting_semaphore<>>(delay_duration);
    test_semaphore_wait_until<std::counting_semaphore<>>(delay_duration);
    test_semaphore_wait_for<std::binary_semaphore>(delay_duration);
    test_semaphore_wait_until<std::binary_semaphore>(delay_duration);
#endif // CAN_FAIL_ON_TIMING_ASSUMPTION
}
