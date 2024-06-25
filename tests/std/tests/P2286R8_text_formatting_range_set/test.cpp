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
// * support/format.functions.common.h
// * std/utilities/format/format.range/format.range.fmtset/format.functions.tests.h
// * std/utilities/format/format.range/format.range.fmtset/format.functions.format.pass.cpp
// * std/utilities/format/format.range/format.range.fmtset/format.functions.vformat.pass.cpp

#include <algorithm>
#include <charconv>
#include <concepts>
#include <cstddef>
#include <cstring>
#include <format>
#include <iterator>
#include <ranges>
#include <set>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

#include <is_permissive.hpp>
#include <test_format_support.hpp>

using namespace std;

#define CSTR(Str) TYPED_LITERAL(CharT, Str)
#define STR(Str)  basic_string(CSTR(Str))
#define SV(Str)   basic_string_view(CSTR(Str))

namespace detail {
    consteval string_view get_format_types() noexcept {
        return "aAbBcdeEfFgGopPsxX?";
    }

    template <class CharT, size_t N>
    basic_string<CharT> get_colons() {
        return basic_string<CharT>(N, CharT(':'));
    }

    template <class CharT, /*format_types types,*/ size_t N>
    vector<basic_string<CharT>> fmt_invalid_types(string_view valid) {
        vector<basic_string<CharT>> result;
        if constexpr (is_permissive_v<CharT>) {
            for (const char c : get_format_types()) {
                if (valid.find(c) == string_view::npos) {
                    result.push_back(format(SV("{{{}{}}}"), get_colons<CharT, N>(), c));
                }
            }
        } else {
            // ranges::to is not available in C++20.
            ranges::copy(get_format_types() | views::filter([&](char type) {
                return valid.find(type) == string_view::npos;
            }) | views::transform([&](char type) { return format(SV("{{{}{}}}"), get_colons<CharT, N>(), type); }),
                back_inserter(result));
        }
        return result;
    }
} // namespace detail

// Creates format string for the invalid types.
//
// valid contains a list of types that are valid.
//
// The return value is a collection of basic_strings, instead of
// basic_string_views since the values are temporaries.
template <class CharT>
vector<basic_string<CharT>> fmt_invalid_types(string_view valid) {
    return detail::fmt_invalid_types<CharT, 1>(valid);
}

// Like fmt_invalid_types but when the format spec is for an underlying formatter.
template <class CharT>
vector<basic_string<CharT>> fmt_invalid_nested_types(string_view valid) {
    return detail::fmt_invalid_types<CharT, 2>(valid);
}

template <class T>
struct context {};

template <>
struct context<char> {
    using type = format_context;
};

template <>
struct context<wchar_t> {
    using type = wformat_context;
};

template <class T>
using context_t = context<T>::type;

// A user-defined type used to test the handle formatter.
enum class status : uint16_t { foo = 0xAAAA, bar = 0x5555, foobar = 0xAA55 };

// The formatter for a user-defined type used to test the handle formatter.
template <class CharT>
struct formatter<status, CharT> {
    // During the 2023 Issaquah meeting LEWG made it clear a formatter is
    // required to call its parse function. LWG3892 Adds the wording for that
    // requirement. Therefore this formatter is initialized in an invalid state.
    // A call to parse sets it in a valid state and a call to format validates
    // the state.
    int type = -1;

    constexpr auto parse(basic_format_parse_context<CharT>& parse_ctx) -> decltype(parse_ctx.begin()) {
        auto begin = parse_ctx.begin();
        auto end   = parse_ctx.end();
        type       = 0;
        if (begin == end) {
            return begin;
        }

        switch (*begin) {
        case CharT('x'):
            break;
        case CharT('X'):
            type = 1;
            break;
        case CharT('s'):
            type = 2;
            break;
        case CharT('}'):
            return begin;
        default:
            throw_format_error("The type option contains an invalid value for a status formatting argument");
        }

        ++begin;
        if (begin != end && *begin != CharT('}')) {
            throw_format_error("The format specifier should consume the input or end with a '}'");
        }

        return begin;
    }

    template <class Out>
    auto format(status s, basic_format_context<Out, CharT>& ctx) const -> decltype(ctx.out()) {
        const char* names[] = {"foo", "bar", "foobar"};
        char buffer[7];
        const char* pbegin = names[0];
        const char* pend   = names[0];
        switch (type) {
        case -1:
            throw_format_error("The formatter's parse function has not been called.");

        case 0:
            pbegin    = buffer;
            buffer[0] = '0';
            buffer[1] = 'x';
            pend      = to_chars(&buffer[2], end(buffer), static_cast<uint16_t>(s), 16).ptr;
            buffer[6] = '\0';
            break;

        case 1:
            pbegin    = buffer;
            buffer[0] = '0';
            buffer[1] = 'X';
            pend      = to_chars(&buffer[2], end(buffer), static_cast<uint16_t>(s), 16).ptr;
            transform(static_cast<const char*>(&buffer[2]), pend, &buffer[2],
                [](char c) { return static_cast<char>(toupper(c)); });
            buffer[6] = '\0';
            break;

        case 2:
            switch (s) {
            case status::foo:
                pbegin = names[0];
                break;
            case status::bar:
                pbegin = names[1];
                break;
            case status::foobar:
                pbegin = names[2];
                break;
            }
            pend = pbegin + strlen(pbegin);
            break;
        }

#if !defined(__clang__) && !defined(__EDG__)
#pragma warning(push)
#pragma warning(disable : 4365)
#endif // !defined(__clang__) && !defined(__EDG__)
        return copy(pbegin, pend, ctx.out());
#if !defined(__clang__) && !defined(__EDG__)
#pragma warning(pop)
#endif // !defined(__clang__) && !defined(__EDG__)
    }

private:
    [[noreturn]] void throw_format_error(const char* s) const {
        throw format_error(s);
    }
};


//
// Char
//

template <class CharT, class TestFunction, class ExceptionTest>
void test_char_default(TestFunction check, ExceptionTest check_exception) {
    set input{CharT('a'), CharT('c'), CharT('b')}; // input not sorted.

    // Note when no range-underlying-spec is present the char is escaped,
    check(SV("{'a', 'b', 'c'}"), SV("{}"), input);
    check(SV("{'a', 'b', 'c'}^42"), SV("{}^42"), input);
    check(SV("{'a', 'b', 'c'}^42"), SV("{:}^42"), input);
    // when one is present there is no escaping,
    check(SV("{a, b, c}"), SV("{::}"), input);
    check(SV("{a, b, c}"), SV("{::<}"), input);
    // unless forced by the type specifier.
    check(SV("{'a', 'b', 'c'}"), SV("{::?}"), input);
    check(SV("{'a', 'b', 'c'}"), SV("{::<?}"), input);

    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV("{'a', 'b', 'c'}     "), SV("{:20}"), input);
    check(SV("{'a', 'b', 'c'}*****"), SV("{:*<20}"), input);
    check(SV("__{'a', 'b', 'c'}___"), SV("{:_^20}"), input);
    check(SV("#####{'a', 'b', 'c'}"), SV("{:#>20}"), input);

    check(SV("{'a', 'b', 'c'}     "), SV("{:{}}"), input, 20);
    check(SV("{'a', 'b', 'c'}*****"), SV("{:*<{}}"), input, 20);
    check(SV("__{'a', 'b', 'c'}___"), SV("{:_^{}}"), input, 20);
    check(SV("#####{'a', 'b', 'c'}"), SV("{:#>{}}"), input, 20);

    check_exception("The format string contains an invalid escape sequence", SV("{:}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{:{<}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:-}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:+}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{: }"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:#}"), input);

    // *** zero-padding ***
    check_exception("The width option should not have a leading zero", SV("{:0}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:.}"), input);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:L}"), input);

    // *** n
    check(SV("__'a', 'b', 'c'___"), SV("{:_^18n}"), input);

    // *** type ***
    check_exception("Type m requires a pair or a tuple with two elements", SV("{:m}"), input);

    // ***** Only underlying has a format-spec
    check(SV("{a   , b   , c   }"), SV("{::4}"), input);
    check(SV("{a***, b***, c***}"), SV("{::*<4}"), input);
    check(SV("{_a__, _b__, _c__}"), SV("{::_^4}"), input);
    check(SV("{:::a, :::b, :::c}"), SV("{:::>4}"), input);

    check(SV("{a   , b   , c   }"), SV("{::{}}"), input, 4);
    check(SV("{a***, b***, c***}"), SV("{::*<{}}"), input, 4);
    check(SV("{_a__, _b__, _c__}"), SV("{::_^{}}"), input, 4);
    check(SV("{:::a, :::b, :::c}"), SV("{:::>{}}"), input, 4);

    check_exception("The format string contains an invalid escape sequence", SV("{::}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{::{<}"), input);

    // *** sign ***
    check_exception("The format specifier for a character does not allow the sign option", SV("{::-}"), input);
    check_exception("The format specifier for a character does not allow the sign option", SV("{::+}"), input);
    check_exception("The format specifier for a character does not allow the sign option", SV("{:: }"), input);

    check(SV("{97, 98, 99}"), SV("{::-d}"), input);
    check(SV("{+97, +98, +99}"), SV("{::+d}"), input);
    check(SV("{ 97,  98,  99}"), SV("{:: d}"), input);

    // *** alternate form ***
    check_exception(
        "The format specifier for a character does not allow the alternate form option", SV("{::#}"), input);

    check(SV("{0x61, 0x62, 0x63}"), SV("{::#x}"), input);

    // *** zero-padding ***
    check_exception("The format specifier for a character does not allow the zero-padding option", SV("{::05}"), input);

    check(SV("{00141, 00142, 00143}"), SV("{::05o}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::.}"), input);

    // *** locale-specific form ***
    check(SV("{a, b, c}"), SV("{::L}"), input);

    // *** type ***
    for (basic_string_view<CharT> fmt : fmt_invalid_nested_types<CharT>("bBcdoxX?")) {
        check_exception("The type option contains an invalid value for a character formatting argument", fmt, input);
    }

    // ***** Both have a format-spec
    check(SV("^^{:a, :b, :c}^^^"), SV("{:^^17::>2}"), input);
    check(SV("^^{:a, :b, :c}^^^"), SV("{:^^{}::>2}"), input, 17);
    check(SV("^^{:a, :b, :c}^^^"), SV("{:^^{}::>{}}"), input, 17, 2);

    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}::>2}"), input);
    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}::>{}}"), input, 17);
}

