// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma warning(disable : 4365) // conversion from 'X' to 'Y', signed/unsigned mismatch

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <forward_list>
#include <ranges>
#include <span>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>

using namespace std;

template <class T>
struct c_string {
    const T* ptr_ = nullptr;

    struct sentinel {
        constexpr bool operator==(const T* p) const {
            return *p == 0;
        }
    };

    constexpr const T* begin() const {
        return ptr_;
    }
    constexpr sentinel end() const {
        return {};
    }
};

template <ranges::input_range Rng, class T>
constexpr bool test_string(Rng&& rng, const T* expected) {
    basic_string<T> str(from_range, forward<Rng>(rng));
    assert(ranges::equal(str, c_string<T>{expected}));

    return true;
}

static constexpr char hw[]    = "Hello, world!";
static constexpr auto span_hw = span{hw}.first<span{hw}.size() - 1>();

static constexpr signed char hw_s[] = "Hello, world!";
static constexpr auto span_hw_s     = span{hw_s}.first<span{hw_s}.size() - 1>();

static constexpr unsigned char hw_u[] = "Hello, world!";
static constexpr auto span_hw_u       = span{hw_u}.first<span{hw_u}.size() - 1>();

struct string_instantiator {
    template <ranges::input_range R>
    static void call() {
        test_string(R{span_hw}, hw);
        static_assert(test_string(R{span_hw}, hw));
    }
};

#ifdef __cpp_char8_t
static constexpr char8_t hw_u8[] = u8"Hello, world!";
static constexpr auto span_hw_u8 = span{hw_u8}.first<span{hw_u8}.size() - 1>();

struct u8string_instantiator {
    template <ranges::input_range R>
    static void call() {
        test_string(R{span_hw_u8}, hw_u8);
        static_assert(test_string(R{span_hw_u8}, hw_u8));
    }
};
#endif // defined(__cpp_char8_t)

static constexpr char16_t hw_u16[] = u"Hello, world!";
static constexpr auto span_hw_u16  = span{hw_u16}.first<span{hw_u16}.size() - 1>();

static constexpr int_least16_t hw_u16s[]{
    u'H', u'e', u'l', u'l', u'o', u',', u' ', u'w', u'o', u'r', u'l', u'd', u'!', u'\0'};
static constexpr auto span_hw_u16s = span{hw_u16s}.first<span{hw_u16s}.size() - 1>();

static constexpr uint_least16_t hw_u16u[]{
    u'H', u'e', u'l', u'l', u'o', u',', u' ', u'w', u'o', u'r', u'l', u'd', u'!', u'\0'};
static constexpr auto span_hw_u16u = span{hw_u16u}.first<span{hw_u16u}.size() - 1>();

struct u16string_instantiator {
    template <ranges::input_range R>
    static void call() {
        test_string(R{span_hw_u16}, hw_u16);
        static_assert(test_string(R{span_hw_u16}, hw_u16));
    }
};

static constexpr char32_t hw_u32[] = U"Hello, world!";
static constexpr auto span_hw_u32  = span{hw_u32}.first<span{hw_u32}.size() - 1>();

static constexpr int_least32_t hw_u32s[]{
    U'H', U'e', U'l', U'l', U'o', U',', U' ', U'w', U'o', U'r', U'l', U'd', U'!', U'\0'};
static constexpr auto span_hw_u32s = span{hw_u32s}.first<span{hw_u32s}.size() - 1>();

static constexpr uint_least32_t hw_u32u[]{
    U'H', U'e', U'l', U'l', U'o', U',', U' ', U'w', U'o', U'r', U'l', U'd', U'!', U'\0'};
static constexpr auto span_hw_u32u = span{hw_u32u}.first<span{hw_u32u}.size() - 1>();

static constexpr long hw_slong[]{U'H', U'e', U'l', U'l', U'o', U',', U' ', U'w', U'o', U'r', U'l', U'd', U'!', U'\0'};
static constexpr auto span_hw_slong = span{hw_slong}.first<span{hw_slong}.size() - 1>();

static constexpr unsigned long hw_ulong[]{
    U'H', U'e', U'l', U'l', U'o', U',', U' ', U'w', U'o', U'r', U'l', U'd', U'!', U'\0'};
