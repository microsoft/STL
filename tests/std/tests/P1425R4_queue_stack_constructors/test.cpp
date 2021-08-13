// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <deque>
#include <forward_list>
#include <functional>
#include <memory>
#include <queue>
#include <set>
#include <stack>
#include <type_traits>
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

    template <class U>
    bool operator==(const custom_allocator<U>&) const noexcept {
        return true;
    }

#if !_HAS_CXX20
    template <class U>
    bool operator!=(const custom_allocator<U>&) const noexcept {
        return false;
    }
#endif // !_HAS_CXX20
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

#if _HAS_CXX17
    priority_queue pq1(range.begin(), range.end());
    static_assert(is_same_v<decltype(pq1), priority_queue<int, vector<int>, less<int>>>);
#else // ^^^ _HAS_CXX17 ^^^ / vvv !_HAS_CXX17 vvv
    priority_queue<int, vector<int>, less<int>> pq1(range.begin(), range.end());
#endif // !_HAS_CXX17
    assert(pq1.size() == size(some_data));
    result = 5;
    while (!pq1.empty()) {
        assert(pq1.top() == result--);
        pq1.pop();
    }

#if _HAS_CXX17
    priority_queue xy3(range.begin(), range.end(), custom_allocator<int>{});
    static_assert(is_same_v<decltype(xy3), priority_queue<int, vector<int, custom_allocator<int>>, less<int>>>);
#else // ^^^ _HAS_CXX17 ^^^ / vvv !_HAS_CXX17 vvv
    priority_queue<int, vector<int, custom_allocator<int>>, less<int>> xy3(
        range.begin(), range.end(), custom_allocator<int>{});
#endif // !_HAS_CXX17
    assert(xy3.size() == size(some_data));
    result = 5;
    while (!xy3.empty()) {
        assert(xy3.top() == result--);
        xy3.pop();
    }

#if _HAS_CXX17
    priority_queue xy4(range.begin(), range.end(), greater<int>{}, custom_allocator<int>{});
    static_assert(is_same_v<decltype(xy4), priority_queue<int, vector<int, custom_allocator<int>>, greater<int>>>);
#else // ^^^ _HAS_CXX17 ^^^ / vvv !_HAS_CXX17 vvv
    priority_queue<int, vector<int, custom_allocator<int>>, greater<int>> xy4(
        range.begin(), range.end(), greater<int>{}, custom_allocator<int>{});
#endif // !_HAS_CXX17
    assert(xy4.size() == size(some_data));
    result = 0;
    while (!xy4.empty()) {
        assert(xy4.top() == result++);
        xy4.pop();
    }

    deque<int> cont(begin(additional_data), end(additional_data));
#if _HAS_CXX17
    priority_queue xy5(range.begin(), range.end(), greater<int>{}, cont);
    static_assert(is_same_v<decltype(xy5), priority_queue<int, deque<int>, greater<int>>>);
#else // ^^^ _HAS_CXX17 ^^^ / vvv !_HAS_CXX17 vvv
    priority_queue<int, deque<int>, greater<int>> xy5(range.begin(), range.end(), greater<int>{}, cont);
#endif // !_HAS_CXX17
    assert(xy5.size() == size(some_data) + size(additional_data));
    result = 0;
    while (!xy5.empty()) {
        assert(xy5.top() == result++);
        xy5.pop();
    }

#if _HAS_CXX17
    priority_queue xy6(
        range.begin(), range.end(), greater<int>{}, deque<int>{begin(additional_data), end(additional_data)});
    static_assert(is_same_v<decltype(xy6), priority_queue<int, deque<int>, greater<int>>>);
#else // ^^^ _HAS_CXX17 ^^^ / vvv !_HAS_CXX17 vvv
    priority_queue<int, deque<int>, greater<int>> xy6(
        range.begin(), range.end(), greater<int>{}, deque<int>{begin(additional_data), end(additional_data)});
#endif // !_HAS_CXX17
    assert(xy6.size() == size(some_data) + size(additional_data));
    result = 0;
    while (!xy6.empty()) {
        assert(xy6.top() == result++);
        xy6.pop();
    }

    deque<int, custom_allocator<int>> cont2(begin(additional_data), end(additional_data), custom_allocator<int>{});
#if _HAS_CXX17
    priority_queue xy7(range.begin(), range.end(), greater<int>{}, cont2, custom_allocator<int>{});
    static_assert(is_same_v<decltype(xy7), priority_queue<int, deque<int, custom_allocator<int>>, greater<int>>>);
#else // ^^^ _HAS_CXX17 ^^^ / vvv !_HAS_CXX17 vvv
    priority_queue<int, deque<int, custom_allocator<int>>, greater<int>> xy7(
        range.begin(), range.end(), greater<int>{}, cont2, custom_allocator<int>{});
#endif // !_HAS_CXX17
    assert(xy7.size() == size(some_data) + size(additional_data));
    result = 0;
    while (!xy7.empty()) {
        assert(xy7.top() == result++);
        xy7.pop();
    }

#if _HAS_CXX17
    priority_queue xy8(range.begin(), range.end(), greater<int>{},
        deque<int, custom_allocator<int>>{begin(additional_data), end(additional_data)}, custom_allocator<int>{});
    static_assert(is_same_v<decltype(xy8), priority_queue<int, deque<int, custom_allocator<int>>, greater<int>>>);
#else // ^^^ _HAS_CXX17 ^^^ / vvv !_HAS_CXX17 vvv
    priority_queue<int, deque<int, custom_allocator<int>>, greater<int>> xy8(range.begin(), range.end(), greater<int>{},
        deque<int, custom_allocator<int>>{begin(additional_data), end(additional_data)}, custom_allocator<int>{});
#endif // !_HAS_CXX17
    assert(xy8.size() == size(some_data) + size(additional_data));
    result = 0;
    while (!xy8.empty()) {
        assert(xy8.top() == result++);
        xy8.pop();
    }
}

int main() {
    test_container<deque<int>>();
    test_container<forward_list<int>>();
    test_container<vector<int>>();
    test_container<set<int>>();
}