// A set can be written as a string, based on
//   [tab:formatter.range.type]
//   s  T shall be charT. ...
// This does not seem very useful, but it is allowed.
template <class CharT, class TestFunction, class ExceptionTest>
void test_char_string(TestFunction check, [[maybe_unused]] ExceptionTest check_exception) {
    set input{CharT('a'), CharT('c'), CharT('b')}; // input not sorted.

    check(SV("abc"), SV("{:s}"), input);

    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV("abc   "), SV("{:6s}"), input);
    check(SV("abc***"), SV("{:*<6s}"), input);
    check(SV("_abc__"), SV("{:_^6s}"), input);
    check(SV("###abc"), SV("{:#>6s}"), input);

    check(SV("abc   "), SV("{:{}s}"), input, 6);
    check(SV("abc***"), SV("{:*<{}s}"), input, 6);
    check(SV("_abc__"), SV("{:_^{}s}"), input, 6);
    check(SV("###abc"), SV("{:#>{}s}"), input, 6);

    check_exception("The format string contains an invalid escape sequence", SV("{:}<s}"), input);
    check_exception("The fill option contains an invalid value", SV("{:{<s}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:-s}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:+s}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{: s}"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:#s}"), input);

    // *** zero-padding ***
    check_exception("The width option should not have a leading zero", SV("{:0s}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:.s}"), input);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:Ls}"), input);

    // *** n
    check_exception("The n option and type s can't be used together", SV("{:ns}"), input);

    // *** type ***
    check_exception("Type m requires a pair or a tuple with two elements", SV("{:m}"), input);
    check_exception(
        "The type option contains an invalid value for a character formatting argument", SV("{::<s}"), input);

    // ***** Only underlying has a format-spec
    check_exception("Type s and an underlying format specification can't be used together", SV("{:s:}"), input);
    for (basic_string_view<CharT> fmt : fmt_invalid_nested_types<CharT>("bBcdoxX?")) {
        check_exception("The type option contains an invalid value for a character formatting argument", fmt, input);
    }

    // ***** Both have a format-spec
    check_exception("Type s and an underlying format specification can't be used together", SV("{:5s:5}"), input);
}

// A set can be written as a debug_string, based on
//   [tab:formatter.range.type]
//   ?s T shall be charT. ...
// This does not seem very useful, but it is allowed.
template <class CharT, class TestFunction, class ExceptionTest>
void test_char_escaped_string(TestFunction check, [[maybe_unused]] ExceptionTest check_exception) {
    set input{CharT('a'), CharT('c'), CharT('b')}; // input not sorted.

    check(SV("\"abc\""), SV("{:?s}"), input);

    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV(R"("abc"   )"), SV("{:8?s}"), input);
    check(SV(R"("abc"***)"), SV("{:*<8?s}"), input);
    check(SV(R"(_"abc"__)"), SV("{:_^8?s}"), input);
    check(SV(R"(###"abc")"), SV("{:#>8?s}"), input);

    check(SV(R"("abc"   )"), SV("{:{}?s}"), input, 8);
    check(SV(R"("abc"***)"), SV("{:*<{}?s}"), input, 8);
    check(SV(R"(_"abc"__)"), SV("{:_^{}?s}"), input, 8);
    check(SV(R"(###"abc")"), SV("{:#>{}?s}"), input, 8);

    check_exception("The format string contains an invalid escape sequence", SV("{:}<?s}"), input);
    check_exception("The fill option contains an invalid value", SV("{:{<?s}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::<?s}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:-?s}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:+?s}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{: ?s}"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:#?s}"), input);

    // *** zero-padding ***
    check_exception("The width option should not have a leading zero", SV("{:0?s}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:.?s}"), input);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:L?s}"), input);

    // *** n
    check_exception("The n option and type ?s can't be used together", SV("{:n?s}"), input);

    // *** type ***
    check_exception("Type m requires a pair or a tuple with two elements", SV("{:m}"), input);

    // ***** Only underlying has a format-spec
    check_exception("Type ?s and an underlying format specification can't be used together", SV("{:?s:}"), input);

    // ***** Both have a format-spec
    check_exception("Type ?s and an underlying format specification can't be used together", SV("{:5?s:5}"), input);
}

template <class CharT, class TestFunction, class ExceptionTest>
void test_char(TestFunction check, ExceptionTest check_exception) {
    test_char_default<CharT>(check, check_exception);
    test_char_string<CharT>(check, check_exception);
    test_char_escaped_string<CharT>(check, check_exception);
}

//
// char -> wchar_t
//

