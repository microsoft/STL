// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <climits>
#include <cstdlib>
#include <deque>
#include <flat_set>
#include <functional>
#include <iostream>
#include <memory>
#include <random>
#include <ranges>
#include <tuple>
#include <utility>
#include <vector>

#define TEST_ASSERT(...) assert((__VA_ARGS__))

using namespace std;
using namespace ranges;

template <class R, class T>
concept container_compatible_range = input_range<R> && convertible_to<range_reference_t<R>, T>;

template <class T, class Alloc = allocator<T>>
class alternative_vector : private vector<T, Alloc> { // not allocator-aware, but can be used-allocator constructed
private:
    using base_type = vector<T, Alloc>;

public:
    using allocator_type         = base_type::allocator_type;
    using const_iterator         = base_type::const_iterator;
    using const_pointer          = base_type::const_pointer;
    using const_reference        = base_type::const_reference;
    using const_reverse_iterator = base_type::const_reverse_iterator;
    using difference_type        = base_type::difference_type;
    using iterator               = base_type::iterator;
    using pointer                = base_type::pointer;
    using reference              = base_type::reference;
    using reverse_iterator       = base_type::reverse_iterator;
    using size_type              = base_type::size_type;
    using value_type             = base_type::value_type;

    constexpr alternative_vector() noexcept(noexcept(Alloc())) : base_type(Alloc()) {}
    constexpr explicit alternative_vector(allocator_arg_t, const Alloc& a) : base_type(a) {}
    constexpr explicit alternative_vector(size_type n) : base_type(n) {}
    constexpr explicit alternative_vector(allocator_arg_t, const Alloc& a, size_type n) : base_type(n, a) {}
    constexpr alternative_vector(size_type n, const T& v) : base_type(n, v) {}
    constexpr alternative_vector(allocator_arg_t, const Alloc& a, size_type n, const T& v) : base_type(n, v, a) {}
    template <class InputIt>
    constexpr alternative_vector(InputIt first, InputIt last) : base_type(first, last) {}
    template <class InputIt>
    constexpr alternative_vector(allocator_arg_t, const Alloc& a, InputIt first, InputIt last)
        : base_type(first, last, a) {}
    template <container_compatible_range<T> R>
    constexpr alternative_vector(from_range_t, R&& rg) : base_type(from_range, forward<R>(rg)) {}
    template <container_compatible_range<T> R>
    constexpr alternative_vector(allocator_arg_t, const Alloc& a, from_range_t, R&& rg)
        : base_type(from_range, forward<R>(rg), a) {}

    constexpr alternative_vector(allocator_arg_t, const type_identity_t<Alloc>& a, const alternative_vector& other)
        : base_type(other, a) {}
    constexpr alternative_vector(allocator_arg_t, const type_identity_t<Alloc>& a, alternative_vector&& other)
        : base_type(std::move(other), a) {}
    constexpr alternative_vector(initializer_list<T> il) : base_type(il) {}
    constexpr alternative_vector(allocator_arg_t, const Alloc& a, initializer_list<T> il) : base_type(il, a) {}

    alternative_vector(const alternative_vector&) = default;
    alternative_vector(alternative_vector&&)      = default;

    alternative_vector& operator=(const alternative_vector&) = default;
    alternative_vector& operator=(alternative_vector&&)      = default;
    constexpr alternative_vector& operator=(initializer_list<T> il) {
        base_type::operator=(il);
        return *this;
    }

    using base_type::assign;
    using base_type::assign_range;
    using base_type::get_allocator;

    using base_type::begin;
    using base_type::end;
    using base_type::rbegin;
    using base_type::rend;

    using base_type::cbegin;
    using base_type::cend;
    using base_type::crbegin;
    using base_type::crend;

    using base_type::capacity;
    using base_type::empty;
    using base_type::max_size;
    using base_type::reserve;
    using base_type::resize;
    using base_type::shrink_to_fit;
    using base_type::size;

    using base_type::operator[];
    using base_type::at;
    using base_type::back;
    using base_type::front;

    using base_type::data;

    using base_type::append_range;
    using base_type::emplace_back;
    using base_type::pop_back;
    using base_type::push_back;

    using base_type::emplace;
    using base_type::erase;
    using base_type::insert;
    using base_type::insert_range;
    using base_type::swap;

    using base_type::clear;

    friend auto operator<=>(const alternative_vector&, const alternative_vector&) = default;

