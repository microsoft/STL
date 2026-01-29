// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstdlib>
#include <deque>
#include <flat_set>
#include <functional>
#include <limits>
#include <memory>
#include <print>
#include <random>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <test_container_requirements.hpp>
#include <test_death.hpp>
#define TEST_ASSERT(...) assert((__VA_ARGS__))

using namespace std;

enum class iterator_pair_construction : bool { no_allocator, with_allocator };

template <class R, class T>
concept container_compatible_range = ranges::input_range<R> && convertible_to<ranges::range_reference_t<R>, T>;

template <class T, class Alloc, iterator_pair_construction Choice>
class alternative_vector : private vector<T, Alloc> { // not allocator-aware, but can be uses-allocator constructed
private:
    using base_type = vector<T, Alloc>;

public:
    using typename base_type::allocator_type;
    using typename base_type::const_iterator;
    using typename base_type::const_pointer;
    using typename base_type::const_reference;
    using typename base_type::const_reverse_iterator;
    using typename base_type::difference_type;
    using typename base_type::iterator;
    using typename base_type::pointer;
    using typename base_type::reference;
    using typename base_type::reverse_iterator;
    using typename base_type::size_type;
    using typename base_type::value_type;

    constexpr alternative_vector() noexcept(noexcept(Alloc())) : base_type(Alloc()) {}
    constexpr alternative_vector(allocator_arg_t, const Alloc& a) : base_type(a) {}
    constexpr explicit alternative_vector(size_type n) : base_type(n) {}
    constexpr explicit alternative_vector(size_type n, const T& v) : base_type(n, v) {}
    template <class InputIt>
    constexpr explicit alternative_vector(InputIt first, InputIt last) : base_type(first, last) {}
    template <class InputIt>
        requires (Choice == iterator_pair_construction::with_allocator)
    constexpr explicit alternative_vector(allocator_arg_t, const Alloc& a, InputIt first, InputIt last)
        : base_type(first, last, a) {}

    template <container_compatible_range<T> R>
    constexpr explicit alternative_vector(from_range_t, R&& rg) : base_type(from_range, forward<R>(rg)) {}