template <class TestFunction, class ExceptionTest>
void test_char_to_wchar(TestFunction check, ExceptionTest check_exception) {
    set input{'a', 'c', 'b'}; // input not sorted.

    using CharT = wchar_t;

    // Note when no range-underlying-spec is present the char is escaped,
    check(SV("{'a', 'b', 'c'}"), SV("{}"), input);
    check(SV("{'a', 'b', 'c'}^42"), SV("{}^42"), input);
    check(SV("{'a', 'b', 'c'}^42"), SV("{:}^42"), input);
    // when one is present there is no escaping,
    check(SV("{a, b, c}"), SV("{::}"), input);
    check(SV("{a, b, c}"), SV("{::<}"), input);
    // unless forced by the type specifier.
    check(SV("{'a', 'b', 'c'}"), SV("{::?}"), input);
    check(SV("{'a', 'b', 'c'}"), SV("{::<?}"), input);

    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV("{'a', 'b', 'c'}     "), SV("{:20}"), input);
    check(SV("{'a', 'b', 'c'}*****"), SV("{:*<20}"), input);
    check(SV("__{'a', 'b', 'c'}___"), SV("{:_^20}"), input);
    check(SV("#####{'a', 'b', 'c'}"), SV("{:#>20}"), input);

    check(SV("{'a', 'b', 'c'}     "), SV("{:{}}"), input, 20);
    check(SV("{'a', 'b', 'c'}*****"), SV("{:*<{}}"), input, 20);
    check(SV("__{'a', 'b', 'c'}___"), SV("{:_^{}}"), input, 20);
    check(SV("#####{'a', 'b', 'c'}"), SV("{:#>{}}"), input, 20);

    check_exception("The format string contains an invalid escape sequence", SV("{:}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{:{<}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:-}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:+}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{: }"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:#}"), input);

    // *** zero-padding ***
    check_exception("The width option should not have a leading zero", SV("{:0}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:.}"), input);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:L}"), input);

    // *** n
    check(SV("__'a', 'b', 'c'___"), SV("{:_^18n}"), input);

    // *** type ***
    check_exception("Type m requires a pair or a tuple with two elements", SV("{:m}"), input);

    // ***** Only underlying has a format-spec
    check(SV("{a   , b   , c   }"), SV("{::4}"), input);
    check(SV("{a***, b***, c***}"), SV("{::*<4}"), input);
    check(SV("{_a__, _b__, _c__}"), SV("{::_^4}"), input);
    check(SV("{:::a, :::b, :::c}"), SV("{:::>4}"), input);

    check(SV("{a   , b   , c   }"), SV("{::{}}"), input, 4);
    check(SV("{a***, b***, c***}"), SV("{::*<{}}"), input, 4);
    check(SV("{_a__, _b__, _c__}"), SV("{::_^{}}"), input, 4);
    check(SV("{:::a, :::b, :::c}"), SV("{:::>{}}"), input, 4);

    check_exception("The format string contains an invalid escape sequence", SV("{::}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{::{<}"), input);

    // *** sign ***
    check_exception("The format specifier for a character does not allow the sign option", SV("{::-}"), input);
    check_exception("The format specifier for a character does not allow the sign option", SV("{::+}"), input);
    check_exception("The format specifier for a character does not allow the sign option", SV("{:: }"), input);

    check(SV("{97, 98, 99}"), SV("{::-d}"), input);
    check(SV("{+97, +98, +99}"), SV("{::+d}"), input);
    check(SV("{ 97,  98,  99}"), SV("{:: d}"), input);

    // *** alternate form ***
    check_exception(
        "The format specifier for a character does not allow the alternate form option", SV("{::#}"), input);

    check(SV("{0x61, 0x62, 0x63}"), SV("{::#x}"), input);

    // *** zero-padding ***
    check_exception("The format specifier for a character does not allow the zero-padding option", SV("{::05}"), input);

    check(SV("{00141, 00142, 00143}"), SV("{::05o}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::.}"), input);

    // *** locale-specific form ***
    check(SV("{a, b, c}"), SV("{::L}"), input);

    // *** type ***
    for (basic_string_view<CharT> fmt : fmt_invalid_nested_types<CharT>("bBcdoxX?")) {
        check_exception("The type option contains an invalid value for a character formatting argument", fmt, input);
    }

    // ***** Both have a format-spec
    check(SV("^^{:a, :b, :c}^^^"), SV("{:^^17::>2}"), input);
    check(SV("^^{:a, :b, :c}^^^"), SV("{:^^{}::>2}"), input, 17);
    check(SV("^^{:a, :b, :c}^^^"), SV("{:^^{}::>{}}"), input, 17, 2);

    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}::>2}"), input);
    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}::>{}}"), input, 17);

    // The types s and ?s may only be used when using range_formatter<T, charT>
    // where the types T and charT are the same. This means this can't be used for
    // range_formatter<wchar_t, char> even when formatter<wchar_t, char> has a
    // debug-enabled specialization.

    using CharT = wchar_t;
    check_exception("Type s requires character type as formatting argument", SV("{:s}"), input);
    check_exception("Type ?s requires character type as formatting argument", SV("{:?s}"), input);
}

//
// Bool
//

template <class CharT, class TestFunction, class ExceptionTest>
void test_bool(TestFunction check, ExceptionTest check_exception) {
    set input{true, false};

    check(SV("{false, true}"), SV("{}"), input);
    check(SV("{false, true}^42"), SV("{}^42"), input);
    check(SV("{false, true}^42"), SV("{:}^42"), input);

    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV("{false, true}     "), SV("{:18}"), input);
    check(SV("{false, true}*****"), SV("{:*<18}"), input);
    check(SV("__{false, true}___"), SV("{:_^18}"), input);
    check(SV("#####{false, true}"), SV("{:#>18}"), input);

    check(SV("{false, true}     "), SV("{:{}}"), input, 18);
    check(SV("{false, true}*****"), SV("{:*<{}}"), input, 18);
    check(SV("__{false, true}___"), SV("{:_^{}}"), input, 18);
    check(SV("#####{false, true}"), SV("{:#>{}}"), input, 18);

    check_exception("The format string contains an invalid escape sequence", SV("{:}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{:{<}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:-}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:+}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{: }"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:#}"), input);

    // *** zero-padding ***
    check_exception("The width option should not have a leading zero", SV("{:0}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:.}"), input);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:L}"), input);

    // *** n
    check(SV("__false, true___"), SV("{:_^16n}"), input);

    // *** type ***
    check_exception("Type m requires a pair or a tuple with two elements", SV("{:m}"), input);
    check_exception("Type s requires character type as formatting argument", SV("{:s}"), input);
    check_exception("Type ?s requires character type as formatting argument", SV("{:?s}"), input);

    for (basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("s")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Only underlying has a format-spec
    check(SV("{false  , true   }"), SV("{::7}"), input);
    check(SV("{false**, true***}"), SV("{::*<7}"), input);
    check(SV("{_false_, _true__}"), SV("{::_^7}"), input);
    check(SV("{::false, :::true}"), SV("{:::>7}"), input);

    check(SV("{false  , true   }"), SV("{::{}}"), input, 7);
    check(SV("{false**, true***}"), SV("{::*<{}}"), input, 7);
    check(SV("{_false_, _true__}"), SV("{::_^{}}"), input, 7);
    check(SV("{::false, :::true}"), SV("{:::>{}}"), input, 7);

    check_exception("The format string contains an invalid escape sequence", SV("{::}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{::{<}"), input);

    // *** sign ***
    check_exception("The format specifier for a bool does not allow the sign option", SV("{::-}"), input);
    check_exception("The format specifier for a bool does not allow the sign option", SV("{::+}"), input);
    check_exception("The format specifier for a bool does not allow the sign option", SV("{:: }"), input);

    check(SV("{0, 1}"), SV("{::-d}"), input);
    check(SV("{+0, +1}"), SV("{::+d}"), input);
    check(SV("{ 0,  1}"), SV("{:: d}"), input);

    // *** alternate form ***
    check_exception("The format specifier for a bool does not allow the alternate form option", SV("{::#}"), input);

    check(SV("{0x0, 0x1}"), SV("{::#x}"), input);

    // *** zero-padding ***
    check_exception("The format specifier for a bool does not allow the zero-padding option", SV("{::05}"), input);

    check(SV("{00000, 00001}"), SV("{::05o}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::.}"), input);

    // *** locale-specific form ***
    check(SV("{false, true}"), SV("{::L}"), input);

    // *** type ***
    for (basic_string_view<CharT> fmt : fmt_invalid_nested_types<CharT>("bBdosxX")) {
        check_exception("The type option contains an invalid value for a bool formatting argument", fmt, input);
    }

    // ***** Both have a format-spec
    check(SV("^^{::false, :::true}^^^"), SV("{:^^23::>7}"), input);
    check(SV("^^{::false, :::true}^^^"), SV("{:^^{}::>7}"), input, 23);
    check(SV("^^{::false, :::true}^^^"), SV("{:^^{}::>{}}"), input, 23, 7);

    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}::>5}"), input);
    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}::>{}}"), input, 23);
}

