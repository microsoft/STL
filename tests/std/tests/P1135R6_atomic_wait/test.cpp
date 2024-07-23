// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "test_atomic_wait.hpp"

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
    assert(__std_atomic_set_api_level(__std_atomic_api_level::__has_wait_on_address)
           == __std_atomic_api_level::__has_wait_on_address);
    test_atomic_wait();
}
