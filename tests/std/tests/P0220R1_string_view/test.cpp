// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include <constexpr_char_traits.hpp>

using namespace std;

constexpr auto ptrdiff_max = numeric_limits<ptrdiff_t>::max();

template <typename Expected, typename Fn>
void assert_throws(Fn fn) {
    try {
        (void) fn();
        abort();
    } catch (const Expected&) {
    }
}

using c_string_view = basic_string_view<char, constexpr_char_traits>;

struct conversion_to_string_view {
    operator string_view() {
        return string_view{};
    }
};

struct nofail_conversion_to_string_view {
    operator string_view() noexcept {
        return string_view{};
    }
};

struct evil_conversion_to_string_view_rvalue_only {
    operator string_view() && noexcept {
        return string_view{};
    }

    operator string_view() const& {
        return string_view{};
    }
};

struct evil_conversion_to_string_view_lvalue_only {
    operator string_view() && {
        return string_view{};
    }

    operator string_view() const& noexcept {
        return string_view{};
    }
};

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunneeded-internal-declaration"
#endif // __clang__
const evil_conversion_to_string_view_rvalue_only convert_rvalue_only{};
const evil_conversion_to_string_view_lvalue_only convert_lvalue_only{};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif // __clang__

// N4901 [string.view.template.general]/4
static_assert(is_trivially_copyable_v<string_view>);
static_assert(is_trivially_copyable_v<wstring_view>);

// Implied by N4901 [class.prop]/1
static_assert(is_trivially_move_constructible_v<string_view>);
static_assert(is_trivially_copy_constructible_v<string_view>);
static_assert(is_trivially_move_assignable_v<string_view>);
static_assert(is_trivially_copy_assignable_v<string_view>);
static_assert(is_trivially_destructible_v<string_view>);

static_assert(is_trivially_move_constructible_v<wstring_view>);
static_assert(is_trivially_copy_constructible_v<wstring_view>);
static_assert(is_trivially_move_assignable_v<wstring_view>);
static_assert(is_trivially_copy_assignable_v<wstring_view>);
static_assert(is_trivially_destructible_v<wstring_view>);

// noexcept assertions:
// (functions that explicitly throw have their throws tested and therefore have no static_asserts)
static_assert(noexcept(string_view{}));
static_assert(noexcept(string_view{string_view{}}));
string_view g_example{"text", 2};

// the assignment operator is arguably noexcept in the WP (it is defaulted)
static_assert(noexcept(declval<string_view&>() = declval<string_view>()));
static_assert(noexcept(string_view{"text"})); // strengthened
static_assert(noexcept(string_view{"text", 2})); // strengthened
static_assert(noexcept(g_example.begin()));
static_assert(noexcept(g_example.end()));
static_assert(noexcept(g_example.cbegin()));
static_assert(noexcept(g_example.cend()));
static_assert(noexcept(g_example.rbegin()));
static_assert(noexcept(g_example.rend()));
static_assert(noexcept(g_example.crbegin()));
static_assert(noexcept(g_example.crend()));
static_assert(noexcept(g_example.size()));
static_assert(noexcept(g_example.length()));
static_assert(noexcept(g_example.max_size()));
static_assert(noexcept(g_example.empty()));
static_assert(noexcept(g_example[0])); // strengthened
// at throws out_of_range
static_assert(noexcept(g_example.front())); // strengthened
static_assert(noexcept(g_example.back())); // strengthened
static_assert(noexcept(g_example.data()));
static_assert(noexcept(g_example.remove_prefix(1))); // strengthened
static_assert(noexcept(g_example.remove_suffix(1))); // strengthened
string_view g_example_swap_target{"text", 3};
static_assert(noexcept(g_example.swap(g_example_swap_target)));
// copy throws out_of_range
// substr throws out_of_range
static_assert(noexcept(g_example.compare(string_view{})));
// compare(pos1, n1, basic_string_view) throws out_of_range
// compare(pos1, n1, basic_string_view, pos2, n2) throws out_of_range
static_assert(noexcept(g_example.compare("literal"))); // strengthened
// compare(pos1, n1, const charT*) throws out_of_range and calls through char_traits
// compare(pos1, n1, const charT*, n2) throws out_of_range and calls through char_traits
static_assert(noexcept(g_example.find(string_view{})));
static_assert(noexcept(g_example.find('c')));
static_assert(noexcept(g_example.find("text", 0, 0))); // strengthened
static_assert(noexcept(g_example.find("text"))); // strengthened
static_assert(noexcept(g_example.rfind(string_view{})));
static_assert(noexcept(g_example.rfind('c')));
static_assert(noexcept(g_example.rfind("text", 0, 0))); // strengthened
static_assert(noexcept(g_example.rfind("text"))); // strengthened
static_assert(noexcept(g_example.find_first_of(string_view{})));
static_assert(noexcept(g_example.find_first_of('c')));
static_assert(noexcept(g_example.find_first_of("text", 0, 0))); // strengthened
static_assert(noexcept(g_example.find_first_of("text"))); // strengthened
static_assert(noexcept(g_example.find_last_of(string_view{})));
static_assert(noexcept(g_example.find_last_of('c')));
static_assert(noexcept(g_example.find_last_of("text", 0, 0))); // strengthened
static_assert(noexcept(g_example.find_last_of("text"))); // strengthened
static_assert(noexcept(g_example.find_first_not_of(string_view{})));
static_assert(noexcept(g_example.find_first_not_of('c')));
static_assert(noexcept(g_example.find_first_not_of("text", 0, 0))); // strengthened
static_assert(noexcept(g_example.find_first_not_of("text"))); // strengthened
static_assert(noexcept(g_example.find_last_not_of(string_view{})));
static_assert(noexcept(g_example.find_last_not_of('c')));
static_assert(noexcept(g_example.find_last_not_of("text", 0, 0))); // strengthened
static_assert(noexcept(g_example.find_last_not_of("text"))); // strengthened

static_assert(noexcept(string_view{} == string_view{}));
static_assert(!noexcept(conversion_to_string_view{} == string_view{}));
static_assert(noexcept(evil_conversion_to_string_view_rvalue_only{} == string_view{}));
static_assert(!noexcept(convert_rvalue_only == string_view{}));
static_assert(!noexcept(evil_conversion_to_string_view_lvalue_only{} == string_view{}));
static_assert(noexcept(convert_lvalue_only == string_view{}));
static_assert(!noexcept(string_view{} == conversion_to_string_view{}));
static_assert(noexcept(string_view{} == evil_conversion_to_string_view_rvalue_only{}));
static_assert(!noexcept(string_view{} == convert_rvalue_only));
static_assert(!noexcept(string_view{} == evil_conversion_to_string_view_lvalue_only{}));
static_assert(noexcept(string_view{} == convert_lvalue_only));

static_assert(noexcept(string_view{} != string_view{}));
static_assert(!noexcept(conversion_to_string_view{} != string_view{}));
static_assert(noexcept(evil_conversion_to_string_view_rvalue_only{} != string_view{}));
static_assert(!noexcept(convert_rvalue_only != string_view{}));
static_assert(!noexcept(evil_conversion_to_string_view_lvalue_only{} != string_view{}));
static_assert(noexcept(convert_lvalue_only != string_view{}));
static_assert(!noexcept(string_view{} != conversion_to_string_view{}));
static_assert(noexcept(string_view{} != evil_conversion_to_string_view_rvalue_only{}));
static_assert(!noexcept(string_view{} != convert_rvalue_only));
static_assert(!noexcept(string_view{} != evil_conversion_to_string_view_lvalue_only{}));
static_assert(noexcept(string_view{} != convert_lvalue_only));