    constexpr alternative_vector(allocator_arg_t, const type_identity_t<Alloc>& a, const alternative_vector& other)
        : base_type(other, a) {}
    constexpr alternative_vector(allocator_arg_t, const type_identity_t<Alloc>& a, alternative_vector&& other)
        : base_type(move(other), a) {}
    constexpr explicit alternative_vector(initializer_list<T> il) : base_type(il) {}

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
void assert_all_requirements(const T& s) {
    assert_container_requirements(s);
    assert_reversible_container_requirements(s);
    assert_three_way_comparability<T>();
    assert_set_requirements<T>();

    assert_noexcept_requirements(s);
    assert_noexcept_requirements(const_cast<T&>(s));

    assert_is_sorted_maybe_unique<_Is_specialization_v<T, flat_set>>(s);
}

template <class T, class U = initializer_list<typename T::value_type>>
void assert_all_requirements_and_equals(const T& s, const U& correct) {
    assert_all_requirements(s);

    if (!ranges::equal(s, correct)) {
        println("Unexpected content!\nExpected {}\nActual {}", correct, s);
        assert(false);
    }
}

template <class C>
void test_constructors() {
    using lt = less<int>;
    using gt = greater<int>;

    // Test flat_set()
    // and  flat_set(const key_compare&)
    assert_all_requirements_and_equals(flat_set<int, lt, C>(), {});
    assert_all_requirements_and_equals(flat_multiset<int, lt, C>(), {});
    assert_all_requirements_and_equals(flat_set<int, lt, C>(lt{}), {});
    assert_all_requirements_and_equals(flat_multiset<int, lt, C>(lt{}), {});

    {
        // Test flat_set(container_type, const key_compare& = key_compare())
        flat_set fs1(C{3, 7, 1, 85, 222, 1});
        flat_multiset fms1(C{3, 7, 1, 85, 7, 222, 1});
        flat_set fs2(C{1, 2, 3, 3}, gt{});
        flat_multiset fms2(C{1, 1, 2, 3}, gt{});
        assert_all_requirements_and_equals(fs1, {1, 3, 7, 85, 222});
        assert_all_requirements_and_equals(fms1, {1, 1, 3, 7, 7, 85, 222});
        assert_all_requirements_and_equals(fs2, {3, 2, 1});
        assert_all_requirements_and_equals(fms2, {3, 2, 1, 1});

        // Test flat_set(const flat_set&)
        // and  flat_set(flat_set&&)
        flat_set fs3(fs1);
        flat_multiset fms3(fms1);
        flat_set fs4(move(fs2));
        flat_multiset fms4(move(fms2));
        assert_all_requirements_and_equals(fs3, {1, 3, 7, 85, 222});
        assert_all_requirements_and_equals(fms3, {1, 1, 3, 7, 7, 85, 222});
        assert_all_requirements_and_equals(fs4, {3, 2, 1});
        assert_all_requirements_and_equals(fms4, {3, 2, 1, 1});
    }

    // Test flat_set(sorted_unique_t, container_type, const key_compare& = key_compare())
    assert_all_requirements_and_equals(flat_set(sorted_unique, C{1, 200, 30000}), {1, 200, 30000});
    assert_all_requirements_and_equals(flat_multiset(sorted_equivalent, C{-1, 3, 3}), {-1, 3, 3});
    assert_all_requirements_and_equals(flat_set(sorted_unique, C{30000, 200, 1}, gt{}), {30000, 200, 1});
    assert_all_requirements_and_equals(flat_multiset(sorted_equivalent, C{3, 3, -1}, gt{}), {3, 3, -1});

    const int arr[] = {2, 7, 18, 28, 18, 28, 45, 90, 45, 23};

    // Test flat_set(InIt, InIt, const key_compare& = key_compare())
    assert_all_requirements_and_equals(flat_set<int, lt, C>(begin(arr), end(arr)), {2, 7, 18, 23, 28, 45, 90});
    assert_all_requirements_and_equals(
        flat_multiset<int, lt, C>(begin(arr), end(arr)), {2, 7, 18, 18, 23, 28, 28, 45, 45, 90});
    assert_all_requirements_and_equals(flat_set<int, gt, C>(begin(arr), end(arr), gt{}), {90, 45, 28, 23, 18, 7, 2});
    assert_all_requirements_and_equals(
        flat_multiset<int, gt, C>(begin(arr), end(arr), gt{}), {90, 45, 45, 28, 28, 23, 18, 18, 7, 2});

    {
        // Test flat_set(sorted_unique_t, InIt, InIt, const key_compare& = key_compare())
        const int ua[] = {10, 20, 30}; // unique ascending
        const int ea[] = {40, 50, 50, 60, 60, 60}; // equivalent ascending
        const int ud[] = {66, 55, 44}; // unique descending
        const int ed[] = {33, 22, 22, 11, 11, 11}; // equivalent descending
        assert_all_requirements_and_equals(flat_set<int, lt, C>(sorted_unique, begin(ua), end(ua)), ua);
        assert_all_requirements_and_equals(flat_multiset<int, lt, C>(sorted_equivalent, begin(ea), end(ea)), ea);
        assert_all_requirements_and_equals(flat_set<int, gt, C>(sorted_unique, begin(ud), end(ud), gt{}), ud);
        assert_all_requirements_and_equals(flat_multiset<int, gt, C>(sorted_equivalent, begin(ed), end(ed), gt{}), ed);
    }

    // Test flat_set(from_range_t, R&&)
    // and  flat_set(from_range_t, R&&, const key_compare&)
    assert_all_requirements_and_equals(flat_set<int, lt, C>(from_range, arr), {2, 7, 18, 23, 28, 45, 90});
    assert_all_requirements_and_equals(
        flat_multiset<int, lt, C>(from_range, arr), {2, 7, 18, 18, 23, 28, 28, 45, 45, 90});
    assert_all_requirements_and_equals(flat_set<int, gt, C>(from_range, arr, gt{}), {90, 45, 28, 23, 18, 7, 2});
    assert_all_requirements_and_equals(
        flat_multiset<int, gt, C>(from_range, arr, gt{}), {90, 45, 45, 28, 28, 23, 18, 18, 7, 2});

    // Test flat_set(initializer_list<value_type>, const key_compare& = key_compare())
    assert_all_requirements_and_equals(flat_set<int, lt, C>({3, 7, 1, 85, 222, 1}), {1, 3, 7, 85, 222});
    assert_all_requirements_and_equals(flat_multiset<int, lt, C>({3, 7, 1, 85, 7, 222, 1}), {1, 1, 3, 7, 7, 85, 222});
    assert_all_requirements_and_equals(flat_set<int, gt, C>({1, 2, 3, 3}, gt{}), {3, 2, 1});
    assert_all_requirements_and_equals(flat_multiset<int, gt, C>({1, 1, 2, 3}, gt{}), {3, 2, 1, 1});

    // Test flat_set(sorted_unique_t, initializer_list<value_type>, const key_compare& = key_compare())
    assert_all_requirements_and_equals(flat_set<int, lt, C>(sorted_unique, {1, 200, 30000}), {1, 200, 30000});
    assert_all_requirements_and_equals(flat_multiset<int, lt, C>(sorted_equivalent, {-1, 3, 3}), {-1, 3, 3});
    assert_all_requirements_and_equals(flat_set<int, gt, C>(sorted_unique, {30000, 200, 1}, gt{}), {30000, 200, 1});
    assert_all_requirements_and_equals(flat_multiset<int, gt, C>(sorted_equivalent, {3, 3, -1}, gt{}), {3, 3, -1});
}

template <iterator_pair_construction Choice>
void test_allocator_extended_constructors() {
    using vec = alternative_vector<int, allocator<int>, Choice>;
    constexpr allocator<int> ator;
    constexpr less<int> comp;
    {
        using fs = flat_set<int, less<int>, vec>;

        fs s{3, 7, 1, 85, 222, 1};
        fs s_expected{1, 3, 7, 85, 222};
        vec v_raw{3, 7, 1, 85, 222, 1};
        vec v_sorted_unique{1, 3, 7, 85, 222};

        TEST_ASSERT(fs{ator} == fs{});
        TEST_ASSERT(fs{comp, ator} == fs{});

        TEST_ASSERT(fs{s, ator} == s_expected);
        TEST_ASSERT(fs{s_expected, ator} == s_expected);
        TEST_ASSERT(fs{move(s), ator} == s_expected);
        TEST_ASSERT(fs{fs{s_expected}, ator} == s_expected);

        TEST_ASSERT(fs{v_raw, ator} == s_expected);
        TEST_ASSERT(fs{{3, 7, 1, 85, 222, 1}, ator} == s_expected);
        TEST_ASSERT(fs{v_raw.begin(), v_raw.end(), ator} == s_expected);
        TEST_ASSERT(fs{from_range, v_raw, ator} == s_expected);

        TEST_ASSERT(fs{v_raw, comp, ator} == s_expected);
        TEST_ASSERT(fs{{3, 7, 1, 85, 222, 1}, comp, ator} == s_expected);
        TEST_ASSERT(fs{v_raw.begin(), v_raw.end(), comp, ator} == s_expected);
        TEST_ASSERT(fs{from_range, v_raw, comp, ator} == s_expected);

        TEST_ASSERT(fs{sorted_unique, v_sorted_unique, ator} == s_expected);
        TEST_ASSERT(fs{sorted_unique, v_sorted_unique, comp, ator} == s_expected);

        TEST_ASSERT(fs{sorted_unique, {1, 3, 7, 85, 222}, ator} == s_expected);
        TEST_ASSERT(fs{sorted_unique, {1, 3, 7, 85, 222}, comp, ator} == s_expected);

        TEST_ASSERT(fs{sorted_unique, v_sorted_unique.begin(), v_sorted_unique.end(), ator} == s_expected);
        TEST_ASSERT(fs{sorted_unique, v_sorted_unique.begin(), v_sorted_unique.end(), comp, ator} == s_expected);
    }
    {
        using fms = flat_multiset<int, less<int>, vec>;

        fms s{3, 7, 1, 85, 222, 1};
        fms s_expected{1, 1, 3, 7, 85, 222};
        vec v_raw{3, 7, 1, 85, 222, 1};
        vec v_sorted_eq{1, 1, 3, 7, 85, 222};

        TEST_ASSERT(fms{ator} == fms{});
        TEST_ASSERT(fms{comp, ator} == fms{});

        TEST_ASSERT(fms{s, ator} == s_expected);
        TEST_ASSERT(fms{s_expected, ator} == s_expected);
        TEST_ASSERT(fms{move(s), ator} == s_expected);
        TEST_ASSERT(fms{fms{s_expected}, ator} == s_expected);

        TEST_ASSERT(fms{v_raw, ator} == s_expected);
        TEST_ASSERT(fms{{3, 7, 1, 85, 222, 1}, ator} == s_expected);
        TEST_ASSERT(fms{v_raw.begin(), v_raw.end(), ator} == s_expected);
        TEST_ASSERT(fms{from_range, v_raw, ator} == s_expected);

        TEST_ASSERT(fms{v_raw, comp, ator} == s_expected);
        TEST_ASSERT(fms{{3, 7, 1, 85, 222, 1}, comp, ator} == s_expected);
        TEST_ASSERT(fms{v_raw.begin(), v_raw.end(), comp, ator} == s_expected);
        TEST_ASSERT(fms{from_range, v_raw, comp, ator} == s_expected);

        TEST_ASSERT(fms{sorted_equivalent, v_sorted_eq, ator} == s_expected);
        TEST_ASSERT(fms{sorted_equivalent, v_sorted_eq, comp, ator} == s_expected);

        TEST_ASSERT(fms{sorted_equivalent, {1, 1, 3, 7, 85, 222}, ator} == s_expected);
        TEST_ASSERT(fms{sorted_equivalent, {1, 1, 3, 7, 85, 222}, comp, ator} == s_expected);

        TEST_ASSERT(fms{sorted_equivalent, v_sorted_eq.begin(), v_sorted_eq.end(), ator} == s_expected);
        TEST_ASSERT(fms{sorted_equivalent, v_sorted_eq.begin(), v_sorted_eq.end(), comp, ator} == s_expected);
    }
}

template <class T>
void test_iterators_and_capacity() {
    T t{11, 22, 33, 44};
    const T& c = t;

    {
        const int arr[]{11, 22, 33, 44};
        assert(equal(t.begin(), t.end(), begin(arr), end(arr)));
        assert(equal(c.begin(), c.end(), begin(arr), end(arr)));
        assert(equal(t.cbegin(), t.cend(), begin(arr), end(arr)));
        assert(equal(c.cbegin(), c.cend(), begin(arr), end(arr)));
    }
    {
        const int rev[]{44, 33, 22, 11};
        assert(equal(t.rbegin(), t.rend(), begin(rev), end(rev)));
        assert(equal(c.rbegin(), c.rend(), begin(rev), end(rev)));
        assert(equal(t.crbegin(), t.crend(), begin(rev), end(rev)));
        assert(equal(c.crbegin(), c.crend(), begin(rev), end(rev)));
    }

    {
        const T zero;

        assert(!t.empty());
        assert(zero.empty());

        assert(t.size() == 4);
        assert(zero.size() == 0);

        assert(t.max_size() > 10);
        assert(zero.max_size() > 10);
    }

    // Finally, test replace() and clear().
    typename T::container_type primes{17, 29, 47};
    t.replace(move(primes));
    assert(t.size() == 3);
    assert(*t.begin() == 17);
    t.clear();
    assert(t.empty());
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

    Set<int, greater<int>, not_reversible> fs({1, 2, 3});
    assert_all_requirements_and_equals(fs, {3, 2, 1});
    assert(fs.rbegin() + 3 == fs.rend());
    assert(fs.crend() - fs.crbegin() == 3);
}

template <class C>
void test_insert_1() {
    using lt = less<int>;

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
        a.insert_range(sorted_unique, vector<int>{2, 5, 8});
        assert_all_requirements_and_equals(a, {0, 1, 2, 5, 8});
        a.insert({6, 2, 3});
        assert_all_requirements_and_equals(a, {0, 1, 2, 3, 5, 6, 8});
        a.insert(sorted_unique, {4, 5});
        assert_all_requirements_and_equals(a, {0, 1, 2, 3, 4, 5, 6, 8});
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
        a.insert_range(sorted_equivalent, vector<int>{2, 8, 8});
        assert_all_requirements_and_equals(a, {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 5, 5, 8, 8});
        a.insert({6, 2, 3});
        assert_all_requirements_and_equals(a, {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 5, 5, 6, 8, 8});
        a.insert(sorted_equivalent, {4, 5});
        assert_all_requirements_and_equals(a, {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 4, 5, 5, 5, 6, 8, 8});
    }
}

