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
#include <optional>
#include <queue>
#include <ranges>
#include <regex>
#include <set>
#include <stack>
#include <string>
#include <system_error>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

template <class T>
using SpaceshipType = std::compare_three_way_result_t<T, T>;

using PartiallyOrdered = double;
struct WeaklyOrdered {};
using StronglyOrdered = int;

[[nodiscard]] constexpr bool operator==(const WeaklyOrdered&, const WeaklyOrdered&) {
    return true;
}

[[nodiscard]] constexpr std::weak_ordering operator<=>(const WeaklyOrdered&, const WeaklyOrdered&) {
    return std::weak_ordering::equivalent;
}

[[nodiscard]] constexpr std::partial_ordering operator<=>(const WeaklyOrdered&, const PartiallyOrdered&) {
    return std::partial_ordering::equivalent;
}

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

// TRANSITION, <=> is unimplemented for string.
template <class T>
inline constexpr bool has_string = false;
template <class T>
inline constexpr bool has_string<std::pair<T, const std::string>> = true;
template <class T>
inline constexpr bool has_string<std::pair<const std::string, T>> = true;
template <>
inline constexpr bool has_string<std::pair<const std::string, const std::string>> = true;
template <>
inline constexpr bool has_string<const std::string> = true;

template <class T>
inline constexpr bool has_synth_ordered = false;
template <class T>
inline constexpr bool has_synth_ordered<std::pair<T, const SynthOrdered>> = true;
template <class T>
inline constexpr bool has_synth_ordered<std::pair<const SynthOrdered, T>> = true;
template <>
inline constexpr bool has_synth_ordered<std::pair<const SynthOrdered, const SynthOrdered>> = true;
template <>
inline constexpr bool has_synth_ordered<const SynthOrdered> = true;
template <>
inline constexpr bool has_synth_ordered<SynthOrdered> = true;