static_assert(noexcept(string_view{} < string_view{}));
static_assert(!noexcept(conversion_to_string_view{} < string_view{}));
static_assert(noexcept(evil_conversion_to_string_view_rvalue_only{} < string_view{}));
static_assert(!noexcept(convert_rvalue_only < string_view{}));
static_assert(!noexcept(evil_conversion_to_string_view_lvalue_only{} < string_view{}));
static_assert(noexcept(convert_lvalue_only < string_view{}));
static_assert(!noexcept(string_view{} < conversion_to_string_view{}));
static_assert(noexcept(string_view{} < evil_conversion_to_string_view_rvalue_only{}));
static_assert(!noexcept(string_view{} < convert_rvalue_only));
static_assert(!noexcept(string_view{} < evil_conversion_to_string_view_lvalue_only{}));
static_assert(noexcept(string_view{} < convert_lvalue_only));

static_assert(noexcept(string_view{} > string_view{}));
static_assert(!noexcept(conversion_to_string_view{} > string_view{}));
static_assert(noexcept(evil_conversion_to_string_view_rvalue_only{} > string_view{}));
static_assert(!noexcept(convert_rvalue_only > string_view{}));
static_assert(!noexcept(evil_conversion_to_string_view_lvalue_only{} > string_view{}));
static_assert(noexcept(convert_lvalue_only > string_view{}));
static_assert(!noexcept(string_view{} > conversion_to_string_view{}));
static_assert(noexcept(string_view{} > evil_conversion_to_string_view_rvalue_only{}));
static_assert(!noexcept(string_view{} > convert_rvalue_only));
static_assert(!noexcept(string_view{} > evil_conversion_to_string_view_lvalue_only{}));
static_assert(noexcept(string_view{} > convert_lvalue_only));

static_assert(noexcept(string_view{} <= string_view{}));
static_assert(!noexcept(conversion_to_string_view{} <= string_view{}));
static_assert(noexcept(evil_conversion_to_string_view_rvalue_only{} <= string_view{}));
static_assert(!noexcept(convert_rvalue_only <= string_view{}));
static_assert(!noexcept(evil_conversion_to_string_view_lvalue_only{} <= string_view{}));
static_assert(noexcept(convert_lvalue_only <= string_view{}));
static_assert(!noexcept(string_view{} <= conversion_to_string_view{}));
static_assert(noexcept(string_view{} <= evil_conversion_to_string_view_rvalue_only{}));
static_assert(!noexcept(string_view{} <= convert_rvalue_only));
static_assert(!noexcept(string_view{} <= evil_conversion_to_string_view_lvalue_only{}));
static_assert(noexcept(string_view{} <= convert_lvalue_only));

static_assert(noexcept(string_view{} >= string_view{}));
static_assert(!noexcept(conversion_to_string_view{} >= string_view{}));
static_assert(noexcept(evil_conversion_to_string_view_rvalue_only{} >= string_view{}));
static_assert(!noexcept(convert_rvalue_only >= string_view{}));
static_assert(!noexcept(evil_conversion_to_string_view_lvalue_only{} >= string_view{}));
static_assert(noexcept(convert_lvalue_only >= string_view{}));
static_assert(!noexcept(string_view{} >= conversion_to_string_view{}));
static_assert(noexcept(string_view{} >= evil_conversion_to_string_view_rvalue_only{}));
static_assert(!noexcept(string_view{} >= convert_rvalue_only));
static_assert(!noexcept(string_view{} >= evil_conversion_to_string_view_lvalue_only{}));
static_assert(noexcept(string_view{} >= convert_lvalue_only));

template <typename CharT>
struct choose_literal; // not defined

template <>
struct choose_literal<char> {
    static constexpr const char* choose(const char* s, const wchar_t*) {
        return s;
    }
};

template <>
struct choose_literal<wchar_t> {
    static constexpr const wchar_t* choose(const char*, const wchar_t* s) {
        return s;
    }
};

#define TYPED_LITERAL(CharT, Literal) (choose_literal<CharT>::choose(Literal, L##Literal))

constexpr bool test_case_default_constructor() {
    string_view sv;
    assert(sv.data() == nullptr);
    assert(sv.size() == 0);
    assert(sv.empty());

    return true;
}

template <typename Traits = char_traits<char>>
constexpr bool test_case_ntcts_constructor() {
    const char* const expectedData = "null-terminated";
    basic_string_view<char, Traits> sv(expectedData);
    assert(sv.data() == expectedData);
    assert(sv.size() == 15);
    assert(sv.length() == 15);
    assert(!sv.empty());
    assert(sv[1] == 'u');
    assert(sv.at(1) == 'u');
    assert(sv.front() == 'n');
    assert(sv.back() == 'd');

    const char* const expectedEmptyData = "";
    basic_string_view<char, Traits> svEmpty(expectedEmptyData);
    assert(svEmpty.data() == expectedEmptyData);
    assert(svEmpty.size() == 0);
    assert(svEmpty.length() == 0);
    assert(svEmpty.empty());

    return true;
}

constexpr bool test_case_buffer_constructor() {
    const char expectedDataArr[]   = {'n', 'o', ' ', 'n', 'u', 'l', 'l'};
    const char* const expectedData = expectedDataArr;
    string_view sv(expectedData, 7);
    assert(sv.data() == expectedData);
    assert(sv.size() == 7);
    assert(sv.length() == 7);
    assert(!sv.empty());
    assert(sv[1] == 'o');
    assert(sv.at(1) == 'o');
    assert(sv.front() == 'n');
    assert(sv.back() == 'l');

    return true;
}

constexpr bool test_case_contiguous_constructor() {
#ifdef __cpp_lib_concepts
    const array expectedData{'n', 'o', ' ', 'n', 'u', 'l', 'l'};
    // Also tests the corresponding deduction guide:
    same_as<string_view> auto sv = basic_string_view(expectedData.begin(), expectedData.end());
    assert(sv.data() == expectedData.data());
    assert(sv.size() == 7);
    assert(sv.length() == 7);
    assert(!sv.empty());
    assert(sv[1] == 'o');
    assert(sv.at(1) == 'o');
    assert(sv.front() == 'n');
    assert(sv.back() == 'l');
#endif // __cpp_lib_concepts

    return true;
}

constexpr bool test_case_range_constructor() {
#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
    const array expectedData{'n', 'o', ' ', 'n', 'u', 'l', 'l'};
    // Also tests the corresponding deduction guide:
    same_as<string_view> auto sv = basic_string_view(expectedData);
    assert(sv.data() == expectedData.data());
    assert(sv.size() == 7);

    // Also tests some of the constraints:
    static_assert(is_constructible_v<string_view, vector<char>>);

    // P2499R0 string_view Range Constructor Should Be explicit
    static_assert(!is_convertible_v<vector<char>, string_view>);

    static_assert(!is_constructible_v<string_view, deque<char>>); // not contiguous
    static_assert(!is_convertible_v<deque<char>, string_view>);

    static_assert(!is_constructible_v<string_view, vector<unsigned char>>); // different elements
    static_assert(!is_convertible_v<vector<unsigned char>, string_view>);

    static_assert(!is_convertible_v<basic_string<char, constexpr_char_traits>, string_view>); // different traits
    static_assert(!is_convertible_v<basic_string_view<char, constexpr_char_traits>, string_view>);

    static_assert(!is_convertible_v<string_view, basic_string_view<char, constexpr_char_traits>>);
    static_assert(!is_convertible_v<basic_string_view<char, constexpr_char_traits>, string>);

    static_assert(is_constructible_v<string_view, basic_string<char, constexpr_char_traits>>);
    static_assert(is_constructible_v<basic_string_view<char, constexpr_char_traits>, string_view>);

    static_assert(is_constructible_v<string_view, basic_string_view<char, constexpr_char_traits>>);
    static_assert(is_constructible_v<basic_string_view<char, constexpr_char_traits>, string>);
#endif // _HAS_CXX23 && defined(__cpp_lib_concepts)

    return true;
}