template <class C>
void test_insert_2() {
    using lt = less<int>;

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

    using gt = greater<int>;
    {
        flat_set<int, gt, C> a{5, 7, 9};
        assert_all_requirements_and_equals(a, {9, 7, 5});
        a.emplace_hint(a.end());
        assert_all_requirements_and_equals(a, {9, 7, 5, 0});
        a.emplace_hint(a.begin() + 1, 8);
        assert_all_requirements_and_equals(a, {9, 8, 7, 5, 0});
        a.insert(a.begin() + 3, 6);
        assert_all_requirements_and_equals(a, {9, 8, 7, 6, 5, 0});
        a.insert(a.end() - 1, val);
        assert_all_requirements_and_equals(a, {9, 8, 7, 6, 5, 1, 0});
    }
    {
        flat_multiset<int, gt, C> a{3, 1, 4, 1, 5, 9};
        assert_all_requirements_and_equals(a, {9, 5, 4, 3, 1, 1});
        a.emplace_hint(a.end());
        assert_all_requirements_and_equals(a, {9, 5, 4, 3, 1, 1, 0});
        a.emplace_hint(a.begin() + 2, 4);
        assert_all_requirements_and_equals(a, {9, 5, 4, 4, 3, 1, 1, 0});
        a.insert(a.begin() + 1, 6);
        assert_all_requirements_and_equals(a, {9, 6, 5, 4, 4, 3, 1, 1, 0});
        a.insert(a.end() - 3, val);
        assert_all_requirements_and_equals(a, {9, 6, 5, 4, 4, 3, 1, 1, 1, 0});
    }
}

