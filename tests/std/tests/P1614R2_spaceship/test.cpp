// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <charconv>
#include <chrono>
#include <compare>
#include <concepts>
#include <deque>
#include <filesystem>
#include <forward_list>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <memory_resource>
#include <new>
#include <optional>
#include <queue>
#include <ranges>
#include <ratio>
#include <regex>
#include <scoped_allocator>
#include <set>
#include <stack>
#include <string>
#include <string_view>
#include <system_error>
#include <thread>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <valarray>
#include <variant>
#include <vector>

template <class T, class U>
concept HasSpaceshipWith = requires { std::declval<T>() <=> std::declval<U>(); };

using PartiallyOrdered = double;

struct WeaklyOrdered {
    [[nodiscard]] constexpr bool operator==(const WeaklyOrdered&) const {
        return true;
    }

    [[nodiscard]] constexpr std::weak_ordering operator<=>(const WeaklyOrdered&) const {
        return std::weak_ordering::equivalent;
    }
};

using StronglyOrdered = int;

// Activates synth-three-way in N4861 16.4.2.1 [expos.only.func]/2.
struct SynthOrdered {
    int val;

    constexpr SynthOrdered(const int x) : val{x} {}

    [[nodiscard]] constexpr bool operator==(const SynthOrdered& other) const {
        return val == other.val;
    }

    [[nodiscard]] constexpr bool operator<(const SynthOrdered& other) const {
        return val < other.val;
    }
};

struct OrderedChar {
    OrderedChar() = default;
    OrderedChar(const char other) : c(other) {}

    OrderedChar& operator=(const char& other) {
        c = other;
        return *this;
    }

    auto operator<=>(const OrderedChar&) const = default;

    operator char() const {
        return c;
    }

    char c;
};

struct WeaklyOrderedChar : OrderedChar {};
struct WeaklyOrderedByOmissionChar : OrderedChar {};
struct PartiallyOrderedChar : OrderedChar {};

namespace std {
    template <>
    struct char_traits<OrderedChar> : char_traits<char> {
        using char_type = OrderedChar;

        static int compare(const char_type* first1, const char_type* first2, size_t count) {
            for (; 0 < count; --count, ++first1, ++first2) {
                if (*first1 != *first2) {
                    return *first1 < *first2 ? -1 : +1;
                }
            }

            return 0;
        }

        static bool eq(const char_type l, const char_type r) {
            return l.c == r.c;
        }
    };

    template <>
    struct char_traits<WeaklyOrderedChar> : char_traits<OrderedChar> {
        using char_type           = WeaklyOrderedChar;
        using comparison_category = weak_ordering;
    };

    template <>
    struct char_traits<WeaklyOrderedByOmissionChar> : char_traits<OrderedChar> {
        using char_type = WeaklyOrderedByOmissionChar;

    private:
        using comparison_category = strong_ordering;
    };

    template <>
    struct char_traits<PartiallyOrderedChar> : char_traits<OrderedChar> {
        using char_type           = PartiallyOrderedChar;
        using comparison_category = partial_ordering;
    };
} // namespace std

struct dummy_diagnostic : std::error_category {
    const char* name() const noexcept override {
        return "dummy";
    }
    std::string message(int) const override {
        return "";
    }
};

template <class ReturnType, class SmallType, class EqualType, class LargeType>
constexpr bool spaceship_test(const SmallType& smaller, const EqualType& smaller_equal, const LargeType& larger) {
    assert(smaller == smaller_equal);
    assert(smaller_equal == smaller);
    assert(smaller != larger);
    assert(larger != smaller);
    assert(smaller < larger);
    assert(!(larger < smaller));
    assert(larger > smaller);
    assert(!(smaller > larger));
    assert(smaller <= larger);
    assert(!(larger <= smaller));
    assert(larger >= smaller);
    assert(!(smaller >= larger));
    assert((smaller <=> larger) < 0);
    assert((larger <=> smaller) > 0);
    assert((smaller <=> smaller_equal) == 0);

    static_assert(std::is_same_v<decltype(smaller <=> larger), ReturnType>);

    return true;
}

template <class T>
inline constexpr bool has_synth_ordered = false;
template <class V>
inline constexpr bool has_synth_ordered<std::pair<const SynthOrdered, V>> = true;
template <>
inline constexpr bool has_synth_ordered<SynthOrdered> = true;

template <class Container>
constexpr void ordered_containers_test(
    const Container& smaller, const Container& smaller_equal, const Container& larger) {
    using Elem = typename Container::value_type;

    if constexpr (has_synth_ordered<Elem>) {
        spaceship_test<std::weak_ordering>(smaller, smaller_equal, larger);
    } else {
        spaceship_test<std::strong_ordering>(smaller, smaller_equal, larger);
    }
}

template <class Container>
void unordered_containers_test(
    const Container& something, const Container& something_equal, const Container& different) {
    assert(something == something_equal);
    assert(something != different);
}

template <class Iter, class ConstIter>
constexpr void ordered_iterator_test(const Iter& smaller, const Iter& smaller_equal, const Iter& larger,
    const ConstIter& const_smaller, const ConstIter& const_smaller_equal, const ConstIter& const_larger) {
    spaceship_test<std::strong_ordering>(smaller, smaller_equal, larger);
    spaceship_test<std::strong_ordering>(const_smaller, const_smaller_equal, const_larger);
    spaceship_test<std::strong_ordering>(const_smaller, smaller_equal, larger);
}

template <class Iter, class ConstIter>
void unordered_iterator_test(const Iter& something, const Iter& something_equal, const Iter& different,
    const ConstIter& const_something, const ConstIter& const_something_equal, const ConstIter& const_different) {
    assert(something == something_equal);
    assert(something != different);

    assert(const_something == const_something_equal);
    assert(const_something != const_different);

    assert(something == const_something_equal);
    assert(something != const_different);
}