template <class CharT, class TestFunction, class ExceptionTest>
void test_bool_multiset(TestFunction check, ExceptionTest check_exception) {
    multiset<bool, greater<bool>> input{true, false, true}; // unordered

    check(SV("{true, true, false}"), SV("{}"), input);
    check(SV("{true, true, false}^42"), SV("{}^42"), input);
    check(SV("{true, true, false}^42"), SV("{:}^42"), input);

    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV("{true, true, false}     "), SV("{:24}"), input);
    check(SV("{true, true, false}*****"), SV("{:*<24}"), input);
    check(SV("__{true, true, false}___"), SV("{:_^24}"), input);
    check(SV("#####{true, true, false}"), SV("{:#>24}"), input);

    check(SV("{true, true, false}     "), SV("{:{}}"), input, 24);
    check(SV("{true, true, false}*****"), SV("{:*<{}}"), input, 24);
    check(SV("__{true, true, false}___"), SV("{:_^{}}"), input, 24);
    check(SV("#####{true, true, false}"), SV("{:#>{}}"), input, 24);

    check_exception("The format string contains an invalid escape sequence", SV("{:}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{:{<}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:-}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:+}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{: }"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:#}"), input);

    // *** zero-padding ***
    check_exception("The width option should not have a leading zero", SV("{:0}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:.}"), input);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:L}"), input);

    // *** n
    check(SV("__true, true, false___"), SV("{:_^22n}"), input);

    // *** type ***
    check_exception("Type m requires a pair or a tuple with two elements", SV("{:m}"), input);
    check_exception("Type s requires character type as formatting argument", SV("{:s}"), input);
    check_exception("Type ?s requires character type as formatting argument", SV("{:?s}"), input);

    for (basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("s")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Only underlying has a format-spec
    check(SV("{true   , true   , false  }"), SV("{::7}"), input);
    check(SV("{true***, true***, false**}"), SV("{::*<7}"), input);
    check(SV("{_true__, _true__, _false_}"), SV("{::_^7}"), input);
    check(SV("{:::true, :::true, ::false}"), SV("{:::>7}"), input);

    check(SV("{true   , true   , false  }"), SV("{::{}}"), input, 7);
    check(SV("{true***, true***, false**}"), SV("{::*<{}}"), input, 7);
    check(SV("{_true__, _true__, _false_}"), SV("{::_^{}}"), input, 7);
    check(SV("{:::true, :::true, ::false}"), SV("{:::>{}}"), input, 7);

    check_exception("The format string contains an invalid escape sequence", SV("{::}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{::{<}"), input);

    // *** sign ***
    check_exception("The format specifier for a bool does not allow the sign option", SV("{::-}"), input);
    check_exception("The format specifier for a bool does not allow the sign option", SV("{::+}"), input);
    check_exception("The format specifier for a bool does not allow the sign option", SV("{:: }"), input);

    check(SV("{1, 1, 0}"), SV("{::-d}"), input);
    check(SV("{+1, +1, +0}"), SV("{::+d}"), input);
    check(SV("{ 1,  1,  0}"), SV("{:: d}"), input);

    // *** alternate form ***
    check_exception("The format specifier for a bool does not allow the alternate form option", SV("{::#}"), input);

    check(SV("{0x1, 0x1, 0x0}"), SV("{::#x}"), input);

    // *** zero-padding ***
    check_exception("The format specifier for a bool does not allow the zero-padding option", SV("{::05}"), input);

    check(SV("{00001, 00001, 00000}"), SV("{::05o}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::.}"), input);

    // *** locale-specific form ***
    check(SV("{true, true, false}"), SV("{::L}"), input);

    // *** type ***
    for (basic_string_view<CharT> fmt : fmt_invalid_nested_types<CharT>("bBdosxX")) {
        check_exception("The type option contains an invalid value for a bool formatting argument", fmt, input);
    }

    // ***** Both have a format-spec
    check(SV("^^{:::true, :::true, ::false}^^^"), SV("{:^^32::>7}"), input);
    check(SV("^^{:::true, :::true, ::false}^^^"), SV("{:^^{}::>7}"), input, 32);
    check(SV("^^{:::true, :::true, ::false}^^^"), SV("{:^^{}::>{}}"), input, 32, 7);

    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}::>5}"), input);
    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}::>{}}"), input, 32);
}

//
// Integral
//

template <class CharT, class TestFunction, class ExceptionTest>
void test_int(TestFunction check, ExceptionTest check_exception, auto&& input) {
    check(SV("{-42, 1, 2, 42}"), SV("{}"), input);
    check(SV("{-42, 1, 2, 42}^42"), SV("{}^42"), input);
    check(SV("{-42, 1, 2, 42}^42"), SV("{:}^42"), input);

    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV("{-42, 1, 2, 42}     "), SV("{:20}"), input);
    check(SV("{-42, 1, 2, 42}*****"), SV("{:*<20}"), input);
    check(SV("__{-42, 1, 2, 42}___"), SV("{:_^20}"), input);
    check(SV("#####{-42, 1, 2, 42}"), SV("{:#>20}"), input);

    check(SV("{-42, 1, 2, 42}     "), SV("{:{}}"), input, 20);
    check(SV("{-42, 1, 2, 42}*****"), SV("{:*<{}}"), input, 20);
    check(SV("__{-42, 1, 2, 42}___"), SV("{:_^{}}"), input, 20);
    check(SV("#####{-42, 1, 2, 42}"), SV("{:#>{}}"), input, 20);

    check_exception("The format string contains an invalid escape sequence", SV("{:}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{:{<}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:-}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:+}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{: }"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:#}"), input);

    // *** zero-padding ***
    check_exception("The width option should not have a leading zero", SV("{:0}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:.}"), input);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:L}"), input);

    // *** n
    check(SV("__-42, 1, 2, 42___"), SV("{:_^18n}"), input);

    // *** type ***
    check_exception("Type m requires a pair or a tuple with two elements", SV("{:m}"), input);
    check_exception("Type s requires character type as formatting argument", SV("{:s}"), input);
    check_exception("Type ?s requires character type as formatting argument", SV("{:?s}"), input);

    for (basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("s")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Only underlying has a format-spec
    check(SV("{  -42,     1,     2,    42}"), SV("{::5}"), input);
    check(SV("{-42**, 1****, 2****, 42***}"), SV("{::*<5}"), input);
    check(SV("{_-42_, __1__, __2__, _42__}"), SV("{::_^5}"), input);
    check(SV("{::-42, ::::1, ::::2, :::42}"), SV("{:::>5}"), input);

    check(SV("{  -42,     1,     2,    42}"), SV("{::{}}"), input, 5);
    check(SV("{-42**, 1****, 2****, 42***}"), SV("{::*<{}}"), input, 5);
    check(SV("{_-42_, __1__, __2__, _42__}"), SV("{::_^{}}"), input, 5);
    check(SV("{::-42, ::::1, ::::2, :::42}"), SV("{:::>{}}"), input, 5);

    check_exception("The format string contains an invalid escape sequence", SV("{::}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{::{<}"), input);

    // *** sign ***
    check(SV("{-42, 1, 2, 42}"), SV("{::-}"), input);
    check(SV("{-42, +1, +2, +42}"), SV("{::+}"), input);
    check(SV("{-42,  1,  2,  42}"), SV("{:: }"), input);

    // *** alternate form ***
    check(SV("{-0x2a, 0x1, 0x2, 0x2a}"), SV("{::#x}"), input);

    // *** zero-padding ***
    check(SV("{-0042, 00001, 00002, 00042}"), SV("{::05}"), input);
    check(SV("{-002a, 00001, 00002, 0002a}"), SV("{::05x}"), input);
    check(SV("{-0x2a, 0x001, 0x002, 0x02a}"), SV("{::#05x}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::.}"), input);

    // *** locale-specific form ***
    check(SV("{-42, 1, 2, 42}"), SV("{::L}"), input); // does nothing in this test, but is accepted.

    // *** type ***
    for (basic_string_view<CharT> fmt : fmt_invalid_nested_types<CharT>("bBcdoxX")) {
        check_exception("The type option contains an invalid value for an integer formatting argument", fmt, input);
    }

    // ***** Both have a format-spec
    check(SV("^^{::-42, ::::1, ::::2, :::42}^^^"), SV("{:^^33::>5}"), input);
    check(SV("^^{::-42, ::::1, ::::2, :::42}^^^"), SV("{:^^{}::>5}"), input, 33);
    check(SV("^^{::-42, ::::1, ::::2, :::42}^^^"), SV("{:^^{}::>{}}"), input, 33, 5);

    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}::>5}"), input);
    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}::>{}}"), input, 33);
}

template <class CharT, class TestFunction, class ExceptionTest>
void test_int(TestFunction check, ExceptionTest check_exception) {
    test_int<CharT>(check, check_exception, set{1, 42, 2, -42}); // unsorted
    test_int<CharT>(check, check_exception, multiset{1, 42, 2, -42}); // unsorted
}

//
// Floating point
//

