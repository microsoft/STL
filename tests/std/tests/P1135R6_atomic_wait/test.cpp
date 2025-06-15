// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <atomic>
#include <cassert>
#include <chrono>
#include <cstring>
#include <memory>
#include <thread>

template <template <class> class Template, class UnderlyingType>
void test_atomic_wait_func_impl(UnderlyingType& old_value, const UnderlyingType new_value,
    const std::chrono::steady_clock::duration waiting_duration) {
    constexpr int seq_max_size = 10;
    char seq[seq_max_size + 1];
    std::atomic<char*> base = seq;
    auto add_seq            = [&](char ch) {
        char* p = base.fetch_add(1, std::memory_order_relaxed);
        assert(p - seq < seq_max_size);
        *p = ch;
    };

    Template<UnderlyingType> a(old_value);
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
#ifdef CAN_FAIL_ON_TIMING_ASSUMPTION
        // timing assumption that the main thread evaluates the `wait(old_value)` before this timeout expires
        std::this_thread::sleep_for(waiting_duration);
        add_seq('6');
#endif
    });

    a.wait(old_value);
    const auto loaded = a.load();
    assert(memcmp(&loaded, &new_value, sizeof(UnderlyingType)) == 0);

    add_seq('5');

    thd.join();

    add_seq('\0');

#ifdef CAN_FAIL_ON_TIMING_ASSUMPTION
    assert(strcmp(seq, "123456") == 0);
#else
    assert(strcmp(seq, "12345") == 0);
#endif
}

template <class UnderlyingType>
void test_atomic_wait_func(UnderlyingType old_value, const UnderlyingType new_value,
    const std::chrono::steady_clock::duration waiting_duration) {
    test_atomic_wait_func_impl<std::atomic, UnderlyingType>(old_value, new_value, waiting_duration);
    alignas(std::atomic_ref<UnderlyingType>::required_alignment) UnderlyingType old_value_for_ref = old_value;
    test_atomic_wait_func_impl<std::atomic_ref, UnderlyingType>(old_value_for_ref, new_value, waiting_duration);
}

template <class UnderlyingType>
void test_atomic_wait_func_ptr(UnderlyingType old_value, const UnderlyingType new_value,
    const std::chrono::steady_clock::duration waiting_duration) {
    test_atomic_wait_func_impl<std::atomic, UnderlyingType>(old_value, new_value, waiting_duration);
}