    friend constexpr void swap(alternative_vector& lhs, alternative_vector& rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }
};

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
    static_assert(is_same_v<decltype(s.rbegin()), typename T::const_reverse_iterator>);
    static_assert(is_same_v<decltype(s.rend()), typename T::const_reverse_iterator>);
    static_assert(is_same_v<decltype(s.crbegin()), typename T::const_reverse_iterator>);
    static_assert(is_same_v<decltype(s.crend()), typename T::const_reverse_iterator>);
    static_assert(is_convertible_v<typename T::reverse_iterator, typename T::const_reverse_iterator>);
}

template <class T>
void assert_noexcept_requirements(T& s) {
    static_assert(noexcept(s.begin()));
    static_assert(noexcept(s.end()));
    static_assert(noexcept(s.cbegin()));
    static_assert(noexcept(s.cend()));
    static_assert(noexcept(s.rbegin()));
    static_assert(noexcept(s.rend()));
    static_assert(noexcept(s.crbegin()));
    static_assert(noexcept(s.crend()));

    static_assert(noexcept(s.empty()));
    static_assert(noexcept(s.size()));
    static_assert(noexcept(s.max_size()));

    if constexpr (!is_const_v<T>) {
        static_assert(noexcept(s.swap(s)));
        static_assert(noexcept(ranges::swap(s, s))); // using ADL-swap
        static_assert(noexcept(s.clear()));
    }
}

template <class T>
void assert_all_requirements(const T& s) {
    assert_container_requirements(s);
    assert_reversible_container_requirements(s);

    assert_noexcept_requirements(s);
    assert_noexcept_requirements(const_cast<T&>(s));

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
}