static constexpr auto span_hw_ulong = span{hw_ulong}.first<span{hw_ulong}.size() - 1>();

struct u32string_instantiator {
    template <ranges::input_range R>
    static void call() {
        test_string(R{span_hw_u32}, hw_u32);
        static_assert(test_string(R{span_hw_u32}, hw_u32));
    }
};

using swchar_t = make_signed_t<wchar_t>;
using uwchar_t = make_unsigned_t<wchar_t>;

static constexpr wchar_t whw[] = L"Hello, world!";
static constexpr auto span_whw = span{whw}.first<span{whw}.size() - 1>();

static constexpr swchar_t whw_s[]{L'H', L'e', L'l', L'l', L'o', L',', L' ', L'w', L'o', L'r', L'l', L'd', L'!', L'\0'};
static constexpr auto span_whw_s = span{whw_s}.first<span{whw_s}.size() - 1>();

static constexpr uwchar_t whw_u[]{L'H', L'e', L'l', L'l', L'o', L',', L' ', L'w', L'o', L'r', L'l', L'd', L'!', L'\0'};
static constexpr auto span_whw_u = span{whw_u}.first<span{whw_u}.size() - 1>();

struct wstring_instantiator {
    template <ranges::input_range R>
    static void call() {
        test_string(R{span_whw}, whw);
        static_assert(test_string(R{span_whw}, whw));
    }
};

template <class Category, class CharT, test::Common IsCommon,
    bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const CharT, test::Sized{is_random}, test::CanDifference{is_random},
    IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag>},
    test::ProxyRef{!derived_from<Category, contiguous_iterator_tag>}, test::CanView::yes, test::Copyability::move_only>;

constexpr bool test_copyable_views() {
    test_string(span_hw, hw);
    test_string(span_hw_s, hw);
    test_string(span_hw_u, hw);
#ifdef __cpp_char8_t
    test_string(span_hw_u8, hw);

    test_string(span_hw_u8, hw_u8);
    test_string(span_hw, hw_u8);
    test_string(span_hw_s, hw_u8);
    test_string(span_hw_u, hw_u8);
#endif // defined(__cpp_char8_t)

    test_string(span_hw_u16, hw_u16);
    test_string(span_hw_u16s, hw_u16);
    test_string(span_hw_u16u, hw_u16);
    test_string(span_whw, hw_u16);

    test_string(span_hw_u32, hw_u32);
    test_string(span_hw_u32s, hw_u32);
    test_string(span_hw_u32u, hw_u32);
    test_string(span_hw_slong, hw_u32);
    test_string(span_hw_ulong, hw_u32);

    test_string(span_whw, whw);
    test_string(span_whw_s, whw);
    test_string(span_whw_u, whw);
    test_string(span_hw_u16, whw);

    return true;
}

constexpr bool test_move_only_views() {
    test_string(move_only_view<input_iterator_tag, char, test::Common::no>{span_hw}, hw);
    test_string(move_only_view<forward_iterator_tag, char, test::Common::no>{span_hw}, hw);
    test_string(move_only_view<forward_iterator_tag, char, test::Common::yes>{span_hw}, hw);
    test_string(move_only_view<bidirectional_iterator_tag, char, test::Common::no>{span_hw}, hw);
    test_string(move_only_view<bidirectional_iterator_tag, char, test::Common::yes>{span_hw}, hw);
    test_string(move_only_view<random_access_iterator_tag, char, test::Common::no>{span_hw}, hw);
    test_string(move_only_view<random_access_iterator_tag, char, test::Common::yes>{span_hw}, hw);

    test_string(move_only_view<input_iterator_tag, wchar_t, test::Common::no>{span_whw}, whw);
    test_string(move_only_view<forward_iterator_tag, wchar_t, test::Common::no>{span_whw}, whw);
    test_string(move_only_view<forward_iterator_tag, wchar_t, test::Common::yes>{span_whw}, whw);
    test_string(move_only_view<bidirectional_iterator_tag, wchar_t, test::Common::no>{span_whw}, whw);
    test_string(move_only_view<bidirectional_iterator_tag, wchar_t, test::Common::yes>{span_whw}, whw);
    test_string(move_only_view<random_access_iterator_tag, wchar_t, test::Common::no>{span_whw}, whw);
    test_string(move_only_view<random_access_iterator_tag, wchar_t, test::Common::yes>{span_whw}, whw);

    return true;
}