template <class CharT, class Traits>
constexpr bool test_case_iterators() {
    using iterator = typename basic_string_view<CharT, Traits>::iterator;
    iterator itUninit{};
    iterator itCopy{itUninit};
    assert(itUninit == itCopy);

    basic_string_view<CharT, Traits> sv(TYPED_LITERAL(CharT, "hello world"));
    itUninit = sv.begin();
    itCopy   = itUninit;
    assert(itUninit == itCopy);
    assert(sv.end() - sv.begin() == 11);
    assert(sv.rend() - sv.rbegin() == 11);
    iterator testIterator = sv.begin() + 6;
    assert(*testIterator == static_cast<CharT>('w'));
    assert(*(testIterator.operator->()) == static_cast<CharT>('w'));
    ++testIterator;
    assert(*testIterator == static_cast<CharT>('o'));
    --testIterator;
    assert(*testIterator == static_cast<CharT>('w'));
    assert(*testIterator++ == static_cast<CharT>('w'));
    assert(*testIterator == static_cast<CharT>('o'));
    assert(*testIterator-- == static_cast<CharT>('o'));
    assert(*testIterator == static_cast<CharT>('w'));

    while (testIterator != sv.end()) {
        ++testIterator;
    }
    while (testIterator != sv.begin()) {
        --testIterator;
    }

    testIterator += 0;
    assert(*testIterator == static_cast<CharT>('h'));
    testIterator += 8;
    assert(*testIterator == static_cast<CharT>('r'));
    testIterator += -4;
    assert(*testIterator == static_cast<CharT>('o'));

    assert(*(testIterator + 6) == static_cast<CharT>('d'));
    assert(*(6 + testIterator) == static_cast<CharT>('d'));

    testIterator -= 1;
    assert(*testIterator == static_cast<CharT>('l'));

    assert(*(testIterator - 3) == static_cast<CharT>('h'));
    assert(testIterator[1] == static_cast<CharT>('o'));

    iterator eq    = testIterator;
    iterator after = testIterator + 2;

    assert(testIterator == testIterator);
    assert(testIterator == eq);
    assert(!(testIterator == after));
    assert(!(after == testIterator));

    assert(!(testIterator != testIterator));
    assert(!(testIterator != eq));
    assert(testIterator != after);
    assert(after != testIterator);

    assert(!(testIterator < testIterator));
    assert(!(testIterator < eq));
    assert(testIterator < after);
    assert(!(after < testIterator));

    assert(!(testIterator > testIterator));
    assert(!(testIterator > eq));
    assert(!(testIterator > after));
    assert(after > testIterator);

    assert(testIterator <= testIterator);
    assert(testIterator <= eq);
    assert(testIterator <= after);
    assert(!(after <= testIterator));

    assert(testIterator >= testIterator);
    assert(testIterator >= eq);
    assert(!(testIterator >= after));
    assert(after >= testIterator);

    using reverse_iterator = typename basic_string_view<CharT, Traits>::reverse_iterator;
    reverse_iterator reverseUninitIt{};
    reverse_iterator reverseUninitItCopy{reverseUninitIt};
    assert(reverseUninitIt == reverseUninitItCopy);
    assert(*sv.rbegin() == static_cast<CharT>('d'));
    assert(sv.rend()[-1] == static_cast<CharT>('h'));
    assert(sv.rend().base() == sv.begin());
    assert(sv.rbegin().base() == sv.end());
    reverse_iterator testRIterator = sv.rbegin() + 6;
    assert(*testRIterator == static_cast<CharT>('o'));
    testRIterator += 3;
    assert(*testRIterator == static_cast<CharT>('e'));
    testRIterator -= 9;
    assert(*testRIterator == static_cast<CharT>('d'));
    assert(*(testRIterator.operator->()) == static_cast<CharT>('d'));

    ++testRIterator;
    assert(*testRIterator == static_cast<CharT>('l'));
    assert(*testRIterator++ == static_cast<CharT>('l'));
    assert(*testRIterator == static_cast<CharT>('r'));
    --testRIterator;
    assert(*testRIterator == static_cast<CharT>('l'));
    assert(*testRIterator-- == static_cast<CharT>('l'));
    assert(*testRIterator == static_cast<CharT>('d'));

    reverse_iterator revEq    = testRIterator;
    reverse_iterator revAfter = testRIterator + 1;

    assert(testRIterator == testRIterator);
    assert(testRIterator == revEq);
    assert(!(testRIterator == revAfter));
    assert(!(revAfter == testRIterator));

    assert(!(testRIterator != testRIterator));
    assert(!(testRIterator != revEq));
    assert(testRIterator != revAfter);
    assert(revAfter != testRIterator);

    assert(!(testRIterator < testRIterator));
    assert(!(testRIterator < revEq));
    assert(testRIterator < revAfter);
    assert(!(revAfter < testRIterator));

    assert(!(testRIterator > testRIterator));
    assert(!(testRIterator > revEq));
    assert(!(testRIterator > revAfter));
    assert(revAfter > testRIterator);

    assert(testRIterator <= testRIterator);
    assert(testRIterator <= revEq);
    assert(testRIterator <= revAfter);
    assert(!(revAfter <= testRIterator));

    assert(testRIterator >= testRIterator);
    assert(testRIterator >= revEq);
    assert(!(testRIterator >= revAfter));
    assert(revAfter >= testRIterator);

    return true;
}

template <class CharT, class Traits>
constexpr bool test_case_prefix() {
    basic_string_view<CharT, Traits> sv(TYPED_LITERAL(CharT, "hello world"));
    sv.remove_prefix(6);
    assert((sv == basic_string_view<CharT, Traits>{TYPED_LITERAL(CharT, "world")}));

    return true;
}

template <class CharT, class Traits>
constexpr bool test_case_suffix() {
    basic_string_view<CharT, Traits> sv(TYPED_LITERAL(CharT, "hello world"));
    sv.remove_suffix(6);
    assert((sv == basic_string_view<CharT, Traits>{TYPED_LITERAL(CharT, "hello")}));

    return true;
}

template <class CharT, class Traits>
constexpr bool test_case_swap() {
    basic_string_view<CharT, Traits> a(TYPED_LITERAL(CharT, "a"));
    basic_string_view<CharT, Traits> b(TYPED_LITERAL(CharT, "b"));
    a.swap(b);
    assert(a == TYPED_LITERAL(CharT, "b"));
    assert(b == TYPED_LITERAL(CharT, "a"));

    return true;
}

void test_case_at_out_of_range() {
    string_view sv("text");
    assert_throws<out_of_range>([&] { return sv.at(4); });
}

using copy_buff_ty = char[5];
constexpr void fill_exclaim(copy_buff_ty& a) {
    for (size_t idx = 0; idx < 5; ++idx) {
        a[idx] = '!';
    }
}