template <class T>
void assert_all_requirements_and_equals(const T& s, const initializer_list<typename T::value_type>& il) {
    assert_all_requirements(s);

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

void test_allocator_extended_constructors() {
    constexpr allocator<int> ator;
    constexpr std::less<int> comp;
    {
        using fs = flat_set<int, std::less<int>, alternative_vector<int>>;

        fs s0{1, 1, 2, 3, 5, 8};
        alternative_vector<int> v{1, 1, 2, 3, 5, 8};
        alternative_vector<int> v2{1, 2, 3, 5, 8};

        TEST_ASSERT(fs{comp, ator} == fs{});

        TEST_ASSERT(fs{s0, ator} == s0);
        TEST_ASSERT(fs{fs{s0}, ator} == s0);

        TEST_ASSERT(fs{v, ator} == s0);
        TEST_ASSERT(fs{{1, 1, 2, 3, 5, 8}, ator} == s0);
        TEST_ASSERT(fs{v.begin(), v.end(), ator} == s0);
        TEST_ASSERT(fs{from_range, v, ator} == s0);

        TEST_ASSERT(fs{v, comp, ator} == s0);
        TEST_ASSERT(fs{{1, 1, 2, 3, 5, 8}, comp, ator} == s0);
        TEST_ASSERT(fs{v.begin(), v.end(), comp, ator} == s0);
        TEST_ASSERT(fs{from_range, v, comp, ator} == s0);

        TEST_ASSERT(fs{sorted_unique, v2, ator} == s0);
        TEST_ASSERT(fs{sorted_unique, {1, 2, 3, 5, 8}, ator} == s0);
        TEST_ASSERT(fs{sorted_unique, v2.begin(), v2.end(), ator} == s0);

        TEST_ASSERT(fs{sorted_unique, v2, comp, ator} == s0);
        TEST_ASSERT(fs{sorted_unique, {1, 2, 3, 5, 8}, comp, ator} == s0);
        TEST_ASSERT(fs{sorted_unique, v2.begin(), v2.end(), comp, ator} == s0);
    }
    {
        using fms = flat_multiset<int, std::less<int>, alternative_vector<int>>;

        fms s0{1, 1, 2, 3, 5, 8};
        alternative_vector<int> v{1, 1, 2, 3, 5, 8};

        TEST_ASSERT(fms{comp, ator} == fms{});

        TEST_ASSERT(fms{s0, ator} == s0);
        TEST_ASSERT(fms{fms{s0}, ator} == s0);

        TEST_ASSERT(fms{v, ator} == s0);
        TEST_ASSERT(fms{{1, 1, 2, 3, 5, 8}, ator} == s0);
        TEST_ASSERT(fms{v.begin(), v.end(), ator} == s0);
        TEST_ASSERT(fms{from_range, v, ator} == s0);

        TEST_ASSERT(fms{v, comp, ator} == s0);
        TEST_ASSERT(fms{{1, 1, 2, 3, 5, 8}, comp, ator} == s0);
        TEST_ASSERT(fms{v.begin(), v.end(), comp, ator} == s0);
        TEST_ASSERT(fms{from_range, v, comp, ator} == s0);

        TEST_ASSERT(fms{sorted_equivalent, v, ator} == s0);
        TEST_ASSERT(fms{sorted_equivalent, {1, 1, 2, 3, 5, 8}, ator} == s0);
        TEST_ASSERT(fms{sorted_equivalent, v.begin(), v.end(), ator} == s0);

        TEST_ASSERT(fms{sorted_equivalent, v, comp, ator} == s0);
        TEST_ASSERT(fms{sorted_equivalent, {1, 1, 2, 3, 5, 8}, comp, ator} == s0);
        TEST_ASSERT(fms{sorted_equivalent, v.begin(), v.end(), comp, ator} == s0);
    }
}

template <template <class...> class Set>
void test_always_reversible() {
    // Test that flat_meow is unconditionally reversible.
    class not_reversible : public vector<int> {
    private:
        using base = vector<int>;

        using base::const_reverse_iterator;
        using base::crbegin;
        using base::crend;
        using base::rbegin;
        using base::rend;
        using base::reverse_iterator;

    public:
        using base::base;
    };

    Set<int, std::greater<int>, not_reversible> fs({1, 2, 3});
    assert_all_requirements_and_equals(fs, {3, 2, 1});
    assert(fs.rbegin() + 3 == fs.rend());
    assert(fs.crend() - fs.crbegin() == 3);
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

void test_insert_upper_bound() {
    // For flat_multiset's single-element insertion, the key should be inserted before the upper_bound.
    struct test_position {
        int key;
        int extra;
        bool operator==(const test_position&) const = default;
    };

    mt19937 eng(24);
    uniform_int_distribution<int> dist_seq(0, 20);

    vector<test_position> seq(200);
    for (int e = 0; auto& [key, extra] : seq) {
        key   = dist_seq(eng);
        extra = e++;
    }

    flat_multiset<test_position, key_comparer> fs;
    for (const auto& val : seq) {
        fs.insert(val);
    }

    // The result should be identical to as if doing stable_sort on seq.
    ranges::stable_sort(seq, key_comparer{});
    assert(ranges::equal(fs, seq));
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

template <template <class...> class Set>
void test_extract_1() {
    // Test that the container will be emptied, even if the container's move ctor exits via an exception.

    static bool will_throw = false;

    class test_exception : public vector<int> {
    private:
        using base = vector<int>;

    public:
        using base::base;
        test_exception(const test_exception&)            = default;
        test_exception& operator=(const test_exception&) = default;
        test_exception& operator=(test_exception&&)      = default;

        test_exception(test_exception&& other) : base(static_cast<base&&>(other)) {
            if (will_throw) {
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
        will_throw = false;
        assert_all_requirements_and_equals(fs, {}); // assert empty
        return;
    }

    assert(false);
}

template <template <class...> class Set>
void test_extract_2() {
    // Test that the container will be emptied, even if the container's move ctor doesn't empty the container.

    class always_copy : public vector<int> {
    private:
        using base = vector<int>;

    public:
        using base::base;
        always_copy(const always_copy&)            = default;
        always_copy& operator=(const always_copy&) = default;

        // the move ctor and assignment will not empty the container.
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

void test_invariant_robustness() {
    static int copy_limit   = 2;
    constexpr int unlimited = INT_MAX;

    struct odd_key {
        static void countdown() {
            if (copy_limit == unlimited) {
                return;
            }

            if (--copy_limit < 0) {
                throw 0; // will be caught by "catch (...)".
            }
        }

        int key;

        odd_key(int k = 0) : key(k) {}

        bool operator==(const odd_key&) const = default;

        odd_key(const odd_key& other) {
            countdown();
            key = other.key;
        }

        odd_key(odd_key&& other) {
            countdown();
            key = exchange(other.key, 0);
        }

        odd_key& operator=(const odd_key& other) {
            countdown();
            key = other.key;
            return *this;
        }

        odd_key& operator=(odd_key&& other) {
            countdown();
            key = exchange(other.key, 0);
            return *this;
        }
    };

    class odd_container : public vector<odd_key> {
    private:
        using base = vector<odd_key>;

    public:
        using base::base;
        odd_container(const odd_container&) = default;

        // this copy-assignment cannot provide strong-guarantee for `this`:
        odd_container& operator=(const odd_container& other) {
            resize(other.size());
            std::copy(other.begin(), other.end(), begin());
            return *this;
        }

        // this move-ctor cannot provide strong-guarantee for `other`, and even successful, will leave elements of
        // `other` in moved-from state:
        odd_container(odd_container&& other) {
            reserve(other.size());
            for (auto& e : other) {
                push_back(std::move(e));
            }
        }

        // this move-assignment cannot provide strong-guarantee for `this` and `other`, and even successful, will leave
        // elements of `other` in moved-from state:
        odd_container& operator=(odd_container&& other) {
            resize(other.size());
            std::move(other.begin(), other.end(), begin());
            return *this;
        }
    };

    using SetT = flat_set<odd_key, key_comparer, odd_container>;

    // copy-assignment
    {
        copy_limit = unlimited;
        SetT fs1{0, 1, 2, 3, 4};
        SetT fs2{5, 6, 7, 8, 9};

        assert(ranges::equal(fs1, vector{0, 1, 2, 3, 4}, {}, &odd_key::key));
        assert(ranges::equal(fs2, vector{5, 6, 7, 8, 9}, {}, &odd_key::key));

        bool caught = false;
        try {
            copy_limit = 2;
            fs1        = fs2; // will throw after copying 2 odd_key.
        } catch (...) {
            copy_limit = unlimited;
            assert_all_requirements(fs1);
            caught = true;
        }
        assert(caught);
    }
    // move-ctor
    {
        copy_limit = unlimited;
        SetT fs1{0, 1, 2, 3, 4};
        SetT fs2{std::move(fs1)};

        assert_all_requirements(fs1);
        assert(ranges::equal(fs2, vector{0, 1, 2, 3, 4}, {}, &odd_key::key));

        bool caught = false;
        try {
            copy_limit = 2;
            SetT fs3{std::move(fs2)}; // will throw after moving 2 odd_key.
        } catch (...) {
            copy_limit = unlimited;
            assert_all_requirements(fs2);
            caught = true;
        }
        assert(caught);
    }
    // move-assignment
    {
        copy_limit = unlimited;
        SetT fs1{0, 1, 2, 3, 4};
        SetT fs2;
        SetT fs3{5, 6, 7, 8, 9};
        fs2 = std::move(fs1);

        assert_all_requirements(fs1);
        assert(ranges::equal(fs2, vector{0, 1, 2, 3, 4}, {}, &odd_key::key));
        assert(ranges::equal(fs3, vector{5, 6, 7, 8, 9}, {}, &odd_key::key));

        bool caught = false;
        try {
            copy_limit = 2;
            fs2        = std::move(fs3); // will throw after moving 2 odd_key.
        } catch (...) {
            copy_limit = unlimited;
            assert_all_requirements(fs2);
            assert_all_requirements(fs3);
            caught = true;
        }
        assert(caught);
    }
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

template <template <class...> class Set>
void test_observers() {
    struct lt_with_state {
        int state = 0;
        bool operator()(int l, int r) const {
            return l < r;
        }
    };

    using SetT = Set<int, lt_with_state>;

    SetT fs;
    static_assert(is_same_v<typename SetT::key_compare, typename SetT::value_compare>);
    static_assert(is_same_v<decltype(as_const(fs).key_comp()), typename SetT::key_compare>);
    static_assert(is_same_v<decltype(as_const(fs).value_comp()), typename SetT::value_compare>);
    assert(fs.key_comp().state == 0);
    assert(fs.value_comp().state == 0);

    SetT fs2(lt_with_state{2});
    assert(fs2.key_comp().state == 2);
    assert(fs2.value_comp().state == 2);
}

template <template <class...> class Set>
void test_set_operations() {
    using SetT           = Set<int>;
    using iterator       = SetT::iterator;
    using const_iterator = SetT::const_iterator;

    SetT fs{3, 2, 11, 11, 3, 8, 11, 20};

    static_assert(is_same_v<decltype(fs.find(0)), iterator>);
    static_assert(is_same_v<decltype(as_const(fs).find(0)), const_iterator>);

    static_assert(is_same_v<decltype(as_const(fs).count(0)), typename SetT::size_type>);
    static_assert(is_same_v<decltype(as_const(fs).contains(0)), bool>);

    static_assert(is_same_v<decltype(fs.lower_bound(0)), iterator>);
    static_assert(is_same_v<decltype(as_const(fs).lower_bound(0)), const_iterator>);
    static_assert(is_same_v<decltype(fs.upper_bound(0)), iterator>);
    static_assert(is_same_v<decltype(as_const(fs).upper_bound(0)), const_iterator>);

    static_assert(is_same_v<decltype(fs.equal_range(0)), pair<iterator, iterator>>);
    static_assert(is_same_v<decltype(as_const(fs).equal_range(0)), pair<const_iterator, const_iterator>>);

    if constexpr (_Is_specialization_v<SetT, flat_set>) {
        // flat_set:
        assert_all_requirements_and_equals(fs, {2, 3, 8, 11, 20});

        assert(fs.find(3) != fs.end());
        assert(fs.find(4) == fs.end());

        assert(fs.count(1) == 0);
        assert(fs.count(11) == 1);
        assert(fs.contains(8));
        assert(!fs.contains(12));

        assert(fs.lower_bound(-1) == fs.begin());
        assert(fs.lower_bound(3) == fs.find(3));
        assert(fs.lower_bound(19) == fs.find(20));
        assert(fs.lower_bound(20) + 1 == fs.end());

        assert(fs.upper_bound(-1) == fs.begin());
        assert(fs.upper_bound(20) == fs.end());
        assert(fs.lower_bound(2) + 2 == fs.upper_bound(3));
        assert(fs.upper_bound(8) == fs.find(11));

        auto [first, last] = fs.equal_range(3);
        assert(first + 1 == last);
        tie(first, last) = fs.equal_range(12);
        assert(first == last);
    } else {
        // flat_multiset:
        assert_all_requirements_and_equals(fs, {2, 3, 3, 8, 11, 11, 11, 20});

        assert(fs.find(3) != fs.end());
        assert(fs.find(4) == fs.end());

        assert(fs.count(1) == 0);
        assert(fs.count(11) == 3);
        assert(fs.contains(8));
        assert(!fs.contains(12));

        assert(fs.lower_bound(-1) == fs.begin());
        assert(fs.lower_bound(3) == fs.find(3));
        assert(fs.lower_bound(19) == fs.find(20));
        assert(fs.lower_bound(11) + 4 == fs.end());

        assert(fs.upper_bound(-1) == fs.begin());
        assert(fs.upper_bound(20) == fs.end());
        assert(fs.lower_bound(3) + 6 == fs.upper_bound(11));
        assert(fs.upper_bound(11) == fs.find(20));

        auto [first, last] = fs.equal_range(3);
        assert(first + 2 == last);
        tie(first, last) = fs.equal_range(12);
        assert(first == last);
    }
}

template <template <class...> class Set>
void test_set_operations_transparent() {
    struct shouldnt_convert {
        int key;
        /* implicit */ [[noreturn]] operator int() const {
            abort();
        }
    };

    Set<int, key_comparer> fs{0, 3, 5};
    assert_all_requirements_and_equals(fs, {0, 3, 5});

    assert(fs.find(shouldnt_convert{0}) != fs.end());
    assert(fs.count(shouldnt_convert{3}) == 1);
    assert(!fs.contains(shouldnt_convert{1}));
    assert(fs.lower_bound(shouldnt_convert{-1}) == fs.begin());
    assert(fs.lower_bound(shouldnt_convert{8}) == fs.end());
    assert(fs.upper_bound(shouldnt_convert{2}) == fs.find(3));
    auto [first, last] = fs.equal_range(shouldnt_convert{5});
    assert(first + 1 == last);
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
    test_allocator_extended_constructors();

    test_always_reversible<flat_set>();
    test_always_reversible<flat_multiset>();

    test_insert_1<vector<int>>();
    test_insert_1<deque<int>>();
    test_insert_2<vector<int>>();
    test_insert_2<deque<int>>();
    test_insert_transparent();
    test_insert_using_invalid_hint();
    test_insert_upper_bound();

    test_comparer_application();
    test_non_static_comparer();

    test_extract_1<flat_set>();
    test_extract_1<flat_multiset>();
    test_extract_2<flat_set>();
    test_extract_2<flat_multiset>();

    test_erase_1();
    test_erase_2();
    test_invariant_robustness();

    test_erase_if<flat_set<int>>();
    test_erase_if<flat_multiset<int>>();

    test_observers<flat_set>();
    test_observers<flat_multiset>();

    test_set_operations<flat_set>();
    test_set_operations<flat_multiset>();
    test_set_operations_transparent<flat_set>();
    test_set_operations_transparent<flat_multiset>();
}
