// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _ENABLE_ATOMIC_ALIGNMENT_FIX

#include <atomic>
#include <cassert>
#include <chrono>
#include <string.h>
#include <thread>

template <class UnderlyingType>
void test_atomic_wait_func(const UnderlyingType old_value, const UnderlyingType new_value,
    const std::chrono::steady_clock::duration waiting_duration) {

    constexpr int seq_max_size = 10;
    char seq[seq_max_size + 1];
    std::atomic<char*> base = seq;
    auto add_seq            = [&](char ch) {
        char* p = base.fetch_add(1, std::memory_order_relaxed);
        assert(p - seq < seq_max_size);
        *p = ch;
    };

    std::atomic<UnderlyingType> a{old_value};
    a.wait(new_value);

    add_seq('1');

    std::thread thd([&] {
        std::this_thread::sleep_for(waiting_duration);
        add_seq('2');
        a.notify_all();
        std::this_thread::sleep_for(waiting_duration);
        add_seq('3');
        a.store(old_value);
        a.notify_one();
        std::this_thread::sleep_for(waiting_duration);
        add_seq('4');
        a.store(new_value);
        a.notify_one();
        std::this_thread::sleep_for(waiting_duration);
        add_seq('6');
    });

    a.wait(old_value);
    assert(a.load() == new_value);

    add_seq('5');

    thd.join();

    add_seq('\0');
    assert(strcmp(seq, "123456") == 0);
}

int main() {
    constexpr std::chrono::milliseconds waiting_duration{200};
    test_atomic_wait_func<char>(1, 2, waiting_duration);
    test_atomic_wait_func<signed char>(1, 2, waiting_duration);
    test_atomic_wait_func<unsigned char>(1, 2, waiting_duration);
    test_atomic_wait_func<short>(1, 2, waiting_duration);
    test_atomic_wait_func<unsigned short>(1, 2, waiting_duration);
    test_atomic_wait_func<int>(1, 2, waiting_duration);
    test_atomic_wait_func<unsigned int>(1, 2, waiting_duration);
    test_atomic_wait_func<long>(1, 2, waiting_duration);
    test_atomic_wait_func<unsigned long>(1, 2, waiting_duration);
    test_atomic_wait_func<long long>(1, 2, waiting_duration);
    test_atomic_wait_func<unsigned long long>(1, 2, waiting_duration);
    test_atomic_wait_func<float>(1, 2, waiting_duration);
    test_atomic_wait_func<double>(1, 2, waiting_duration);
    test_atomic_wait_func<long double>(1, 2, waiting_duration);

    test_atomic_wait_func<const void*>("1", "2", waiting_duration);

    struct two_shorts {
        short a;
        short b;

        _NODISCARD bool operator==(const two_shorts& other) const {
            return a == other.a && b == other.b;
        }
    };

    test_atomic_wait_func<two_shorts>({1, 1}, {1, 2}, waiting_duration);

    struct three_chars {
        char a;
        char b;
        char c;

        _NODISCARD bool operator==(const three_chars& other) const {
            return a == other.a && b == other.b && c == other.c;
        }
    };

    test_atomic_wait_func<three_chars>({1, 1, 3}, {1, 2, 3}, waiting_duration);
}
