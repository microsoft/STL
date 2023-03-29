// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <format>
#include <iterator>
#include <limits>
#include <locale>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
using namespace std;

// copied from the text_formatting_formatting test case
template <class charT, class... Args>
auto make_testing_format_args(Args&&... vals) {
    if constexpr (is_same_v<charT, wchar_t>) {
        return make_wformat_args(forward<Args>(vals)...);
    } else {
        return make_format_args(forward<Args>(vals)...);
    }
}

// copied from the string_view tests
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
#define STR(Str)                      TYPED_LITERAL(charT, Str)

struct basic_custom_formattable_type {
    string_view string_content;
};

struct not_const_formattable_type {
    string_view string_content;
    explicit not_const_formattable_type(string_view val) : string_content(val) {}
    not_const_formattable_type(const not_const_formattable_type&) = delete;
    not_const_formattable_type(not_const_formattable_type&&)      = delete;
};

template <>
struct std::formatter<basic_custom_formattable_type, char> {
    basic_format_parse_context<char>::iterator parse(basic_format_parse_context<char>& parse_ctx) {
        if (parse_ctx.begin() != parse_ctx.end()) {
            throw format_error{"only empty specs please"};
        }
        return parse_ctx.end();
    }
    format_context::iterator format(const basic_custom_formattable_type& val, format_context& ctx) const {
        ctx.advance_to(copy(val.string_content.begin(), val.string_content.end(), ctx.out()));
        return ctx.out();
    }
};

template <>
struct std::formatter<not_const_formattable_type, char> {
    basic_format_parse_context<char>::iterator parse(basic_format_parse_context<char>& parse_ctx) {
        if (parse_ctx.begin() != parse_ctx.end()) {
            throw format_error{"only empty specs please"};
        }
        return parse_ctx.end();
    }
    format_context::iterator format(not_const_formattable_type& val, format_context& ctx) const {
        ctx.advance_to(copy(val.string_content.begin(), val.string_content.end(), ctx.out()));
        return ctx.out();
    }
};

template <class T>
struct custom_formattable_type {
    T val;
};

template <class T, class charT>
struct std::formatter<custom_formattable_type<T>, charT> : std::formatter<T, charT> {
    template <class FC>
    auto format(const custom_formattable_type<T>& val, FC& format_ctx) const {
        return std::formatter<T, charT>::format(val.val, format_ctx);
    }
};

constexpr void test_disabled_formatter_is_disabled() {
    using F = formatter<void, void>;

    static_assert(!is_default_constructible_v<F>);
    static_assert(!is_copy_constructible_v<F>);
    static_assert(!is_move_constructible_v<F>);
    static_assert(!is_copy_assignable_v<F>);
    static_assert(!is_move_assignable_v<F>);
}

template <class T, class charT>
void test_custom_equiv_with_format(const charT* fmt_string, const T& val) {
    assert(vformat(fmt_string, make_testing_format_args<charT>(custom_formattable_type<T>{val}))
           == vformat(fmt_string, make_testing_format_args<charT>(val)));
}

template <class T, class charT>
void test_custom_equiv_with_format_mixed(const charT* fmt_string, const T& val) {
    assert(vformat(fmt_string, make_testing_format_args<charT>(custom_formattable_type<T>{val}, val))
           == vformat(fmt_string, make_testing_format_args<charT>(val, val)));
}

template <class T, class charT>
void test_numeric_custom_formattable_type() {
    test_custom_equiv_with_format<T>(STR("{}"), 0);
    test_custom_equiv_with_format<T>(STR("{}"), 42);
    test_custom_equiv_with_format<T>(STR("{:+}"), 0);
    test_custom_equiv_with_format<T>(STR("{}"), numeric_limits<T>::min());
    test_custom_equiv_with_format<T>(STR("{}"), numeric_limits<T>::max());
    test_custom_equiv_with_format<T>(STR("{:3}"), 1);
    if constexpr (!is_floating_point_v<T> && !is_same_v<T, charT> && !is_same_v<T, bool>) {
        test_custom_equiv_with_format<T>(STR("{:#x}"), 255);
        test_custom_equiv_with_format<T>(STR("{:#X}"), 255);
    }
}