constexpr bool copy_buff_eq(const copy_buff_ty& lhs, const copy_buff_ty& rhs) {
    for (size_t idx = 0; idx < 5; ++idx) {
        if (lhs[idx] != rhs[idx]) {
            return false;
        }
    }

    return true;
}

template <class Traits>
constexpr bool test_case_copy() {
    basic_string_view<char, Traits> sv("text");
    copy_buff_ty buffer{};
    fill_exclaim(buffer);
    sv.copy(buffer, 5);
    assert(copy_buff_eq(buffer, {'t', 'e', 'x', 't', '!'}));
    fill_exclaim(buffer);
    sv.copy(buffer, 5, 2);
    assert(copy_buff_eq(buffer, {'x', 't', '!', '!', '!'}));
    fill_exclaim(buffer);
    sv.copy(buffer, 2);
    assert(copy_buff_eq(buffer, {'t', 'e', '!', '!', '!'}));
    fill_exclaim(buffer);
    sv.copy(buffer, 1, 2);
    assert(copy_buff_eq(buffer, {'x', '!', '!', '!', '!'}));

    return true;
}

void test_case_copy_out_of_range() {
    string_view sv("text");
    copy_buff_ty buffer;
    assert_throws<out_of_range>([&] { sv.copy(buffer, 4, 10); });
}

template <class Traits>
constexpr bool test_case_Copy_s() {
    // nonstandard secure CRT function, _Copy_s
    // bounds check failure tested with the IDL checks
    basic_string_view<char, Traits> sv("text");
    copy_buff_ty buffer{};
    fill_exclaim(buffer);
    sv._Copy_s(buffer, 5, 5);
    assert(copy_buff_eq(buffer, {'t', 'e', 'x', 't', '!'}));
    fill_exclaim(buffer);
    sv._Copy_s(buffer, 5, 5, 2);
    assert(copy_buff_eq(buffer, {'x', 't', '!', '!', '!'}));
    fill_exclaim(buffer);
    sv._Copy_s(buffer, 5, 2);
    assert(copy_buff_eq(buffer, {'t', 'e', '!', '!', '!'}));
    fill_exclaim(buffer);
    sv._Copy_s(buffer, 5, 1, 2);
    assert(copy_buff_eq(buffer, {'x', '!', '!', '!', '!'}));

    return true;
}

void test_case_Copy_s_out_of_range() {
    string_view sv("text");
    copy_buff_ty buffer;
    assert_throws<out_of_range>([&] { sv._Copy_s(buffer, 5, 4, 10); });
}

template <class Traits>
constexpr bool test_case_substr() {
    basic_string_view<char, Traits> sv("text");

    assert(sv.substr() == sv);
    assert((sv.substr(2) == basic_string_view<char, Traits>{"xt"}));
    assert((sv.substr(0, 2) == basic_string_view<char, Traits>{"te"}));
    assert((sv.substr(1, 2) == basic_string_view<char, Traits>{"ex"}));

    return true;
}

void test_case_substr_out_of_range() {
    assert_throws<out_of_range>([] { return string_view("text").substr(5); });
}

template <class CharT, class Traits>
constexpr bool test_case_compare() {
    const auto npos = basic_string_view<CharT, Traits>::npos;
    basic_string_view<CharT, Traits> rr(TYPED_LITERAL(CharT, "rr"));
    basic_string_view<CharT, Traits> s4(TYPED_LITERAL(CharT, "s4"));
    basic_string_view<CharT, Traits> s4s4(TYPED_LITERAL(CharT, "s4s4"));

    assert(rr.compare(rr) == 0); // equal

    assert(rr.compare(s4) < 0); // differ by contents only
    assert(s4.compare(rr) > 0);

    assert(s4.compare(s4s4) < 0); // differ by length only
    assert(s4s4.compare(s4) > 0);

    assert(rr.compare(1, 0, basic_string_view<CharT, Traits>{}) == 0);
    assert(rr.compare(0, 1000, rr) == 0);
    assert(s4s4.compare(2, 2, s4) == 0);
    assert(s4s4.compare(2, 200, s4) == 0);

    assert(rr.compare(0, npos, s4s4, 0, 2) < 0);
    assert(s4.compare(0, npos, s4s4, 2, 2) == 0);
    assert(s4.compare(0, npos, s4s4, 0, 2) == 0);
    assert(s4.compare(0, npos, s4s4, 2, 1000) == 0);

    assert(rr.compare(TYPED_LITERAL(CharT, "rr")) == 0);
    assert(rr.compare(TYPED_LITERAL(CharT, "s4")) < 0);
    assert(rr.compare(TYPED_LITERAL(CharT, "s4s4")) < 0);
    assert(s4.compare(TYPED_LITERAL(CharT, "s4")) == 0);
    assert(s4.compare(TYPED_LITERAL(CharT, "rr")) > 0);

    assert(rr.compare(1, 0, TYPED_LITERAL(CharT, "")) == 0);
    assert(rr.compare(1, 0, TYPED_LITERAL(CharT, "rr")) < 0);
    assert(rr.compare(0, 1000, TYPED_LITERAL(CharT, "rr")) == 0);
    assert(s4.compare(0, 2, TYPED_LITERAL(CharT, "s4s4")) < 0);
    assert(s4s4.compare(2, 2, TYPED_LITERAL(CharT, "s4")) == 0);
    assert(s4s4.compare(2, 200, TYPED_LITERAL(CharT, "s4")) == 0);
    assert(s4s4.compare(1, 2, TYPED_LITERAL(CharT, "s4")) < 0);

    return true;
}

void test_case_compare_out_of_range() {
    assert_throws<out_of_range>([] { return string_view{"hi"}.compare(10, 2, string_view{}); });
    assert_throws<out_of_range>([] { return string_view{"hi"}.compare(10, 2, string_view{"other"}, 4, 2); });
    assert_throws<out_of_range>([] { return string_view{"hi"}.compare(0, 2, string_view{"other"}, 6, 2); });
    assert_throws<out_of_range>([] { return string_view{"hi"}.compare(3, 0, "hello"); });
    assert_throws<out_of_range>([] { return string_view{"hi"}.compare(3, 0, "hello", 5); });
}

