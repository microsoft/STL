// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <memory_resource>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

extern pmr::monotonic_buffer_resource mr;

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

auto test_deque(pmr::deque<int>& px) {
    auto x = SfinaeTester<deque>::test(px, &mr);
}

auto test_forward_list(pmr::forward_list<int>& px) {
    auto x = SfinaeTester<forward_list>::test(px, &mr);
}

auto test_list(pmr::list<int>& px) {
    auto x = SfinaeTester<list>::test(px, &mr);
}

auto test_vector(pmr::vector<int>& px) {
    auto x = SfinaeTester<vector>::test(px, &mr);
}

auto test_map(pmr::map<int, int>& px) {
    auto x = SfinaeTester<map>::test(px, &mr);
}

auto test_multimap(pmr::multimap<int, int>& px) {
    auto x = SfinaeTester<multimap>::test(px, &mr);
}

auto test_multiset(pmr::multiset<int>& px) {
    auto x = SfinaeTester<multiset>::test(px, &mr);
}

auto test_set(pmr::set<int>& px) {
    auto x = SfinaeTester<set>::test(px, &mr);
}

auto test_unordered_map(pmr::unordered_map<int, int>& px) {
    auto x = SfinaeTester<unordered_map>::test(px, &mr);
}

auto test_unordered_multimap(pmr::unordered_multimap<int, int>& px) {
    auto x = SfinaeTester<unordered_multimap>::test(px, &mr);
}

auto test_unordered_multiset(pmr::unordered_multiset<int>& px) {
    auto x = SfinaeTester<unordered_multiset>::test(px, &mr);
}

auto test_unordered_set(pmr::unordered_set<int>& px) {
    auto x = SfinaeTester<unordered_set>::test(px, &mr);
}

auto test_priority_queue1(priority_queue<int, pmr::vector<int>>& px) {
    auto x = SfinaeTester<priority_queue>::test(px, &mr);
}

auto test_queue1(queue<int, pmr::deque<int>>& px) {
    auto x = SfinaeTester<queue>::test(px, &mr);
}

auto test_stack1(stack<int, pmr::vector<int>>& px) {
    auto x = SfinaeTester<stack>::test(px, &mr);
}

auto test_priority_queue2(less<int> comp, pmr::vector<int>& pc) {
    auto x = SfinaeTester<priority_queue>::test(comp, pc, &mr);
}

auto test_queue2(pmr::deque<int>& pc) {
    auto x = SfinaeTester<queue>::test(pc, &mr);
}

auto test_stack2(pmr::vector<int>& pc) {
    auto x = SfinaeTester<stack>::test(pc, &mr);
}

int main() {} // COMPILE-ONLY
