// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <deque>
#include <flat_set>
#include <functional>
#include <iostream>
#include <vector>

using namespace std;
using namespace ranges;

template <class T>
void assert_container_requirements(const T& s) {
    T m = s;
    assert(m == s);

    static_assert(is_same_v<decltype(m = s), T&>);
    static_assert(is_same_v<decltype(m = std::move(m)), T&>);
    static_assert(is_same_v<decltype(m.begin()), typename T::iterator>);
    static_assert(is_same_v<decltype(m.end()), typename T::iterator>);
    static_assert(is_same_v<decltype(s.cbegin()), typename T::const_iterator>);
    static_assert(is_same_v<decltype(m.cend()), typename T::const_iterator>);
    static_assert(is_convertible_v<typename T::iterator, typename T::const_iterator>);
    static_assert(is_same_v<decltype(m.begin() <=> m.end()), strong_ordering>);
    static_assert(is_same_v<decltype(s.size()), typename T::size_type>);
    static_assert(is_same_v<decltype(s.max_size()), typename T::size_type>);
    static_assert(is_same_v<decltype(*m.begin()), typename T::value_type&>);
    static_assert(is_same_v<decltype(*m.cbegin()), const typename T::value_type&>);

    T my_moved = std::move(m);
    assert(!(my_moved != s));

    T empty{};
    assert(empty.empty());

    T non_empty = s;
    empty.swap(non_empty);
    assert(non_empty.empty());
    assert(empty == s);

    std::swap(empty, non_empty);
    assert(empty.empty());
    assert(non_empty == s);

    assert(s.cbegin() <= s.cend());
    assert(s.cbegin() < s.cend() || s.empty());

    assert(m.begin() <= m.end());
    assert(m.begin() < m.end() || m.empty());

    assert(static_cast<typename T::size_type>(s.cend() - s.cbegin()) == s.size());
}

template <class T>
void assert_reversible_container_requirements(const T& s) {
    static_assert(is_same_v<reverse_iterator<typename T::iterator>, typename T::reverse_iterator>);
    static_assert(is_same_v<reverse_iterator<typename T::const_iterator>, typename T::const_reverse_iterator>);
    static_assert(is_same_v<decltype(T{}.rbegin()), typename T::reverse_iterator>);
    static_assert(is_same_v<decltype(T{}.rend()), typename T::reverse_iterator>);
    static_assert(is_same_v<decltype(s.crbegin()), typename T::const_reverse_iterator>);
    static_assert(is_same_v<decltype(s.crend()), typename T::const_reverse_iterator>);
    static_assert(is_convertible_v<typename T::reverse_iterator, typename T::const_reverse_iterator>);
}

template <class T>
void assert_all_requirements_and_equals(const T& s, const initializer_list<typename T::value_type>& il) {
    assert_container_requirements(s);
    assert_reversible_container_requirements(s);

    auto val_comp = s.value_comp();
    auto begin_it = s.cbegin();
    auto end_it   = s.cend();
    assert(std::is_sorted(begin_it, end_it, val_comp));
    if constexpr (!_Is_specialization_v<T, flat_multiset>) {
        if (!s.empty()) {
            auto it = begin_it;
            while (++it != end_it) {
                assert(val_comp(*(it - 1), *it));
            }
        }
    }

    if (s.size() != il.size() || !std::equal(s.begin(), s.end(), il.begin())) {
        cout << "Expected: {";
        for (auto&& e : il) {
            cout << e << ", ";
        }
        cout << "}" << endl;
        cout << "Got:      {";
        for (auto&& e : s) {
            cout << e << ", ";
        }
        cout << "}" << endl;
        assert(false);
    }
}

template <class T>
void assert_basic() {
    T s{3, 2, 2, 2, 1};
    assert_all_requirements_and_equals(s, {1, 2, 3});

    s.insert(43);
    assert_all_requirements_and_equals(s, {1, 2, 3, 43});

    int my_ints[] = {1, 2, 3, 4, 55};
    s.insert_range(my_ints);
    assert_all_requirements_and_equals(s, {1, 2, 3, 4, 43, 55});
}

