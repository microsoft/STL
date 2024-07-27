// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
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
auto make_testing_format_args(Args&&... vals) { // references to temporaries are risky, see P2905R2; we'll be careful
    if constexpr (is_same_v<charT, wchar_t>) {
        return make_wformat_args(vals...);
    } else {
        return make_format_args(vals...);
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
    constexpr basic_format_parse_context<char>::iterator parse(basic_format_parse_context<char>& parse_ctx) {
        if (parse_ctx.begin() != parse_ctx.end() && *parse_ctx.begin() != '}') {
            throw format_error{"only empty specs please"};
        }
        return parse_ctx.begin();
    }
    format_context::iterator format(const basic_custom_formattable_type& val, format_context& ctx) const {
        ctx.advance_to(copy(val.string_content.begin(), val.string_content.end(), ctx.out()));
        return ctx.out();
    }
};

template <>
struct std::formatter<not_const_formattable_type, char> {
    constexpr basic_format_parse_context<char>::iterator parse(basic_format_parse_context<char>& parse_ctx) {
        if (parse_ctx.begin() != parse_ctx.end() && *parse_ctx.begin() != '}') {
            throw format_error{"only empty specs please"};
        }
        return parse_ctx.begin();
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

constexpr void test_disabled_formatter_is_disabled() { // COMPILE-ONLY
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

    test_custom_equiv_with_format<charT, charT>(STR("{}"), charT(' '));
    test_custom_equiv_with_format<char, charT>(STR("{}"), ' ');

    charT test_str[] = {charT(' '), charT()};
    test_custom_equiv_with_format<charT*, charT>(STR("{}"), test_str);
    test_custom_equiv_with_format<const charT*, charT>(STR("{}"), test_str);

    struct traits : char_traits<charT> {};
    test_custom_equiv_with_format<basic_string<charT, traits>, charT>(STR("{}"), test_str);
    test_custom_equiv_with_format<basic_string_view<charT, traits>, charT>(STR("{}"), test_str);

    test_custom_equiv_with_format<nullptr_t, charT>(STR("{}"), nullptr);
    test_custom_equiv_with_format<void*, charT>(STR("{}"), nullptr);
    test_custom_equiv_with_format<const void*, charT>(STR("{}"), nullptr);
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

    test_custom_equiv_with_format_mixed<charT, charT>(STR("{}{}"), charT(' '));
    test_custom_equiv_with_format_mixed<char, charT>(STR("{}{}"), ' ');

    charT test_str[] = {charT(' '), charT()};
    test_custom_equiv_with_format_mixed<charT*, charT>(STR("{}{}"), test_str);
    test_custom_equiv_with_format_mixed<const charT*, charT>(STR("{}{}"), test_str);

    struct traits : char_traits<charT> {};
    test_custom_equiv_with_format_mixed<basic_string<charT, traits>, charT>(STR("{}{}"), test_str);
    test_custom_equiv_with_format_mixed<basic_string_view<charT, traits>, charT>(STR("{}{}"), test_str);

    test_custom_equiv_with_format_mixed<nullptr_t, charT>(STR("{}{}"), nullptr);
    test_custom_equiv_with_format_mixed<void*, charT>(STR("{}{}"), nullptr);
    test_custom_equiv_with_format_mixed<const void*, charT>(STR("{}{}"), nullptr);
}

// Test that handle doesn't have public non-Standard constructors.
template <class OutIt, class CharT>
void test_basic_format_arg_handle_construction() {
    using handle = basic_format_arg<basic_format_context<OutIt, CharT>>::handle;

    static_assert(is_constructible_v<handle, handle>);
    static_assert(is_constructible_v<handle, const handle&>);

    static_assert(!is_constructible_v<handle, int&>);
    static_assert(!is_constructible_v<handle, const int&>);
    static_assert(!is_constructible_v<handle, int>);
    static_assert(!is_constructible_v<handle, const int>);

    static_assert(!is_constructible_v<handle, custom_formattable_type<CharT>&>);
    static_assert(!is_constructible_v<handle, const custom_formattable_type<CharT>&>);
    static_assert(!is_constructible_v<handle, custom_formattable_type<CharT>>);
    static_assert(!is_constructible_v<handle, const custom_formattable_type<CharT>>);
}

template <class T, class... Args>
constexpr bool is_constructible_with_trailing_empty_brace_impl = requires { T(declval<Args>()..., {}); };

static_assert(is_constructible_with_trailing_empty_brace_impl<string>);
static_assert(is_constructible_with_trailing_empty_brace_impl<pair<int, int>, int>);
static_assert(!is_constructible_with_trailing_empty_brace_impl<pair<int, int>, int, int>);

// Test that basic_format_context doesn't have public non-Standard constructors.
template <class OutIt, class CharT>
void test_basic_format_context_construction() {
    using context = basic_format_context<OutIt, CharT>;

    static_assert(!is_default_constructible_v<context>);
    static_assert(!is_copy_constructible_v<context>);
    static_assert(!is_move_constructible_v<context>);

    // Also test the deleted copy assignment operator
    // from LWG-4061 "Should std::basic_format_context be default-constructible/copyable/movable?"
    static_assert(!is_copy_assignable_v<context>);
    static_assert(!is_move_assignable_v<context>);

    static_assert(!is_constructible_v<context, OutIt, basic_format_args<context>>);
    static_assert(!is_constructible_v<context, OutIt, const basic_format_args<context>&>);

    static_assert(!is_constructible_with_trailing_empty_brace_impl<context>);
    static_assert(!is_constructible_with_trailing_empty_brace_impl<context, OutIt, basic_format_args<context>>);
    static_assert(!is_constructible_with_trailing_empty_brace_impl<context, OutIt, const basic_format_args<context>&>);

    // Also test LWG-4106 "basic_format_args should not be default-constructible"
    static_assert(!is_default_constructible_v<basic_format_args<context>>);
}

// Test GH-4636 "<format>: Call to next_arg_id may result in unexpected error (regression)"

struct FormatNextArg {};

template <class CharT>
struct std::formatter<FormatNextArg, CharT> {
public:
    template <class ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        auto it = ctx.begin();
        if (it != ctx.end() && *it != '}') {
            throw format_error{"Expected empty spec"};
        }

        arg_id = ctx.next_arg_id();
        return it;
    }

    template <class FormatContext>
    auto format(FormatNextArg, FormatContext& ctx) const {
        return format_to(ctx.out(), TYPED_LITERAL(CharT, "arg-id: {}"), arg_id);
    }

private:
    size_t arg_id;
};

template <class CharT>
void test_parsing_with_next_id() {
    assert(format(TYPED_LITERAL(CharT, "{}, {}"), FormatNextArg{}, 0, FormatNextArg{}, TYPED_LITERAL(CharT, "1"))
           == TYPED_LITERAL(CharT, "arg-id: 1, arg-id: 3"));
    assert(format(TYPED_LITERAL(CharT, "{:}, {:}"), FormatNextArg{}, 2, FormatNextArg{}, TYPED_LITERAL(CharT, "3"))
           == TYPED_LITERAL(CharT, "arg-id: 1, arg-id: 3"));
}

struct NeedMagicWord {};

template <class CharT>
struct std::formatter<NeedMagicWord, CharT> {
    constexpr auto parse(const basic_format_parse_context<CharT>& ctx) {
        constexpr basic_string_view<CharT> magic_word{TYPED_LITERAL(CharT, "narf")};
        auto [i, j] = ranges::mismatch(ctx, magic_word);
        if (j != magic_word.end()) {
            throw format_error{"you didn't say the magic word!"};
        }

        if (i != ctx.end() && *i != '}') {
            throw format_error{"the whole spec must be the magic word!"};
        }
        return i;
    }

    template <class OutIt>
    auto format(NeedMagicWord, basic_format_context<OutIt, CharT>& ctx) const {
        return ctx.out();
    }
};

template <class CharT>
void test_parsing_needing_magic_word() {
    assert(format(TYPED_LITERAL(CharT, "{:narf}"), NeedMagicWord{}).empty());
}

int main() {
    test_format_family_overloads<basic_custom_formattable_type>();
    test_format_family_overloads<not_const_formattable_type>();
    test_custom_formattable_type<char>();
    test_custom_formattable_type<wchar_t>();
    test_mixed_custom_formattable_type<char>();
    test_mixed_custom_formattable_type<wchar_t>();

    test_basic_format_arg_handle_construction<char*, char>();
    test_basic_format_arg_handle_construction<string::iterator, char>();
    test_basic_format_arg_handle_construction<back_insert_iterator<string>, char>();
    test_basic_format_arg_handle_construction<wchar_t*, wchar_t>();
    test_basic_format_arg_handle_construction<wstring::iterator, wchar_t>();
    test_basic_format_arg_handle_construction<back_insert_iterator<wstring>, wchar_t>();

    test_basic_format_context_construction<char*, char>();
    test_basic_format_context_construction<string::iterator, char>();
    test_basic_format_context_construction<back_insert_iterator<string>, char>();
    test_basic_format_context_construction<wchar_t*, wchar_t>();
    test_basic_format_context_construction<wstring::iterator, wchar_t>();
    test_basic_format_context_construction<back_insert_iterator<wstring>, wchar_t>();

    test_parsing_with_next_id<char>();
    test_parsing_with_next_id<wchar_t>();
    test_parsing_needing_magic_word<char>();
    test_parsing_needing_magic_word<wchar_t>();
}