template <template <class> class Template, class UnderlyingType>
void test_notify_all_notifies_all_impl(UnderlyingType& old_value, const UnderlyingType new_value,
    const std::chrono::steady_clock::duration waiting_duration) {
    Template<UnderlyingType> c(old_value);
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

template <class UnderlyingType>
void test_notify_all_notifies_all(UnderlyingType old_value, const UnderlyingType new_value,
    const std::chrono::steady_clock::duration waiting_duration) {
    test_notify_all_notifies_all_impl<std::atomic, UnderlyingType>(old_value, new_value, waiting_duration);
    alignas(std::atomic_ref<UnderlyingType>::required_alignment) UnderlyingType old_value_for_ref = old_value;
    test_notify_all_notifies_all_impl<std::atomic_ref, UnderlyingType>(old_value_for_ref, new_value, waiting_duration);
}

template <class UnderlyingType>
void test_notify_all_notifies_all_ptr(UnderlyingType old_value, const UnderlyingType new_value,
    const std::chrono::steady_clock::duration waiting_duration) {
    // increased waiting_duration because timing assumption might not hold for atomic smart pointers
    test_notify_all_notifies_all_impl<std::atomic, UnderlyingType>(old_value, new_value, 3 * waiting_duration);
}


template <template <class> class Template, class UnderlyingType>
void test_pad_bits_impl(const std::chrono::steady_clock::duration waiting_duration) {
    alignas(std::atomic_ref<UnderlyingType>::required_alignment) UnderlyingType old_value;
    memset(&old_value, 0x66, sizeof(UnderlyingType));
    old_value.set(1);

    UnderlyingType same_old_value;
    memset(&same_old_value, 0x99, sizeof(UnderlyingType));
    same_old_value.set(1);

    Template<UnderlyingType> c(old_value);

    bool trigger      = false;
    const auto waitFn = [&c, same_old_value, &trigger] {
        c.wait(same_old_value);
        trigger = true;
    };

    std::thread w1{waitFn};

    std::this_thread::sleep_for(waiting_duration);
    assert(!trigger);

    c.store(old_value);
    c.notify_one();

    std::this_thread::sleep_for(waiting_duration);
    assert(!trigger);

    UnderlyingType new_value;
    memset(&new_value, 0x99, sizeof(UnderlyingType));
    new_value.set(2);
    c.store(new_value);
    c.notify_one();

#ifdef CAN_FAIL_ON_TIMING_ASSUMPTION
    std::this_thread::sleep_for(waiting_duration);
    assert(trigger);
    w1.join();
#else // ^^^ CAN_FAIL_ON_TIMING_ASSUMPTION / !CAN_FAIL_ON_TIMING_ASSUMPTION vvv
    w1.join();
    assert(trigger);
#endif // ^^^ !CAN_FAIL_ON_TIMING_ASSUMPTION ^^^
}

template <class UnderlyingType>
void test_pad_bits(const std::chrono::steady_clock::duration waiting_duration) {
    test_pad_bits_impl<std::atomic, UnderlyingType>(waiting_duration);
    test_pad_bits_impl<std::atomic_ref, UnderlyingType>(waiting_duration);
}

struct two_shorts {
    short a;
    short b;

    friend bool operator==(two_shorts, two_shorts) = delete;
};

struct three_chars {
    char a;
    char b;
    char c;

    friend bool operator==(three_chars, three_chars) = delete;
};

struct big_char_like {
    char value;
    char unused[16];

    explicit big_char_like(char value_) : value(value_), unused{} {}

    friend bool operator==(big_char_like, big_char_like) = delete;
};

#pragma warning(push)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier
template <size_t size>
struct with_padding_bits {
    alignas(size) char value;

    void set(const char value_) {
        value = value_;
    }

    friend bool operator==(with_padding_bits, with_padding_bits) = delete;
};
#pragma warning(pop)

template <class T, class U>
[[nodiscard]] bool ownership_equal(const T& t, const U& u) {
    return !t.owner_before(u) && !u.owner_before(t);
}

inline void test_gh_3602() {
    // GH-3602 std::atomic<std::shared_ptr>::wait does not seem to care about control block difference. Is this a bug?
    {
        auto sp1    = std::make_shared<char>();
        auto holder = [sp1] {};
        auto sp2    = std::make_shared<decltype(holder)>(holder);
        std::shared_ptr<char> sp3{sp2, sp1.get()};

        std::atomic<std::shared_ptr<char>> asp{sp1};
        asp.wait(sp3);
    }
    {
        auto sp1    = std::make_shared<char>();
        auto holder = [sp1] {};
        auto sp2    = std::make_shared<decltype(holder)>(holder);
        std::shared_ptr<char> sp3{sp2, sp1.get()};
        std::weak_ptr<char> wp3{sp3};

        std::atomic<std::weak_ptr<char>> awp{sp1};
        awp.wait(wp3);
    }

    {
        auto sp1    = std::make_shared<char>();
        auto holder = [sp1] {};
        auto sp2    = std::make_shared<decltype(holder)>(holder);
        std::shared_ptr<char> sp3{sp2, sp1.get()};

        std::atomic<std::shared_ptr<char>> asp{sp3};

        std::thread t([&] {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            asp = sp1;
            asp.notify_one();
        });

        asp.wait(sp3);

        t.join();
    }

    { // Also test shared_ptrs that own the null pointer.
        int* const raw = nullptr;

        std::shared_ptr<int> sp_empty;
        std::shared_ptr<int> sp_also_empty;
        std::shared_ptr<int> sp_original(raw);
        std::shared_ptr<int> sp_copy(sp_original);
        std::shared_ptr<int> sp_different(raw);

        assert(ownership_equal(sp_empty, sp_also_empty));
        assert(!ownership_equal(sp_original, sp_empty));
        assert(ownership_equal(sp_original, sp_copy));
        assert(!ownership_equal(sp_original, sp_different));

        std::atomic<std::shared_ptr<int>> asp_empty;
        asp_empty.wait(sp_original);

        std::atomic<std::shared_ptr<int>> asp_copy{sp_copy};
        asp_copy.wait(sp_empty);
        asp_copy.wait(sp_different);
    }
}

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

    test_atomic_wait_func_ptr(std::make_shared<int>('a'), std::make_shared<int>('a'), waiting_duration);
    test_atomic_wait_func_ptr(
        std::weak_ptr{std::make_shared<int>('a')}, std::weak_ptr{std::make_shared<int>('a')}, waiting_duration);
    test_atomic_wait_func_ptr(std::make_shared<int[]>(0), std::make_shared<int[]>(0), waiting_duration);
    test_atomic_wait_func_ptr(
        std::weak_ptr{std::make_shared<int[]>(0)}, std::weak_ptr{std::make_shared<int[]>(0)}, waiting_duration);
    test_atomic_wait_func_ptr(std::make_shared<int[]>(1), std::make_shared<int[]>(1), waiting_duration);
    test_atomic_wait_func_ptr(
        std::weak_ptr{std::make_shared<int[]>(1)}, std::weak_ptr{std::make_shared<int[]>(1)}, waiting_duration);
    test_atomic_wait_func_ptr(std::make_shared<int[2]>(), std::make_shared<int[2]>(), waiting_duration);
    test_atomic_wait_func_ptr(
        std::weak_ptr{std::make_shared<int[2]>()}, std::weak_ptr{std::make_shared<int[2]>()}, waiting_duration);
    test_atomic_wait_func_ptr(std::make_shared<int[][2]>(2), std::make_shared<int[][2]>(2), waiting_duration);
    test_atomic_wait_func_ptr(
        std::weak_ptr{std::make_shared<int[][2]>(2)}, std::weak_ptr{std::make_shared<int[][2]>(2)}, waiting_duration);
    test_atomic_wait_func_ptr(std::make_shared<int[2][2]>(), std::make_shared<int[2][2]>(), waiting_duration);
    test_atomic_wait_func_ptr(
        std::weak_ptr{std::make_shared<int[2][2]>()}, std::weak_ptr{std::make_shared<int[2][2]>()}, waiting_duration);

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

    test_notify_all_notifies_all_ptr(std::make_shared<int>('a'), std::make_shared<int>('a'), waiting_duration);
    test_notify_all_notifies_all_ptr(
        std::weak_ptr{std::make_shared<int>('a')}, std::weak_ptr{std::make_shared<int>('a')}, waiting_duration);
    test_notify_all_notifies_all_ptr(std::make_shared<int[]>(0), std::make_shared<int[]>(0), waiting_duration);
    test_notify_all_notifies_all_ptr(
        std::weak_ptr{std::make_shared<int[]>(0)}, std::weak_ptr{std::make_shared<int[]>(0)}, waiting_duration);
    test_notify_all_notifies_all_ptr(std::make_shared<int[]>(1), std::make_shared<int[]>(1), waiting_duration);
    test_notify_all_notifies_all_ptr(
        std::weak_ptr{std::make_shared<int[]>(1)}, std::weak_ptr{std::make_shared<int[]>(1)}, waiting_duration);
    test_notify_all_notifies_all_ptr(std::make_shared<int[2]>(), std::make_shared<int[2]>(), waiting_duration);
    test_notify_all_notifies_all_ptr(
        std::weak_ptr{std::make_shared<int[2]>()}, std::weak_ptr{std::make_shared<int[2]>()}, waiting_duration);
    test_notify_all_notifies_all_ptr(std::make_shared<int[][2]>(2), std::make_shared<int[][2]>(2), waiting_duration);
    test_notify_all_notifies_all_ptr(
        std::weak_ptr{std::make_shared<int[][2]>(2)}, std::weak_ptr{std::make_shared<int[][2]>(2)}, waiting_duration);
    test_notify_all_notifies_all_ptr(std::make_shared<int[2][2]>(), std::make_shared<int[2][2]>(), waiting_duration);
    test_notify_all_notifies_all_ptr(
        std::weak_ptr{std::make_shared<int[2][2]>()}, std::weak_ptr{std::make_shared<int[2][2]>()}, waiting_duration);

#ifndef __clang__ // TRANSITION, LLVM-46685
    test_pad_bits<with_padding_bits<2>>(waiting_duration);
    test_pad_bits<with_padding_bits<4>>(waiting_duration);
    test_pad_bits<with_padding_bits<8>>(waiting_duration);
    test_pad_bits<with_padding_bits<16>>(waiting_duration);
    test_pad_bits<with_padding_bits<32>>(waiting_duration);
#endif // ^^^ no workaround ^^^

    test_gh_3602();
}

