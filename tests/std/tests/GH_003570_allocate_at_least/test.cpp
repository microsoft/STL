// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <utility>
#include <memory>
#include <deque>
#include <sstream>
#include <string>
#include <syncstream>
#include <vector>

using namespace std;

struct {
    void assert_set() {
        assert(consume());
    }

    bool consume() {
        return exchange(is_set, false);
    }

    void set() {
        is_set = true;
    }

private:
    bool is_set = false;
} allocate_at_least_signal;

template <class T>
struct signalling_allocator {
    using value_type = T;

    signalling_allocator() = default;

    template <typename U>
    signalling_allocator(const signalling_allocator<U>&) {}

    T* allocate(size_t count) {
        T* ptr = (T*) malloc(count * sizeof(T));
        if (ptr) {
            return ptr;
        }

        throw bad_alloc();
    }

    allocation_result<T*> allocate_at_least(size_t count) {
        allocate_at_least_signal.set();
        return {allocate(count * 2), count * 2};
    }

    void deallocate(T* ptr, size_t) noexcept {
        free(ptr);
    }

    friend bool operator==(const signalling_allocator&, const signalling_allocator&) = default;
};

template <typename T>
void test_container() {
    T container;
    size_t reserve_count = container.capacity() + 100;
    container.reserve(reserve_count);
    allocate_at_least_signal.assert_set();
    assert(container.capacity() >= reserve_count * 2);
    assert(container.size() == 0);
}

void test_deque() {
    deque<int, signalling_allocator<int>> d;
    d.resize(100);
    allocate_at_least_signal.assert_set();
    assert(d.size() == 100);
}

void test_stream_overflow(auto& stream) {
    stream << "my very long string that is indeed very long in order to make sure"
           << "that overflow is called, hopefully calling allocate_at_least in return";
    allocate_at_least_signal.assert_set();
}

void test_sstream() {
    basic_stringstream<char, char_traits<char>, signalling_allocator<char>> ss;
    ss.str("my_cool_string");
    allocate_at_least_signal.assert_set();
    test_stream_overflow(ss);
}

void test_syncstream() {
    basic_syncbuf<char, char_traits<char>, signalling_allocator<char>> buf;
    basic_osyncstream<char, char_traits<char>, signalling_allocator<char>> ss(&buf);
    test_stream_overflow(ss);
}

int main() {
    test_deque();
    test_container<basic_string<char, char_traits<char>, signalling_allocator<char>>>();
    test_container<vector<int, signalling_allocator<int>>>();
    test_sstream();
    test_syncstream();
}