// P0457R2 starts_with()/ends_with() For basic_string/basic_string_view
#if _HAS_CXX20
template <class CharT, class Traits, bool TestBasicString>
constexpr bool test_case_starts_with_ends_with() {
    const basic_string_view<CharT, Traits> empty_sv(TYPED_LITERAL(CharT, ""));
    const CharT null_c = '\0';
    const CharT* const empty_cp(TYPED_LITERAL(CharT, ""));

    const basic_string_view<CharT, Traits> a(TYPED_LITERAL(CharT, "a"));
    const CharT b = 'b';
    const CharT* const c(TYPED_LITERAL(CharT, "c"));

    const basic_string_view<CharT, Traits> moving_sv(TYPED_LITERAL(CharT, "moving"));
    const CharT* const moving_cp(TYPED_LITERAL(CharT, "moving"));

    if constexpr (TestBasicString) {
        basic_string<CharT, Traits> text(TYPED_LITERAL(CharT, "text"));
        basic_string<CharT, Traits> empty_text(TYPED_LITERAL(CharT, ""));

        const basic_string_view<CharT, Traits> te(TYPED_LITERAL(CharT, "te"));
        const CharT t = 't';
        const CharT* const tex(TYPED_LITERAL(CharT, "tex"));
        const basic_string_view<CharT, Traits> text_sv(TYPED_LITERAL(CharT, "text"));
        const CharT* const text_cp(TYPED_LITERAL(CharT, "text"));
        const basic_string_view<CharT, Traits> next_sv(TYPED_LITERAL(CharT, "next"));
        const CharT* const next_cp(TYPED_LITERAL(CharT, "next"));
        const basic_string_view<CharT, Traits> texture_sv(TYPED_LITERAL(CharT, "texture"));
        const CharT* const texture_cp(TYPED_LITERAL(CharT, "texture"));

        assert(text.starts_with(te));
        assert(text.starts_with(t));
        assert(text.starts_with(tex));
        assert(text.starts_with(text));
        assert(text.starts_with(text_sv));
        assert(text.starts_with(text_cp));
        assert(!text.starts_with(next_sv));
        assert(!text.starts_with(next_cp));
        assert(!text.starts_with(texture_sv));
        assert(!text.starts_with(texture_cp));
        assert(text.starts_with(empty_sv));
        assert(!text.starts_with(null_c));
        assert(text.starts_with(empty_cp));
        assert(!text.starts_with(a));
        assert(!text.starts_with(b));
        assert(!text.starts_with(c));

        assert(!empty_text.starts_with(te));
        assert(!empty_text.starts_with(t));
        assert(!empty_text.starts_with(tex));
        assert(empty_text.starts_with(empty_text));
        assert(empty_text.starts_with(empty_sv));
        assert(!empty_text.starts_with(null_c));
        assert(empty_text.starts_with(empty_cp));

        const basic_string_view<CharT, Traits> xt(TYPED_LITERAL(CharT, "xt"));
        const CharT* const ext(TYPED_LITERAL(CharT, "ext"));
        const basic_string_view<CharT, Traits> context_sv(TYPED_LITERAL(CharT, "context"));
        const CharT* const context_cp(TYPED_LITERAL(CharT, "context"));

        assert(text.ends_with(xt));
        assert(text.ends_with(t));
        assert(text.ends_with(ext));
        assert(text.ends_with(text));
        assert(text.ends_with(text_sv));
        assert(text.ends_with(text_cp));
        assert(!text.ends_with(next_sv));
        assert(!text.ends_with(next_cp));
        assert(!text.ends_with(context_sv));
        assert(!text.ends_with(context_cp));
        assert(text.ends_with(empty_sv));
        assert(!text.ends_with(null_c));
        assert(text.ends_with(empty_cp));
        assert(!text.ends_with(a));
        assert(!text.ends_with(b));
        assert(!text.ends_with(c));

        assert(!empty_text.ends_with(xt));
        assert(!empty_text.ends_with(t));
        assert(!empty_text.ends_with(ext));
        assert(empty_text.starts_with(empty_text));
        assert(empty_text.ends_with(empty_sv));
        assert(!empty_text.ends_with(null_c));
        assert(empty_text.ends_with(empty_cp));

        basic_string<CharT, Traits> rocking(TYPED_LITERAL(CharT, "rocking"));

        const basic_string_view<CharT, Traits> rocket_sv(TYPED_LITERAL(CharT, "rocket"));
        const CharT* const rocket_cp(TYPED_LITERAL(CharT, "rocket"));

        assert(!rocking.starts_with(rocket_sv));
        assert(!rocking.starts_with(rocket_cp));

        assert(!rocking.ends_with(moving_sv));
        assert(!rocking.ends_with(moving_cp));
    }

    const basic_string_view<CharT, Traits> hello(TYPED_LITERAL(CharT, "hello"));

    const basic_string_view<CharT, Traits> he(TYPED_LITERAL(CharT, "he"));
    const CharT h = 'h';
    const CharT* const hel(TYPED_LITERAL(CharT, "hel"));
    const basic_string_view<CharT, Traits> hello_sv(TYPED_LITERAL(CharT, "hello"));
    const CharT* const hello_cp(TYPED_LITERAL(CharT, "hello"));
    const basic_string_view<CharT, Traits> cello_sv(TYPED_LITERAL(CharT, "cello"));
    const CharT* const cello_cp(TYPED_LITERAL(CharT, "cello"));
    const basic_string_view<CharT, Traits> helloworld_sv(TYPED_LITERAL(CharT, "helloworld"));
    const CharT* const helloworld_cp(TYPED_LITERAL(CharT, "helloworld"));

    assert(hello.starts_with(he));
    assert(hello.starts_with(h));
    assert(hello.starts_with(hel));
    assert(hello.starts_with(hello));
    assert(hello.starts_with(hello_sv));
    assert(hello.starts_with(hello_cp));
    assert(!hello.starts_with(cello_sv));
    assert(!hello.starts_with(cello_cp));
    assert(!hello.starts_with(helloworld_sv));
    assert(!hello.starts_with(helloworld_cp));
    assert(hello.starts_with(empty_sv));
    assert(!hello.starts_with(null_c));
    assert(hello.starts_with(empty_cp));
    assert(!hello.starts_with(a));
    assert(!hello.starts_with(b));
    assert(!hello.starts_with(c));

    assert(!empty_sv.starts_with(he));
    assert(!empty_sv.starts_with(h));
    assert(!empty_sv.starts_with(hel));
    assert(empty_sv.starts_with(empty_sv));
    assert(!empty_sv.starts_with(null_c));
    assert(empty_sv.starts_with(empty_cp));

    const basic_string_view<CharT, Traits> lo(TYPED_LITERAL(CharT, "lo"));
    const CharT o = 'o';
    const CharT* const llo(TYPED_LITERAL(CharT, "llo"));
    const basic_string_view<CharT, Traits> worldhello_sv(TYPED_LITERAL(CharT, "worldhello"));
    const CharT* const worldhello_cp(TYPED_LITERAL(CharT, "worldhello"));

    assert(hello.ends_with(lo));
    assert(hello.ends_with(o));
    assert(hello.ends_with(llo));
    assert(hello.ends_with(hello));
    assert(hello.ends_with(hello_sv));
    assert(hello.ends_with(hello_cp));
    assert(!hello.ends_with(cello_sv));
    assert(!hello.ends_with(cello_cp));
    assert(!hello.ends_with(worldhello_sv));
    assert(!hello.ends_with(worldhello_cp));
    assert(hello.ends_with(empty_sv));
    assert(!hello.ends_with(null_c));
    assert(hello.ends_with(empty_cp));
    assert(!hello.ends_with(a));
    assert(!hello.ends_with(b));
    assert(!hello.ends_with(c));

    assert(!empty_sv.ends_with(lo));
    assert(!empty_sv.ends_with(o));
    assert(!empty_sv.ends_with(llo));
    assert(empty_sv.ends_with(empty_sv));
    assert(!empty_sv.ends_with(null_c));
    assert(empty_sv.ends_with(empty_cp));

    const basic_string_view<CharT, Traits> playing(TYPED_LITERAL(CharT, "playing"));

    const basic_string_view<CharT, Traits> player_sv(TYPED_LITERAL(CharT, "player"));
    const CharT* const player_cp(TYPED_LITERAL(CharT, "player"));

    assert(!playing.starts_with(player_sv));
    assert(!playing.starts_with(player_cp));

    assert(!playing.ends_with(moving_sv));
    assert(!playing.ends_with(moving_cp));

    return true;
}
#endif // _HAS_CXX20

