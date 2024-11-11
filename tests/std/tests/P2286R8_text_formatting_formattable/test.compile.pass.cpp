// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// derived from libc++'s test files:
// * std/utilities/format/format.formattable/concept.formattable.compile.pass.cpp
// * std/utilities/format/format.formattable/concept.formattable.float.compile.pass.cpp

#include <array>
#include <chrono>
#include <concepts>
#include <cstddef>
#include <deque>
#include <format>
#include <forward_list>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <set>
#include <span>
#include <stack>
#include <stacktrace>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <valarray>
#include <variant>
#include <vector>

using namespace std;

template <class CharT>
concept formatter_supporting_character_type = same_as<CharT, char> || same_as<CharT, wchar_t>;

template <class CharT>
concept encoded_character_type = same_as<CharT, char>
#ifdef __cpp_char8_t
                              || same_as<CharT, char8_t>
#endif // defined(__cpp_char8_t)
                              || same_as<CharT, char16_t> || same_as<CharT, char32_t> || same_as<CharT, wchar_t>;

template <class T>
struct alternative_allocator {
    using value_type = T;

    alternative_allocator() = default;
    template <class U>
    constexpr alternative_allocator(const alternative_allocator<U>&) noexcept {}

    T* allocate(size_t n) {
        return allocator<T>{}.allocate(n);
    }

    void deallocate(T* p, size_t n) {
        allocator<T>{}.deallocate(p, n);
    }

    template <class U>
    bool operator==(const alternative_allocator<U>&) const noexcept {
        return true;
    }
};

template <class T, class CharT>
void assert_is_not_formattable() {
    static_assert(!formattable<T, CharT>);
    static_assert(!formattable<T&, CharT>);
    static_assert(!formattable<T&&, CharT>);
    static_assert(!formattable<const T, CharT>);
    static_assert(!formattable<const T&, CharT>);
    static_assert(!formattable<const T&&, CharT>);
}

template <class T, class CharT>
void assert_is_formattable() {
    // Only formatters for CharT == char || CharT == wchar_t are enabled for the standard formatters. When CharT is a
    // different type the formatter should be disabled.
    if constexpr (formatter_supporting_character_type<CharT>) {
        static_assert(formattable<T, CharT>);
        static_assert(formattable<T&, CharT>);
        static_assert(formattable<T&&, CharT>);
        static_assert(formattable<const T, CharT>);
        static_assert(formattable<const T&, CharT>);
        static_assert(formattable<const T&&, CharT>);
    } else {
        assert_is_not_formattable<T, CharT>();
    }
}

// Tests for P0645 Text Formatting
template <class CharT>
void test_P0645() {
    assert_is_formattable<CharT, CharT>();

    assert_is_formattable<CharT*, CharT>();
    assert_is_formattable<const CharT*, CharT>();
    assert_is_formattable<CharT[42], CharT>();
    if constexpr (encoded_character_type<CharT>) { // string and string_view only work with proper character types
        assert_is_formattable<basic_string<CharT>, CharT>();
        assert_is_formattable<basic_string_view<CharT>, CharT>();
    }

    assert_is_formattable<bool, CharT>();

    assert_is_formattable<signed char, CharT>();
    assert_is_formattable<short, CharT>();
    assert_is_formattable<int, CharT>();
    assert_is_formattable<long, CharT>();
    assert_is_formattable<long long, CharT>();

    assert_is_formattable<unsigned char, CharT>();
    assert_is_formattable<unsigned short, CharT>();
    assert_is_formattable<unsigned int, CharT>();
    assert_is_formattable<unsigned long, CharT>();
    assert_is_formattable<unsigned long long, CharT>();

    assert_is_formattable<float, CharT>();
    assert_is_formattable<double, CharT>();
    assert_is_formattable<long double, CharT>();

    assert_is_formattable<nullptr_t, CharT>();
    assert_is_formattable<void*, CharT>();
    assert_is_formattable<const void*, CharT>();
}

// Tests the special formatter that converts a char to a wchar_t.
void test_P0645_char_wchar_t() {
    assert_is_formattable<char, wchar_t>();
}

