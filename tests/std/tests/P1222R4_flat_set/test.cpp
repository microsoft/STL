// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <deque>
#include <flat_set>
#include <functional>
#include <iostream>
#include <memory>
#include <random>
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

    if (!std::equal(s.begin(), s.end(), il.begin(), il.end())) {
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
    assert_all_requirements_and_equals(flat_set<int>(a, allocator<int>{}), {-1, 1, 2, 7, 100});
    assert_all_requirements_and_equals(flat_set<int>(std::move(a), allocator<int>{}), {-1, 1, 2, 7, 100});
    flat_multiset<int> b{};
    b = {1, 7, 7, 7, 2, 100, -1};
    assert_all_requirements_and_equals(b, {-1, 1, 2, 7, 7, 7, 100});
    assert_all_requirements_and_equals(flat_multiset<int>(b, allocator<int>{}), {-1, 1, 2, 7, 7, 7, 100});
    assert_all_requirements_and_equals(flat_multiset<int>(std::move(b), allocator<int>{}), {-1, 1, 2, 7, 7, 7, 100});
}

template <class C>
void test_insert_1() {
    using lt = std::less<int>;

    const vector<int> vec{0, 1, 2};
    {
        flat_set<int, lt, C> a{5, 5};
        assert_all_requirements_and_equals(a, {5});
        a.emplace();
        assert_all_requirements_and_equals(a, {0, 5});
        a.emplace(1);
        assert_all_requirements_and_equals(a, {0, 1, 5});
        a.insert(vec[2]);
        assert_all_requirements_and_equals(a, {0, 1, 2, 5});
        a.insert(2);
        assert_all_requirements_and_equals(a, {0, 1, 2, 5});
        a.insert(vec.rbegin(), vec.rend());
        assert_all_requirements_and_equals(a, {0, 1, 2, 5});
        a.insert(sorted_unique, vec.begin(), vec.end());
        assert_all_requirements_and_equals(a, {0, 1, 2, 5});
        a.insert_range(vec);
        assert_all_requirements_and_equals(a, {0, 1, 2, 5});
        a.insert({6, 2, 3});
        assert_all_requirements_and_equals(a, {0, 1, 2, 3, 5, 6});
        a.insert(sorted_unique, {4, 5});
        assert_all_requirements_and_equals(a, {0, 1, 2, 3, 4, 5, 6});
    }
    {
        flat_multiset<int, lt, C> a{5, 5};
        assert_all_requirements_and_equals(a, {5, 5});
        a.emplace();
        assert_all_requirements_and_equals(a, {0, 5, 5});
        a.emplace(1);
        assert_all_requirements_and_equals(a, {0, 1, 5, 5});
        a.insert(vec[2]);
        assert_all_requirements_and_equals(a, {0, 1, 2, 5, 5});
        a.insert(2);
        assert_all_requirements_and_equals(a, {0, 1, 2, 2, 5, 5});
        a.insert(vec.rbegin(), vec.rend());
        assert_all_requirements_and_equals(a, {0, 0, 1, 1, 2, 2, 2, 5, 5});
        a.insert(sorted_equivalent, vec.begin(), vec.end());
        assert_all_requirements_and_equals(a, {0, 0, 0, 1, 1, 1, 2, 2, 2, 2, 5, 5});
        a.insert_range(vec);
        assert_all_requirements_and_equals(a, {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 5, 5});
        a.insert({6, 2, 3});
        assert_all_requirements_and_equals(a, {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 5, 5, 6});
        a.insert(sorted_equivalent, {4, 5});
        assert_all_requirements_and_equals(a, {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 4, 5, 5, 5, 6});
    }
}