template <class T, class charT>
void test_numeric_mixed_args_custom_formattable_type() {
    test_custom_equiv_with_format_mixed<T>(STR("{}{}"), 0);
    test_custom_equiv_with_format_mixed<T>(STR("{1}{0}"), 42);
    test_custom_equiv_with_format_mixed<T>(STR("{:+}{:-}"), 0);
    test_custom_equiv_with_format_mixed<T>(STR("{}{}"), numeric_limits<T>::min());
    test_custom_equiv_with_format_mixed<T>(STR("{}{}"), numeric_limits<T>::max());
    test_custom_equiv_with_format_mixed<T>(STR("{:3}{:4}"), 1);
    test_custom_equiv_with_format_mixed<T>(STR("{0}{0}"), 42);
    if constexpr (!is_floating_point_v<T> && !is_same_v<T, charT> && !is_same_v<T, bool>) {
        test_custom_equiv_with_format_mixed<T>(STR("{:#x}{}"), 255);
        test_custom_equiv_with_format_mixed<T>(STR("{:#X}{}"), 255);
    }
}

template <class CustomFormattableType>
void test_format_family_overloads() {
    string str;

    assert(format("{}", CustomFormattableType{"f"}) == "f"s);
    assert(format(locale{}, "{}", CustomFormattableType{"f"}) == "f"s);
    format_to(back_insert_iterator(str), "{}", CustomFormattableType{"f"});
    assert(str == "f");
    str.clear();
    format_to(back_insert_iterator(str), locale{}, "{}", CustomFormattableType{"f"});
    assert(str == "f");
    str.clear();
    format_to_n(back_insert_iterator(str), 5, "{}", CustomFormattableType{"f"});
    assert(str == "f");
    str.clear();
    format_to_n(back_insert_iterator(str), 5, locale{}, "{}", CustomFormattableType{"f"});
    assert(str == "f");
    str.clear();
    assert(formatted_size("{}", CustomFormattableType{"f"}) == 1);
    assert(formatted_size(locale{}, "{}", CustomFormattableType{"f"}) == 1);
}

template <class charT>
void test_custom_formattable_type() {
    test_numeric_custom_formattable_type<int, charT>();
    test_numeric_custom_formattable_type<long, charT>();
    test_numeric_custom_formattable_type<long long, charT>();
    test_numeric_custom_formattable_type<unsigned int, charT>();
    test_numeric_custom_formattable_type<unsigned long, charT>();
    test_numeric_custom_formattable_type<unsigned long long, charT>();
    test_numeric_custom_formattable_type<short, charT>();
#ifdef _NATIVE_WCHAR_T_DEFINED
    test_numeric_custom_formattable_type<unsigned short, charT>();
#endif
    test_numeric_custom_formattable_type<float, charT>();
    test_numeric_custom_formattable_type<double, charT>();
    test_numeric_custom_formattable_type<long double, charT>();
}

template <class charT>
void test_mixed_custom_formattable_type() {
    test_numeric_mixed_args_custom_formattable_type<int, charT>();
    test_numeric_mixed_args_custom_formattable_type<long, charT>();
    test_numeric_mixed_args_custom_formattable_type<long long, charT>();
    test_numeric_mixed_args_custom_formattable_type<unsigned int, charT>();
    test_numeric_mixed_args_custom_formattable_type<unsigned long, charT>();
    test_numeric_mixed_args_custom_formattable_type<unsigned long long, charT>();
    test_numeric_mixed_args_custom_formattable_type<short, charT>();
#ifdef _NATIVE_WCHAR_T_DEFINED
    test_numeric_mixed_args_custom_formattable_type<unsigned short, charT>();
#endif
    test_numeric_mixed_args_custom_formattable_type<float, charT>();
    test_numeric_mixed_args_custom_formattable_type<double, charT>();
    test_numeric_mixed_args_custom_formattable_type<long double, charT>();
}

int main() {
    test_format_family_overloads<basic_custom_formattable_type>();
    test_format_family_overloads<not_const_formattable_type>();
    test_custom_formattable_type<char>();
    test_custom_formattable_type<wchar_t>();
    test_mixed_custom_formattable_type<char>();
    test_mixed_custom_formattable_type<wchar_t>();
    return 0;
}
