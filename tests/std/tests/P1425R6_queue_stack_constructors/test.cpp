// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <deque>
#include <forward_list>
#include <iostream>
#include <memory>
#include <queue>
#include <set>
#include <stack>
#include <type_traits>
#include <utility>
#include <vector>
using namespace std;

constexpr int some_data[] = {0, 1, 2, 3, 4, 5};

template <typename T>
struct custom_allocator {
    using value_type = T;

    custom_allocator() noexcept = default;
    template <typename U>
    custom_allocator(const custom_allocator<U>&) noexcept {}

    T* allocate(const size_t n) {
        return allocator<T>{}.allocate(n);
    }

    void deallocate(T* const p, const size_t n) noexcept {
        allocator<T>{}.deallocate(p, n);
    }

    template <class... Args>
    void construct(T* const p, Args&&... args) {
        construct_at(p, forward<Args>(args)...);
    }
};

template <class Range>
void test_container() {
    Range range{begin(some_data), end(some_data)};

    stack s1(range.begin(), range.end());
    static_assert(is_same_v<decltype(s1), stack<int, deque<int, allocator<int>>>>);
    assert(s1.size() == size(some_data));
    int result = 5;
    while (!s1.empty()) {
        assert(s1.top() == result--);
        s1.pop();
    }

    stack s2(range.begin(), range.end(), custom_allocator<int>{});
    static_assert(is_same_v<decltype(s2), stack<int, deque<int, custom_allocator<int>>>>);
    assert(s2.size() == size(some_data));
    result = 5;
    while (!s2.empty()) {
        assert(s2.top() == result--);
        s2.pop();
    }

    queue q1(range.begin(), range.end());
    static_assert(is_same_v<decltype(q1), queue<int, deque<int, allocator<int>>>>);
    assert(q1.size() == size(some_data));
    result = 0;
    while (!q1.empty()) {
        assert(q1.front() == result++);
        q1.pop();
    }

    queue q2(range.begin(), range.end(), custom_allocator<int>{});
    static_assert(is_same_v<decltype(q2), queue<int, deque<int, custom_allocator<int>>>>);
    assert(q2.size() == size(some_data));
    result = 0;
    while (!q2.empty()) {
        assert(q2.front() == result++);
        q2.pop();
    }
}

int main() {
    test_container<deque<int>>();
    test_container<forward_list<int>>();
    test_container<vector<int>>();
    test_container<set<int>>();
}
