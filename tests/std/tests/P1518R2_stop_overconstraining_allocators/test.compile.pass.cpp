// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <deque>
#include <forward_list>
#include <functional>
#include <list>
#include <map>
#include <memory_resource>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

pmr::monotonic_buffer_resource mr;

template <template <class...> class Ctr>
struct SfinaeTester {
    template <class... Args>
    static auto f(int, Args&&... args) -> decltype(Ctr(static_cast<Args&&>(args)...)) {
        return Ctr(static_cast<Args&&>(args)...);
    }
    template <class... Args>
    static void f(long, Args&&...) {}

    template <class... Args>
    static auto test(Args&&... args) {
        return f(0, static_cast<Args&&>(args)...);
    }
};

void test_deque(pmr::deque<int>& px) {
    [[maybe_unused]] auto x = SfinaeTester<deque>::test(px, &mr);
    [[maybe_unused]] auto y = SfinaeTester<deque>::test(move(px), &mr);
}

void test_forward_list(pmr::forward_list<int>& px) {
    [[maybe_unused]] auto x = SfinaeTester<forward_list>::test(px, &mr);
    [[maybe_unused]] auto y = SfinaeTester<forward_list>::test(move(px), &mr);
}

void test_list(pmr::list<int>& px) {
    [[maybe_unused]] auto x = SfinaeTester<list>::test(px, &mr);
    [[maybe_unused]] auto y = SfinaeTester<list>::test(move(px), &mr);
}

void test_vector(pmr::vector<int>& px) {
    [[maybe_unused]] auto x = SfinaeTester<vector>::test(px, &mr);
    [[maybe_unused]] auto y = SfinaeTester<vector>::test(move(px), &mr);
}

void test_vector_bool(pmr::vector<bool>& px) {
    [[maybe_unused]] auto x = SfinaeTester<vector>::test(px, &mr);
    [[maybe_unused]] auto y = SfinaeTester<vector>::test(move(px), &mr);
}

void test_map(pmr::map<int, int>& px) {
    [[maybe_unused]] auto x = SfinaeTester<map>::test(px, &mr);
    [[maybe_unused]] auto y = SfinaeTester<map>::test(move(px), &mr);
}

void test_multimap(pmr::multimap<int, int>& px) {
    [[maybe_unused]] auto x = SfinaeTester<multimap>::test(px, &mr);
    [[maybe_unused]] auto y = SfinaeTester<multimap>::test(move(px), &mr);
}

void test_multiset(pmr::multiset<int>& px) {
    [[maybe_unused]] auto x = SfinaeTester<multiset>::test(px, &mr);
    [[maybe_unused]] auto y = SfinaeTester<multiset>::test(move(px), &mr);
}

void test_set(pmr::set<int>& px) {
    [[maybe_unused]] auto x = SfinaeTester<set>::test(px, &mr);
    [[maybe_unused]] auto y = SfinaeTester<set>::test(move(px), &mr);
}

void test_unordered_map(pmr::unordered_map<int, int>& px) {
    [[maybe_unused]] auto x = SfinaeTester<unordered_map>::test(px, &mr);
    [[maybe_unused]] auto y = SfinaeTester<unordered_map>::test(move(px), &mr);
}

void test_unordered_multimap(pmr::unordered_multimap<int, int>& px) {
    [[maybe_unused]] auto x = SfinaeTester<unordered_multimap>::test(px, &mr);
    [[maybe_unused]] auto y = SfinaeTester<unordered_multimap>::test(move(px), &mr);
}

void test_unordered_multiset(pmr::unordered_multiset<int>& px) {
    [[maybe_unused]] auto x = SfinaeTester<unordered_multiset>::test(px, &mr);
    [[maybe_unused]] auto y = SfinaeTester<unordered_multiset>::test(move(px), &mr);
}

void test_unordered_set(pmr::unordered_set<int>& px) {
    [[maybe_unused]] auto x = SfinaeTester<unordered_set>::test(px, &mr);
    [[maybe_unused]] auto y = SfinaeTester<unordered_set>::test(move(px), &mr);
}

void test_priority_queue1(priority_queue<int, pmr::vector<int>>& px) {
    [[maybe_unused]] auto x = SfinaeTester<priority_queue>::test(px, &mr);
    [[maybe_unused]] auto y = SfinaeTester<priority_queue>::test(move(px), &mr);
}

void test_queue1(queue<int, pmr::deque<int>>& px) {
    [[maybe_unused]] auto x = SfinaeTester<queue>::test(px, &mr);
    [[maybe_unused]] auto y = SfinaeTester<queue>::test(move(px), &mr);
}

void test_stack1(stack<int, pmr::vector<int>>& px) {
    [[maybe_unused]] auto x = SfinaeTester<stack>::test(px, &mr);
    [[maybe_unused]] auto y = SfinaeTester<stack>::test(move(px), &mr);
}

void test_priority_queue2(less<int> comp, pmr::vector<int>& pc) {
    [[maybe_unused]] auto x = SfinaeTester<priority_queue>::test(comp, pc, &mr);
    [[maybe_unused]] auto y = SfinaeTester<priority_queue>::test(comp, move(pc), &mr);
}

void test_priority_queue3(int* begin, int* end, less<int> comp, pmr::vector<int>& pc) {
    [[maybe_unused]] auto x = SfinaeTester<priority_queue>::test(begin, end, comp, pc, &mr);
    [[maybe_unused]] auto y = SfinaeTester<priority_queue>::test(begin, end, comp, move(pc), &mr);
}

void test_queue2(pmr::deque<int>& pc) {
    [[maybe_unused]] auto x = SfinaeTester<queue>::test(pc, &mr);
    [[maybe_unused]] auto y = SfinaeTester<queue>::test(move(pc), &mr);
}

void test_stack2(pmr::vector<int>& pc) {
    [[maybe_unused]] auto x = SfinaeTester<stack>::test(pc, &mr);
    [[maybe_unused]] auto y = SfinaeTester<stack>::test(move(pc), &mr);
}