// Test that hint to emplace/insert is respected, when possible; check returned iterator
template <class C>
void test_insert_hint_is_respected() {
    using lt = less<int>;

    {
        flat_multiset<int, lt, C> a{-1, -1, 1, 1};
        bool problem_seen                      = false;
        const auto assert_inserted_at_position = [&a, &problem_seen](
                                                     const int expected_index, const auto insert_position) {
            const auto expected_position = a.begin() + expected_index;
            if (expected_position != insert_position) {
                println("Wrong insert position: expected {}, actual {}\nContainer after insert {}", expected_index,
                    insert_position - a.begin(), a);
                problem_seen = true;
            }
        };

        // hint is greater
        assert_all_requirements_and_equals(a, {-1, -1, 1, 1});
        assert_inserted_at_position(2, a.insert(a.end(), 0));
        assert_all_requirements_and_equals(a, {-1, -1, 0, 1, 1});
        assert_inserted_at_position(3, a.insert(a.find(1), 0));
        assert_all_requirements_and_equals(a, {-1, -1, 0, 0, 1, 1});
        assert_inserted_at_position(4, a.insert(a.find(1), 0));
        assert_all_requirements_and_equals(a, {-1, -1, 0, 0, 0, 1, 1});
        assert_inserted_at_position(5, a.insert(a.upper_bound(0), 0));
        assert_all_requirements_and_equals(a, {-1, -1, 0, 0, 0, 0, 1, 1});

        // hint is correct
        assert_inserted_at_position(5, a.insert(a.upper_bound(0) - 1, 0));
        assert_all_requirements_and_equals(a, {-1, -1, 0, 0, 0, 0, 0, 1, 1});
        assert_inserted_at_position(6, a.insert(a.upper_bound(0) - 1, 0));
        assert_all_requirements_and_equals(a, {-1, -1, 0, 0, 0, 0, 0, 0, 1, 1});
        assert_inserted_at_position(6, a.insert(a.begin() + 6, 0));
        assert_all_requirements_and_equals(a, {-1, -1, 0, 0, 0, 0, 0, 0, 0, 1, 1});
        assert_inserted_at_position(4, a.insert(a.begin() + 4, 0));
        assert_all_requirements_and_equals(a, {-1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1});
        assert_inserted_at_position(2, a.insert(a.begin() + 2, 0));
        assert_all_requirements_and_equals(a, {-1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1});
        assert_inserted_at_position(2, a.emplace_hint(a.lower_bound(0), 0));
        assert_all_requirements_and_equals(a, {-1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1});

        // hint is less
        assert_inserted_at_position(2, a.emplace_hint(a.lower_bound(0) - 1, 0));
        assert_all_requirements_and_equals(a, {-1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1});
        assert_inserted_at_position(2, a.insert(a.begin() + 1, 0));
        assert_all_requirements_and_equals(a, {-1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1});
        assert_inserted_at_position(2, a.insert(a.begin(), 0));
        assert_all_requirements_and_equals(a, {-1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1});

        assert(!problem_seen);

        assert(13 == erase_if(a, [](const auto value) { return value == 0; }));
        assert_all_requirements_and_equals(a, {-1, -1, 1, 1});
    }
}

