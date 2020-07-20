// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Covers:
// * spaceship for containers

#include <array>
#include <cassert>
#include <compare>
#include <concepts>
#include <deque>
#include <forward_list>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <ranges>
#include <set>
#include <stack>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

template <class Container>
void ordered_containers_test(const Container& smaller, const Container& smaller_equal, const Container& larger) {
    assert(smaller < larger);
    assert(smaller <= larger);
    assert(larger > smaller);
    assert(larger >= smaller);
    assert(smaller == smaller_equal);
    assert(smaller != larger);
    assert((smaller <=> larger) < 0);
    assert((larger <=> smaller) > 0);
    assert((smaller_equal <=> smaller) == 0);
}

template <class Container>
void unordered_containers_test(
    const Container& something, const Container& something_equal, const Container& different) {
    assert(something == something_equal);
    assert(something != different);
}

void ordering_test_cases() {
    { // constexpr array
        constexpr std::array<int, 5> a0{{2, 8, 9, 1, 9}};
        constexpr std::array<int, 3> a1{{2, 8, 9}};
        constexpr std::array<int, 5> a2{{2, 8, 9, 1, 8}};

        static_assert((a0 <=> a0) == 0);
        static_assert((a1 <=> a1) == 0);
        static_assert((a2 <=> a0) < 0);
        static_assert((a0 <=> a2) > 0);
    }
    { // array
        std::array<int, 3> a1 = {100, 100, 100};
        std::array<int, 3> a2 = {100, 100, 100};
        std::array<int, 3> b1 = {200, 200};
        ordered_containers_test(a1, a2, b1);
    }
    { // deque
        std::deque<int> a1(3, 100);
        std::deque<int> a2(3, 100);
        std::deque<int> b1(2, 200);
        ordered_containers_test(a1, a2, b1);
    }
    { // list
        std::list<int> a1(3, 100);
        std::list<int> a2(3, 100);
        std::list<int> b1(2, 200);
        ordered_containers_test(a1, a2, b1);
    }
    { // vector
        std::vector<int> a1(3, 100);
        std::vector<int> a2(3, 100);
        std::vector<int> b1(2, 200);
        ordered_containers_test(a1, a2, b1);

        std::vector<bool> c1(3, 0);
        std::vector<bool> c2(3, 0);
        std::vector<bool> d1(2, 1);
        ordered_containers_test(c1, c2, d1);
    }
    { // forward_list
        std::forward_list<int> a1(3, 100);
        std::forward_list<int> a2(3, 100);
        std::forward_list<int> b1(2, 200);
        ordered_containers_test(a1, a2, b1);
    }
    { // map
        std::map<std::string, int> a1;
        a1["hi"]   = 1;
        a1["hola"] = 2;
        std::map<std::string, int> a2;
        a2["hi"]   = 1;
        a2["hola"] = 2;
        std::map<std::string, int> b1;
        b1["zoe"]   = 3;
        b1["koala"] = 4;
        ordered_containers_test(a1, a2, b1);
    }
    { // multimap
        std::multimap<char, int> a1 = {{'a', 1}, {'b', 2}, {'a', 3}};
        std::multimap<char, int> a2 = {{'a', 1}, {'a', 3}, {'b', 2}};
        std::multimap<char, int> b1 = {{'z', 4}, {'y', 90}, {'z', 12}};
        ordered_containers_test(a1, a2, b1);
    }
    { // set
        std::set<int> a1;
        a1.insert(10);
        a1.insert(20);

        std::set<int> a2;
        a2.insert(10);
        a2.insert(20);

        std::set<int> b1;
        b1.insert(30);
        b1.insert(40);
        ordered_containers_test(a1, a2, b1);
    }
    { // multiset
        std::multiset<int> a1;
        a1.insert(10);
        a1.insert(10);
        a1.insert(20);

        std::multiset<int> a2;
        a2.insert(10);
        a2.insert(20);
        a2.insert(10);

        std::multiset<int> b1;
        b1.insert(30);
        b1.insert(40);
        b1.insert(40);
        ordered_containers_test(a1, a2, b1);
    }
    { // unordered_map
        using stringmap = std::unordered_map<std::string, std::string>;
        stringmap a     = {{"cat", "tabby"}, {"dog", "poodle"}, {"bear", "grizzly"}};
        stringmap b     = {{"dog", "poodle"}, {"bear", "grizzly"}, {"cat", "tabby"}};
        stringmap c     = {{"cat", "siamese"}, {"dog", "lab"}, {"bear", "polar"}};
        unordered_containers_test(a, b, c);
    }
    { // unordered_multimap
        using stringmap = std::unordered_multimap<std::string, std::string>;
        stringmap a     = {{"cat", "tabby"}, {"dog", "poodle"}, {"cat", "siamese"}, {"dog", "poodle"}};
        stringmap b     = {{"dog", "poodle"}, {"cat", "siamese"}, {"cat", "tabby"}, {"dog", "poodle"}};
        stringmap c     = {{"cat", "siamese"}, {"dog", "lab"}, {"bear", "polar"}};
        unordered_containers_test(a, b, c);
    }
    { // unordered_set
        std::unordered_set<std::string> a = {"cat", "dog", "bear"};
        std::unordered_set<std::string> b = {"bear", "cat", "dog"};
        std::unordered_set<std::string> c = {"mouse", "cat", "bear", "dog"};
        unordered_containers_test(a, b, c);
    }
    { // unordered_multiset
        std::unordered_multiset<std::string> a = {"cat", "dog", "cat"};
        std::unordered_multiset<std::string> b = {"cat", "cat", "dog"};
        std::unordered_multiset<std::string> c = {"mouse", "cat", "bear", "dog"};
        unordered_containers_test(a, b, c);
    }
    { // queue
        std::deque<int> deq1(3, 100);
        std::deque<int> deq2(2, 200);
        std::queue<int> a(deq1);
        std::queue<int> b(deq1);
        std::queue<int> c(deq2);
        ordered_containers_test(a, b, c);
    }
    { // stack
        std::stack<int> a;
        a.push(2);
        a.push(2);
        std::stack<int> b;
        b.push(2);
        b.push(2);
        std::stack<int> c;
        c.push(3);
        c.push(3);
        ordered_containers_test(a, b, c);
    }
}

int main() {
    ordering_test_cases();
}