template <class C>
void test_insert_2() {
    using lt = std::less<int>;

    const int val = 1;
    {
        flat_set<int, lt, C> a{0, 5};
        assert_all_requirements_and_equals(a, {0, 5});
        a.emplace_hint(a.end());
        assert_all_requirements_and_equals(a, {0, 5});
        a.emplace_hint(a.end(), 0);
        assert_all_requirements_and_equals(a, {0, 5});
        a.insert(a.begin(), 6);
        assert_all_requirements_and_equals(a, {0, 5, 6});
        a.insert(a.begin(), val);
        assert_all_requirements_and_equals(a, {0, 1, 5, 6});
    }
    {
        flat_multiset<int, lt, C> a{0, 5};
        assert_all_requirements_and_equals(a, {0, 5});
        a.emplace_hint(a.end());
        assert_all_requirements_and_equals(a, {0, 0, 5});
        a.emplace_hint(a.end(), 0);
        assert_all_requirements_and_equals(a, {0, 0, 0, 5});
        a.insert(a.begin(), 6);
        assert_all_requirements_and_equals(a, {0, 0, 0, 5, 6});
        a.insert(a.begin(), val);
        assert_all_requirements_and_equals(a, {0, 0, 0, 1, 5, 6});
    }

    // TRANSITION, too simple
    using gt = std::greater<int>;
    {
        flat_set<int, gt, C> a{0, 5};
        assert_all_requirements_and_equals(a, {5, 0});
        a.insert(a.begin(), 3);
        assert_all_requirements_and_equals(a, {5, 3, 0});
        a.insert(a.end(), 4);
        assert_all_requirements_and_equals(a, {5, 4, 3, 0});
    }
}

struct key_comparer {
    const auto& extract_key(const auto& obj) const {
        if constexpr (requires { obj.key; }) {
            return obj.key;
        } else {
            return obj;
        }
    }

    bool operator()(const auto& lhs, const auto& rhs) const {
        return extract_key(lhs) < extract_key(rhs);
    }

    using is_transparent = int;
};

void test_comparer_application() {
    // The set must rely on its comparer to do the comparisons.
    struct incomparable {
        int key;
        bool operator<(const incomparable&) const  = delete;
        bool operator==(const incomparable&) const = delete;
    };

    flat_set<incomparable, key_comparer> fs{{0}, {3}, {1}, {0}, {5}};
    assert(fs.contains(0));
    assert(!fs.contains(2));
    fs.insert(fs.begin(), incomparable{4});
    fs.insert(2);
    assert(fs.contains(4));
    assert(fs.contains(incomparable{2}));

    assert(fs.lower_bound(3) == fs.lower_bound(incomparable{3}));
    fs.erase(2);
    assert(!fs.contains(incomparable{2}));
}

void test_insert_transparent() {
    // For flat_set::insert([hint,]auto&&), the input should be unchanged if the set already
    // contains an equivalent element.
    struct detect_conversion {
        int key;
        mutable bool converted = false;

        explicit operator int() const {
            converted = true;
            return key;
        }
    };

    flat_set<int, key_comparer> fs{0, 3, 5};
    assert_all_requirements_and_equals(fs, {0, 3, 5});
    detect_conversion detector{3};

    assert(!detector.converted);
    fs.insert(detector /*3*/);
    assert_all_requirements_and_equals(fs, {0, 3, 5});
    assert(!detector.converted);

    detector.key = 1;

    assert(!detector.converted);
    fs.insert(detector /*1*/);
    assert_all_requirements_and_equals(fs, {0, 1, 3, 5});
    assert(detector.converted);

    detector.converted = false;

    assert(!detector.converted);
    fs.insert(fs.end(), detector /*1*/);
    assert_all_requirements_and_equals(fs, {0, 1, 3, 5});
    assert(!detector.converted);

    detector.key = 2;

    assert(!detector.converted);
    fs.insert(fs.begin(), detector /*2*/);
    assert_all_requirements_and_equals(fs, {0, 1, 2, 3, 5});
    assert(detector.converted);
}