void test_comparator_application() {
    // The set must rely on its comparator to do the comparisons.
    struct incomparable {
        int key;
        bool operator<(const incomparable&) const  = delete;
        bool operator==(const incomparable&) const = delete;
    };

    flat_set<incomparable, key_comparator> fs{{0}, {3}, {1}, {0}, {5}};
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

    flat_set<int, key_comparator> fs{0, 3, 5};
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

void test_insert_using_invalid_hint(mt19937_64& eng) {
    vector<int> seq(200);

    {
        uniform_int_distribution<int> dist_seq(0, 20);
        for (auto& val : seq) {
            val = dist_seq(eng);
        }
    }

    {
        flat_multiset<int> with_hint;
        flat_multiset<int> no_hint;
        for (const auto& val : seq) {
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
        for (const auto& val : seq) {
            uniform_int_distribution<int> dist_idx(0, static_cast<int>(with_hint.size()));
            auto random_hint = with_hint.begin() + dist_idx(eng);
            with_hint.insert(random_hint, val);
            no_hint.insert(val);
        }

        assert(with_hint == no_hint);
    }
}

void test_insert_upper_bound(mt19937_64& eng) {
    // For flat_multiset's single-element insertion, the key should be inserted before the upper_bound.
    struct test_position {
        int key;
        int extra;
        bool operator==(const test_position&) const = default;
    };

    uniform_int_distribution<int> dist_seq(0, 20);

    vector<test_position> seq(200);
    for (int e = 0; auto& [key, extra] : seq) {
        key   = dist_seq(eng);
        extra = e++;
    }

    flat_multiset<test_position, key_comparator> fs;
    for (const auto& val : seq) {
        fs.insert(val);
    }

    // The result should be identical to as if doing stable_sort on seq.
    ranges::stable_sort(seq, key_comparator{});
    assert(ranges::equal(fs, seq));
}

template <class T>
void test_spaceship_operator() {
    static constexpr bool multi  = _Is_specialization_v<T, flat_multiset>;
    static constexpr bool invert = is_same_v<typename T::key_compare, greater<typename T::key_type>>;

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

    // also test equality
    assert(f == f);
    assert(!(f != f));
    assert(!(f == a));
    assert(f != a);
}

template <class T>
struct proxy_comparator {
    bool operator()(const T& lhs, const T& rhs) const {
        return m_less ? (lhs < rhs) : (lhs > rhs);
    }

    bool m_less = true;
};

void test_non_static_comparator() {
    flat_set<int, proxy_comparator<int>> a{3, 2, 2, 1};
    assert_all_requirements_and_equals(a, {1, 2, 3});
    flat_set<int, proxy_comparator<int>> b({-1, 5, 9, 9, 9, 9, 9}, proxy_comparator<int>{.m_less = false});
    assert_all_requirements_and_equals(b, {9, 5, -1});

    auto aBackup = a;
    a            = b;
    assert_all_requirements_and_equals(a, {9, 5, -1});
    a.insert_range(vector{7, 7, 3, 3, 2});
    assert_all_requirements_and_equals(a, {9, 7, 5, 3, 2, -1});

    a = move(aBackup);
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
    Set<int, less<int>, test_exception> fs{4, 3, 2, 1};
    assert_all_requirements_and_equals(fs, {1, 2, 3, 4});
    auto extr = move(fs).extract();
    assert(ranges::equal(extr, vector{1, 2, 3, 4}));
    assert_all_requirements_and_equals(fs, {}); // assert empty

    fs = {4, 3, 2, 1};
    assert_all_requirements_and_equals(fs, {1, 2, 3, 4});
    try {
        will_throw = true;
        (void) move(fs).extract();
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

    Set<int, less<int>, always_copy> fs{4, 3, 2, 1};
    assert_all_requirements_and_equals(fs, {1, 2, 3, 4});
    auto extr = move(fs).extract();
    assert(ranges::equal(extr, vector{1, 2, 3, 4}));
    assert_all_requirements_and_equals(fs, {}); // assert empty
}

void test_invariant_robustness() {
    static int copy_limit   = 2;
    constexpr int unlimited = (numeric_limits<int>::max)();

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

        // this copy assignment operator cannot provide the strong guarantee for `this`:
        odd_container& operator=(const odd_container& other) {
            resize(other.size());
            copy(other.begin(), other.end(), begin());
            return *this;
        }

        // this move ctor cannot provide the strong guarantee for `other`,
        // and even if it is successful, it will leave the elements of `other` in a moved-from state:
        odd_container(odd_container&& other) {
            reserve(other.size());
            for (auto& e : other) {
                push_back(move(e));
            }
        }

        // this move assignment operator cannot provide the strong guarantee for `this` and `other`,
        // and even if it is successful, it will leave the elements of `other` in a moved-from state:
        odd_container& operator=(odd_container&& other) {
            resize(other.size());
            move(other.begin(), other.end(), begin());
            return *this;
        }
    };

    using SetT = flat_set<odd_key, key_comparator, odd_container>;

    // copy assignment
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

    // move ctor
    {
        copy_limit = unlimited;
        SetT fs1{0, 1, 2, 3, 4};
        SetT fs2{move(fs1)};

        assert_all_requirements(fs1);
        assert(ranges::equal(fs2, vector{0, 1, 2, 3, 4}, {}, &odd_key::key));

        bool caught = false;
        try {
            copy_limit = 2;
            SetT fs3{move(fs2)}; // will throw after moving 2 odd_key.
        } catch (...) {
            copy_limit = unlimited;
            assert_all_requirements(fs2);
            caught = true;
        }
        assert(caught);
    }

    // move assignment
    {
        copy_limit = unlimited;
        SetT fs1{0, 1, 2, 3, 4};
        SetT fs2;
        SetT fs3{5, 6, 7, 8, 9};
        fs2 = move(fs1);

        assert_all_requirements(fs1);
        assert(ranges::equal(fs2, vector{0, 1, 2, 3, 4}, {}, &odd_key::key));
        assert(ranges::equal(fs3, vector{5, 6, 7, 8, 9}, {}, &odd_key::key));

        bool caught = false;
        try {
            copy_limit = 2;
            fs2        = move(fs3); // will throw after moving 2 odd_key.
        } catch (...) {
            copy_limit = unlimited;
            assert_all_requirements(fs2);
            assert_all_requirements(fs3);
            caught = true;
        }
        assert(caught);
    }
}

void test_erase_1() {
    flat_set<int> fs{1};
    assert(1 == fs.erase(1));
    assert_all_requirements_and_equals(fs, {});
    const int numbers[]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    fs.insert_range(numbers);
    assert_all_requirements_and_equals(fs, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
    fs.insert_range(numbers);
    assert_all_requirements_and_equals(fs, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
    assert(0 == fs.erase(-1));
    assert(0 == fs.erase(10));
    assert_all_requirements_and_equals(fs, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
    {
        const auto iter = fs.erase(fs.begin() + 3, fs.begin() + 6);
        assert(fs.begin() + 3 == iter);
        assert(6 == *iter);
        assert_all_requirements_and_equals(fs, {0, 1, 2, 6, 7, 8, 9});
    }
}

template <class T>
struct holder {
    T t;
    operator T() && {
        return move(t);
    }
};

static_assert(is_convertible_v<holder<flat_set<int>::const_iterator>, flat_set<int>::const_iterator>);

void test_erase_2() {
    using C = flat_set<int, less<>>;
    C fs{0, 1, 2, 3};
    assert_all_requirements_and_equals(fs, {0, 1, 2, 3});
    // this should be allowed per N5032 [associative.reqmts.general]/7.23.2 and /122-125:
    fs.erase(holder<C::const_iterator>{fs.cbegin()});
    assert_all_requirements_and_equals(fs, {1, 2, 3});
    fs.erase(holder<C::iterator>{fs.begin()});
    assert_all_requirements_and_equals(fs, {2, 3});
    int i = 2;
    fs.erase(ref(i));
    assert_all_requirements_and_equals(fs, {3});
}

namespace test_erase_compile_time {
    template <class Cont, class OtherKey>
    concept can_erase_key = requires(Cont c, OtherKey key) {
        { c.erase(key) } -> same_as<typename Cont::size_type>;
    };
    template <class Cont>
    concept can_erase_iterator_holder = requires(Cont c) {
        { c.erase(holder<typename Cont::const_iterator>{c.begin()}) } -> same_as<typename Cont::iterator>;
    };

    using C = flat_set<int>;
    static_assert(same_as<C::iterator, decltype(declval<C>().erase(declval<C::iterator>()))>);
    static_assert(same_as<C::iterator, decltype(declval<C>().erase(declval<holder<C::iterator>>()))>);
    static_assert(same_as<C::iterator, decltype(declval<C>().erase(declval<C::const_iterator>()))>);
    static_assert(same_as<C::iterator, decltype(declval<C>().erase(declval<holder<C::const_iterator>>()))>);
    static_assert(same_as<C::iterator, decltype(declval<C>().erase(declval<C::iterator>(), declval<C::iterator>()))>);
    static_assert(same_as<C::size_type, decltype(declval<C>().erase(declval<holder<int>>()))>);
    static_assert(same_as<C::size_type, decltype(declval<C>().erase(declval<int>()))>);

    // Note that std::less<T> is not transparent, while std::less<> and ranges::less are

    // erase key_type
    static_assert(can_erase_key<flat_set<int, less<int>>, int>);
    static_assert(can_erase_key<flat_set<int, less<>>, int>);
    static_assert(can_erase_key<flat_set<int, ranges::less>, int>);

    // erase wrapped key_type
    static_assert(!can_erase_key<flat_set<int, less<int>>, holder<int>>);
    static_assert(can_erase_key<flat_set<int, less<>>, holder<int>>);
    static_assert(can_erase_key<flat_set<int, ranges::less>, holder<int>>);

    // erase wrapped iterator - the member function template returning size_type must not be selected
    static_assert(can_erase_iterator_holder<flat_set<int, less<int>>>);
    static_assert(can_erase_iterator_holder<flat_set<int, less<>>>);
    static_assert(can_erase_iterator_holder<flat_set<int, ranges::less>>);
} // namespace test_erase_compile_time

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

        assert(fs.find(3) == fs.begin() + 1);
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

        assert(fs.find(3) == fs.begin() + 1);
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

    Set<int, key_comparator> fs{0, 3, 5};
    assert_all_requirements_and_equals(fs, {0, 3, 5});

    assert(fs.find(shouldnt_convert{0}) == fs.begin());
    assert(fs.count(shouldnt_convert{3}) == 1);
    assert(!fs.contains(shouldnt_convert{1}));
    assert(fs.lower_bound(shouldnt_convert{-1}) == fs.begin());
    assert(fs.lower_bound(shouldnt_convert{8}) == fs.end());
    assert(fs.upper_bound(shouldnt_convert{2}) == fs.find(3));
    assert(fs.equal_range(shouldnt_convert{5}) == (pair{fs.begin() + 2, fs.end()}));

    // Also test const overloads:
    const auto& cfs = fs;
    assert(cfs.find(shouldnt_convert{0}) == cfs.begin());
    assert(cfs.count(shouldnt_convert{3}) == 1);
    assert(!cfs.contains(shouldnt_convert{1}));
    assert(cfs.lower_bound(shouldnt_convert{-1}) == cfs.begin());
    assert(cfs.lower_bound(shouldnt_convert{8}) == cfs.end());
    assert(cfs.upper_bound(shouldnt_convert{2}) == cfs.find(3));
    assert(cfs.equal_range(shouldnt_convert{5}) == (pair{cfs.begin() + 2, cfs.end()}));
}

namespace test_throwing_swap {
    struct unique_exception {};

    template <class T>
    struct throwing_less {
        static bool operator()(const T& left, const T& right) {
            return left < right;
        }

        bool throws_;
    };

    template <class T>
    void swap(throwing_less<T>& lhs, throwing_less<T>& rhs) {
        if (lhs.throws_ || rhs.throws_) {
            throw unique_exception{};
        }
    }
} // namespace test_throwing_swap

template <template <class...> class FlatSetCont, template <class...> class AdaptedCont>
void test_throwing_compare_single() {
    using test_throwing_swap::unique_exception;
    using comparator = test_throwing_swap::throwing_less<int>;

    using set_type = FlatSetCont<int, comparator, AdaptedCont<int, allocator<int>>>;
    static_assert(!is_nothrow_swappable_v<set_type>);
    {
        set_type s1{{1, 2, 3}, comparator{false}};
        set_type s2{{4, 5, 6}, comparator{false}};
        s1.swap(s2);
        assert(ranges::equal(s1, initializer_list<int>{4, 5, 6}));
        assert(ranges::equal(s2, initializer_list<int>{1, 2, 3}));
    }
    {
        set_type s1{{1, 2, 3}, comparator{false}};
        set_type s2{{4, 5, 6}, comparator{false}};
        swap(s1, s2);
        assert(ranges::equal(s1, initializer_list<int>{4, 5, 6}));
        assert(ranges::equal(s2, initializer_list<int>{1, 2, 3}));
    }
    {
        set_type s1{{1, 2, 3}, comparator{false}};
        set_type s2{{4, 5, 6}, comparator{false}};
        ranges::swap(s1, s2);
        assert(ranges::equal(s1, initializer_list<int>{4, 5, 6}));
        assert(ranges::equal(s2, initializer_list<int>{1, 2, 3}));
    }
    {
        set_type s1{{1, 2, 3}, comparator{true}};
        set_type s2{{4, 5, 6}, comparator{false}};
        try {
            s1.swap(s2);
            assert(false);
        } catch (const unique_exception&) {
            assert(s1.empty());
            assert(s2.empty());
        } catch (...) {
            assert(false);
        }
    }
    {
        set_type s1{{1, 2, 3}, comparator{true}};
        set_type s2{{4, 5, 6}, comparator{false}};
        try {
            swap(s1, s2);
            assert(false);
        } catch (const unique_exception&) {
            assert(s1.empty());
            assert(s2.empty());
        } catch (...) {
            assert(false);
        }
    }
    {
        set_type s1{{1, 2, 3}, comparator{true}};
        set_type s2{{4, 5, 6}, comparator{false}};
        try {
            ranges::swap(s1, s2);
            assert(false);
        } catch (const unique_exception&) {
            assert(s1.empty());
            assert(s2.empty());
        } catch (...) {
            assert(false);
        }
    }
}

void test_throwing_compare_swap() {
    test_throwing_compare_single<flat_set, vector>();
    test_throwing_compare_single<flat_set, deque>();

    test_throwing_compare_single<flat_multiset, vector>();
    test_throwing_compare_single<flat_multiset, deque>();
}

// Test heterogeneous lookup and erase operations when the compare object does not satisfy strict_weak_order (GH-5992)
enum class strange_int {};

// No overload divless::operator()(strange_int, strange_int), does not satisfy std::strict_weak_order
struct divless {
    using is_transparent = void;

    template <class X, class Y>
        requires (is_same_v<X, int> && is_same_v<Y, strange_int>)
              || (is_same_v<X, strange_int> && is_same_v<Y, int>) || (is_same_v<X, int> && is_same_v<Y, int>)
    constexpr bool operator()(X x, Y y) const noexcept {
        if constexpr (is_same_v<X, strange_int>) {
            return static_cast<int>(x) < y / 10;
        } else if constexpr (is_same_v<Y, strange_int>) {
            return x / 10 < static_cast<int>(y);
        } else {
            return x < y;
        }
    }
};
static_assert(!strict_weak_order<divless, int, strange_int>);

// ranges:: algorithms can't be called with divless compare, as it does not satisfy std::strict_weak_order
void test_non_strict_weak_order_compare() {
    {
        flat_set<int, divless> cont{1, 2, 11, 12};
        assert(2 == cont.count(strange_int{0}));

        assert(cont.contains(strange_int{0}));
        assert(!cont.contains(strange_int{2}));

        assert(cont.begin() + 2 == cont.lower_bound(strange_int{1}));
        assert(cont.begin() + 2 == cont.upper_bound(strange_int{0}));

        const auto [first, last] = cont.equal_range(strange_int{0});
        assert(first == cont.begin());
        assert(last == cont.begin() + 2);

        assert(cont.begin() + 2 == cont.find(strange_int{1}));
        assert(cont.end() == cont.find(strange_int{3}));

        assert(2 == cont.erase(strange_int{0}));
        assert_all_requirements_and_equals(cont, {11, 12});
    }
    {
        flat_multiset<int, divless> cont{1, 2, 11, 12};
        assert(2 == cont.count(strange_int{0}));

        assert(cont.contains(strange_int{0}));
        assert(!cont.contains(strange_int{2}));

        assert(cont.begin() + 2 == cont.lower_bound(strange_int{1}));
        assert(cont.begin() + 2 == cont.upper_bound(strange_int{0}));

        const auto [first, last] = cont.equal_range(strange_int{0});
        assert(first == cont.begin());
        assert(last == cont.begin() + 2);

        assert(cont.begin() + 2 == cont.find(strange_int{1}));
        assert(cont.end() == cont.find(strange_int{3}));

        assert(2 == cont.erase(strange_int{0}));
        assert_all_requirements_and_equals(cont, {11, 12});
    }
}

void run_normal_tests() {
    mt19937_64 eng(42);

    test_constructors<vector<int>>();
    test_constructors<deque<int>>();
    test_allocator_extended_constructors<iterator_pair_construction::no_allocator>();
    test_allocator_extended_constructors<iterator_pair_construction::with_allocator>();

    test_iterators_and_capacity<flat_set<int>>();
    test_iterators_and_capacity<flat_multiset<int>>();
    test_iterators_and_capacity<flat_set<int, less<int>, deque<int>>>();
    test_iterators_and_capacity<flat_multiset<int, less<int>, deque<int>>>();

    test_always_reversible<flat_set>();
    test_always_reversible<flat_multiset>();

    test_insert_1<vector<int>>();
    test_insert_1<deque<int>>();
    test_insert_2<vector<int>>();
    test_insert_2<deque<int>>();
    test_insert_hint_is_respected<vector<int>>();
    test_insert_hint_is_respected<deque<int>>();
    test_comparator_application();
    test_insert_transparent();
    test_insert_using_invalid_hint(eng);
    test_insert_upper_bound(eng);

    test_spaceship_operator<flat_set<int>>();
    test_spaceship_operator<flat_multiset<int>>();
    test_spaceship_operator<flat_set<int, greater<int>>>();
    test_spaceship_operator<flat_multiset<int, greater<int>>>();
    test_spaceship_operator<flat_set<int, less<int>, deque<int>>>();
    test_spaceship_operator<flat_multiset<int, less<int>, deque<int>>>();
    test_spaceship_operator<flat_set<int, greater<int>, deque<int>>>();
    test_spaceship_operator<flat_multiset<int, greater<int>, deque<int>>>();

    test_non_static_comparator();

    test_extract_1<flat_set>();
    test_extract_1<flat_multiset>();
    test_extract_2<flat_set>();
    test_extract_2<flat_multiset>();

    test_invariant_robustness();
    test_erase_1();
    test_erase_2();

    test_erase_if<flat_set<int>>();
    test_erase_if<flat_multiset<int>>();

    test_observers<flat_set>();
    test_observers<flat_multiset>();

    test_set_operations<flat_set>();
    test_set_operations<flat_multiset>();
    test_set_operations_transparent<flat_set>();
    test_set_operations_transparent<flat_multiset>();

    test_throwing_compare_swap();
    test_non_strict_weak_order_compare();
}

enum class cont_type { multi, unique };

template <cont_type type>
void test_death_construct_unsorted_initializer_list() {
    using C = conditional_t<type == cont_type::unique, flat_set<int>, flat_multiset<int>>;
    const conditional_t<type == cont_type::unique, sorted_unique_t, sorted_equivalent_t> sorted;
    C cont(sorted, {137, 42, 3337, 15});
}

template <cont_type type>
void test_death_construct_unsorted_iter_iter() {
    using C = conditional_t<type == cont_type::unique, flat_set<int>, flat_multiset<int>>;
    const conditional_t<type == cont_type::unique, sorted_unique_t, sorted_equivalent_t> sorted;
    typename C::container_type values{137, 42, 3337, 15};
    C cont(sorted, values.begin(), values.end());
}

template <cont_type type>
void test_death_construct_unsorted_container() {
    using C = conditional_t<type == cont_type::unique, flat_set<int>, flat_multiset<int>>;
    const conditional_t<type == cont_type::unique, sorted_unique_t, sorted_equivalent_t> sorted;
    typename C::container_type values{137, 42, 3337, 15};
    C cont(sorted, values);
}

template <cont_type type>
void test_death_replace_unsorted_container() {
    using C = conditional_t<type == cont_type::unique, flat_set<int>, flat_multiset<int>>;
    C cont;
    cont.replace({137, 42, 3337, 15});
}

template <cont_type type>
void test_death_insert_unsorted_iter_iter() {
    using C = conditional_t<type == cont_type::unique, flat_set<int>, flat_multiset<int>>;
    const conditional_t<type == cont_type::unique, sorted_unique_t, sorted_equivalent_t> sorted;
    typename C::container_type values{137, 42, 3337, 15};
    C cont;
    cont.insert(sorted, values.begin(), values.end());
}

template <cont_type type>
void test_death_insert_unsorted_range() {
    using C = conditional_t<type == cont_type::unique, flat_set<int>, flat_multiset<int>>;
    const conditional_t<type == cont_type::unique, sorted_unique_t, sorted_equivalent_t> sorted;
    typename C::container_type values{137, 42, 3337, 15};
    C cont;
    cont.insert_range(sorted, values);
}

template <cont_type type>
void test_death_insert_unsorted_initializer_list() {
    using C = conditional_t<type == cont_type::unique, flat_set<int>, flat_multiset<int>>;
    const conditional_t<type == cont_type::unique, sorted_unique_t, sorted_equivalent_t> sorted;
    C cont;
    cont.insert(sorted, {137, 42, 3337, 15});
}

void test_death_construct_duplicates_initializer_list() {
    using C = flat_set<int>;
    C cont(sorted_unique, {42, 137, 137, 3337});
}

void test_death_construct_duplicates_iter_iter() {
    using C = flat_set<int>;
    C::container_type values{42, 137, 137, 3337};
    C cont(sorted_unique, values.begin(), values.end());
}

void test_death_construct_duplicates_container() {
    using C = flat_set<int>;
    C::container_type values{42, 137, 137, 3337};
    C cont(sorted_unique, values);
}

void test_death_replace_duplicates_container() {
    using C = flat_set<int>;
    C cont;
    cont.replace({42, 137, 137, 3337});
}

void test_death_insert_duplicates_iter_iter() {
    using C = flat_set<int>;
    C::container_type values{42, 137, 137, 3337};
    C cont;
    cont.insert(sorted_unique, values.begin(), values.end());
}

void test_death_insert_duplicates_range() {
    using C = flat_set<int>;
    C::container_type values{42, 137, 137, 3337};
    C cont;
    cont.insert_range(sorted_unique, values);
}

void test_death_insert_duplicates_initializer_list() {
    using C = flat_set<int>;
    C cont;
    cont.insert(sorted_unique, {42, 137, 137, 3337});
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec([] { run_normal_tests(); });

#if defined(_DEBUG)
    exec.add_death_tests({

        // Tests shared between flat_set and flat_map - violation of sorted elements
        test_death_construct_unsorted_initializer_list<cont_type::unique>,
        test_death_construct_unsorted_initializer_list<cont_type::multi>,
        test_death_construct_unsorted_iter_iter<cont_type::unique>,
        test_death_construct_unsorted_iter_iter<cont_type::multi>,
        test_death_construct_unsorted_container<cont_type::unique>,
        test_death_construct_unsorted_container<cont_type::multi>,
        test_death_replace_unsorted_container<cont_type::unique>,
        test_death_replace_unsorted_container<cont_type::multi>,
        test_death_insert_unsorted_iter_iter<cont_type::unique>,
        test_death_insert_unsorted_iter_iter<cont_type::multi>,
        test_death_insert_unsorted_range<cont_type::unique>,
        test_death_insert_unsorted_range<cont_type::multi>,
        test_death_insert_unsorted_initializer_list<cont_type::unique>,
        test_death_insert_unsorted_initializer_list<cont_type::multi>,

        // Tests shared between flat_set and flat_map - violation of unique elements
        test_death_construct_duplicates_initializer_list,
        test_death_construct_duplicates_iter_iter,
        test_death_construct_duplicates_container,
        test_death_replace_duplicates_container,
        test_death_insert_duplicates_iter_iter,
        test_death_insert_duplicates_range,
        test_death_insert_duplicates_initializer_list,
    });
#endif // defined(_DEBUG)

    return exec.run(argc, argv);
}