template <class ErrorType>
void diagnostics_test() {
    dummy_diagnostic c_mem[2];
    {
        ErrorType e_smaller(0, c_mem[0]);
        ErrorType e_equal(0, c_mem[0]);
        ErrorType e_larger(1, c_mem[1]);

        spaceship_test<std::strong_ordering>(e_smaller, e_equal, e_larger);
    }
    {
        ErrorType e_smaller(0, c_mem[0]);
        ErrorType e_larger(0, c_mem[1]);

        assert(e_smaller < e_larger);
        assert(!(e_larger < e_smaller));
        assert((e_smaller <=> e_larger) < 0);
        assert((e_larger <=> e_smaller) > 0);
    }
    {
        ErrorType e_smaller(0, c_mem[0]);
        ErrorType e_larger(1, c_mem[0]);

        assert(e_smaller < e_larger);
        assert(!(e_larger < e_smaller));
        assert((e_smaller <=> e_larger) < 0);
        assert((e_larger <=> e_smaller) > 0);
    }
}

template <template <class...> class TupleLike>
constexpr bool tuple_like_test() {
    {
        constexpr TupleLike<int, int> t1{1, 1};
        constexpr TupleLike<int, int> t1_equal{1, 1};
        constexpr TupleLike<int, int> t2{2, 1};

        assert(spaceship_test<std::strong_ordering>(t1, t1_equal, t2));
    }
    {
        constexpr TupleLike<int, double> t1{1, 1.0};
        constexpr TupleLike<int, double> t1_equal{1, 1.0};
        constexpr TupleLike<int, double> t2{2, 1.0};

        assert(spaceship_test<std::partial_ordering>(t1, t1_equal, t2));
    }

    if constexpr (std::is_same_v<TupleLike<int, int>, std::tuple<int, int>>) {
        {
            constexpr TupleLike<int, double> t1{1, 1.0};
            constexpr TupleLike<double, double> t1_equal{1.0, 1.0};
            constexpr TupleLike<int, double> t2{2, 1.0};

            assert(spaceship_test<std::partial_ordering>(t1, t1_equal, t2));
        }
        {
            constexpr TupleLike<int, double> t1{1, 1.0};
            constexpr TupleLike<int, double> t1_equal{1, 1.0};
            constexpr TupleLike<double, double> t2{2.0, 1.0};

            assert(spaceship_test<std::partial_ordering>(t1, t1_equal, t2));
        }
    }

    static_assert(std::is_same_v<std::compare_three_way_result_t<TupleLike<PartiallyOrdered, PartiallyOrdered>>,
        std::partial_ordering>);
    static_assert(
        std::is_same_v<std::compare_three_way_result_t<TupleLike<WeaklyOrdered, WeaklyOrdered>>, std::weak_ordering>);
    static_assert(std::is_same_v<std::compare_three_way_result_t<TupleLike<StronglyOrdered, StronglyOrdered>>,
        std::strong_ordering>);

    return true;
}

template <class T>
struct derived_optional : std::optional<T> {
    friend bool operator==(const derived_optional&, const derived_optional&) = default;
};

template <auto SmallVal, decltype(SmallVal) EqualVal, decltype(EqualVal) LargeVal>
constexpr bool optional_test() {
    using ReturnType = std::compare_three_way_result_t<decltype(SmallVal)>;

    {
        constexpr std::optional o1(SmallVal);
        constexpr std::optional o1_equal(EqualVal);
        constexpr std::optional o2(LargeVal);

        assert(spaceship_test<ReturnType>(o1, o1_equal, o2));
    }
    {
        constexpr std::optional<decltype(SmallVal)> o1(std::nullopt);
        constexpr std::optional<decltype(EqualVal)> o1_equal(std::nullopt);
        constexpr std::optional o2(LargeVal);

        assert(spaceship_test<ReturnType>(o1, o1_equal, o2));
    }
    {
        constexpr std::optional o1(SmallVal);
        constexpr derived_optional<decltype(SmallVal)> derived1{std::optional(SmallVal)};
        constexpr derived_optional<decltype(SmallVal)> derived2{std::optional(LargeVal)};

        static_assert(!std::three_way_comparable<derived_optional<decltype(SmallVal)>>);

        assert(spaceship_test<ReturnType>(o1, EqualVal, LargeVal));
        assert(spaceship_test<ReturnType>(o1, derived1, derived2));
    }
    {
        constexpr std::optional<decltype(SmallVal)> o1(std::nullopt);
        constexpr std::optional o2(LargeVal);

        assert(spaceship_test<ReturnType>(o1, std::nullopt, o2));
    }

    return true;
}

template <bool Equal>
struct compare_resource final : std::pmr::memory_resource {
private:
    void* do_allocate(size_t, size_t) override {
        throw std::bad_alloc{};
    }
    void do_deallocate(void*, size_t, size_t) override {}
    bool do_is_equal(const memory_resource&) const noexcept override {
        return Equal;
    }
};

template <class T, bool Equal>
struct basic_compare_allocator : std::allocator<T> {
    template <class U>
    struct rebind {
        using other = basic_compare_allocator<U, Equal>;
    };

    basic_compare_allocator() = default;
    template <class U>
    basic_compare_allocator(const basic_compare_allocator<U, Equal>&) {}
};

template <class T, bool Ignored, class U, bool Equal>
bool operator==(const basic_compare_allocator<T, Ignored>&, const basic_compare_allocator<U, Equal>&) {
    return Equal;
}