// Tests for P1361 Integration of chrono with text formatting
template <class CharT>
void test_P1361() {
    // The chrono formatters require localization support.
    // N4964 [time.format]/7
    //   If the chrono-specs is omitted, the chrono object is formatted as if by
    //   streaming it to basic_ostringstream<charT> os with the formatting
    //   locale imbued and copying os.str() through the output iterator of the
    //   context with additional padding and adjustments as specified by the format
    //   specifiers.

    assert_is_formattable<chrono::microseconds, CharT>();

    assert_is_formattable<chrono::sys_time<chrono::microseconds>, CharT>();
    assert_is_formattable<chrono::utc_time<chrono::microseconds>, CharT>();
    assert_is_formattable<chrono::tai_time<chrono::microseconds>, CharT>();
    assert_is_formattable<chrono::gps_time<chrono::microseconds>, CharT>();
    assert_is_formattable<chrono::file_time<chrono::microseconds>, CharT>();
    assert_is_formattable<chrono::local_time<chrono::microseconds>, CharT>();

    assert_is_formattable<chrono::day, CharT>();
    assert_is_formattable<chrono::month, CharT>();
    assert_is_formattable<chrono::year, CharT>();

    assert_is_formattable<chrono::weekday, CharT>();
    assert_is_formattable<chrono::weekday_indexed, CharT>();
    assert_is_formattable<chrono::weekday_last, CharT>();

    assert_is_formattable<chrono::month_day, CharT>();
    assert_is_formattable<chrono::month_day_last, CharT>();
    assert_is_formattable<chrono::month_weekday, CharT>();
    assert_is_formattable<chrono::month_weekday_last, CharT>();

    assert_is_formattable<chrono::year_month, CharT>();
    assert_is_formattable<chrono::year_month_day, CharT>();
    assert_is_formattable<chrono::year_month_day_last, CharT>();
    assert_is_formattable<chrono::year_month_weekday, CharT>();
    assert_is_formattable<chrono::year_month_weekday_last, CharT>();

    assert_is_formattable<chrono::hh_mm_ss<chrono::microseconds>, CharT>();

    assert_is_formattable<chrono::sys_info, CharT>();
    assert_is_formattable<chrono::local_info, CharT>();

    assert_is_formattable<chrono::zoned_time<chrono::seconds>, CharT>();
}

// Tests for P2693 Formatting thread::id and stacktrace
template <class CharT>
void test_P2693() {
    assert_is_formattable<thread::id, CharT>();
    if constexpr (same_as<CharT, char>) {
        assert_is_formattable<stacktrace_entry, CharT>();
        assert_is_formattable<stacktrace, CharT>();
    } else {
        assert_is_not_formattable<stacktrace_entry, CharT>();
        assert_is_not_formattable<stacktrace, CharT>();
    }
}

template <class CharT, class Vector>
void test_P2286_vector_bool() {
    assert_is_formattable<typename Vector::reference, CharT>();

    // The const_reference shall be bool.
    assert_is_formattable<typename Vector::const_reference, CharT>();
}

// Tests for P2286 Formatting ranges
template <class CharT>
void test_P2286() {
    assert_is_formattable<array<int, 42>, CharT>();
    assert_is_formattable<vector<int>, CharT>();
    assert_is_formattable<deque<int>, CharT>();
    assert_is_formattable<forward_list<int>, CharT>();
    assert_is_formattable<list<int>, CharT>();

    assert_is_formattable<set<int>, CharT>();
    assert_is_formattable<map<int, int>, CharT>();
    assert_is_formattable<multiset<int>, CharT>();
    assert_is_formattable<multimap<int, int>, CharT>();

    assert_is_formattable<unordered_set<int>, CharT>();
    assert_is_formattable<unordered_map<int, int>, CharT>();
    assert_is_formattable<unordered_multiset<int>, CharT>();
    assert_is_formattable<unordered_multimap<int, int>, CharT>();

    assert_is_formattable<span<int>, CharT>();

    assert_is_formattable<valarray<int>, CharT>();

    assert_is_formattable<pair<int, int>, CharT>();
    assert_is_formattable<tuple<int>, CharT>();

    test_P2286_vector_bool<CharT, vector<bool>>();
    test_P2286_vector_bool<CharT, pmr::vector<bool>>();
    test_P2286_vector_bool<CharT, vector<bool, alternative_allocator<bool>>>();
}

// Tests volatile qualified objects are no longer formattable.
template <class CharT>
void test_LWG3631() {
    assert_is_not_formattable<volatile CharT, CharT>();

    assert_is_not_formattable<volatile bool, CharT>();

    assert_is_not_formattable<volatile int, CharT>();
    assert_is_not_formattable<volatile unsigned int, CharT>();

    assert_is_not_formattable<volatile chrono::microseconds, CharT>();
    assert_is_not_formattable<volatile chrono::sys_time<chrono::microseconds>, CharT>();
    assert_is_not_formattable<volatile chrono::day, CharT>();

    assert_is_not_formattable<array<volatile int, 42>, CharT>();

    assert_is_not_formattable<pair<volatile int, int>, CharT>();
    assert_is_not_formattable<pair<int, volatile int>, CharT>();
    assert_is_not_formattable<pair<volatile int, volatile int>, CharT>();
}

struct c {};
enum e { a };
enum class ec { a };