template <class Container>
void ordered_containers_test(const Container& smaller, const Container& smaller_equal, const Container& larger) {
    using Elem = typename Container::value_type;

    if constexpr (has_string<Elem> || has_synth_ordered<Elem>) {
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
void tuple_like_test() {
    {
        constexpr TupleLike<int, int> t1{1, 1};
        constexpr TupleLike<int, int> t1_equal{1, 1};
        constexpr TupleLike<int, int> t2{2, 1};

        static_assert(spaceship_test<std::strong_ordering>(t1, t1_equal, t2));
    }
    {
        TupleLike<int, int> t1{1, 1};
        TupleLike<int, int> t1_equal{1, 1};
        TupleLike<int, int> t2{2, 1};

        spaceship_test<std::strong_ordering>(t1, t1_equal, t2);
    }
    {
        constexpr TupleLike<int, double> t1{1, 1.0};
        constexpr TupleLike<int, double> t1_equal{1, 1.0};
        constexpr TupleLike<int, double> t2{2, 1.0};

        static_assert(spaceship_test<std::partial_ordering>(t1, t1_equal, t2));
    }
    {
        TupleLike<int, double> t1{1, 1.0};
        TupleLike<int, double> t1_equal{1, 1.0};
        TupleLike<int, double> t2{2, 1.0};

        spaceship_test<std::partial_ordering>(t1, t1_equal, t2);
    }
}

template <auto SmallVal, decltype(SmallVal) EqualVal, decltype(EqualVal) LargeVal>
void optional_test() {
    using ReturnType = std::compare_three_way_result_t<decltype(SmallVal), decltype(LargeVal)>;

    {
        constexpr std::optional o1(SmallVal);
        constexpr std::optional o1_equal(EqualVal);
        constexpr std::optional o2(LargeVal);

        static_assert(spaceship_test<ReturnType>(o1, o1_equal, o2));
    }
    {
        std::optional o1(SmallVal);
        std::optional o1_equal(EqualVal);
        std::optional o2(LargeVal);

        spaceship_test<ReturnType>(o1, o1_equal, o2);
    }
    {
        constexpr std::optional<decltype(SmallVal)> o1(std::nullopt);
        constexpr std::optional<decltype(EqualVal)> o1_equal(std::nullopt);
        constexpr std::optional o2(LargeVal);

        static_assert(spaceship_test<ReturnType>(o1, o1_equal, o2));
    }
    {
        std::optional<decltype(SmallVal)> o1(std::nullopt);
        std::optional<decltype(EqualVal)> o1_equal(std::nullopt);
        std::optional o2(LargeVal);

        spaceship_test<ReturnType>(o1, o1_equal, o2);
    }
    {
        constexpr std::optional o1(SmallVal);

        static_assert(spaceship_test<ReturnType>(o1, EqualVal, LargeVal));
    }
    {
        std::optional o1(SmallVal);

        spaceship_test<ReturnType>(o1, EqualVal, LargeVal);
    }
    {
        constexpr std::optional<decltype(SmallVal)> o1(std::nullopt);
        constexpr std::optional o2(LargeVal);

        static_assert(spaceship_test<ReturnType>(o1, std::nullopt, o2));
    }
    {
        std::optional<decltype(SmallVal)> o1(std::nullopt);
        std::optional o2(LargeVal);

        spaceship_test<ReturnType>(o1, std::nullopt, o2);
    }
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
    }
    { // array SynthOrdered
        std::array<SynthOrdered, 3> a = {10, 20, 30};
        std::array<SynthOrdered, 3> b = {10, 20, 40};
        ordered_containers_test(a, a, b);
    }
    { // deque
        std::deque<int> a1(3, 100);
        std::deque<int> a2(3, 100);
        std::deque<int> b1(2, 200);
        ordered_containers_test(a1, a2, b1);
    }
    { // deque SynthOrdered
        std::deque<SynthOrdered> a = {10, 20, 30};
        std::deque<SynthOrdered> b = {10, 20, 40};
        ordered_containers_test(a, a, b);
    }
    { // list
        std::list<int> a1(3, 100);
        std::list<int> a2(3, 100);
        std::list<int> b1(2, 200);
        ordered_containers_test(a1, a2, b1);
    }
    { // list SynthOrdered
        std::list<SynthOrdered> a = {10, 20, 30};
        std::list<SynthOrdered> b = {10, 20, 40};
        ordered_containers_test(a, a, b);
    }
    { // vector
        std::vector<int> a1(3, 100);
        std::vector<int> a2(3, 100);
        std::vector<int> b1(2, 200);
        ordered_containers_test(a1, a2, b1);
    }
    { // vector SynthOrdered
        std::vector<SynthOrdered> a = {10, 20, 30};
        std::vector<SynthOrdered> b = {10, 20, 40};
        ordered_containers_test(a, a, b);
    }
    { // vector<bool>
        std::vector<bool> c1 = {false, true, false};
        std::vector<bool> c2 = {false, true, false};
        std::vector<bool> d1 = {true, false};
        ordered_containers_test(c1, c2, d1);
    }
    { // forward_list
        std::forward_list<int> a1(3, 100);
        std::forward_list<int> a2(3, 100);
        std::forward_list<int> b1(2, 200);
        ordered_containers_test(a1, a2, b1);
    }
    { // forward_list SynthOrdered
        std::forward_list<SynthOrdered> a = {10, 20, 30};
        std::forward_list<SynthOrdered> b = {10, 20, 40};
        ordered_containers_test(a, a, b);
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
    { // map SynthOrdered
        std::map<SynthOrdered, char> a = {{10, 'z'}, {20, 'z'}, {30, 'z'}};
        std::map<SynthOrdered, char> b = {{10, 'z'}, {20, 'z'}, {40, 'z'}};
        ordered_containers_test(a, a, b);
    }
    { // multimap
        std::multimap<char, int> a1 = {{'a', 1}, {'b', 2}, {'a', 3}};
        std::multimap<char, int> a2 = {{'a', 1}, {'a', 3}, {'b', 2}};
        std::multimap<char, int> b1 = {{'z', 4}, {'y', 90}, {'z', 12}};
        ordered_containers_test(a1, a2, b1);
    }
    { // multimap SynthOrdered
        std::multimap<SynthOrdered, char> a = {{10, 'z'}, {20, 'z'}, {30, 'z'}};
        std::multimap<SynthOrdered, char> b = {{10, 'z'}, {20, 'z'}, {40, 'z'}};
        ordered_containers_test(a, a, b);
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
    { // set SynthOrdered
        std::set<SynthOrdered> a = {10, 20, 30};
        std::set<SynthOrdered> b = {10, 20, 40};
        ordered_containers_test(a, a, b);
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
    { // multiset SynthOrdered
        std::multiset<SynthOrdered> a = {10, 20, 30};
        std::multiset<SynthOrdered> b = {10, 20, 40};
        ordered_containers_test(a, a, b);
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

        // TRANSITION, std::char_traits<char> doesn't define comparison_category
        spaceship_test<std::weak_ordering>(sm1, sm1_equal, sm2);
        spaceship_test<std::weak_ordering>(sm1, s1, s2);
        spaceship_test<std::weak_ordering>(sm1, s1.c_str(), s2.c_str());
        spaceship_test<std::weak_ordering>(sm3, 'c', 'm');
        spaceship_test<std::weak_ordering>(s1, sm1, sm2);
        spaceship_test<std::weak_ordering>(s1.c_str(), sm1, sm2);
        spaceship_test<std::weak_ordering>('c', sm3, sm4);

        using StronglyOrderedMatch = std::ssub_match;
        using WeaklyOrderedMatch   = std::sub_match<std::basic_string<WeaklyOrderedChar>::const_iterator>;
        using WeaklyOrderdByOmissionMatch =
            std::sub_match<std::basic_string<WeaklyOrderedByOmissionChar>::const_iterator>;
        using PartiallyOrderedMatch = std::sub_match<std::basic_string<PartiallyOrderedChar>::const_iterator>;

        // TRANSITION, std::char_traits<char> doesn't define comparison_category
        static_assert(std::is_same_v<SpaceshipType<StronglyOrderedMatch>, std::weak_ordering>);
        static_assert(std::is_same_v<SpaceshipType<WeaklyOrderedMatch>, std::weak_ordering>);
        static_assert(std::is_same_v<SpaceshipType<WeaklyOrderdByOmissionMatch>, std::weak_ordering>);
        static_assert(std::is_same_v<SpaceshipType<PartiallyOrderedMatch>, std::partial_ordering>);
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

#if defined(__cpp_nontype_template_args) && __cpp_non_type_template_args >= 201911
        optional_test<0.0, 0.0, 1.0>();
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

        {
            constexpr std::tuple<int, double> t1{1, 1.0};
            constexpr std::tuple<double, double> t1_equal{1.0, 1.0};
            constexpr std::tuple<int, double> t2{2, 1.0};

            static_assert(spaceship_test<std::partial_ordering>(t1, t1_equal, t2));
        }
        {
            std::tuple<int, double> t1{1, 1.0};
            std::tuple<double, double> t1_equal{1.0, 1.0};
            std::tuple<int, double> t2{2, 1.0};

            spaceship_test<std::partial_ordering>(t1, t1_equal, t2);
        }
        {
            constexpr std::tuple<int, double> t1{1, 1.0};
            constexpr std::tuple<int, double> t1_equal{1, 1.0};
            constexpr std::tuple<double, double> t2{2.0, 1.0};

            static_assert(spaceship_test<std::partial_ordering>(t1, t1_equal, t2));
        }
        {
            std::tuple<int, double> t1{1, 1.0};
            std::tuple<int, double> t1_equal{1, 1.0};
            std::tuple<double, double> t2{2.0, 1.0};

            spaceship_test<std::partial_ordering>(t1, t1_equal, t2);
        }
        {
            constexpr std::tuple<> empty1, empty2;

            static_assert(std::is_same_v<decltype(empty1 <=> empty2), std::strong_ordering>);
            static_assert((empty1 <=> empty2) == std::strong_ordering::equal);
        }

        // TRANSITION, probably a defect in the standard. Should return std::partial_ordering.
        static_assert(std::is_same_v<std::compare_three_way_result_t<std::tuple<WeaklyOrdered, WeaklyOrdered>,
                                         std::tuple<PartiallyOrdered, PartiallyOrdered>>,
            std::weak_ordering>);
        static_assert(std::is_same_v<std::compare_three_way_result_t<std::tuple<WeaklyOrdered, WeaklyOrdered>,
                                         std::tuple<WeaklyOrdered, WeaklyOrdered>>,
            std::weak_ordering>);
        static_assert(std::is_same_v<std::compare_three_way_result_t<std::tuple<WeaklyOrdered, WeaklyOrdered>,
                                         std::tuple<StronglyOrdered, StronglyOrdered>>,
            std::weak_ordering>);
    }
    { // pair
        tuple_like_test<std::pair>();
    }
}

template <class Element, class Ordering>
void test_element_ordering() {
    if constexpr (!std::is_same_v<Element, SynthOrdered>) { // SynthOrdered inherently doesn't support <=> directly
        static_assert(std::is_same_v<SpaceshipType<Element>, Ordering>);
    }

    static_assert(std::is_same_v<SpaceshipType<std::array<Element, 3>>, Ordering>);
    static_assert(std::is_same_v<SpaceshipType<std::deque<Element>>, Ordering>);
    static_assert(std::is_same_v<SpaceshipType<std::list<Element>>, Ordering>);
    static_assert(std::is_same_v<SpaceshipType<std::vector<Element>>, Ordering>);
    static_assert(std::is_same_v<SpaceshipType<std::forward_list<Element>>, Ordering>);

    static_assert(std::is_same_v<SpaceshipType<std::map<Element, Element>>, Ordering>);
    static_assert(std::is_same_v<SpaceshipType<std::multimap<Element, Element>>, Ordering>);

    static_assert(std::is_same_v<SpaceshipType<std::set<Element>>, Ordering>);
    static_assert(std::is_same_v<SpaceshipType<std::multiset<Element>>, Ordering>);

    static_assert(std::is_same_v<SpaceshipType<std::queue<Element>>, Ordering>);
    static_assert(std::is_same_v<SpaceshipType<std::stack<Element>>, Ordering>);
}

int main() {
    ordering_test_cases();

    test_element_ordering<PartiallyOrdered, std::partial_ordering>();
    test_element_ordering<WeaklyOrdered, std::weak_ordering>();
    test_element_ordering<StronglyOrdered, std::strong_ordering>();
    test_element_ordering<SynthOrdered, std::weak_ordering>();
}