static constexpr char simple_hw[]{'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!'};
static constexpr signed char simple_hw_s[]{'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!'};
static constexpr unsigned char simple_hw_u[]{'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!'};
#ifdef __cpp_char8_t
static constexpr char8_t simple_hw_u8[]{
    u8'H', u8'e', u8'l', u8'l', u8'o', u8',', u8' ', u8'w', u8'o', u8'r', u8'l', u8'd', u8'!'};
#endif // defined(__cpp_char8_t)

static constexpr char16_t simple_hw_u16[]{u'H', u'e', u'l', u'l', u'o', u',', u' ', u'w', u'o', u'r', u'l', u'd', u'!'};
static constexpr int_least16_t simple_hw_u16s[]{
    u'H', u'e', u'l', u'l', u'o', u',', u' ', u'w', u'o', u'r', u'l', u'd', u'!'};
static constexpr uint_least16_t simple_hw_u16u[]{
    u'H', u'e', u'l', u'l', u'o', u',', u' ', u'w', u'o', u'r', u'l', u'd', u'!'};

static constexpr char32_t simple_hw_u32[]{U'H', U'e', U'l', U'l', U'o', U',', U' ', U'w', U'o', U'r', U'l', U'd', U'!'};
static constexpr int_least32_t simple_hw_u32s[]{
    U'H', U'e', U'l', U'l', U'o', U',', U' ', U'w', U'o', U'r', U'l', U'd', U'!'};
static constexpr uint_least32_t simple_hw_u32u[]{
    U'H', U'e', U'l', U'l', U'o', U',', U' ', U'w', U'o', U'r', U'l', U'd', U'!'};
static constexpr long simple_hw_slong[]{U'H', U'e', U'l', U'l', U'o', U',', U' ', U'w', U'o', U'r', U'l', U'd', U'!'};
static constexpr unsigned long simple_hw_ulong[]{
    U'H', U'e', U'l', U'l', U'o', U',', U' ', U'w', U'o', U'r', U'l', U'd', U'!'};

static constexpr wchar_t simple_whw[]{L'H', L'e', L'l', L'l', L'o', L',', L' ', L'w', L'o', L'r', L'l', L'd', L'!'};
static constexpr swchar_t simple_whw_s[]{L'H', L'e', L'l', L'l', L'o', L',', L' ', L'w', L'o', L'r', L'l', L'd', L'!'};
static constexpr uwchar_t simple_whw_u[]{L'H', L'e', L'l', L'l', L'o', L',', L' ', L'w', L'o', L'r', L'l', L'd', L'!'};

constexpr bool test_c_array() {
    test_string(simple_hw, hw);
    test_string(simple_hw_s, hw);
    test_string(simple_hw_u, hw);
#ifdef __cpp_char8_t
    test_string(simple_hw_u8, hw);

    test_string(simple_hw_u8, hw_u8);
    test_string(simple_hw, hw_u8);
    test_string(simple_hw_s, hw_u8);
    test_string(simple_hw_u, hw_u8);
#endif // defined(__cpp_char8_t)

    test_string(simple_hw_u16, hw_u16);
    test_string(simple_hw_u16s, hw_u16);
    test_string(simple_hw_u16u, hw_u16);
    test_string(simple_whw, hw_u16);

    test_string(simple_hw_u32, hw_u32);
    test_string(simple_hw_u32s, hw_u32);
    test_string(simple_hw_u32u, hw_u32);
    test_string(simple_hw_slong, hw_u32);
    test_string(simple_hw_ulong, hw_u32);

    test_string(simple_whw, whw);
    test_string(simple_whw_s, whw);
    test_string(simple_whw_u, whw);
    test_string(simple_hw_u16, whw);

    return true;
}