template <class CharT>
void test_disabled() {
    if constexpr (!same_as<CharT, char>) {
        assert_is_not_formattable<char*, CharT>();
        assert_is_not_formattable<const char*, CharT>();
        assert_is_not_formattable<char[42], CharT>();
        assert_is_not_formattable<string, CharT>();
        assert_is_not_formattable<string_view, CharT>();
    }

    assert_is_not_formattable<c, CharT>();
    assert_is_not_formattable<const c, CharT>();
    assert_is_not_formattable<volatile c, CharT>();
    assert_is_not_formattable<const volatile c, CharT>();

    assert_is_not_formattable<e, CharT>();
    assert_is_not_formattable<const e, CharT>();
    assert_is_not_formattable<volatile e, CharT>();
    assert_is_not_formattable<const volatile e, CharT>();

    assert_is_not_formattable<ec, CharT>();
    assert_is_not_formattable<const ec, CharT>();
    assert_is_not_formattable<volatile ec, CharT>();
    assert_is_not_formattable<const volatile ec, CharT>();

    assert_is_not_formattable<int*, CharT>();
    assert_is_not_formattable<const int*, CharT>();
    assert_is_not_formattable<volatile int*, CharT>();
    assert_is_not_formattable<const volatile int*, CharT>();

    assert_is_not_formattable<c*, CharT>();
    assert_is_not_formattable<const c*, CharT>();
    assert_is_not_formattable<volatile c*, CharT>();
    assert_is_not_formattable<const volatile c*, CharT>();

    assert_is_not_formattable<e*, CharT>();
    assert_is_not_formattable<const e*, CharT>();
    assert_is_not_formattable<volatile e*, CharT>();
    assert_is_not_formattable<const volatile e*, CharT>();

    assert_is_not_formattable<ec*, CharT>();
    assert_is_not_formattable<const ec*, CharT>();
    assert_is_not_formattable<volatile ec*, CharT>();
    assert_is_not_formattable<const volatile ec*, CharT>();

    assert_is_not_formattable<void (*)(), CharT>();
    assert_is_not_formattable<void (c::*)(), CharT>();
    assert_is_not_formattable<void (c::*)() const, CharT>();

    assert_is_not_formattable<optional<int>, CharT>();
    assert_is_not_formattable<variant<int>, CharT>();

    assert_is_not_formattable<shared_ptr<c>, CharT>();
    assert_is_not_formattable<unique_ptr<c>, CharT>();

    assert_is_not_formattable<array<c, 42>, CharT>();
    assert_is_not_formattable<vector<c>, CharT>();
    assert_is_not_formattable<deque<c>, CharT>();
    assert_is_not_formattable<forward_list<c>, CharT>();
    assert_is_not_formattable<list<c>, CharT>();

    assert_is_not_formattable<set<c>, CharT>();
    assert_is_not_formattable<map<c, int>, CharT>();
    assert_is_not_formattable<multiset<c>, CharT>();
    assert_is_not_formattable<multimap<c, int>, CharT>();

    assert_is_not_formattable<unordered_set<c>, CharT>();
    assert_is_not_formattable<unordered_map<c, int>, CharT>();
    assert_is_not_formattable<unordered_multiset<c>, CharT>();
    assert_is_not_formattable<unordered_multimap<c, int>, CharT>();

    assert_is_not_formattable<stack<c>, CharT>();
    assert_is_not_formattable<queue<c>, CharT>();
    assert_is_not_formattable<priority_queue<c>, CharT>();

    assert_is_not_formattable<span<c>, CharT>();

    assert_is_not_formattable<valarray<c>, CharT>();

    assert_is_not_formattable<pair<c, int>, CharT>();
    assert_is_not_formattable<tuple<c>, CharT>();

    assert_is_not_formattable<optional<c>, CharT>();
    assert_is_not_formattable<variant<c>, CharT>();
}

struct abstract {
    virtual ~abstract() = 0;
};

template <formatter_supporting_character_type CharT>
struct formatter<abstract, CharT> {
    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& parse_ctx) {
        return parse_ctx.begin();
    }

    template <class FormatContext>
    FormatContext::iterator format(const abstract&, FormatContext& ctx) const {
        return ctx.out();
    }
};

template <class CharT>
void test_abstract_class() {
    assert_is_formattable<abstract, CharT>();
}

template <class CharT>
void test() {
    test_P0645<CharT>();
    test_P1361<CharT>();
    test_P2693<CharT>();
    test_P2286<CharT>();
    test_LWG3631<CharT>();
    test_abstract_class<CharT>();
    test_disabled<CharT>();
}

void test() {
    test_P0645_char_wchar_t();
    test<char>();
    test<wchar_t>();
#ifdef __cpp_char8_t
    test<char8_t>();
#endif // defined(__cpp_char8_t)
    test<char16_t>();
    test<char32_t>();
}