template <class CharT, class TestFunction, class ExceptionTest>
void test_floating_point(TestFunction check, ExceptionTest check_exception, auto&& input) {
    check(SV("{-42.5, 0, 1.25, 42.5}"), SV("{}"), input);
    check(SV("{-42.5, 0, 1.25, 42.5}^42"), SV("{}^42"), input);
    check(SV("{-42.5, 0, 1.25, 42.5}^42"), SV("{:}^42"), input);

    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV("{-42.5, 0, 1.25, 42.5}     "), SV("{:27}"), input);
    check(SV("{-42.5, 0, 1.25, 42.5}*****"), SV("{:*<27}"), input);
    check(SV("__{-42.5, 0, 1.25, 42.5}___"), SV("{:_^27}"), input);
    check(SV("#####{-42.5, 0, 1.25, 42.5}"), SV("{:#>27}"), input);

    check(SV("{-42.5, 0, 1.25, 42.5}     "), SV("{:{}}"), input, 27);
    check(SV("{-42.5, 0, 1.25, 42.5}*****"), SV("{:*<{}}"), input, 27);
    check(SV("__{-42.5, 0, 1.25, 42.5}___"), SV("{:_^{}}"), input, 27);
    check(SV("#####{-42.5, 0, 1.25, 42.5}"), SV("{:#>{}}"), input, 27);

    check_exception("The format string contains an invalid escape sequence", SV("{:}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{:{<}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:-}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:+}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{: }"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:#}"), input);

    // *** zero-padding ***
    check_exception("The width option should not have a leading zero", SV("{:0}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:.}"), input);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:L}"), input);

    // *** n
    check(SV("__-42.5, 0, 1.25, 42.5___"), SV("{:_^25n}"), input);

    // *** type ***
    check_exception("Type m requires a pair or a tuple with two elements", SV("{:m}"), input);
    check_exception("Type s requires character type as formatting argument", SV("{:s}"), input);
    check_exception("Type ?s requires character type as formatting argument", SV("{:?s}"), input);

    for (basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("s")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Only underlying has a format-spec
    check(SV("{-42.5,     0,  1.25,  42.5}"), SV("{::5}"), input);
    check(SV("{-42.5, 0****, 1.25*, 42.5*}"), SV("{::*<5}"), input);
    check(SV("{-42.5, __0__, 1.25_, 42.5_}"), SV("{::_^5}"), input);
    check(SV("{-42.5, ::::0, :1.25, :42.5}"), SV("{:::>5}"), input);

    check(SV("{-42.5,     0,  1.25,  42.5}"), SV("{::{}}"), input, 5);
    check(SV("{-42.5, 0****, 1.25*, 42.5*}"), SV("{::*<{}}"), input, 5);
    check(SV("{-42.5, __0__, 1.25_, 42.5_}"), SV("{::_^{}}"), input, 5);
    check(SV("{-42.5, ::::0, :1.25, :42.5}"), SV("{:::>{}}"), input, 5);

    check_exception("The format string contains an invalid escape sequence", SV("{::}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{::{<}"), input);

    // *** sign ***
    check(SV("{-42.5, 0, 1.25, 42.5}"), SV("{::-}"), input);
    check(SV("{-42.5, +0, +1.25, +42.5}"), SV("{::+}"), input);
    check(SV("{-42.5,  0,  1.25,  42.5}"), SV("{:: }"), input);

    // *** alternate form ***
    check(SV("{-42.5, 0., 1.25, 42.5}"), SV("{::#}"), input);

    // *** zero-padding ***
    check(SV("{-42.5, 00000, 01.25, 042.5}"), SV("{::05}"), input);
    check(SV("{-42.5, 0000., 01.25, 042.5}"), SV("{::#05}"), input);

    // *** precision ***
    check(SV("{-42, 0, 1.2, 42}"), SV("{::.2}"), input);
    check(SV("{-42.500, 0.000, 1.250, 42.500}"), SV("{::.3f}"), input);

    check(SV("{-42, 0, 1.2, 42}"), SV("{::.{}}"), input, 2);
    check(SV("{-42.500, 0.000, 1.250, 42.500}"), SV("{::.{}f}"), input, 3);

    check_exception("The precision option does not contain a value or an argument index", SV("{::.}"), input);

    // *** locale-specific form ***
    check(SV("{-42.5, 0, 1.25, 42.5}"), SV("{::L}"), input); // does not require locales present

    // *** type ***
    for (basic_string_view<CharT> fmt : fmt_invalid_nested_types<CharT>("aAeEfFgG")) {
        check_exception(
            "The type option contains an invalid value for a floating-point formatting argument", fmt, input);
    }

    // ***** Both have a format-spec
    check(SV("^^{-42.5, ::::0, :1.25, :42.5}^^^"), SV("{:^^33::>5}"), input);
    check(SV("^^{-42.5, ::::0, :1.25, :42.5}^^^"), SV("{:^^{}::>5}"), input, 33);
    check(SV("^^{-42.5, ::::0, :1.25, :42.5}^^^"), SV("{:^^{}::>{}}"), input, 33, 5);

    check(SV("^^{::-42, ::::0, ::1.2, :::42}^^^"), SV("{:^^33::>5.2}"), input);
    check(SV("^^{::-42, ::::0, ::1.2, :::42}^^^"), SV("{:^^{}::>5.2}"), input, 33);
    check(SV("^^{::-42, ::::0, ::1.2, :::42}^^^"), SV("{:^^{}::>{}.2}"), input, 33, 5);
    check(SV("^^{::-42, ::::0, ::1.2, :::42}^^^"), SV("{:^^{}::>{}.{}}"), input, 33, 5, 2);

    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}::>5.2}"), input);
    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}::>{}.2}"), input, 33);
    check_exception("The argument index value is too large for the number of arguments supplied", SV("{:^^{}::>{}.{}}"),
        input, 33, 5);
}

template <class CharT, class TestFunction, class ExceptionTest>
void test_floating_point(TestFunction check, ExceptionTest check_exception) {
    test_floating_point<CharT>(check, check_exception, set{-42.5f, 0.0f, 1.25f, 42.5f});
    test_floating_point<CharT>(check, check_exception, multiset{-42.5, 0.0, 1.25, 42.5});
    test_floating_point<CharT>(check, check_exception, set{-42.5l, 0.0l, 1.25l, 42.5l});
}

//
// Pointer
//

template <class CharT, class TestFunction, class ExceptionTest>
void test_pointer(TestFunction check, ExceptionTest check_exception, auto&& input) {
    check(SV("{0x0}"), SV("{}"), input);
    check(SV("{0x0}^42"), SV("{}^42"), input);
    check(SV("{0x0}^42"), SV("{:}^42"), input);

    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV("{0x0}     "), SV("{:10}"), input);
    check(SV("{0x0}*****"), SV("{:*<10}"), input);
    check(SV("__{0x0}___"), SV("{:_^10}"), input);
    check(SV("#####{0x0}"), SV("{:#>10}"), input);

    check(SV("{0x0}     "), SV("{:{}}"), input, 10);
    check(SV("{0x0}*****"), SV("{:*<{}}"), input, 10);
    check(SV("__{0x0}___"), SV("{:_^{}}"), input, 10);
    check(SV("#####{0x0}"), SV("{:#>{}}"), input, 10);

    check_exception("The format string contains an invalid escape sequence", SV("{:}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{:{<}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:#}"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:#}"), input);

    // *** zero-padding ***
    check_exception("The width option should not have a leading zero", SV("{:0}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:.}"), input);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:L}"), input);

    // *** n
    check(SV("_0x0_"), SV("{:_^5n}"), input);

    // *** type ***
    check_exception("Type m requires a pair or a tuple with two elements", SV("{:m}"), input);
    check_exception("Type s requires character type as formatting argument", SV("{:s}"), input);
    check_exception("Type ?s requires character type as formatting argument", SV("{:?s}"), input);

    for (basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("s")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Only underlying has a format-spec
    check(SV("{  0x0}"), SV("{::5}"), input);
    check(SV("{0x0**}"), SV("{::*<5}"), input);
    check(SV("{_0x0_}"), SV("{::_^5}"), input);
    check(SV("{::0x0}"), SV("{:::>5}"), input);

    check(SV("{  0x0}"), SV("{::{}}"), input, 5);
    check(SV("{0x0**}"), SV("{::*<{}}"), input, 5);
    check(SV("{_0x0_}"), SV("{::_^{}}"), input, 5);
    check(SV("{::0x0}"), SV("{:::>{}}"), input, 5);

    check_exception("The format string contains an invalid escape sequence", SV("{::}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{::{<}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::-}"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::#}"), input);

    // *** zero-padding ***
    check(SV("{0x0000}"), SV("{::06}"), input);
    check(SV("{0x0000}"), SV("{::06p}"), input);
    check(SV("{0X0000}"), SV("{::06P}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::.}"), input);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::L}"), input);

    // *** type ***
    for (basic_string_view<CharT> fmt : fmt_invalid_nested_types<CharT>("pP")) {
        check_exception("The type option contains an invalid value for a pointer formatting argument", fmt, input);
    }

    // ***** Both have a format-spec
    check(SV("^^{::0x0}^^^"), SV("{:^^12::>5}"), input);
    check(SV("^^{::0x0}^^^"), SV("{:^^{}::>5}"), input, 12);
    check(SV("^^{::0x0}^^^"), SV("{:^^{}::>{}}"), input, 12, 5);

    check(SV("^^{::0x0}^^^"), SV("{:^^12::>5}"), input);
    check(SV("^^{::0x0}^^^"), SV("{:^^{}::>5}"), input, 12);
    check(SV("^^{::0x0}^^^"), SV("{:^^{}::>{}}"), input, 12, 5);

    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}::>5}"), input);
    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}::>{}}"), input, 12);
}

