// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstdlib>
#include <deque>
#include <functional>
#include <memory>
#include <new>
#include <numeric>
#include <sstream>
#include <string>
#include <syncstream>
#include <utility>
#include <vector>

using namespace std;

struct signaller {
    [[nodiscard]] bool consume() {
        return exchange(is_set, false);
    }

    void set() {
        is_set = true;
    }

private:
    bool is_set = false;
};

signaller allocate_at_least_signal;

template <class T>
struct signalling_allocator {
    using value_type = T;

    signalling_allocator() = default;

    template <typename U>
    signalling_allocator(const signalling_allocator<U>&) {}

    T* allocate(size_t count) {
        T* const ptr = static_cast<T*>(malloc(count * sizeof(T)));
        if (ptr) {
            return ptr;
        }

        throw bad_alloc();
    }

    allocation_result<T*> allocate_at_least(size_t count) {
        allocate_at_least_signal.set();
        return {allocate(count * 2 + 1), count * 2 + 1};
    }

    void deallocate(T* ptr, size_t) noexcept {
        free(ptr);
    }

    friend bool operator==(const signalling_allocator&, const signalling_allocator&) = default;
};

template <typename T>
void test_container() {
    T container;
    const size_t reserve_count = container.capacity() + 100;
    container.reserve(reserve_count);
    assert(allocate_at_least_signal.consume());
    assert(container.capacity() >= reserve_count * 2);
    assert(container.size() == 0);
}

void test_deque() {
    deque<size_t, signalling_allocator<size_t>> d;
    for (size_t i = 0; i < 100; ++i) {
        d.push_back(i);
    }
    assert(allocate_at_least_signal.consume());
    assert(d.size() == 100);
    for (size_t i = 0; i < 100; ++i) {
        assert(d[i] == i);
    }
}

void test_stream_overflow(auto& stream) {
    stream << "my very long string that is indeed very long in order to make sure "
           << "that overflow is called, hopefully calling allocate_at_least in return";
    assert(allocate_at_least_signal.consume());
}

void test_sstream() {
    basic_stringstream<char, char_traits<char>, signalling_allocator<char>> ss;
    ss.str("my_cool_string");
    assert(allocate_at_least_signal.consume());
    test_stream_overflow(ss);
}

void test_syncstream() {
    basic_syncbuf<char, char_traits<char>, signalling_allocator<char>> buf;
    basic_osyncstream<char, char_traits<char>, signalling_allocator<char>> ss(&buf);
    test_stream_overflow(ss);
}

template <class T>
struct icky_allocator : allocator<T> {
    template <class U>
    struct rebind {
        using other = icky_allocator<U>;
    };

    allocation_result<T*, size_t> allocate_at_least(size_t) {
        // The initial implementation of this feature had a problem with (icky) allocators that
        // publicly derive from std::allocator and implement allocate/deallocate. The STL would
        // call allocate_at_least in the std::allocator base (which would then call std::allocator::allocate),
        // and would then call deallocate in the derived class (hiding the base implementation), a terrible mismatch.
        // We now detect public derivation from std::allocator and avoid using allocate_at_least in that case.
        static_assert(false);
    }
};

void test_inheriting_allocator() {
    vector<int, icky_allocator<int>> vec{2, 1, 4, 7, 5, 6, 3, 8};
    assert(accumulate(vec.begin(), vec.end(), 0, plus<>{}) == 36);
}

// Also test GH-3890, in which we incorrectly tried to use allocate_at_least from an inaccessible std::allocator
// base due to an MSVC bug.
template <class T>
struct less_icky_allocator : private allocator<T> {
    using value_type = T;

    less_icky_allocator() = default;
    template <class U>
    less_icky_allocator(const less_icky_allocator<U>&) {}

    T* allocate(size_t n) {
        return allocator<T>::allocate(n);
    }

    void deallocate(T* ptr, size_t n) {
        return allocator<T>::deallocate(ptr, n);
    }

    template <class U>
    bool operator==(const less_icky_allocator<U>&) const {
        return true;
    }
};
static_assert(!std::_Should_allocate_at_least<less_icky_allocator<int>>);

int main() {
    test_deque();
    test_container<basic_string<char, char_traits<char>, signalling_allocator<char>>>();
    test_container<vector<int, signalling_allocator<int>>>();
    test_sstream();
    test_syncstream();
    test_inheriting_allocator();
}
