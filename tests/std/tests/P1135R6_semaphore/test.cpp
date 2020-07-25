// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <semaphore>
#include <thread>


using namespace std::chrono_literals;


void test_counting_semaphore_count(const std::chrono::milliseconds delay_duration) {
    std::counting_semaphore<4> semaphore{2};
    std::atomic<int> v{0};

    auto thread_function = [&] {
        for (int i = 0; i < 3; ++i) {
            semaphore.acquire();
            v.fetch_add(1);
        }
    };

    std::thread t1{thread_function};
    std::thread t2{thread_function};
    std::thread t3{thread_function};

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
    std::binary_semaphore semaphore{1};

    std::atomic<int> v{0};

    auto thread_function = [&] {
        for (int i = 0; i < 2; ++i) {
            semaphore.acquire();
            v.fetch_add(1);
        }
    };

    std::thread t1{thread_function};
    std::thread t2{thread_function};

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

template<class Semaphore>
void test_semaphore_wait_for(const std::chrono::milliseconds delay_duration) {
    Semaphore semaphore{0};

    std::thread t([&] { 
        assert(semaphore.try_acquire_for(delay_duration / 4));
        assert(!semaphore.try_acquire_for(delay_duration * 4));
    });

    semaphore.release();

    std::this_thread::sleep_for(delay_duration);


    t.join();
}

template <class Semaphore>
void test_semaphore_wait_until(const std::chrono::milliseconds delay_duration) {
    Semaphore semaphore{0};

    std::thread t([&] {
        assert(semaphore.try_acquire_for(delay_duration / 4));
        assert(!semaphore.try_acquire_for(delay_duration * 4));
    });

    semaphore.release();

    std::this_thread::sleep_for(delay_duration);

    t.join();
}

int main() {
    constexpr auto delay_duration = std::chrono::milliseconds(200);

    test_counting_semaphore_count(delay_duration);
    test_binary_semaphore_count(delay_duration);

    test_semaphore_wait_for<std::counting_semaphore<>>(delay_duration);
    test_semaphore_wait_until<std::counting_semaphore<>>(delay_duration);
    test_semaphore_wait_for<std::binary_semaphore>(delay_duration);
    test_semaphore_wait_until<std::binary_semaphore>(delay_duration);
}