// P1679R3 contains() For basic_string/basic_string_view
#if _HAS_CXX23
template <class CharT, class Traits, bool TestBasicString>
constexpr bool test_case_contains() {
    const basic_string_view<CharT, Traits> empty_sv(TYPED_LITERAL(CharT, ""));
    const CharT null_c = '\0';
    const CharT* const empty_cp(TYPED_LITERAL(CharT, ""));

    const basic_string_view<CharT, Traits> a(TYPED_LITERAL(CharT, "a"));
    const CharT b = 'b';
    const CharT* const c(TYPED_LITERAL(CharT, "c"));

    if constexpr (TestBasicString) {
        basic_string<CharT, Traits> text(TYPED_LITERAL(CharT, "text"));
        basic_string<CharT, Traits> empty_text(TYPED_LITERAL(CharT, ""));

        const basic_string_view<CharT, Traits> te(TYPED_LITERAL(CharT, "te"));
        const CharT x = 'x';
        const CharT* const ext(TYPED_LITERAL(CharT, "ext"));
        const basic_string_view<CharT, Traits> text_sv(TYPED_LITERAL(CharT, "text"));
        const CharT* const text_cp(TYPED_LITERAL(CharT, "text"));
        const basic_string_view<CharT, Traits> next_sv(TYPED_LITERAL(CharT, "next"));
        const CharT* const next_cp(TYPED_LITERAL(CharT, "next"));
        const basic_string_view<CharT, Traits> texture_sv(TYPED_LITERAL(CharT, "texture"));
        const CharT* const texture_cp(TYPED_LITERAL(CharT, "texture"));

        assert(text.contains(te));
        assert(text.contains(x));
        assert(text.contains(ext));
        assert(text.contains(text));
        assert(text.contains(text_sv));
        assert(text.contains(text_cp));
        assert(!text.contains(next_sv));
        assert(!text.contains(next_cp));
        assert(!text.contains(texture_sv));
        assert(!text.contains(texture_cp));
        assert(text.contains(empty_sv));
        assert(!text.contains(null_c));
        assert(text.contains(empty_cp));
        assert(!text.contains(a));
        assert(!text.contains(b));
        assert(!text.contains(c));

        assert(!empty_text.contains(te));
        assert(!empty_text.contains(x));
        assert(!empty_text.contains(ext));
        assert(empty_text.contains(empty_text));
        assert(empty_text.contains(empty_sv));
        assert(!empty_text.contains(null_c));
        assert(empty_text.contains(empty_cp));

        basic_string<CharT, Traits> rocking(TYPED_LITERAL(CharT, "rocking"));

        const basic_string_view<CharT, Traits> rocket_sv(TYPED_LITERAL(CharT, "rocket"));
        const CharT* const rocket_cp(TYPED_LITERAL(CharT, "rocket"));

        assert(!rocking.contains(rocket_sv));
        assert(!rocking.contains(rocket_cp));
    }

    const basic_string_view<CharT, Traits> hello(TYPED_LITERAL(CharT, "hello"));

    const basic_string_view<CharT, Traits> he(TYPED_LITERAL(CharT, "he"));
    const CharT e = 'e';
    const CharT* const llo(TYPED_LITERAL(CharT, "llo"));
    const basic_string_view<CharT, Traits> hello_sv(TYPED_LITERAL(CharT, "hello"));
    const CharT* const hello_cp(TYPED_LITERAL(CharT, "hello"));
    const basic_string_view<CharT, Traits> cello_sv(TYPED_LITERAL(CharT, "cello"));
    const CharT* const cello_cp(TYPED_LITERAL(CharT, "cello"));
    const basic_string_view<CharT, Traits> helloworld_sv(TYPED_LITERAL(CharT, "helloworld"));
    const CharT* const helloworld_cp(TYPED_LITERAL(CharT, "helloworld"));

    assert(hello.contains(he));
    assert(hello.contains(e));
    assert(hello.contains(llo));
    assert(hello.contains(hello));
    assert(hello.contains(hello_sv));
    assert(hello.contains(hello_cp));
    assert(!hello.contains(cello_sv));
    assert(!hello.contains(cello_cp));
    assert(!hello.contains(helloworld_sv));
    assert(!hello.contains(helloworld_cp));
    assert(hello.contains(empty_sv));
    assert(!hello.contains(null_c));
    assert(hello.contains(empty_cp));
    assert(!hello.contains(a));
    assert(!hello.contains(b));
    assert(!hello.contains(c));

    assert(!empty_sv.contains(he));
    assert(!empty_sv.contains(e));
    assert(!empty_sv.contains(llo));
    assert(empty_sv.contains(empty_sv));
    assert(!empty_sv.contains(null_c));
    assert(empty_sv.contains(empty_cp));

    const basic_string_view<CharT, Traits> playing(TYPED_LITERAL(CharT, "playing"));

    const basic_string_view<CharT, Traits> player_sv(TYPED_LITERAL(CharT, "player"));
    const CharT* const player_cp(TYPED_LITERAL(CharT, "player"));

    assert(!playing.contains(player_sv));
    assert(!playing.contains(player_cp));

    return true;
}
#endif // _HAS_CXX23