template <class C>
void test_constructors() {
    using lt = std::less<int>;
    using gt = std::greater<int>;

    assert_all_requirements_and_equals(flat_set<int, lt, C>(), {});
    assert_all_requirements_and_equals(flat_multiset<int, lt, C>(), {});
    assert_all_requirements_and_equals(flat_set<int, lt, C>(C{3, 7, 1, 85, 222, 1}), {1, 3, 7, 85, 222});
    assert_all_requirements_and_equals(flat_multiset<int, lt, C>(C{3, 7, 1, 85, 7, 222, 1}), {1, 1, 3, 7, 7, 85, 222});
    assert_all_requirements_and_equals(flat_set<int, gt, C>(C{1, 2, 3, 3}, gt()), {3, 2, 1});
    assert_all_requirements_and_equals(flat_multiset<int, gt, C>(C{1, 1, 2, 3}, gt()), {3, 2, 1, 1});
    assert_all_requirements_and_equals(flat_set<int, gt, C>(sorted_unique, C{30000, 200, 1}, gt()), {30000, 200, 1});
    assert_all_requirements_and_equals(flat_multiset<int, gt, C>(sorted_equivalent, C{3, 3, -1}, gt()), {3, 3, -1});
    assert_all_requirements_and_equals(flat_set<int, gt, C>({30000, 200, 1}, gt()), {30000, 200, 1});
    assert_all_requirements_and_equals(flat_multiset<int, gt, C>({3, 3, -1}, gt()), {3, 3, -1});
    assert_all_requirements_and_equals(flat_set<int, gt, C>(sorted_unique, {30000, 200, 1}, gt()), {30000, 200, 1});
    assert_all_requirements_and_equals(flat_multiset<int, gt, C>(sorted_equivalent, {3, 3, -1}, gt()), {3, 3, -1});

    flat_set<int> a{};
    a = {1, 7, 7, 7, 2, 100, -1};
    assert_all_requirements_and_equals(a, {-1, 1, 2, 7, 100});
    flat_multiset<int> b{};
    b = {1, 7, 7, 7, 2, 100, -1};
    assert_all_requirements_and_equals(b, {-1, 1, 2, 7, 7, 7, 100});
}

template <class T>
void test_spaceship_operator() {
    static constexpr bool multi  = _Is_specialization_v<T, flat_multiset>;
    static constexpr bool invert = is_same_v<typename T::key_compare, std::greater<typename T::key_type>>;

    T a{3, 2, 2, 1};
    T b{1, 2, 3};
    assert((a <=> b) == (multi ? (invert ? strong_ordering::greater : strong_ordering::less) : strong_ordering::equal));

    T c{3, 2};
    assert((c <=> b) == (invert ? strong_ordering::less : strong_ordering::greater));

    T d{5, 6, 7, 7, 8, 9};
    T e{5, 6, 7, 8, 100};
    assert((d <=> e) == strong_ordering::less);

    T f{1, 2, 3, 4};
    assert((f <=> a) == strong_ordering::greater);
}

template <class T>
struct proxy_comparer {
    bool operator()(const T& lhs, const T& rhs) const {
        return m_less ? (lhs < rhs) : (lhs > rhs);
    }

    bool m_less = true;
};

void test_non_static_comparer() {
    flat_set<int, proxy_comparer<int>> a{3, 2, 2, 1};
    assert_all_requirements_and_equals(a, {1, 2, 3});
    auto b = flat_set<int, proxy_comparer<int>>({-1, 5, 9, 9, 9, 9, 9}, proxy_comparer<int>{.m_less = false});
    assert_all_requirements_and_equals(b, {9, 5, -1});

    auto aBackup = a;
    a            = b;
    assert_all_requirements_and_equals(a, {9, 5, -1});
    a.insert_range(vector{7, 7, 3, 3, 2});
    assert_all_requirements_and_equals(a, {9, 7, 5, 3, 2, -1});

    a = std::move(aBackup);
    assert_all_requirements_and_equals(a, {1, 2, 3});

    a.insert(-100);
    assert_all_requirements_and_equals(a, {-100, 1, 2, 3});

    a = b;
    assert_all_requirements_and_equals(a, {9, 5, -1});

    a.insert(7);
    assert_all_requirements_and_equals(a, {9, 7, 5, -1});
}

template <class C>
void test_extract() {
    constexpr int elements[]{1, 2, 3, 4};
    C fs{1, 2, 3, 4};
    auto cont = std::move(fs).extract();
    assert(fs.empty());
    assert(ranges::equal(cont, elements));
}

template <class C>
void test_erase_if() {
    constexpr int erased_result[]{1, 3};
    C fs{1, 2, 3, 4};
    erase_if(fs, [](int n) { return n % 2 == 0; });
    assert(fs.size() == 2);
    assert(ranges::equal(fs, erased_result));
}

int main() {
    test_spaceship_operator<flat_set<int>>();
    test_spaceship_operator<flat_multiset<int>>();
    test_spaceship_operator<flat_set<int, std::greater<int>>>();
    test_spaceship_operator<flat_multiset<int, std::greater<int>>>();
    test_spaceship_operator<flat_set<int, std::less<int>, deque<int>>>();
    test_spaceship_operator<flat_multiset<int, std::less<int>, deque<int>>>();
    test_spaceship_operator<flat_set<int, std::greater<int>, deque<int>>>();
    test_spaceship_operator<flat_multiset<int, std::greater<int>, deque<int>>>();

    test_constructors<vector<int>>();
    test_constructors<deque<int>>();

    test_non_static_comparer();

    test_extract<flat_set<int>>();
    test_extract<flat_multiset<int>>();

    test_erase_if<flat_set<int>>();
    test_erase_if<flat_multiset<int>>();

    assert_basic<flat_set<int>>();
    assert_basic<flat_set<int, std::less<int>, deque<int>>>();

    flat_multiset<int, std::less<int>, deque<int>> d;
}