template <class CharT, class TestFunction, class ExceptionTest>
void test_pointer(TestFunction check, ExceptionTest check_exception) {
    // Note nullptr_t can only be equality compared so not used in a set.
    test_pointer<CharT>(check, check_exception, unordered_set{static_cast<const void*>(nullptr)});
    test_pointer<CharT>(check, check_exception, unordered_multiset{static_cast<void*>(nullptr)});
}

//
// String
//

template <class CharT, class TestFunction, class ExceptionTest>
void test_string(TestFunction check, ExceptionTest check_exception, auto&& input) {
    check(SV(R"({"Hello", "world"})"), SV("{}"), input);
    check(SV(R"({"Hello", "world"}^42)"), SV("{}^42"), input);
    check(SV(R"({"Hello", "world"}^42)"), SV("{:}^42"), input);

    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV(R"({"Hello", "world"}     )"), SV("{:23}"), input);
    check(SV(R"({"Hello", "world"}*****)"), SV("{:*<23}"), input);
    check(SV(R"(__{"Hello", "world"}___)"), SV("{:_^23}"), input);
    check(SV(R"(#####{"Hello", "world"})"), SV("{:#>23}"), input);

    check(SV(R"({"Hello", "world"}     )"), SV("{:{}}"), input, 23);
    check(SV(R"({"Hello", "world"}*****)"), SV("{:*<{}}"), input, 23);
    check(SV(R"(__{"Hello", "world"}___)"), SV("{:_^{}}"), input, 23);
    check(SV(R"(#####{"Hello", "world"})"), SV("{:#>{}}"), input, 23);

    check_exception("The format string contains an invalid escape sequence", SV("{:}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{:{<}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:#}"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:#}"), input);

    // *** zero-padding ***
    check_exception("The width option should not have a leading zero", SV("{:0}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:.}"), input);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:L}"), input);

    // *** n
    check(SV(R"(_"Hello", "world"_)"), SV("{:_^18n}"), input);

    // *** type ***
    check_exception("Type m requires a pair or a tuple with two elements", SV("{:m}"), input);
    check_exception("Type s requires character type as formatting argument", SV("{:s}"), input);
    check_exception("Type ?s requires character type as formatting argument", SV("{:?s}"), input);

    for (basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("s")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Only underlying has a format-spec
    check(SV(R"({Hello   , world   })"), SV("{::8}"), input);
    check(SV(R"({Hello***, world***})"), SV("{::*<8}"), input);
    check(SV(R"({_Hello__, _world__})"), SV("{::_^8}"), input);
    check(SV(R"({:::Hello, :::world})"), SV("{:::>8}"), input);

    check(SV(R"({Hello   , world   })"), SV("{::{}}"), input, 8);
    check(SV(R"({Hello***, world***})"), SV("{::*<{}}"), input, 8);
    check(SV(R"({_Hello__, _world__})"), SV("{::_^{}}"), input, 8);
    check(SV(R"({:::Hello, :::world})"), SV("{:::>{}}"), input, 8);

    check_exception("The format string contains an invalid escape sequence", SV("{::}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{::{<}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::-}"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::#}"), input);

    // *** zero-padding ***
    check_exception("The width option should not have a leading zero", SV("{::05}"), input);

    // *** precision ***
    check(SV(R"({Hel, wor})"), SV("{::.3}"), input);

    check(SV(R"({Hel, wor})"), SV("{::.{}}"), input, 3);

    check_exception("The precision option does not contain a value or an argument index", SV("{::.}"), input);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::L}"), input);

    // *** type ***
    for (basic_string_view<CharT> fmt : fmt_invalid_nested_types<CharT>("s?")) {
        check_exception("The type option contains an invalid value for a string formatting argument", fmt, input);
    }

    // ***** Both have a format-spec
    check(SV(R"(^^{:::Hello, :::world}^^^)"), SV("{:^^25::>8}"), input);
    check(SV(R"(^^{:::Hello, :::world}^^^)"), SV("{:^^{}::>8}"), input, 25);
    check(SV(R"(^^{:::Hello, :::world}^^^)"), SV("{:^^{}::>{}}"), input, 25, 8);

    check(SV(R"(^^{:::Hello, :::world}^^^)"), SV("{:^^25::>8}"), input);
    check(SV(R"(^^{:::Hello, :::world}^^^)"), SV("{:^^{}::>8}"), input, 25);
    check(SV(R"(^^{:::Hello, :::world}^^^)"), SV("{:^^{}::>{}}"), input, 25, 8);

    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}::>8}"), input);
    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}::>{}}"), input, 25);
}

template <class CharT, class TestFunction, class ExceptionTest>
void test_string(TestFunction check, ExceptionTest check_exception) {
    test_string<CharT>(check, check_exception, set{STR("Hello"), STR("world")});
    test_string<CharT>(check, check_exception, set{SV("Hello"), SV("world")});
}

//
// Handle
//

template <class CharT, class TestFunction, class ExceptionTest>
void test_status(TestFunction check, ExceptionTest check_exception) {
    set input{status::foo, status::bar, status::foobar}; // unordered input

    check(SV("{0x5555, 0xaa55, 0xaaaa}"), SV("{}"), input);
    check(SV("{0x5555, 0xaa55, 0xaaaa}^42"), SV("{}^42"), input);
    check(SV("{0x5555, 0xaa55, 0xaaaa}^42"), SV("{:}^42"), input);

    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV("{0x5555, 0xaa55, 0xaaaa}     "), SV("{:29}"), input);
    check(SV("{0x5555, 0xaa55, 0xaaaa}*****"), SV("{:*<29}"), input);
    check(SV("__{0x5555, 0xaa55, 0xaaaa}___"), SV("{:_^29}"), input);
    check(SV("#####{0x5555, 0xaa55, 0xaaaa}"), SV("{:#>29}"), input);

    check(SV("{0x5555, 0xaa55, 0xaaaa}     "), SV("{:{}}"), input, 29);
    check(SV("{0x5555, 0xaa55, 0xaaaa}*****"), SV("{:*<{}}"), input, 29);
    check(SV("__{0x5555, 0xaa55, 0xaaaa}___"), SV("{:_^{}}"), input, 29);
    check(SV("#####{0x5555, 0xaa55, 0xaaaa}"), SV("{:#>{}}"), input, 29);

    check_exception("The format string contains an invalid escape sequence", SV("{:}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{:{<}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:-}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:+}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{: }"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:#}"), input);

    // *** zero-padding ***
    check_exception("The width option should not have a leading zero", SV("{:0}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:.}"), input);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:L}"), input);

    // *** n
    check(SV("__0x5555, 0xaa55, 0xaaaa___"), SV("{:_^27n}"), input);

    // *** type ***
    check_exception("Type m requires a pair or a tuple with two elements", SV("{:m}"), input);
    check_exception("Type s requires character type as formatting argument", SV("{:s}"), input);
    check_exception("Type ?s requires character type as formatting argument", SV("{:?s}"), input);

    for (basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("s")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Only underlying has a format-spec
    check_exception("The type option contains an invalid value for a status formatting argument", SV("{::*<7}"), input);

    check(SV("{0x5555, 0xaa55, 0xaaaa}"), SV("{::x}"), input);
    check(SV("{0X5555, 0XAA55, 0XAAAA}"), SV("{::X}"), input);
    check(SV("{bar, foobar, foo}"), SV("{::s}"), input);

    // ***** Both have a format-spec
    check(SV("^^{0X5555, 0XAA55, 0XAAAA}^^^"), SV("{:^^29:X}"), input);
    check(SV("^^{0X5555, 0XAA55, 0XAAAA}^^^"), SV("{:^^{}:X}"), input, 29);

    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}:X}"), input);
}

