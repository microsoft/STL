// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <deque>
#include <memory>
#include <sstream>
#include <string>
#include <syncstream>
#include <vector>

using namespace std;

bool allocate_at_least_called = false;

bool consume_signal() {
    bool ret                 = allocate_at_least_called;
    allocate_at_least_called = false;
    return ret;
}

void set_signal() {
    allocate_at_least_called = true;
}

template <class T>
struct signaling_allocator {
    using value_type = T;

    signaling_allocator() = default;

    template <typename U>
    signaling_allocator(const signaling_allocator<U>&) {}

    T* allocate(size_t count) {
        T* ptr = (T*) malloc(count * sizeof(T));
        if (ptr) {
            return ptr;
        }

        throw bad_alloc();
    }

    allocation_result<T*> allocate_at_least(size_t count) {
        set_signal();
        return {allocate(count * 2), count * 2};
    }

    void deallocate(T* ptr, size_t) noexcept {
        free(ptr);
    }

    friend bool operator==(const signaling_allocator&, const signaling_allocator&) = default;
};

template <typename T>
void test_container() {
    T container;
    size_t reserve_count = container.capacity() + 100;
    container.reserve(reserve_count);
    assert(consume_signal());
    assert(container.capacity() >= reserve_count * 2);
    assert(container.size() == 0);
}

void test_deque() {
    deque<int, signaling_allocator<int>> d;
    d.resize(100);
    assert(consume_signal());
    assert(d.size() == 100);
}

void test_stream_overflow(auto& stream) {
    stream << "my very long string that is indeed very long in order to make sure"
           << "that overflow is called, hopefully calling allocate_at_least in return";
    assert(consume_signal());
}

void test_sstream() {
    basic_stringstream<char, char_traits<char>, signaling_allocator<char>> ss;
    ss.str("my_cool_string");
    assert(consume_signal());
    test_stream_overflow(ss);
}

void test_syncstream() {
    basic_syncbuf<char, char_traits<char>, signaling_allocator<char>> buf;
    basic_osyncstream<char, char_traits<char>, signaling_allocator<char>> ss(&buf);
    test_stream_overflow(ss);
}

int main() {
    test_deque();
    test_container<basic_string<char, char_traits<char>, signaling_allocator<char>>>();
    test_container<vector<int, signaling_allocator<int>>>();
    test_sstream();
    test_syncstream();
}
