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

constexpr int some_data[]       = {0, 1, 2, 3, 4, 5};
constexpr int additional_data[] = {6, 7, 8, 9, 10, 11};

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

    int result;
#if _HAS_CXX23
    stack s1(range.begin(), range.end());
    static_assert(is_same_v<decltype(s1), stack<int, deque<int, allocator<int>>>>);
    assert(s1.size() == size(some_data));
    result = 5;
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
#endif // _HAS_CXX23

    priority_queue pq1(range.begin(), range.end());
    static_assert(is_same_v<decltype(pq1), priority_queue<int, vector<int, allocator<int>>, less<int>>>);
    assert(pq1.size() == size(some_data));
    result = 5;
    while (!pq1.empty()) {
        assert(pq1.top() == result--);
        pq1.pop();
    }

    priority_queue pq2(range.begin(), range.end(), custom_allocator<int>{});
    static_assert(is_same_v<decltype(pq2), priority_queue<int, vector<int, custom_allocator<int>>, less<int>>>);
    assert(pq2.size() == size(some_data));
    result = 5;
    while (!pq2.empty()) {
        assert(pq2.top() == result--);
        pq2.pop();
    }

    priority_queue pq3(range.begin(), range.end(), greater<int>{}, custom_allocator<int>{});
    static_assert(is_same_v<decltype(pq3), priority_queue<int, vector<int, custom_allocator<int>>, greater<int>>>);
    assert(pq3.size() == size(some_data));
    result = 0;
    while (!pq3.empty()) {
        assert(pq3.top() == result++);
        pq3.pop();
    }

    vector cont(begin(additional_data), end(additional_data));
    priority_queue pq4(range.begin(), range.end(), greater<int>{}, cont);
    static_assert(is_same_v<decltype(pq4), priority_queue<int, vector<int, allocator<int>>, greater<int>>>);
    assert(pq4.size() == size(some_data) + size(additional_data));
    result = 0;
    while (!pq4.empty()) {
        assert(pq4.top() == result++);
        pq4.pop();
    }

    priority_queue pq5(range.begin(), range.end(), greater<int>{},
        vector<int, allocator<int>>{begin(additional_data), end(additional_data)});
    static_assert(is_same_v<decltype(pq5), priority_queue<int, vector<int, allocator<int>>, greater<int>>>);
    assert(pq5.size() == size(some_data) + size(additional_data));
    result = 0;
    while (!pq5.empty()) {
        assert(pq5.top() == result++);
        pq5.pop();
    }

    vector cont2(begin(additional_data), end(additional_data), custom_allocator<int>{});
    priority_queue pq6(range.begin(), range.end(), greater<int>{}, cont2, custom_allocator<int>{});
    static_assert(is_same_v<decltype(pq6), priority_queue<int, vector<int, custom_allocator<int>>, greater<int>>>);
    assert(pq6.size() == size(some_data) + size(additional_data));
    result = 0;
    while (!pq6.empty()) {
        assert(pq6.top() == result++);
        pq6.pop();
    }

    priority_queue pq7(range.begin(), range.end(), greater<int>{},
        vector<int, custom_allocator<int>>{begin(additional_data), end(additional_data)}, custom_allocator<int>{});
    static_assert(is_same_v<decltype(pq7), priority_queue<int, vector<int, custom_allocator<int>>, greater<int>>>);
    assert(pq7.size() == size(some_data) + size(additional_data));
    result = 0;
    while (!pq7.empty()) {
        assert(pq7.top() == result++);
        pq7.pop();
    }
}

int main() {
    test_container<deque<int>>();
    test_container<forward_list<int>>();
    test_container<vector<int>>();
    test_container<set<int>>();
}