template <typename CharT, typename Traits>
constexpr bool test_case_find() {
    basic_string_view<CharT, Traits> aaa(TYPED_LITERAL(CharT, "aaa"));
    basic_string_view<CharT, Traits> s4(TYPED_LITERAL(CharT, "s4"));
    basic_string_view<CharT, Traits> s4s4(TYPED_LITERAL(CharT, "s4s4"));
    basic_string_view<CharT, Traits> sssss4(TYPED_LITERAL(CharT, "sssss4"));
    basic_string_view<CharT, Traits> s5(TYPED_LITERAL(CharT, "s5"));

    basic_string_view<CharT, Traits> embeddedNullsHaystack(TYPED_LITERAL(CharT, "za\0bz"), 5);
    basic_string_view<CharT, Traits> embeddedNullsNeedle(TYPED_LITERAL(CharT, "a\0b"), 3);

    const auto npos = basic_string_view<CharT, Traits>::npos;


    assert(s4s4.find(s4) == 0);
    assert(s4s4.find(s4, 1) == 2);
    assert(s4s4.find(s4, 3) == npos);
    assert(s4s4.find(TYPED_LITERAL(CharT, "s4")) == 0);
    assert(s4s4.find(TYPED_LITERAL(CharT, "s4"), 3) == npos); // _Start_at > _Size - _Count
    assert(s4s4.find(TYPED_LITERAL(CharT, "s4XX"), 1, 2) == 2);
    assert(s4s4.find(TYPED_LITERAL(CharT, ""), 2) == 2); // empty string always matches
    assert(s4s4.find(TYPED_LITERAL(CharT, ""), 4) == 4); // even at the end
    assert(s4s4.find(TYPED_LITERAL(CharT, ""), 5) == npos); // but not off the end
    assert(s4.find(TYPED_LITERAL(CharT, "s4XX")) == npos); // _Count > _Size
    assert(s4s4.find(static_cast<CharT>('s')) == 0);
    assert(s4s4.find(static_cast<CharT>('s'), 1) == 2);
    assert(s4s4.find(static_cast<CharT>('x')) == npos);
    assert(sssss4.find(s4) == 4);
    assert(embeddedNullsHaystack.find(embeddedNullsNeedle) == 1);

    assert(s4s4.rfind(s4) == 2);
    assert(s4s4.rfind(s4, 1) == 0);
    assert(s4s4.rfind(s5, 3) == npos);
    assert(s4s4.rfind(TYPED_LITERAL(CharT, "s4")) == 2);
    assert(s4s4.rfind(TYPED_LITERAL(CharT, "s4"), 3) == 2);
    assert(s4s4.rfind(TYPED_LITERAL(CharT, "s4XX"), 1, 3) == npos);
    assert(s4s4.rfind(static_cast<CharT>('s')) == 2);
    assert(s4s4.rfind(static_cast<CharT>('s'), 2) == 2);
    assert(s4s4.rfind(static_cast<CharT>('x')) == npos);

    assert(s4s4.find_first_of(s4) == 0);
    assert(s4s4.find_first_of(s4, 1) == 1);
    assert(s4s4.find_first_of(s4, 4) == npos);
    assert(s4s4.find_first_of(TYPED_LITERAL(CharT, "s4")) == 0);
    assert(s4s4.find_first_of(TYPED_LITERAL(CharT, "s4"), 3) == 3);
    assert(s4s4.find_first_of(TYPED_LITERAL(CharT, "abs"), 1, 2) == npos);
    assert(s4s4.find_first_of(static_cast<CharT>('s')) == 0);
    assert(s4s4.find_first_of(static_cast<CharT>('s'), 1) == 2);
    assert(s4s4.find_first_of(static_cast<CharT>('x')) == npos);

    assert(s4s4.find_last_of(s4) == 3);
    assert(s4s4.find_last_of(s4, 1) == 1);
    assert(s4s4.find_last_of(aaa) == npos);
    assert(s4s4.find_last_of(TYPED_LITERAL(CharT, "s4")) == 3);
    assert(s4s4.find_last_of(TYPED_LITERAL(CharT, "s4"), 2) == 2);
    assert(s4s4.find_last_of(TYPED_LITERAL(CharT, "abs"), 1, 2) == npos);
    assert(s4s4.find_last_of(static_cast<CharT>('s')) == 2);
    assert(s4s4.find_last_of(static_cast<CharT>('s'), 1) == 0);
    assert(s4s4.find_last_of(static_cast<CharT>('x')) == npos);

    assert(s4s4.find_first_not_of(s5) == 1);
    assert(s4s4.find_first_not_of(s5, 2) == 3);
    assert(s4s4.find_first_not_of(s4) == npos);
    assert(s4s4.find_first_not_of(TYPED_LITERAL(CharT, "s5")) == 1);
    assert(s4s4.find_first_not_of(TYPED_LITERAL(CharT, "s5"), 2) == 3);
    assert(s4s4.find_first_not_of(TYPED_LITERAL(CharT, "s4a"), 1, 2) == npos);
    assert(s4s4.find_first_not_of(static_cast<CharT>('s')) == 1);
    assert(s4s4.find_first_not_of(static_cast<CharT>('s'), 2) == 3);
    assert(s4s4.find_first_not_of(static_cast<CharT>('s'), 4) == npos);

    assert(s4s4.find_last_not_of(s5) == 3);
    assert(s4s4.find_last_not_of(s5, 2) == 1);
    assert(s4s4.find_last_not_of(s4) == npos);
    assert(s4s4.find_last_not_of(TYPED_LITERAL(CharT, "s5")) == 3);
    assert(s4s4.find_last_not_of(TYPED_LITERAL(CharT, "s5"), 2) == 1);
    assert(s4s4.find_last_not_of(TYPED_LITERAL(CharT, "s4a"), 1, 2) == npos);
    assert(s4s4.find_last_not_of(static_cast<CharT>('s')) == 3);
    assert(s4s4.find_last_not_of(static_cast<CharT>('s'), 2) == 1);
    assert(s4s4.find_last_not_of(static_cast<CharT>('s'), 0) == npos);

    return true;
}

template <class CharT, class Traits>
constexpr bool test_case_operators() {
    basic_string_view<CharT, Traits> rr(TYPED_LITERAL(CharT, "rr"));
    basic_string_view<CharT, Traits> s4(TYPED_LITERAL(CharT, "s4"));
    basic_string_view<CharT, Traits> cpy(s4);

    assert(s4 == cpy);
    assert(cpy == s4);
    assert(!(s4 == rr));
    assert(!(rr == s4));
    assert(TYPED_LITERAL(CharT, "s4") == s4); // converting operator left
    assert(!(TYPED_LITERAL(CharT, "rr") == s4));
    assert(s4 == TYPED_LITERAL(CharT, "s4")); // converting operator right
    assert(!(s4 == TYPED_LITERAL(CharT, "s4s4")));

    assert(!(s4 != cpy));
    assert(!(cpy != s4));
    assert(s4 != rr);
    assert(rr != s4);
    assert(!(TYPED_LITERAL(CharT, "s4") != s4));
    assert(TYPED_LITERAL(CharT, "rr") != s4);
    assert(!(s4 != TYPED_LITERAL(CharT, "s4")));
    assert(s4 != TYPED_LITERAL(CharT, "s4s4"));

    assert(!(s4 < cpy));
    assert(!(cpy < s4));
    assert(!(s4 < rr));
    assert(rr < s4);
    assert(!(TYPED_LITERAL(CharT, "s4") < s4));
    assert(!(s4 < TYPED_LITERAL(CharT, "s4")));
    assert(s4 < TYPED_LITERAL(CharT, "s4s4"));

    assert(!(s4 > cpy));
    assert(!(cpy > s4));
    assert(s4 > rr);
    assert(!(rr > s4));
    assert(!(TYPED_LITERAL(CharT, "s4") > s4));
    assert(!(s4 > TYPED_LITERAL(CharT, "s4")));
    assert(!(s4 > TYPED_LITERAL(CharT, "s4s4")));

    assert(s4 <= cpy);
    assert(cpy <= s4);
    assert(!(s4 <= rr));
    assert(rr <= s4);
    assert(TYPED_LITERAL(CharT, "s4") <= s4);
    assert(s4 <= TYPED_LITERAL(CharT, "s4"));
    assert(s4 <= TYPED_LITERAL(CharT, "s4s4"));

    assert(s4 >= cpy);
    assert(cpy >= s4);
    assert(s4 >= rr);
    assert(!(rr >= s4));
    assert(TYPED_LITERAL(CharT, "s4") >= s4);
    assert(s4 >= TYPED_LITERAL(CharT, "s4"));
    assert(!(s4 >= TYPED_LITERAL(CharT, "s4s4")));

    return true;
}

void test_case_inserter() {
    ostringstream oss;
    string_view sv("text");
    oss << sv;
    assert(oss.str() == "text");
}

void test_case_hashing() {
    string str("hungry EVIL zombies");
    string_view strView(str);
    assert(hash<string>{}(str) == hash<string_view>{}(strView));
}

