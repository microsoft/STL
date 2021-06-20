// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <assert.h>
#include <format>
#include <limits>
#include <string>
#include <string_view>
#include <type_traits>
using namespace std;

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

template <>
struct std::formatter<basic_custom_formattable_type, char> {
    basic_format_parse_context<char>::iterator parse(basic_format_parse_context<char>& parse_ctx) {
        if (parse_ctx.begin() != parse_ctx.end()) {
            throw format_error{"only empty specs please"};
        }
        return parse_ctx.end();
    }
    format_context::iterator format(const basic_custom_formattable_type& val, format_context& ctx) {
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
    auto format(const custom_formattable_type<T>& val, FC& format_ctx) {
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
    assert(format(fmt_string, custom_formattable_type<T>{val}) == format(fmt_string, val));
}

template <class T, class charT>
void test_custom_equiv_with_format_mixed(const charT* fmt_string, const T& val) {
    assert(format(fmt_string, custom_formattable_type<T>{val}, val) == format(fmt_string, val, val));
}

template <class T, class charT>
void test_numeric_custom_formattable_type() {
    test_custom_equiv_with_format<T>(STR("{}"), 0);
    test_custom_equiv_with_format<T>(STR("{}"), 42);
    test_custom_equiv_with_format<T>(STR("{:+}"), 0);
    test_custom_equiv_with_format<T>(STR("{}"), numeric_limits<T>::min());
    test_custom_equiv_with_format<T>(STR("{}"), numeric_limits<T>::max());
    test_custom_equiv_with_format<T>(STR("{:3}"), 1);
    if constexpr (!is_floating_point_v<T> && !_Is_any_of_v<T, charT, bool>) {
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
    if constexpr (!is_floating_point_v<T> && !_Is_any_of_v<T, charT, bool>) {
        test_custom_equiv_with_format_mixed<T>(STR("{:#x}{}"), 255);
        test_custom_equiv_with_format_mixed<T>(STR("{:#X}{}"), 255);
    }
}

template <class charT>
void test_custom_formattable_type() {
    test_numeric_custom_formattable_type<int, charT>();
    test_numeric_custom_formattable_type<long long, charT>();
    test_numeric_custom_formattable_type<unsigned int, charT>();
    test_numeric_custom_formattable_type<unsigned long long, charT>();
    test_numeric_custom_formattable_type<short, charT>();
#ifdef _NATIVE_WCHAR_T_DEFINED
    test_numeric_custom_formattable_type<unsigned short, charT>();
#endif
    test_numeric_custom_formattable_type<float, charT>();
    test_numeric_custom_formattable_type<double, charT>();
}

template <class charT>
void test_mixed_custom_formattable_type() {
    test_numeric_mixed_args_custom_formattable_type<int, charT>();
    test_numeric_mixed_args_custom_formattable_type<long long, charT>();
    test_numeric_mixed_args_custom_formattable_type<unsigned int, charT>();
    test_numeric_mixed_args_custom_formattable_type<unsigned long long, charT>();
    test_numeric_mixed_args_custom_formattable_type<short, charT>();
#ifdef _NATIVE_WCHAR_T_DEFINED
    test_numeric_mixed_args_custom_formattable_type<unsigned short, charT>();
#endif
    test_numeric_mixed_args_custom_formattable_type<float, charT>();
    test_numeric_mixed_args_custom_formattable_type<double, charT>();
}

int main() {
    assert(format("{}", basic_custom_formattable_type{"f"}) == "f"s);
    test_custom_formattable_type<char>();
    test_custom_formattable_type<wchar_t>();
    test_mixed_custom_formattable_type<char>();
    test_mixed_custom_formattable_type<wchar_t>();
    return 0;
}