//
// Pair
//

template <class CharT, class TestFunction, class ExceptionTest>
void test_pair_tuple(TestFunction check, ExceptionTest check_exception, auto&& input) {
    check(SV("{(1, 'a'), (42, '*')}"), SV("{}"), input);
    check(SV("{(1, 'a'), (42, '*')}^42"), SV("{}^42"), input);
    check(SV("{(1, 'a'), (42, '*')}^42"), SV("{:}^42"), input);

    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV("{(1, 'a'), (42, '*')}     "), SV("{:26}"), input);
    check(SV("{(1, 'a'), (42, '*')}*****"), SV("{:*<26}"), input);
    check(SV("__{(1, 'a'), (42, '*')}___"), SV("{:_^26}"), input);
    check(SV("#####{(1, 'a'), (42, '*')}"), SV("{:#>26}"), input);

    check(SV("{(1, 'a'), (42, '*')}     "), SV("{:{}}"), input, 26);
    check(SV("{(1, 'a'), (42, '*')}*****"), SV("{:*<{}}"), input, 26);
    check(SV("__{(1, 'a'), (42, '*')}___"), SV("{:_^{}}"), input, 26);
    check(SV("#####{(1, 'a'), (42, '*')}"), SV("{:#>{}}"), input, 26);

    check_exception("The format string contains an invalid escape sequence", SV("{:}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{:{<}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:-}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:+}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{: }"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:#}"), input);

    // *** zero-padding ***
    check_exception("The width option should not have a leading zero", SV("{:0}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:.}"), input);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:L}"), input);

    // *** n
    check(SV("__(1, 'a'), (42, '*')___"), SV("{:_^24n}"), input);
    check(SV("____1: 'a', 42: '*'_____"), SV("{:_^24nm}"), input);

    // *** type ***
    check(SV("____{1: 'a', 42: '*'}_____"), SV("{:_^26m}"), input);
    check_exception("Type s requires character type as formatting argument", SV("{:s}"), input);
    check_exception("Type ?s requires character type as formatting argument", SV("{:?s}"), input);

    for (basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("s")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Only underlying has a format-spec
    check(SV("{(1, 'a')   , (42, '*')  }"), SV("{::11}"), input);
    check(SV("{(1, 'a')***, (42, '*')**}"), SV("{::*<11}"), input);
    check(SV("{_(1, 'a')__, _(42, '*')_}"), SV("{::_^11}"), input);
    check(SV("{###(1, 'a'), ##(42, '*')}"), SV("{::#>11}"), input);

    check(SV("{(1, 'a')   , (42, '*')  }"), SV("{::{}}"), input, 11);
    check(SV("{(1, 'a')***, (42, '*')**}"), SV("{::*<{}}"), input, 11);
    check(SV("{_(1, 'a')__, _(42, '*')_}"), SV("{::_^{}}"), input, 11);
    check(SV("{###(1, 'a'), ##(42, '*')}"), SV("{::#>{}}"), input, 11);

    check_exception("The format string contains an invalid escape sequence", SV("{::}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{::{<}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::-}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::+}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:: }"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::#}"), input);

    // *** zero-padding ***
    check_exception("The width option should not have a leading zero", SV("{::05}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::.}"), input);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::L}"), input);

    // *** type ***
    check(SV("{1: 'a', 42: '*'}"), SV("{::m}"), input);
    check(SV("{1, 'a', 42, '*'}"), SV("{::n}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::s}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::?s}"), input);

    for (basic_string_view<CharT> fmt : fmt_invalid_nested_types<CharT>("s")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Both have a format-spec
    check(SV("^^{###(1, 'a'), ##(42, '*')}^^^"), SV("{:^^31:#>11}"), input);
    check(SV("^^{###(1, 'a'), ##(42, '*')}^^^"), SV("{:^^31:#>11}"), input);
    check(SV("^^{###(1, 'a'), ##(42, '*')}^^^"), SV("{:^^{}:#>11}"), input, 31);
    check(SV("^^{###(1, 'a'), ##(42, '*')}^^^"), SV("{:^^{}:#>{}}"), input, 31, 11);

    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}:#>5}"), input);
    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}:#>{}}"), input, 31);

    check(SV("1: 'a', 42: '*'"), SV("{:n:m}"), input);
    check(SV("1, 'a', 42, '*'"), SV("{:n:n}"), input);
    check(SV("{1: 'a', 42: '*'}"), SV("{:m:m}"), input);
    check(SV("{1: 'a', 42: '*'}"), SV("{:m:n}"), input);
}

template <class CharT, class TestFunction, class ExceptionTest>
void test_pair_tuple(TestFunction check, ExceptionTest check_exception) {
    test_pair_tuple<CharT>(check, check_exception, set{make_pair(1, CharT('a')), make_pair(42, CharT('*'))});
    test_pair_tuple<CharT>(check, check_exception, set{make_tuple(1, CharT('a')), make_tuple(42, CharT('*'))});
}

//
// Tuple 1
//

template <class CharT, class TestFunction, class ExceptionTest>
void test_tuple_int(TestFunction check, ExceptionTest check_exception) {
    set input{make_tuple(42), make_tuple(99)};

    check(SV("{(42), (99)}"), SV("{}"), input);
    check(SV("{(42), (99)}^42"), SV("{}^42"), input);
    check(SV("{(42), (99)}^42"), SV("{:}^42"), input);

    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV("{(42), (99)}     "), SV("{:17}"), input);
    check(SV("{(42), (99)}*****"), SV("{:*<17}"), input);
    check(SV("__{(42), (99)}___"), SV("{:_^17}"), input);
    check(SV("#####{(42), (99)}"), SV("{:#>17}"), input);

    check(SV("{(42), (99)}     "), SV("{:{}}"), input, 17);
    check(SV("{(42), (99)}*****"), SV("{:*<{}}"), input, 17);
    check(SV("__{(42), (99)}___"), SV("{:_^{}}"), input, 17);
    check(SV("#####{(42), (99)}"), SV("{:#>{}}"), input, 17);

    check_exception("The format string contains an invalid escape sequence", SV("{:}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{:{<}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:-}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:+}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{: }"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:#}"), input);

    // *** zero-padding ***
    check_exception("The width option should not have a leading zero", SV("{:0}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:.}"), input);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:L}"), input);

    // *** n
    check(SV("__(42), (99)___"), SV("{:_^15n}"), input);

    // *** type ***
    check_exception("Type m requires a pair or a tuple with two elements", SV("{:m}"), input);
    check_exception("Type s requires character type as formatting argument", SV("{:s}"), input);
    check_exception("Type ?s requires character type as formatting argument", SV("{:?s}"), input);

    for (basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("s")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Only underlying has a format-spec
    check(SV("{(42)   , (99)   }"), SV("{::7}"), input);
    check(SV("{(42)***, (99)***}"), SV("{::*<7}"), input);
    check(SV("{_(42)__, _(99)__}"), SV("{::_^7}"), input);
    check(SV("{###(42), ###(99)}"), SV("{::#>7}"), input);

    check(SV("{(42)   , (99)   }"), SV("{::{}}"), input, 7);
    check(SV("{(42)***, (99)***}"), SV("{::*<{}}"), input, 7);
    check(SV("{_(42)__, _(99)__}"), SV("{::_^{}}"), input, 7);
    check(SV("{###(42), ###(99)}"), SV("{::#>{}}"), input, 7);

    check_exception("The format string contains an invalid escape sequence", SV("{::}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{::{<}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::-}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::+}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:: }"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::#}"), input);

    // *** zero-padding ***
    check_exception("The width option should not have a leading zero", SV("{::05}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::.}"), input);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::L}"), input);

    // *** type ***
    check(SV("{42, 99}"), SV("{::n}"), input);
    check_exception("Type m requires a pair or a tuple with two elements", SV("{::m}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::s}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::?s}"), input);

    for (basic_string_view<CharT> fmt : fmt_invalid_nested_types<CharT>("s")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Both have a format-spec
    check(SV("^^{###(42), ###(99)}^^^"), SV("{:^^23:#>7}"), input);
    check(SV("^^{###(42), ###(99)}^^^"), SV("{:^^23:#>7}"), input);
    check(SV("^^{###(42), ###(99)}^^^"), SV("{:^^{}:#>7}"), input, 23);
    check(SV("^^{###(42), ###(99)}^^^"), SV("{:^^{}:#>{}}"), input, 23, 7);

    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}:#>5}"), input);
    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}:#>{}}"), input, 23);
}

//
// Tuple 3
//

template <class CharT, class TestFunction, class ExceptionTest>
void test_tuple_int_int_int(TestFunction check, ExceptionTest check_exception) {
    set input{make_tuple(42, 99, 0), make_tuple(1, 10, 100)}; // unordered

    check(SV("{(1, 10, 100), (42, 99, 0)}"), SV("{}"), input);
    check(SV("{(1, 10, 100), (42, 99, 0)}^42"), SV("{}^42"), input);
    check(SV("{(1, 10, 100), (42, 99, 0)}^42"), SV("{:}^42"), input);

    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV("{(1, 10, 100), (42, 99, 0)}     "), SV("{:32}"), input);
    check(SV("{(1, 10, 100), (42, 99, 0)}*****"), SV("{:*<32}"), input);
    check(SV("__{(1, 10, 100), (42, 99, 0)}___"), SV("{:_^32}"), input);
    check(SV("#####{(1, 10, 100), (42, 99, 0)}"), SV("{:#>32}"), input);

    check(SV("{(1, 10, 100), (42, 99, 0)}     "), SV("{:{}}"), input, 32);
    check(SV("{(1, 10, 100), (42, 99, 0)}*****"), SV("{:*<{}}"), input, 32);
    check(SV("__{(1, 10, 100), (42, 99, 0)}___"), SV("{:_^{}}"), input, 32);
    check(SV("#####{(1, 10, 100), (42, 99, 0)}"), SV("{:#>{}}"), input, 32);

    check_exception("The format string contains an invalid escape sequence", SV("{:}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{:{<}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:-}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:+}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{: }"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:#}"), input);

    // *** zero-padding ***
    check_exception("The width option should not have a leading zero", SV("{:0}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:.}"), input);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:L}"), input);

    // *** n
    check(SV("__(1, 10, 100), (42, 99, 0)___"), SV("{:_^30n}"), input);

    // *** type ***
    check_exception("Type m requires a pair or a tuple with two elements", SV("{:m}"), input);
    check_exception("Type s requires character type as formatting argument", SV("{:s}"), input);
    check_exception("Type ?s requires character type as formatting argument", SV("{:?s}"), input);

    for (basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("s")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Only underlying has a format-spec
    check(SV("{(1, 10, 100)  , (42, 99, 0)   }"), SV("{::14}"), input);
    check(SV("{(1, 10, 100)**, (42, 99, 0)***}"), SV("{::*<14}"), input);
    check(SV("{_(1, 10, 100)_, _(42, 99, 0)__}"), SV("{::_^14}"), input);
    check(SV("{##(1, 10, 100), ###(42, 99, 0)}"), SV("{::#>14}"), input);

    check(SV("{(1, 10, 100)  , (42, 99, 0)   }"), SV("{::{}}"), input, 14);
    check(SV("{(1, 10, 100)**, (42, 99, 0)***}"), SV("{::*<{}}"), input, 14);
    check(SV("{_(1, 10, 100)_, _(42, 99, 0)__}"), SV("{::_^{}}"), input, 14);
    check(SV("{##(1, 10, 100), ###(42, 99, 0)}"), SV("{::#>{}}"), input, 14);

    check_exception("The format string contains an invalid escape sequence", SV("{::}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{::{<}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::-}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::+}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:: }"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::#}"), input);

    // *** zero-padding ***
    check_exception("The width option should not have a leading zero", SV("{::05}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::.}"), input);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::L}"), input);

    // *** type ***
    check(SV("{1, 10, 100, 42, 99, 0}"), SV("{::n}"), input);
    check_exception("Type m requires a pair or a tuple with two elements", SV("{::m}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::s}"), input);
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::?s}"), input);

    for (basic_string_view<CharT> fmt : fmt_invalid_nested_types<CharT>("s")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Both have a format-spec
    check(SV("^^{##(1, 10, 100), ###(42, 99, 0)}^^^"), SV("{:^^37:#>14}"), input);
    check(SV("^^{##(1, 10, 100), ###(42, 99, 0)}^^^"), SV("{:^^37:#>14}"), input);
    check(SV("^^{##(1, 10, 100), ###(42, 99, 0)}^^^"), SV("{:^^{}:#>14}"), input, 37);
    check(SV("^^{##(1, 10, 100), ###(42, 99, 0)}^^^"), SV("{:^^{}:#>{}}"), input, 37, 14);

    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}:#>5}"), input);
    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}:#>{}}"), input, 37);
}

//
// Adaptor
//

class adaptor {
    using adaptee = set<int>;

public:
    using key_type = adaptee::key_type;
    using iterator = adaptee::iterator;

    iterator begin() {
        return data_.begin();
    }
    iterator end() {
        return data_.end();
    }

    explicit adaptor(set<int>&& data) : data_(move(data)) {}

private:
    adaptee data_;
};

static_assert(format_kind<adaptor> == range_format::set);

template <class CharT, class TestFunction, class ExceptionTest>
void test_adaptor(TestFunction check, ExceptionTest check_exception) {
    test_int<CharT>(check, check_exception, adaptor{set{1, 42, 2, -42}});
}

//
// Driver
//

template <class CharT, class TestFunction, class ExceptionTest>
void format_tests(TestFunction check, ExceptionTest check_exception) {
    test_char<CharT>(check, check_exception);

    if (same_as<CharT, wchar_t>) { // avoid testing twice
        test_char_to_wchar(check, check_exception);
    }
    test_bool<CharT>(check, check_exception);
    test_bool_multiset<CharT>(check, check_exception);
    test_int<CharT>(check, check_exception);
    test_floating_point<CharT>(check, check_exception);
    test_pointer<CharT>(check, check_exception);
    test_string<CharT>(check, check_exception);

    test_status<CharT>(check, check_exception); // Has its own handler with its own parser

    test_pair_tuple<CharT>(check, check_exception);
    test_tuple_int<CharT>(check, check_exception);
    test_tuple_int_int_int<CharT>(check, check_exception);

    test_adaptor<CharT>(check, check_exception);
}

auto test_format = []<class CharT, class... Args>(basic_string_view<CharT> expected,
                       type_identity_t<basic_format_string<CharT, Args...>> fmt, Args&&... args) {
    basic_string<CharT> out = format(fmt, forward<Args>(args)...);
    assert(out == expected);
};

auto test_format_exception = []<class CharT, class... Args>(string_view, basic_string_view<CharT>, Args&&...) {
    // After P2216 most exceptions thrown by format become ill-formed.
    // Therefore this test does nothing.
};

auto test_vformat = []<class CharT, class... Args>(
                        basic_string_view<CharT> expected, basic_string_view<CharT> fmt, Args&&... args) {
    basic_string<CharT> out = vformat(fmt, make_format_args<context_t<CharT>>(args...));
    assert(out == expected);
};

auto test_vformat_exception = []<class CharT, class... Args>([[maybe_unused]] string_view what,
                                  [[maybe_unused]] basic_string_view<CharT> fmt, [[maybe_unused]] Args&&... args) {
    try {
        static_cast<void>(vformat(fmt, make_format_args<context_t<CharT>>(args...)));
        assert(false);
    } catch (const format_error&) {
    } catch (...) {
        assert(false);
    }
};

int main() {
    format_tests<char>(test_format, test_format_exception);
    format_tests<wchar_t>(test_format, test_format_exception);

    format_tests<char>(test_vformat, test_vformat_exception);
    format_tests<wchar_t>(test_vformat, test_vformat_exception);
}