template <bool Equal>
using compare_allocator = basic_compare_allocator<int, Equal>;

constexpr bool test_vector() {
    { // vector
        std::vector<int> a1(3, 100);
        std::vector<int> a2(3, 100);
        std::vector<int> b1(2, 200);
        ordered_containers_test(a1, a2, b1);
        ordered_iterator_test(a1.begin(), a1.begin(), a1.end(), a1.cbegin(), a1.cbegin(), a1.cend());
    }
    { // vector SynthOrdered
        std::vector<SynthOrdered> a = {10, 20, 30};
        std::vector<SynthOrdered> b = {10, 20, 40};
        ordered_containers_test(a, a, b);
        ordered_iterator_test(a.begin(), a.begin(), a.end(), a.cbegin(), a.cbegin(), a.cend());
    }
    { // vector<bool>
        std::vector<bool> c1 = {false, true, false};
        std::vector<bool> c2 = {false, true, false};
        std::vector<bool> d1 = {true, false};
        ordered_containers_test(c1, c2, d1);
        ordered_iterator_test(c1.begin(), c1.begin(), c1.end(), c1.cbegin(), c1.cbegin(), c1.cend());
    }

    return true;
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
    { // constexpr array SynthOrdered
        constexpr std::array<SynthOrdered, 3> a = {10, 20, 30};
        constexpr std::array<SynthOrdered, 3> b = {10, 20, 40};

        static_assert((a <=> a) == 0);
        static_assert((a <=> b) < 0);
        static_assert((b <=> a) > 0);
    }
    { // array
        std::array<int, 3> a1 = {100, 100, 100};
        std::array<int, 3> a2 = {100, 100, 100};
        std::array<int, 3> b1 = {200, 200};
        ordered_containers_test(a1, a2, b1);
        ordered_iterator_test(a1.begin(), a1.begin(), a1.end(), a1.cbegin(), a1.cbegin(), a1.cend());
    }
    { // array SynthOrdered
        std::array<SynthOrdered, 3> a = {10, 20, 30};
        std::array<SynthOrdered, 3> b = {10, 20, 40};
        ordered_containers_test(a, a, b);
        ordered_iterator_test(a.begin(), a.begin(), a.end(), a.cbegin(), a.cbegin(), a.cend());
    }
    { // deque
        std::deque<int> a1(3, 100);
        std::deque<int> a2(3, 100);
        std::deque<int> b1(2, 200);
        ordered_containers_test(a1, a2, b1);
        ordered_iterator_test(a1.begin(), a1.begin(), a1.end(), a1.cbegin(), a1.cbegin(), a1.cend());
    }
    { // deque SynthOrdered
        std::deque<SynthOrdered> a = {10, 20, 30};
        std::deque<SynthOrdered> b = {10, 20, 40};
        ordered_containers_test(a, a, b);
        ordered_iterator_test(a.begin(), a.begin(), a.end(), a.cbegin(), a.cbegin(), a.cend());
    }
    { // list
        std::list<int> a1(3, 100);
        std::list<int> a2(3, 100);
        std::list<int> b1(2, 200);
        ordered_containers_test(a1, a2, b1);
        unordered_iterator_test(a1.begin(), a1.begin(), a1.end(), a1.cbegin(), a1.cbegin(), a1.cend());
    }
    { // list SynthOrdered
        std::list<SynthOrdered> a = {10, 20, 30};
        std::list<SynthOrdered> b = {10, 20, 40};
        ordered_containers_test(a, a, b);
        unordered_iterator_test(a.begin(), a.begin(), a.end(), a.cbegin(), a.cbegin(), a.cend());
    }
    { // vector, vector SynthOrdered, vector<bool>
        test_vector();
        static_assert(test_vector());
    }
    { // forward_list
        std::forward_list<int> a1(3, 100);
        std::forward_list<int> a2(3, 100);
        std::forward_list<int> b1(2, 200);
        ordered_containers_test(a1, a2, b1);
        unordered_iterator_test(a1.begin(), a1.begin(), a1.end(), a1.cbegin(), a1.cbegin(), a1.cend());
    }
    { // forward_list SynthOrdered
        std::forward_list<SynthOrdered> a = {10, 20, 30};
        std::forward_list<SynthOrdered> b = {10, 20, 40};
        ordered_containers_test(a, a, b);
        unordered_iterator_test(a.begin(), a.begin(), a.end(), a.cbegin(), a.cbegin(), a.cend());
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
        unordered_iterator_test(a1.begin(), a1.begin(), a1.end(), a1.cbegin(), a1.cbegin(), a1.cend());
    }
    { // map SynthOrdered
        std::map<SynthOrdered, char> a = {{10, 'z'}, {20, 'z'}, {30, 'z'}};
        std::map<SynthOrdered, char> b = {{10, 'z'}, {20, 'z'}, {40, 'z'}};
        ordered_containers_test(a, a, b);
        unordered_iterator_test(a.begin(), a.begin(), a.end(), a.cbegin(), a.cbegin(), a.cend());
    }
    { // multimap
        std::multimap<char, int> a1 = {{'a', 1}, {'b', 2}, {'a', 3}};
        std::multimap<char, int> a2 = {{'a', 1}, {'a', 3}, {'b', 2}};
        std::multimap<char, int> b1 = {{'z', 4}, {'y', 90}, {'z', 12}};
        ordered_containers_test(a1, a2, b1);
        unordered_iterator_test(a1.begin(), a1.begin(), a1.end(), a1.cbegin(), a1.cbegin(), a1.cend());
    }
    { // multimap SynthOrdered
        std::multimap<SynthOrdered, char> a = {{10, 'z'}, {20, 'z'}, {30, 'z'}};
        std::multimap<SynthOrdered, char> b = {{10, 'z'}, {20, 'z'}, {40, 'z'}};
        ordered_containers_test(a, a, b);
        unordered_iterator_test(a.begin(), a.begin(), a.end(), a.cbegin(), a.cbegin(), a.cend());
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
        unordered_iterator_test(a1.begin(), a1.begin(), a1.end(), a1.cbegin(), a1.cbegin(), a1.cend());
    }
    { // set SynthOrdered
        std::set<SynthOrdered> a = {10, 20, 30};
        std::set<SynthOrdered> b = {10, 20, 40};
        ordered_containers_test(a, a, b);
        unordered_iterator_test(a.begin(), a.begin(), a.end(), a.cbegin(), a.cbegin(), a.cend());
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
        unordered_iterator_test(a1.begin(), a1.begin(), a1.end(), a1.cbegin(), a1.cbegin(), a1.cend());
    }
    { // multiset SynthOrdered
        std::multiset<SynthOrdered> a = {10, 20, 30};
        std::multiset<SynthOrdered> b = {10, 20, 40};
        ordered_containers_test(a, a, b);
        unordered_iterator_test(a.begin(), a.begin(), a.end(), a.cbegin(), a.cbegin(), a.cend());
    }
    { // unordered_map
        using stringmap = std::unordered_map<std::string, std::string>;
        stringmap a     = {{"cat", "tabby"}, {"dog", "poodle"}, {"bear", "grizzly"}};
        stringmap b     = {{"dog", "poodle"}, {"bear", "grizzly"}, {"cat", "tabby"}};
        stringmap c     = {{"cat", "siamese"}, {"dog", "lab"}, {"bear", "polar"}};
        unordered_containers_test(a, b, c);
        unordered_iterator_test(a.begin(), a.begin(), a.end(), a.cbegin(), a.cbegin(), a.cend());
    }
    { // unordered_multimap
        using stringmap = std::unordered_multimap<std::string, std::string>;
        stringmap a     = {{"cat", "tabby"}, {"dog", "poodle"}, {"cat", "siamese"}, {"dog", "poodle"}};
        stringmap b     = {{"dog", "poodle"}, {"cat", "siamese"}, {"cat", "tabby"}, {"dog", "poodle"}};
        stringmap c     = {{"cat", "siamese"}, {"dog", "lab"}, {"bear", "polar"}};
        unordered_containers_test(a, b, c);
        unordered_iterator_test(a.begin(), a.begin(), a.end(), a.cbegin(), a.cbegin(), a.cend());
    }
    { // unordered_set
        std::unordered_set<std::string> a = {"cat", "dog", "bear"};
        std::unordered_set<std::string> b = {"bear", "cat", "dog"};
        std::unordered_set<std::string> c = {"mouse", "cat", "bear", "dog"};
        unordered_containers_test(a, b, c);
        unordered_iterator_test(a.begin(), a.begin(), a.end(), a.cbegin(), a.cbegin(), a.cend());
    }
    { // unordered_multiset
        std::unordered_multiset<std::string> a = {"cat", "dog", "cat"};
        std::unordered_multiset<std::string> b = {"cat", "cat", "dog"};
        std::unordered_multiset<std::string> c = {"mouse", "cat", "bear", "dog"};
        unordered_containers_test(a, b, c);
        unordered_iterator_test(a.begin(), a.begin(), a.end(), a.cbegin(), a.cbegin(), a.cend());
    }
    { // queue
        std::deque<int> deq1(3, 100);
        std::deque<int> deq2(2, 200);
        std::queue<int> a(deq1);
        std::queue<int> b(deq1);
        std::queue<int> c(deq2);
        ordered_containers_test(a, b, c);
    }
    { // queue SynthOrdered
        std::queue<SynthOrdered> a{std::deque<SynthOrdered>{10, 20, 30}};
        std::queue<SynthOrdered> b{std::deque<SynthOrdered>{10, 20, 40}};
        ordered_containers_test(a, a, b);
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
    { // stack SynthOrdered
        std::stack<SynthOrdered> a{std::deque<SynthOrdered>{10, 20, 30}};
        std::stack<SynthOrdered> b{std::deque<SynthOrdered>{10, 20, 40}};
        ordered_containers_test(a, a, b);
    }
    { // checked_array_iterator
        int arr[]        = {11, 22, 33};
        constexpr auto N = std::size(arr);

        using I  = stdext::checked_array_iterator<int*>;
        using CI = stdext::checked_array_iterator<int*>; // TRANSITION, GH-943, should be <const int*>

        I first{arr, N};
        I last{arr, N, N};

        CI cfirst{arr, N};
        CI clast{arr, N, N};

        ordered_iterator_test(first, first, last, cfirst, cfirst, clast);
    }
    { // unchecked_array_iterator
        int arr[]        = {11, 22, 33};
        constexpr auto N = std::size(arr);

        using I  = stdext::unchecked_array_iterator<int*>;
        using CI = stdext::unchecked_array_iterator<int*>; // TRANSITION, GH-943, should be <const int*>

        I first{arr};
        I last{arr + N};

        CI cfirst{arr};
        CI clast{arr + N};

        ordered_iterator_test(first, first, last, cfirst, cfirst, clast);
    }
    { // sub_match
        const std::string s1{"cats"};
        const std::string s2{"meow"};
        const std::regex all(".*");
        const std::regex each(".");
        std::smatch m1;
        std::smatch m2;
        std::smatch m3;
        std::smatch m4;

        std::regex_match(s1, m1, all);
        std::regex_match(s2, m2, all);
        std::regex_search(s1, m3, each);
        std::regex_search(s2, m4, each);

        std::ssub_match sm1       = m1[0];
        std::ssub_match sm1_equal = m1[0];
        std::ssub_match sm2       = m2[0];
        std::ssub_match sm3       = m3[0];
        std::ssub_match sm4       = m4[0];

        spaceship_test<std::strong_ordering>(sm1, sm1_equal, sm2);
        spaceship_test<std::strong_ordering>(sm1, s1, s2);
        spaceship_test<std::strong_ordering>(sm1, s1.c_str(), s2.c_str());
        spaceship_test<std::strong_ordering>(sm3, 'c', 'm');
        spaceship_test<std::strong_ordering>(s1, sm1, sm2);
        spaceship_test<std::strong_ordering>(s1.c_str(), sm1, sm2);
        spaceship_test<std::strong_ordering>('c', sm3, sm4);

        using StronglyOrderedMatch = std::ssub_match;
        using WeaklyOrderedMatch   = std::sub_match<std::basic_string<WeaklyOrderedChar>::const_iterator>;
        using WeaklyOrderdByOmissionMatch =
            std::sub_match<std::basic_string<WeaklyOrderedByOmissionChar>::const_iterator>;
        using PartiallyOrderedMatch = std::sub_match<std::basic_string<PartiallyOrderedChar>::const_iterator>;

        static_assert(std::is_same_v<std::compare_three_way_result_t<StronglyOrderedMatch>, std::strong_ordering>);
        static_assert(std::is_same_v<std::compare_three_way_result_t<WeaklyOrderedMatch>, std::weak_ordering>);
        static_assert(std::is_same_v<std::compare_three_way_result_t<WeaklyOrderdByOmissionMatch>, std::weak_ordering>);
        static_assert(std::is_same_v<std::compare_three_way_result_t<PartiallyOrderedMatch>, std::partial_ordering>);
    }
    { // char_traits
        static_assert(std::is_same_v<std::char_traits<char>::comparison_category, std::strong_ordering>);
#ifdef __cpp_char8_t
        static_assert(std::is_same_v<std::char_traits<char8_t>::comparison_category, std::strong_ordering>);
#endif // __cpp_char8_t
        static_assert(std::is_same_v<std::char_traits<char16_t>::comparison_category, std::strong_ordering>);
        static_assert(std::is_same_v<std::char_traits<char32_t>::comparison_category, std::strong_ordering>);
        static_assert(std::is_same_v<std::char_traits<wchar_t>::comparison_category, std::strong_ordering>);
    }
    { // charconv
        char c[7] = "123456";

        std::from_chars_result a1{c + 6, std::errc{}};
        std::from_chars_result a2{c + 6, std::errc{}};
        std::from_chars_result a3{c + 6, std::errc::result_out_of_range};
        std::from_chars_result a4{c + 4, std::errc{}};

        assert(a1 == a2);
        assert(a1 != a3);
        assert(a1 != a4);

        std::to_chars_result b1{c + 6, std::errc{}};
        std::to_chars_result b2{c + 6, std::errc{}};
        std::to_chars_result b3{c + 6, std::errc::value_too_large};
        std::to_chars_result b4{c + 4, std::errc{}};

        assert(b1 == b2);
        assert(b1 != b3);
        assert(b1 != b4);
    }
    { // typeindex
        std::type_index a1 = typeid(int);
        std::type_index a2 = typeid(char);
        std::type_index a3 = typeid(bool);
        std::type_index a4 = typeid(int);
        assert((a1 <=> a4) == std::strong_ordering::equal);
        assert((a1 <=> a2) == std::strong_ordering::greater); // Implementation-specific assumption
        assert((a1 <=> a3) == std::strong_ordering::less); // Implementation-specific assumption
    }
    { // Strings library
        const std::string a1 = "abcdef";
        const std::string a2 = "abcdef";
        const std::string a3 = "abcdefg";
        const std::string a4 = "abcde";
        const std::string a5 = "abddef";
        const std::string a6 = "abbdef";

        assert((a1 <=> a2) == std::strong_ordering::equivalent);
        assert((a1 <=> a3) == std::strong_ordering::less);
        assert((a1 <=> a4) == std::strong_ordering::greater);
        assert((a1 <=> a5) == std::strong_ordering::less);
        assert((a1 <=> a6) == std::strong_ordering::greater);

        assert(a1 == a2);
        assert(a1 >= a2);
        assert(a1 <= a2);
        assert(a1 < a3);
        assert(a1 <= a3);
        assert(a1 != a3);
        assert(a1 > a4);
        assert(a1 >= a4);
        assert(a1 != a4);
        assert(a1 < a5);
        assert(a1 <= a5);
        assert(a1 != a5);
        assert(a1 > a6);
        assert(a1 >= a6);
        assert(a1 != a6);

        assert((a1 <=> "aardvark") == std::strong_ordering::greater);
        assert((a1 <=> "abcdef") == std::strong_ordering::equivalent);
        assert((a1 <=> "zebra") == std::strong_ordering::less);

        assert(("aardvark" <=> a1) == std::strong_ordering::less);
        assert(("abcdef" <=> a1) == std::strong_ordering::equivalent);
        assert(("zebra" <=> a1) == std::strong_ordering::greater);
    }
    { // string iterators
        std::string a1 = "aaa";
        std::string a2 = "aaa";
        std::string b1 = "bb";
        ordered_containers_test(a1, a2, b1);
        ordered_iterator_test(a1.begin(), a1.begin(), a1.end(), a1.cbegin(), a1.cbegin(), a1.cend());
    }
    { // string_view
        const std::string_view a1 = "abcdef";
        const std::string_view a2 = "abcdef";
        const std::string_view a3 = "abcdefg";
        const std::string_view a4 = "abcde";
        const std::string_view a5 = "abddef";
        const std::string_view a6 = "abbdef";

        assert((a1 <=> a2) == std::strong_ordering::equivalent);
        assert((a1 <=> a3) == std::strong_ordering::less);
        assert((a1 <=> a4) == std::strong_ordering::greater);
        assert((a1 <=> a5) == std::strong_ordering::less);
        assert((a1 <=> a6) == std::strong_ordering::greater);

        assert(a1 == a2);
        assert(a1 >= a2);
        assert(a1 <= a2);
        assert(a1 < a3);
        assert(a1 <= a3);
        assert(a1 != a3);
        assert(a1 > a4);
        assert(a1 >= a4);
        assert(a1 != a4);
        assert(a1 < a5);
        assert(a1 <= a5);
        assert(a1 != a5);
        assert(a1 > a6);
        assert(a1 >= a6);
        assert(a1 != a6);

        assert((a1 <=> "aardvark") == std::strong_ordering::greater);
        assert((a1 <=> "abcdef") == std::strong_ordering::equivalent);
        assert((a1 <=> "zebra") == std::strong_ordering::less);

        assert(("aardvark" <=> a1) == std::strong_ordering::less);
        assert(("abcdef" <=> a1) == std::strong_ordering::equivalent);
        assert(("zebra" <=> a1) == std::strong_ordering::greater);
    }
    { // constexpr string_view
        constexpr std::string_view a1 = "abcdef";
        constexpr std::string_view a2 = "abcdef";
        constexpr std::string_view a3 = "abcdefg";
        constexpr std::string_view a4 = "abcde";
        constexpr std::string_view a5 = "abddef";
        constexpr std::string_view a6 = "abbdef";

        static_assert((a1 <=> a2) == std::strong_ordering::equivalent);
        static_assert((a1 <=> a3) == std::strong_ordering::less);
        static_assert((a1 <=> a4) == std::strong_ordering::greater);
        static_assert((a1 <=> a5) == std::strong_ordering::less);
        static_assert((a1 <=> a6) == std::strong_ordering::greater);

        static_assert(a1 == a2);
        static_assert(a1 >= a2);
        static_assert(a1 <= a2);
        static_assert(a1 < a3);
        static_assert(a1 <= a3);
        static_assert(a1 != a3);
        static_assert(a1 > a4);
        static_assert(a1 >= a4);
        static_assert(a1 != a4);
        static_assert(a1 < a5);
        static_assert(a1 <= a5);
        static_assert(a1 != a5);
        static_assert(a1 > a6);
        static_assert(a1 >= a6);
        static_assert(a1 != a6);

        static_assert((a1 <=> "aardvark") == std::strong_ordering::greater);
        static_assert((a1 <=> "abcdef") == std::strong_ordering::equivalent);
        static_assert((a1 <=> "zebra") == std::strong_ordering::less);

        static_assert(("aardvark" <=> a1) == std::strong_ordering::less);
        static_assert(("abcdef" <=> a1) == std::strong_ordering::equivalent);
        static_assert(("zebra" <=> a1) == std::strong_ordering::greater);
    }
    { // string_view iterators
        std::string_view a1 = "aaa";
        std::string_view a2 = "aaa";
        std::string_view b1 = "bb";
        ordered_containers_test(a1, a2, b1);
        ordered_iterator_test(a1.begin(), a1.begin(), a1.end(), a1.cbegin(), a1.cbegin(), a1.cend());
    }
    { // allocator
        constexpr std::allocator<int> a1;
        constexpr std::allocator<double> a2;

        static_assert(a1 == a1);
        static_assert(a1 == a2);
        static_assert(!(a1 != a1));
        static_assert(!(a1 != a2));
    }
    { // memory_resource
        compare_resource<true> t1;
        compare_resource<true>& t2 = t1;
        compare_resource<false> f1;
        compare_resource<false>& f2 = f1;

        // same address
        assert(t1 == t1);
        assert(t1 == t2);
        assert(f1 == f1);
        assert(f1 == f2);

        // dependent on is_equal(__)
        assert(t1 == f1);
        assert(f1 != t1);
    }
    { // polymorphic_allocator
        compare_resource<false> rFalse;
        compare_resource<true> rTrue;
        std::pmr::polymorphic_allocator<int> p1 = &rFalse;
        std::pmr::polymorphic_allocator<int> p2 = p1;
        std::pmr::polymorphic_allocator<int> p3 = &rTrue;

        // same resource address
        assert(p1 == p1);
        assert(p1 == p2);

        // dependent on resource()::is_equal(__)
        assert(p1 != p3);
        assert(p3 == p1);
    }
    { // scoped_allocator_adaptor
        // note: compare_allocator<T> equality is based on the T of the RHS
        std::scoped_allocator_adaptor<compare_allocator<true>> s1;
        std::scoped_allocator_adaptor<compare_allocator<true>> s2;
        std::scoped_allocator_adaptor<compare_allocator<false>> s3;

        assert(s1 == s1);
        assert(s1 == s2);
        assert(s1 != s3);

        std::scoped_allocator_adaptor<compare_allocator<true>, compare_allocator<true>> s4{};
        std::scoped_allocator_adaptor<compare_allocator<true>, compare_allocator<false>> s5{};

        assert(s4 == s4);
        assert(s5 != s5);
    }
    { // function
        std::function<void(int)> f{};

        assert(f == nullptr);
        assert(nullptr == f);
        assert(!(f != nullptr));
        assert(!(nullptr != f));
    }
    { // Diagnostics Library
        diagnostics_test<std::error_code>();
        diagnostics_test<std::error_condition>();

        dummy_diagnostic c_mem[2];
        {
            std::error_code e1(0, c_mem[0]);
            std::error_condition e2(0, c_mem[0]);

            assert(e1 == e2);
            assert(e2 == e1);
        }
        {
            std::error_code e1(0, c_mem[0]);
            std::error_condition e2(0, c_mem[1]);

            assert(e1 != e2);
            assert(e2 != e1);
        }
        {
            std::error_code e1(1, c_mem[0]);
            std::error_condition e2(0, c_mem[0]);

            assert(e1 != e2);
            assert(e2 != e1);
        }

        spaceship_test<std::strong_ordering>(c_mem[0], c_mem[0], c_mem[1]);
    }
    { // optional
        optional_test<0, 0, 1>();
        static_assert(optional_test<0, 0, 1>());

#if defined(__cpp_nontype_template_args) && __cpp_nontype_template_args >= 201911
        optional_test<0.0, 0.0, 1.0>();
        static_assert(optional_test<0.0, 0.0, 1.0>());
#endif

        static_assert(
            std::is_same_v<std::compare_three_way_result_t<std::optional<WeaklyOrdered>, std::optional<WeaklyOrdered>>,
                std::weak_ordering>);
        static_assert(std::is_same_v<std::compare_three_way_result_t<std::optional<WeaklyOrdered>, WeaklyOrdered>,
            std::weak_ordering>);
        static_assert(std::is_same_v<std::compare_three_way_result_t<std::optional<WeaklyOrdered>, std::nullopt_t>,
            std::strong_ordering>);
    }
    { // tuple
        tuple_like_test<std::tuple>();
        static_assert(tuple_like_test<std::tuple>());

        {
            constexpr std::tuple<> empty1;
            constexpr std::tuple<> empty2;

            static_assert(std::is_same_v<decltype(empty1 <=> empty2), std::strong_ordering>);
            static_assert((empty1 <=> empty2) == std::strong_ordering::equal);
        }
    }
    { // pair
        tuple_like_test<std::pair>();
        static_assert(tuple_like_test<std::pair>());
    }
    { // variant
        using V = std::variant<int, long>;
        constexpr V v0_0(std::in_place_index<0>, 0);
        constexpr V v0_1(std::in_place_index<0>, 1);
        constexpr V v1_0(std::in_place_index<1>, 0);
        constexpr V v1_1(std::in_place_index<1>, 1);

        spaceship_test<std::strong_ordering>(v0_0, v0_0, v0_1);
        spaceship_test<std::strong_ordering>(v0_1, v0_1, v1_0);
        spaceship_test<std::strong_ordering>(v1_0, v1_0, v1_1);

        static_assert(spaceship_test<std::strong_ordering>(v0_0, v0_0, v0_1));
        static_assert(spaceship_test<std::strong_ordering>(v0_1, v0_1, v1_0));
        static_assert(spaceship_test<std::strong_ordering>(v1_0, v1_0, v1_1));

        struct ThrowException {
            operator int() {
                throw "woof";
            }
        };
        V valueless(std::in_place_index<1>, 1729L);
        try {
            valueless.emplace<0>(ThrowException{});
        } catch (...) {
            // ignore exception
        }
        assert(valueless.valueless_by_exception());
        spaceship_test<std::strong_ordering>(valueless, valueless, v0_0);
        spaceship_test<std::strong_ordering>(valueless, valueless, v1_1);

        using M = std::monostate;
        constexpr M m1{};
        constexpr M m2{};
        assert((m1 <=> m2) == 0);
        static_assert((m1 <=> m2) == 0);
    }
    { // unique_ptr
        std::unique_ptr<int> p1{nullptr};
        std::unique_ptr<int>& p2 = p1;
        std::unique_ptr<int> p3{new int};

        spaceship_test<std::strong_ordering>(p1, p2, p3);
        spaceship_test<std::strong_ordering>(p1, nullptr, p3);
    }
    { // shared_ptr
        std::shared_ptr<int> p1{nullptr};
        std::shared_ptr<int>& p2 = p1; // same managed ptr
        std::shared_ptr<int> p3  = p2; // same stored ptr

        std::shared_ptr<int> p4{new int};
        std::shared_ptr<int> p5 = p4; // same stored ptr

        spaceship_test<std::strong_ordering>(p1, p2, p4);
        spaceship_test<std::strong_ordering>(p1, p3, p5);
        spaceship_test<std::strong_ordering>(p1, nullptr, p4);
    }
    { // slice
        std::slice a1(2, 3, 4);
        std::slice a2(2, 3, 4);
        std::slice a3(3, 3, 4);
        std::slice a4(2, 4, 4);
        std::slice a5(2, 3, 3);
        assert(a1 == a2);
        assert(a1 != a3);
        assert(a1 != a4);
        assert(a1 != a5);
    }
    { // chrono::duration
        using std::chrono::hours;
        using std::chrono::minutes;
        using std::chrono::seconds;

        spaceship_test<std::strong_ordering>(seconds{1}, seconds{1}, seconds{2});
        spaceship_test<std::strong_ordering>(seconds{3600}, hours{1}, minutes{61});

        using double_seconds     = std::chrono::duration<double>;
        using float_milliseconds = std::chrono::duration<float, std::milli>;
        using ntsc_fields        = std::chrono::duration<long long, std::ratio<1001, 60000>>;

        spaceship_test<std::partial_ordering>(double_seconds{1}, float_milliseconds{1000}, ntsc_fields{60});

        constexpr double_seconds nan_s{std::numeric_limits<double>::quiet_NaN()};
#ifdef __clang__ // TRANSITION, DevCom-445462
        static_assert(nan_s <=> nan_s == std::partial_ordering::unordered);
#endif // defined(__clang__)
        assert(nan_s <=> nan_s == std::partial_ordering::unordered);
    }
    { // chrono::time_point
        using std::chrono::milliseconds;
        using double_seconds = std::chrono::duration<double>;
        using sys_tp         = std::chrono::system_clock::time_point;
        using sys_ms         = std::chrono::time_point<std::chrono::system_clock, milliseconds>;
        using sys_double_s   = std::chrono::time_point<std::chrono::system_clock, double_seconds>;

        spaceship_test<std::strong_ordering>(sys_tp{}, sys_ms{}, sys_ms{milliseconds{1}});
        spaceship_test<std::partial_ordering>(sys_tp{}, sys_double_s{}, sys_double_s{double_seconds{1}});

        constexpr sys_double_s nan_tp{double_seconds{std::numeric_limits<double>::quiet_NaN()}};
#ifdef __clang__ // TRANSITION, DevCom-445462
        static_assert(nan_tp <=> nan_tp == std::partial_ordering::unordered);
#endif // defined(__clang__)
        assert(nan_tp <=> nan_tp == std::partial_ordering::unordered);

        using steady_tp = std::chrono::steady_clock::time_point;
        static_assert(!HasSpaceshipWith<sys_tp, steady_tp>);
    }
    { // filesystem::space_info
        constexpr std::filesystem::space_info si1{4'000'000'000'000, 2'000'000'000'000, 1'000'000'000'000};
        constexpr std::filesystem::space_info si2{4'000'000'000'000, 2'000'000'000'000, 1'000'000'000'000};
        constexpr std::filesystem::space_info si3{4'000'000'000'000, 2'000'000'000'000, 2'000'000'000'000};
        constexpr std::filesystem::space_info si4{4'000'000'000'000, 3'000'000'000'000, 1'000'000'000'000};
        constexpr std::filesystem::space_info si5{3'000'000'000'000, 2'000'000'000'000, 1'000'000'000'000};

        static_assert(si1 == si2);
        static_assert(si1 != si3);
        static_assert(si1 != si4);
        static_assert(si1 != si5);

        assert(si1 == si2);
        assert(si1 != si3);
        assert(si1 != si4);
        assert(si1 != si5);
    }
    { // filesystem::path
        const std::filesystem::path p1{R"(a/b/c)"};
        const std::filesystem::path p2{LR"(a\b\c)"};
        const std::filesystem::path p3{R"(a/b/d)"};

        spaceship_test<std::strong_ordering>(p1, p2, p3);
        unordered_containers_test(p1.begin(), p1.begin(), p1.end());
    }
    { // filesystem::file_status
        std::filesystem::file_status s1;
        s1.type(std::filesystem::file_type::regular);
        s1.permissions(std::filesystem::perms{0755});

        std::filesystem::file_status s2 = s1;

        std::filesystem::file_status s3 = s1;
        s3.type(std::filesystem::file_type::directory);

        std::filesystem::file_status s4 = s1;
        s4.permissions(std::filesystem::perms{0600});

        assert(s1 == s2);
        assert(s1 != s3);
        assert(s1 != s4);
    }
    { // filesystem::directory_entry
        const std::filesystem::directory_entry de1{u8R"(a/b/c)"};
        const std::filesystem::directory_entry de2{uR"(a\b\c)"};
        const std::filesystem::directory_entry de3{u8R"(a/b/d)"};

        spaceship_test<std::strong_ordering>(de1, de2, de3);
    }
    { // thread::id
        std::thread::id id1;
        std::thread::id id1_equal;
        std::thread::id id2 = std::this_thread::get_id();

        // Implementation-specific assumption: std::thread::id{} occurs first in the unspecified total ordering.
        spaceship_test<std::strong_ordering>(id1, id1_equal, id2);
    }
}

template <class Element, class Ordering>
void test_element_ordering() {
    if constexpr (!std::is_same_v<Element, SynthOrdered>) { // SynthOrdered inherently doesn't support <=> directly
        static_assert(std::is_same_v<std::compare_three_way_result_t<Element>, Ordering>);
    }

    static_assert(std::is_same_v<std::compare_three_way_result_t<std::array<Element, 3>>, Ordering>);
    static_assert(std::is_same_v<std::compare_three_way_result_t<std::deque<Element>>, Ordering>);
    static_assert(std::is_same_v<std::compare_three_way_result_t<std::list<Element>>, Ordering>);
    static_assert(std::is_same_v<std::compare_three_way_result_t<std::vector<Element>>, Ordering>);
    static_assert(std::is_same_v<std::compare_three_way_result_t<std::forward_list<Element>>, Ordering>);

    static_assert(std::is_same_v<std::compare_three_way_result_t<std::map<Element, Element>>, Ordering>);
    static_assert(std::is_same_v<std::compare_three_way_result_t<std::multimap<Element, Element>>, Ordering>);

    static_assert(std::is_same_v<std::compare_three_way_result_t<std::set<Element>>, Ordering>);
    static_assert(std::is_same_v<std::compare_three_way_result_t<std::multiset<Element>>, Ordering>);

    static_assert(std::is_same_v<std::compare_three_way_result_t<std::queue<Element>>, Ordering>);
    static_assert(std::is_same_v<std::compare_three_way_result_t<std::stack<Element>>, Ordering>);
}

int main() {
    ordering_test_cases();

    test_element_ordering<PartiallyOrdered, std::partial_ordering>();
    test_element_ordering<WeaklyOrdered, std::weak_ordering>();
    test_element_ordering<StronglyOrdered, std::strong_ordering>();
    test_element_ordering<SynthOrdered, std::weak_ordering>();
}