#ifndef _M_CEE // TRANSITION, VSO-1659496
// GH-140: STL: We should _STD qualify _Ugly function calls to avoid ADL
template <class T>
struct holder {
    T t;
};

struct incomplete;

template <class T, class Tag>
struct tagged_trivial {
    T t;
};

template <class T>
void test_incomplete_associated_class() { // COMPILE-ONLY
    std::atomic<T> a;
    a.wait(T{});
    a.wait(T{}, std::memory_order_relaxed);
    a.notify_one();
    a.notify_all();

    std::atomic_wait(std::addressof(a), T{});
    std::atomic_wait_explicit(std::addressof(a), T{}, std::memory_order_relaxed);
    std::atomic_notify_one(std::addressof(a));
    std::atomic_notify_all(std::addressof(a));
}

void test_incomplete_associated_class_all() { // COMPILE-ONLY
    test_incomplete_associated_class<tagged_trivial<uint8_t, holder<incomplete>>>();
    test_incomplete_associated_class<tagged_trivial<uint16_t, holder<incomplete>>>();
    test_incomplete_associated_class<tagged_trivial<uint32_t, holder<incomplete>>>();
    test_incomplete_associated_class<tagged_trivial<uint64_t, holder<incomplete>>>();
    test_incomplete_associated_class<tagged_trivial<uint64_t[2], holder<incomplete>>>();

    test_incomplete_associated_class<tagged_trivial<uint8_t[3], holder<incomplete>>>();
    test_incomplete_associated_class<tagged_trivial<uint16_t[3], holder<incomplete>>>();
    test_incomplete_associated_class<tagged_trivial<uint32_t[3], holder<incomplete>>>();
    test_incomplete_associated_class<tagged_trivial<uint64_t[3], holder<incomplete>>>();

    test_incomplete_associated_class<tagged_trivial<uint8_t, holder<incomplete>>*>();
    test_incomplete_associated_class<tagged_trivial<uint16_t, holder<incomplete>>*>();
    test_incomplete_associated_class<tagged_trivial<uint32_t, holder<incomplete>>*>();
    test_incomplete_associated_class<tagged_trivial<uint64_t, holder<incomplete>>*>();
    test_incomplete_associated_class<tagged_trivial<uint64_t[2], holder<incomplete>>*>();

    test_incomplete_associated_class<tagged_trivial<uint8_t[3], holder<incomplete>>*>();
    test_incomplete_associated_class<tagged_trivial<uint16_t[3], holder<incomplete>>*>();
    test_incomplete_associated_class<tagged_trivial<uint32_t[3], holder<incomplete>>*>();
    test_incomplete_associated_class<tagged_trivial<uint64_t[3], holder<incomplete>>*>();
}
#endif // ^^^ no workaround ^^^

int main() {
    test_atomic_wait();
}