void test_insert_using_invalid_hint() {
    mt19937 eng(42);

    uniform_int_distribution<int> dist_seq(0, 20);

    vector<int> seq(200);
    for (int& val : seq) {
        val = dist_seq(eng);
    }

    {
        flat_multiset<int> with_hint;
        flat_multiset<int> no_hint;
        for (const int val : seq) {
            uniform_int_distribution<int> dist_idx(0, static_cast<int>(with_hint.size()));
            auto random_hint = with_hint.begin() + dist_idx(eng);
            with_hint.insert(random_hint, val);
            no_hint.insert(val);
        }

        assert(with_hint == no_hint);
    }

    {
        flat_set<int> with_hint;
        flat_set<int> no_hint;
        for (const int val : seq) {
            uniform_int_distribution<int> dist_idx(0, static_cast<int>(with_hint.size()));
            auto random_hint = with_hint.begin() + dist_idx(eng);
            with_hint.insert(random_hint, val);
            no_hint.insert(val);
        }

        assert(with_hint == no_hint);
    }
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

template <template <class, class, class> class Set>
void test_extract_1() {
    // Test that the container will be emptied, regardless of whether an exception is thrown.

    static bool will_throw = false;

    class test_exception : public vector<int> {
        using base = vector<int>;

    public:
        using base::base;
        test_exception(const test_exception&)            = default;
        test_exception& operator=(const test_exception&) = default;
        test_exception& operator=(test_exception&&)      = default;

        test_exception(test_exception&& other) : base(static_cast<base&&>(other)) {
            if (will_throw) {
                will_throw = false;
                throw 0; // will be caught by "catch (...)"
            }
        }
    };

    will_throw = false;
    Set<int, std::less<int>, test_exception> fs{4, 3, 2, 1};
    assert_all_requirements_and_equals(fs, {1, 2, 3, 4});
    auto extr = std::move(fs).extract();
    assert(ranges::equal(extr, vector{1, 2, 3, 4}));
    assert_all_requirements_and_equals(fs, {}); // assert empty

    fs = {4, 3, 2, 1};
    assert_all_requirements_and_equals(fs, {1, 2, 3, 4});
    try {
        will_throw = true;
        (void) std::move(fs).extract();
    } catch (...) {
        assert_all_requirements_and_equals(fs, {}); // assert empty
        return;
    }

    assert(false);
}

template <template <class, class, class> class Set>
void test_extract_2() {
    // Test that the container will be emptied, even if the container's move constructor doesn't empty the container.

    class always_copy : public vector<int> {
        using base = vector<int>;

    public:
        using base::base;
        always_copy(const always_copy&)            = default;
        always_copy& operator=(const always_copy&) = default;

        // the move ctor & assignment will not empty the container.
        always_copy(always_copy&& other) noexcept /* intentional */ : always_copy(as_const(other)) {}
        always_copy& operator=(always_copy&& other) noexcept /* intentional */ {
            return operator=(as_const(other));
        }
    };

    Set<int, std::less<int>, always_copy> fs{4, 3, 2, 1};
    assert_all_requirements_and_equals(fs, {1, 2, 3, 4});
    auto extr = std::move(fs).extract();
    assert(ranges::equal(extr, vector{1, 2, 3, 4}));
    assert_all_requirements_and_equals(fs, {}); // assert empty
}

// TRANSITION, too simple
void test_erase_1() {
    flat_set<int> fs{1};
    fs.erase(1);
    assert_all_requirements_and_equals(fs, {});
}

template <class T>
struct holder {
    T t;
    operator T() && {
        return std::move(t);
    }
};

void test_erase_2() {
    using C = flat_set<int, std::less<>>;
    C fs{0, 1, 2, 3};
    assert_all_requirements_and_equals(fs, {0, 1, 2, 3});
    // this should be allowed per P2077R3:
    fs.erase(holder<C::const_iterator>{fs.cbegin()});
    assert_all_requirements_and_equals(fs, {1, 2, 3});
    fs.erase(holder<C::iterator>{fs.begin()});
    assert_all_requirements_and_equals(fs, {2, 3});
    int i = 2;
    fs.erase(ref(i));
    assert_all_requirements_and_equals(fs, {3});
}

template <class C>
void test_erase_if() {
    constexpr int erased_result[]{1, 3};
    C fs{1, 2, 3, 4};
    erase_if(fs, [](int n) { return n % 2 == 0; });
    assert(fs.size() == 2);
    assert(ranges::equal(fs, erased_result));
}

// TRANSITION, too simple
void test_count() {
    flat_set<int> fs{2};
    assert(fs.count(1) == 0);

    flat_multiset<int> fs2{10, 20, 20, 30};
    assert(fs2.count(20) == 2);
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

    test_insert_1<vector<int>>();
    test_insert_1<deque<int>>();
    test_insert_2<vector<int>>();
    test_insert_2<deque<int>>();
    test_insert_transparent();
    test_insert_using_invalid_hint();

    test_comparer_application();
    test_non_static_comparer();

    test_extract_1<flat_set>();
    test_extract_1<flat_multiset>();
    test_extract_2<flat_set>();
    test_extract_2<flat_multiset>();

    test_erase_1();
    test_erase_2();

    test_erase_if<flat_set<int>>();
    test_erase_if<flat_multiset<int>>();

    test_count();
}