constexpr bool test_std_array() {
    test_string(to_array(simple_hw), hw);
    test_string(to_array(simple_hw_s), hw);
    test_string(to_array(simple_hw_u), hw);
#ifdef __cpp_char8_t
    test_string(to_array(simple_hw_u8), hw);

    test_string(to_array(simple_hw_u8), hw_u8);
    test_string(to_array(simple_hw), hw_u8);
    test_string(to_array(simple_hw_s), hw_u8);
    test_string(to_array(simple_hw_u), hw_u8);
#endif // defined(__cpp_char8_t)

    test_string(to_array(simple_hw_u16), hw_u16);
    test_string(to_array(simple_hw_u16s), hw_u16);
    test_string(to_array(simple_hw_u16u), hw_u16);
    test_string(to_array(simple_whw), hw_u16);

    test_string(to_array(simple_hw_u32), hw_u32);
    test_string(to_array(simple_hw_u32s), hw_u32);
    test_string(to_array(simple_hw_u32u), hw_u32);
    test_string(to_array(simple_hw_slong), hw_u32);
    test_string(to_array(simple_hw_ulong), hw_u32);

    test_string(to_array(simple_whw), whw);
    test_string(to_array(simple_whw_s), whw);
    test_string(to_array(simple_whw_u), whw);
    test_string(to_array(simple_hw_u16), whw);

    return true;
}

constexpr bool test_lvalue_vector() {
    constexpr auto test_lvalue_vector_helper = []<class Span, class CharT>(const Span& sp, const CharT* cstr) {
        vector vec(sp.data(), sp.data() + sp.size());
        test_string(vec, cstr);
    };

    test_lvalue_vector_helper(span_hw, hw);
    test_lvalue_vector_helper(span_hw_s, hw);
    test_lvalue_vector_helper(span_hw_u, hw);
#ifdef __cpp_char8_t
    test_lvalue_vector_helper(span_hw_u8, hw);

    test_lvalue_vector_helper(span_hw_u8, hw_u8);
    test_lvalue_vector_helper(span_hw, hw_u8);
    test_lvalue_vector_helper(span_hw_s, hw_u8);
    test_lvalue_vector_helper(span_hw_u, hw_u8);
#endif // defined(__cpp_char8_t)

    test_lvalue_vector_helper(span_hw_u16, hw_u16);
    test_lvalue_vector_helper(span_hw_u16s, hw_u16);
    test_lvalue_vector_helper(span_hw_u16u, hw_u16);
    test_lvalue_vector_helper(span_whw, hw_u16);

    test_lvalue_vector_helper(span_hw_u32, hw_u32);
    test_lvalue_vector_helper(span_hw_u32s, hw_u32);
    test_lvalue_vector_helper(span_hw_u32u, hw_u32);
    test_lvalue_vector_helper(span_hw_slong, hw_u32);
    test_lvalue_vector_helper(span_hw_ulong, hw_u32);

    test_lvalue_vector_helper(span_whw, whw);
    test_lvalue_vector_helper(span_whw_s, whw);
    test_lvalue_vector_helper(span_whw_u, whw);
    test_lvalue_vector_helper(span_hw_u16, whw);

    return true;
}

void test_lvalue_forward_list() {
    {
        forward_list lst(span_hw.data(), span_hw.data() + span_hw.size());
        test_string(lst, hw);
    }
#ifdef __cpp_char8_t
    {
        forward_list lst(span_hw_u8.data(), span_hw_u8.data() + span_hw_u8.size());
        test_string(lst, hw_u8);
    }
#endif // defined(__cpp_char8_t)
    {
        forward_list lst(span_hw_u16.data(), span_hw_u16.data() + span_hw_u16.size());
        test_string(lst, hw_u16);
    }
    {
        forward_list lst(span_hw_u32.data(), span_hw_u32.data() + span_hw_u32.size());
        test_string(lst, hw_u32);
    }
    {
        forward_list lst(span_whw.data(), span_whw.data() + span_whw.size());
        test_string(lst, whw);
    }
}

int main() {
    // Validate views
    test_copyable_views();
    static_assert(test_copyable_views());

    test_move_only_views();
    static_assert(test_move_only_views());

    // Validate non-views
    test_c_array();
    static_assert(test_c_array());

    test_std_array();
    static_assert(test_std_array());

    test_lvalue_vector();
    static_assert(test_lvalue_vector());

    test_lvalue_forward_list();

    test_in<string_instantiator, const char>();
    test_in<wstring_instantiator, const wchar_t>();

#ifdef __cpp_char8_t
    test_contiguous<u8string_instantiator, const char8_t>();
#endif // defined(__cpp_char8_t)
    test_contiguous<u16string_instantiator, const char16_t>();
    test_contiguous<u32string_instantiator, const char32_t>();
}
