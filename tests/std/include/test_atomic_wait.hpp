// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <assert.h>
#include <atomic>
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
        // timing assumption that the main thread evaluates the `wait(old_value)` before this timeout expires
        std::this_thread::sleep_for(waiting_duration);
        add_seq('6');
    });

    a.wait(old_value);
    const auto loaded = a.load();
    assert(memcmp(&loaded, &new_value, sizeof(UnderlyingType)) == 0);

    add_seq('5');

    thd.join();

    add_seq('\0');
    assert(strcmp(seq, "123456") == 0);
}

template <class UnderlyingType>
void test_notify_all_notifies_all(const UnderlyingType old_value, const UnderlyingType new_value,
    const std::chrono::steady_clock::duration waiting_duration) {
    std::atomic<UnderlyingType> c{old_value};
    const auto waitFn = [&c, old_value] { c.wait(old_value); };

    std::thread w1{waitFn};
    std::thread w2{waitFn};
    std::thread w3{waitFn};

    std::this_thread::sleep_for(waiting_duration);
    c.store(new_value);
    c.notify_all(); // if this doesn't really notify all, the following joins will deadlock

    w1.join();
    w2.join();
    w3.join();
}

struct two_shorts {
    short a;
    short b;

    friend bool operator==(two_shorts, two_shorts) = delete;
    friend bool operator!=(two_shorts, two_shorts) = delete;
};

struct three_chars {
    char a;
    char b;
    char c;

    friend bool operator==(three_chars, three_chars) = delete;
    friend bool operator!=(three_chars, three_chars) = delete;
};

struct big_char_like {
    char value;
    char unused[16];

    explicit big_char_like(char value_) : value(value_), unused{} {}

    big_char_like(const big_char_like&) = default;
    big_char_like& operator=(const big_char_like&) = default;

    friend bool operator==(big_char_like, big_char_like) = delete;
    friend bool operator!=(big_char_like, big_char_like) = delete;
};

inline void test_atomic_wait() {
    // wait for all the threads to be waiting; if this value is too small the test might be ineffective but should not
    // fail due to timing assumptions except where otherwise noted; if it is too large the test will only take longer
    // than necessary
    constexpr std::chrono::milliseconds waiting_duration{100};
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
    test_atomic_wait_func(two_shorts{1, 1}, two_shorts{1, 2}, waiting_duration);
    test_atomic_wait_func(three_chars{1, 1, 3}, three_chars{1, 2, 3}, waiting_duration);
    test_atomic_wait_func(big_char_like{'a'}, big_char_like{'b'}, waiting_duration);

    test_notify_all_notifies_all<char>(1, 2, waiting_duration);
    test_notify_all_notifies_all<signed char>(1, 2, waiting_duration);
    test_notify_all_notifies_all<unsigned char>(1, 2, waiting_duration);
    test_notify_all_notifies_all<short>(1, 2, waiting_duration);
    test_notify_all_notifies_all<unsigned short>(1, 2, waiting_duration);
    test_notify_all_notifies_all<int>(1, 2, waiting_duration);
    test_notify_all_notifies_all<unsigned int>(1, 2, waiting_duration);
    test_notify_all_notifies_all<long>(1, 2, waiting_duration);
    test_notify_all_notifies_all<unsigned long>(1, 2, waiting_duration);
    test_notify_all_notifies_all<long long>(1, 2, waiting_duration);
    test_notify_all_notifies_all<unsigned long long>(1, 2, waiting_duration);
    test_notify_all_notifies_all<float>(1, 2, waiting_duration);
    test_notify_all_notifies_all<double>(1, 2, waiting_duration);
    test_notify_all_notifies_all<long double>(1, 2, waiting_duration);
    test_notify_all_notifies_all<const void*>("1", "2", waiting_duration);
    test_notify_all_notifies_all(two_shorts{1, 1}, two_shorts{1, 2}, waiting_duration);
    test_notify_all_notifies_all(three_chars{1, 1, 3}, three_chars{1, 2, 3}, waiting_duration);
    test_notify_all_notifies_all(big_char_like{'a'}, big_char_like{'b'}, waiting_duration);
}