void test_case_string_integration() {
    string_view svText("text");

    string testStr(svText);
    assert(testStr == "text");

    string_view hello("hello");
    testStr = hello;
    assert(testStr == "hello");

    testStr += svText;
    assert(testStr == "hellotext");

    testStr.append(svText);
    assert(testStr == "hellotexttext");

    assert_throws<out_of_range>([&] { testStr.append(svText, 5, 0); });
    testStr.append(svText, 2, 10);
    assert(testStr == "hellotexttextxt");

    testStr.assign(hello);
    assert(testStr == "hello");

    assert_throws<out_of_range>([&] { testStr.assign(hello, 6, 0); });
    assert(testStr == "hello");
    testStr.assign(hello, 1, 2);
    assert(testStr == "el");

    assert_throws<out_of_range>([&] { testStr.insert(3, svText); });
    assert(testStr == "el");
    testStr.insert(1, hello);
    assert(testStr == "ehellol");
    assert_throws<out_of_range>([&] { testStr.insert(8, svText, 0, 0); });
    assert_throws<out_of_range>([&] { testStr.insert(0, svText, 5, 0); });
    assert(testStr == "ehellol");
    testStr.insert(4, svText, 1, 2);
    assert(testStr == "ehelexlol");

    assert_throws<out_of_range>([&] { testStr.replace(10, 0, svText); });
    assert(testStr == "ehelexlol");
    testStr.replace(1, 7, svText);
    assert(testStr == "etextl");
    assert_throws<out_of_range>([&] { testStr.replace(7, 0, svText, 0, 0); });
    assert(testStr == "etextl");
    assert_throws<out_of_range>([&] { testStr.replace(6, 0, svText, 5, 0); });
    assert(testStr == "etextl");
    testStr.replace(6, 0, svText, 1, 3);
    assert(testStr == "etextlext");

    testStr.replace(testStr.begin() + 1, testStr.end() - 1, svText);
    assert(testStr == "etextt");

    auto implicit_conversion = [](string_view converted) { assert(converted == "etextt"); };
    implicit_conversion(testStr);

    assert(testStr.find(string_view{"xt"}) == 3);
    assert(testStr.rfind(string_view{"e"}) == 2);
    assert(testStr.find_first_of(string_view{"xt"}) == 1);
    assert(testStr.find_last_of(string_view{"te"}) == 5);
    assert(testStr.find_first_not_of(string_view{"te"}) == 3);
    assert(testStr.find_last_not_of(string_view{"tx"}) == 2);

    assert(testStr.compare(string_view{"etextt"}) == 0);
    assert(testStr.compare(string_view{"etext"}) > 0);
    assert(testStr.compare(string_view{"etextta"}) < 0);
    assert(testStr.compare(string_view{"dtextt"}) > 0);

    assert(testStr.compare(2, 3, string_view{"ext"}) == 0);
    assert(testStr.compare(2, 3, string_view{"text"}) < 0);

    assert(testStr.compare(1, 3, string_view{"XXtex"}, 2) == 0);
    assert(testStr.compare(1, 2, string_view{"XXtex"}, 2, 2) == 0);
}

static_assert(c_string_view{"abcd"} == c_string_view{"abcd"});
static_assert(c_string_view{"abcd"} == "abcd");

static_assert(test_case_default_constructor());
static_assert(test_case_ntcts_constructor<constexpr_char_traits>());
static_assert(test_case_buffer_constructor());
static_assert(test_case_contiguous_constructor());
static_assert(test_case_range_constructor());
static_assert(test_case_iterators<char, constexpr_char_traits>());
static_assert(test_case_prefix<char, constexpr_char_traits>());
static_assert(test_case_suffix<char, constexpr_char_traits>());
static_assert(test_case_swap<char, constexpr_char_traits>());
static_assert(test_case_substr<constexpr_char_traits>());
static_assert(test_case_compare<char, constexpr_char_traits>());
#if _HAS_CXX20
static_assert(test_case_copy<constexpr_char_traits>());
static_assert(test_case_Copy_s<constexpr_char_traits>());
static_assert(test_case_starts_with_ends_with<char, constexpr_char_traits, false>());
#endif // _HAS_CXX20
#if _HAS_CXX23
static_assert(test_case_contains<char, constexpr_char_traits, false>());
#endif // _HAS_CXX23
static_assert(test_case_operators<char, constexpr_char_traits>());
static_assert(test_case_find<char, constexpr_char_traits>());

static_assert(string_view{}.max_size() == ptrdiff_max);
#ifdef __cpp_lib_char8_t
static_assert(u8string_view{}.max_size() == ptrdiff_max);
#endif // __cpp_lib_char8_t
static_assert(u16string_view{}.max_size() == ptrdiff_max);
static_assert(u32string_view{}.max_size() == static_cast<size_t>(-1) / sizeof(char32_t));
static_assert(wstring_view{}.max_size() == ptrdiff_max);

// P0403R1 UDLs For <string_view> ("meow"sv, etc.)
static_assert("abc"sv[1] == 'b');
static_assert(noexcept("abc"sv));
static_assert(L"abc"sv[1] == L'b');
static_assert(noexcept(L"abc"sv));
static_assert(u"abc"sv[1] == u'b');
static_assert(noexcept(u"abc"sv));
static_assert(U"abc"sv[1] == U'b');
static_assert(noexcept(U"abc"sv));
#ifdef __cpp_char8_t
static_assert(u8"abc"sv[1] == u8'b');
static_assert(noexcept(u8"abc"sv));
#endif // __cpp_char8_t

// P2166R1 Prohibiting basic_string And basic_string_view Construction From nullptr
#if _HAS_CXX23
static_assert(!is_constructible_v<string_view, nullptr_t>);
static_assert(!is_constructible_v<string, nullptr_t>);
static_assert(!is_assignable_v<string&, nullptr_t>);
#endif // _HAS_CXX23

// Also test that no C6510 warning
struct char_wrapper {
    char c;
};

template <>
struct std::char_traits<char_wrapper> {
    using char_type = char_wrapper;

    static bool eq(char_wrapper lhs, char_wrapper rhs) {
        return lhs.c == rhs.c;
    }

    static size_t length(const char_wrapper* a) {
        static_assert(sizeof(char_wrapper) == 1);
        return strlen(reinterpret_cast<const char*>(a));
    }

    static int compare(const char_wrapper* lhs, const char_wrapper* rhs, size_t count) {
        return char_traits<char>::compare(
            reinterpret_cast<const char*>(lhs), reinterpret_cast<const char*>(rhs), count);
    }
};

using WrappedSV = basic_string_view<char_wrapper, char_traits<char_wrapper>>;

void test_C6510_warning() { // compile-only
    char_wrapper a[] = {{'a'}, {'b'}, {'c'}, {'\0'}};
    WrappedSV sv(a);
    (void) sv;
}

int main() {
    test_case_default_constructor();
    test_case_ntcts_constructor();
    test_case_buffer_constructor();
    test_case_contiguous_constructor();
    test_case_range_constructor();
    test_case_iterators<char, char_traits<char>>();
    test_case_iterators<wchar_t, char_traits<wchar_t>>();
    test_case_prefix<char, char_traits<char>>();
    test_case_prefix<wchar_t, char_traits<wchar_t>>();
    test_case_suffix<char, char_traits<char>>();
    test_case_suffix<wchar_t, char_traits<wchar_t>>();
    test_case_swap<char, char_traits<char>>();
    test_case_swap<wchar_t, char_traits<wchar_t>>();
    test_case_at_out_of_range();
    test_case_copy<char_traits<char>>();
    test_case_copy_out_of_range();
    test_case_Copy_s<char_traits<char>>();
    test_case_Copy_s_out_of_range();
    test_case_substr<char_traits<char>>();
    test_case_substr_out_of_range();
    test_case_compare<char, char_traits<char>>();
    test_case_compare<wchar_t, char_traits<wchar_t>>();
    test_case_compare_out_of_range();
#if _HAS_CXX20
    test_case_starts_with_ends_with<char, char_traits<char>, true>();
    test_case_starts_with_ends_with<wchar_t, char_traits<wchar_t>, true>();
#endif // _HAS_CXX20
#if _HAS_CXX23
    test_case_contains<char, char_traits<char>, true>();
    test_case_contains<wchar_t, char_traits<wchar_t>, true>();
#endif // _HAS_CXX23
    test_case_find<char, char_traits<char>>();
    test_case_find<wchar_t, char_traits<wchar_t>>();
    test_case_operators<char, char_traits<char>>();
    test_case_operators<wchar_t, char_traits<wchar_t>>();
    test_case_inserter();
    test_case_hashing();
    test_case_string_integration();
}
