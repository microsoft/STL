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
// * std/utilities/format/format.range/format.range.fmtmap/format.functions.tests.h
// * std/utilities/format/format.range/format.range.fmtmap/format.functions.format.pass.cpp
// * std/utilities/format/format.range/format.range.fmtmap/format.functions.vformat.pass.cpp

#include <algorithm>
#include <charconv>
#include <concepts>
#include <cstddef>
#include <cstring>
#include <format>
#include <iterator>
#include <map>
#include <ranges>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
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
void test_char(TestFunction check, ExceptionTest check_exception) {
    map<CharT, CharT> input{{CharT('a'), CharT('A')}, {CharT('c'), CharT('C')}, {CharT('b'), CharT('B')}};

    check(SV("{'a': 'A', 'b': 'B', 'c': 'C'}"), SV("{}"), input);
    check(SV("{'a': 'A', 'b': 'B', 'c': 'C'}^42"), SV("{}^42"), input);
    check(SV("{'a': 'A', 'b': 'B', 'c': 'C'}^42"), SV("{:}^42"), input);

    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV("{'a': 'A', 'b': 'B', 'c': 'C'}     "), SV("{:35}"), input);
    check(SV("{'a': 'A', 'b': 'B', 'c': 'C'}*****"), SV("{:*<35}"), input);
    check(SV("__{'a': 'A', 'b': 'B', 'c': 'C'}___"), SV("{:_^35}"), input);
    check(SV("#####{'a': 'A', 'b': 'B', 'c': 'C'}"), SV("{:#>35}"), input);

    check(SV("{'a': 'A', 'b': 'B', 'c': 'C'}     "), SV("{:{}}"), input, 35);
    check(SV("{'a': 'A', 'b': 'B', 'c': 'C'}*****"), SV("{:*<{}}"), input, 35);
    check(SV("__{'a': 'A', 'b': 'B', 'c': 'C'}___"), SV("{:_^{}}"), input, 35);
    check(SV("#####{'a': 'A', 'b': 'B', 'c': 'C'}"), SV("{:#>{}}"), input, 35);

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
    check(SV("__'a': 'A', 'b': 'B', 'c': 'C'___"), SV("{:_^33n}"), input);

    // *** type ***
    check(SV("__{'a': 'A', 'b': 'B', 'c': 'C'}___"), SV("{:_^35m}"), input); // the m type does the same as the default.
    check_exception("Type s requires character type as formatting argument", SV("{:s}"), input);
    check_exception("Type ?s requires character type as formatting argument", SV("{:?s}"), input);

    for (basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("s")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Only underlying has a format-spec

    check(SV("{'a': 'A'     , 'b': 'B'     , 'c': 'C'     }"), SV("{::13}"), input);
    check(SV("{'a': 'A'*****, 'b': 'B'*****, 'c': 'C'*****}"), SV("{::*<13}"), input);
    check(SV("{__'a': 'A'___, __'b': 'B'___, __'c': 'C'___}"), SV("{::_^13}"), input);
    check(SV("{#####'a': 'A', #####'b': 'B', #####'c': 'C'}"), SV("{::#>13}"), input);

    check(SV("{'a': 'A'     , 'b': 'B'     , 'c': 'C'     }"), SV("{::{}}"), input, 13);
    check(SV("{'a': 'A'*****, 'b': 'B'*****, 'c': 'C'*****}"), SV("{::*<{}}"), input, 13);
    check(SV("{__'a': 'A'___, __'b': 'B'___, __'c': 'C'___}"), SV("{::_^{}}"), input, 13);
    check(SV("{#####'a': 'A', #####'b': 'B', #####'c': 'C'}"), SV("{::#>{}}"), input, 13);

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
    check(SV("{'a': 'A', 'b': 'B', 'c': 'C'}"), SV("{::m}"), input);
    check(SV("{'a': 'A', 'b': 'B', 'c': 'C'}"), SV("{::n}"), input);
    check_exception("Type s requires character type as formatting argument", SV("{:s}"), input);
    check_exception("Type ?s requires character type as formatting argument", SV("{:?s}"), input);

    for (basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("s")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Both have a format-spec
    check(SV("^^{###'a': 'A', ###'b': 'B', ###'c': 'C'}^^^"), SV("{:^^44:#>11}"), input);
    check(SV("^^{###'a': 'A', ###'b': 'B', ###'c': 'C'}^^^"), SV("{:^^{}:#>11}"), input, 44);
    check(SV("^^{###'a': 'A', ###'b': 'B', ###'c': 'C'}^^^"), SV("{:^^{}:#>{}}"), input, 44, 11);

    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}:#>11}"), input);
    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}:#>{}}"), input, 44);
}

//
// char -> wchar_t
//

template <class TestFunction, class ExceptionTest>
void test_char_to_wchar(TestFunction check, ExceptionTest check_exception) {
    map<char, char> input{{'a', 'A'}, {'c', 'C'}, {'b', 'B'}};

    using CharT = wchar_t;
    check(SV("{'a': 'A', 'b': 'B', 'c': 'C'}"), SV("{}"), input);
    check(SV("{'a': 'A', 'b': 'B', 'c': 'C'}^42"), SV("{}^42"), input);
    check(SV("{'a': 'A', 'b': 'B', 'c': 'C'}^42"), SV("{:}^42"), input);

    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV("{'a': 'A', 'b': 'B', 'c': 'C'}     "), SV("{:35}"), input);
    check(SV("{'a': 'A', 'b': 'B', 'c': 'C'}*****"), SV("{:*<35}"), input);
    check(SV("__{'a': 'A', 'b': 'B', 'c': 'C'}___"), SV("{:_^35}"), input);
    check(SV("#####{'a': 'A', 'b': 'B', 'c': 'C'}"), SV("{:#>35}"), input);

    check(SV("{'a': 'A', 'b': 'B', 'c': 'C'}     "), SV("{:{}}"), input, 35);
    check(SV("{'a': 'A', 'b': 'B', 'c': 'C'}*****"), SV("{:*<{}}"), input, 35);
    check(SV("__{'a': 'A', 'b': 'B', 'c': 'C'}___"), SV("{:_^{}}"), input, 35);
    check(SV("#####{'a': 'A', 'b': 'B', 'c': 'C'}"), SV("{:#>{}}"), input, 35);

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
    check(SV("__'a': 'A', 'b': 'B', 'c': 'C'___"), SV("{:_^33n}"), input);

    // *** type ***
    check(SV("__{'a': 'A', 'b': 'B', 'c': 'C'}___"), SV("{:_^35m}"), input); // the m type does the same as the default.
    check_exception("Type s requires character type as formatting argument", SV("{:s}"), input);
    check_exception("Type ?s requires character type as formatting argument", SV("{:?s}"), input);

    for (basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("s")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Only underlying has a format-spec
    check(SV("{'a': 'A'     , 'b': 'B'     , 'c': 'C'     }"), SV("{::13}"), input);
    check(SV("{'a': 'A'*****, 'b': 'B'*****, 'c': 'C'*****}"), SV("{::*<13}"), input);
    check(SV("{__'a': 'A'___, __'b': 'B'___, __'c': 'C'___}"), SV("{::_^13}"), input);
    check(SV("{#####'a': 'A', #####'b': 'B', #####'c': 'C'}"), SV("{::#>13}"), input);

    check(SV("{'a': 'A'     , 'b': 'B'     , 'c': 'C'     }"), SV("{::{}}"), input, 13);
    check(SV("{'a': 'A'*****, 'b': 'B'*****, 'c': 'C'*****}"), SV("{::*<{}}"), input, 13);
    check(SV("{__'a': 'A'___, __'b': 'B'___, __'c': 'C'___}"), SV("{::_^{}}"), input, 13);
    check(SV("{#####'a': 'A', #####'b': 'B', #####'c': 'C'}"), SV("{::#>{}}"), input, 13);

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
    check(SV("{'a': 'A', 'b': 'B', 'c': 'C'}"), SV("{::m}"), input);
    check(SV("{'a': 'A', 'b': 'B', 'c': 'C'}"), SV("{::n}"), input);
    check_exception("Type s requires character type as formatting argument", SV("{:s}"), input);
    check_exception("Type ?s requires character type as formatting argument", SV("{:?s}"), input);

    for (basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("s")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Both have a format-spec
    check(SV("^^{###'a': 'A', ###'b': 'B', ###'c': 'C'}^^^"), SV("{:^^44:#>11}"), input);
    check(SV("^^{###'a': 'A', ###'b': 'B', ###'c': 'C'}^^^"), SV("{:^^{}:#>11}"), input, 44);
    check(SV("^^{###'a': 'A', ###'b': 'B', ###'c': 'C'}^^^"), SV("{:^^{}:#>{}}"), input, 44, 11);

    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}:#>11}"), input);
    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}:#>{}}"), input, 44);
}

//
// Bool
//
template <class CharT, class TestFunction, class ExceptionTest>
void test_bool(TestFunction check, ExceptionTest check_exception) {
    // duplicates are stored in order of insertion
    multimap<bool, int> input{{true, 42}, {false, 0}, {true, 1}};

    check(SV("{false: 0, true: 42, true: 1}"), SV("{}"), input);
    check(SV("{false: 0, true: 42, true: 1}^42"), SV("{}^42"), input);
    check(SV("{false: 0, true: 42, true: 1}^42"), SV("{:}^42"), input);

    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV("{false: 0, true: 42, true: 1}     "), SV("{:34}"), input);
    check(SV("{false: 0, true: 42, true: 1}*****"), SV("{:*<34}"), input);
    check(SV("__{false: 0, true: 42, true: 1}___"), SV("{:_^34}"), input);
    check(SV("#####{false: 0, true: 42, true: 1}"), SV("{:#>34}"), input);

    check(SV("{false: 0, true: 42, true: 1}     "), SV("{:{}}"), input, 34);
    check(SV("{false: 0, true: 42, true: 1}*****"), SV("{:*<{}}"), input, 34);
    check(SV("__{false: 0, true: 42, true: 1}___"), SV("{:_^{}}"), input, 34);
    check(SV("#####{false: 0, true: 42, true: 1}"), SV("{:#>{}}"), input, 34);

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
    check(SV("__false: 0, true: 42, true: 1___"), SV("{:_^32n}"), input);

    // *** type ***
    check(SV("__{false: 0, true: 42, true: 1}___"), SV("{:_^34m}"), input); // the m type does the same as the default.
    check_exception("Type s requires character type as formatting argument", SV("{:s}"), input);
    check_exception("Type ?s requires character type as formatting argument", SV("{:?s}"), input);

    for (basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("s")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Only underlying has a format-spec
    check(SV("{false: 0  , true: 42  , true: 1   }"), SV("{::10}"), input);
    check(SV("{false: 0**, true: 42**, true: 1***}"), SV("{::*<10}"), input);
    check(SV("{_false: 0_, _true: 42_, _true: 1__}"), SV("{::_^10}"), input);
    check(SV("{##false: 0, ##true: 42, ###true: 1}"), SV("{::#>10}"), input);

    check(SV("{false: 0  , true: 42  , true: 1   }"), SV("{::{}}"), input, 10);
    check(SV("{false: 0**, true: 42**, true: 1***}"), SV("{::*<{}}"), input, 10);
    check(SV("{_false: 0_, _true: 42_, _true: 1__}"), SV("{::_^{}}"), input, 10);
    check(SV("{##false: 0, ##true: 42, ###true: 1}"), SV("{::#>{}}"), input, 10);

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
    for (basic_string_view<CharT> fmt : fmt_invalid_nested_types<CharT>("")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Both have a format-spec
    check(SV("^^{##false: 0, ##true: 42, ###true: 1}^^^"), SV("{:^^41:#>10}"), input);
    check(SV("^^{##false: 0, ##true: 42, ###true: 1}^^^"), SV("{:^^{}:#>10}"), input, 41);
    check(SV("^^{##false: 0, ##true: 42, ###true: 1}^^^"), SV("{:^^{}:#>{}}"), input, 41, 10);

    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}:#>10}"), input);
    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}:#>{}}"), input, 41);
}

//
// Integral
//

template <class CharT, class TestFunction, class ExceptionTest>
void test_int(TestFunction check, ExceptionTest check_exception, auto&& input) {
    check(SV("{-42: 42, 1: -1, 42: -42}"), SV("{}"), input);
    check(SV("{-42: 42, 1: -1, 42: -42}^42"), SV("{}^42"), input);
    check(SV("{-42: 42, 1: -1, 42: -42}^42"), SV("{:}^42"), input);

    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV("{-42: 42, 1: -1, 42: -42}     "), SV("{:30}"), input);
    check(SV("{-42: 42, 1: -1, 42: -42}*****"), SV("{:*<30}"), input);
    check(SV("__{-42: 42, 1: -1, 42: -42}___"), SV("{:_^30}"), input);
    check(SV("#####{-42: 42, 1: -1, 42: -42}"), SV("{:#>30}"), input);

    check(SV("{-42: 42, 1: -1, 42: -42}     "), SV("{:{}}"), input, 30);
    check(SV("{-42: 42, 1: -1, 42: -42}*****"), SV("{:*<{}}"), input, 30);
    check(SV("__{-42: 42, 1: -1, 42: -42}___"), SV("{:_^{}}"), input, 30);
    check(SV("#####{-42: 42, 1: -1, 42: -42}"), SV("{:#>{}}"), input, 30);

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
    check(SV("__-42: 42, 1: -1, 42: -42___"), SV("{:_^28n}"), input);

    // *** type ***
    check(SV("__{-42: 42, 1: -1, 42: -42}___"), SV("{:_^30m}"), input); // the m type does the same as the default.
    check_exception("Type s requires character type as formatting argument", SV("{:s}"), input);
    check_exception("Type ?s requires character type as formatting argument", SV("{:?s}"), input);

    for (basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("s")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Only underlying has a format-spec
    check(SV("{-42: 42   , 1: -1     , 42: -42   }"), SV("{::10}"), input);
    check(SV("{-42: 42***, 1: -1*****, 42: -42***}"), SV("{::*<10}"), input);
    check(SV("{_-42: 42__, __1: -1___, _42: -42__}"), SV("{::_^10}"), input);
    check(SV("{###-42: 42, #####1: -1, ###42: -42}"), SV("{::#>10}"), input);

    check(SV("{-42: 42   , 1: -1     , 42: -42   }"), SV("{::{}}"), input, 10);
    check(SV("{-42: 42***, 1: -1*****, 42: -42***}"), SV("{::*<{}}"), input, 10);
    check(SV("{_-42: 42__, __1: -1___, _42: -42__}"), SV("{::_^{}}"), input, 10);
    check(SV("{###-42: 42, #####1: -1, ###42: -42}"), SV("{::#>{}}"), input, 10);

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
    for (basic_string_view<CharT> fmt : fmt_invalid_nested_types<CharT>("")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Both have a format-spec
    check(SV("^^{###-42: 42, #####1: -1, ###42: -42}^^^"), SV("{:^^41:#>10}"), input);
    check(SV("^^{###-42: 42, #####1: -1, ###42: -42}^^^"), SV("{:^^{}:#>10}"), input, 41);
    check(SV("^^{###-42: 42, #####1: -1, ###42: -42}^^^"), SV("{:^^{}:#>{}}"), input, 41, 10);

    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}:#>10}"), input);
    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}:#>{}}"), input, 41);
}

template <class CharT, class TestFunction, class ExceptionTest>
void test_int(TestFunction check, ExceptionTest check_exception) {
    test_int<CharT>(check, check_exception, map<int, int>{{1, -1}, {42, -42}, {-42, 42}});
}

//
// Floating point
//

template <class CharT, class TestFunction, class ExceptionTest>
void test_floating_point(TestFunction check, ExceptionTest check_exception) {
    map<double, double> input{{1.0, -1.0}, {-42, 42}};

    check(SV("{-42: 42, 1: -1}"), SV("{}"), input);
    check(SV("{-42: 42, 1: -1}^42"), SV("{}^42"), input);
    check(SV("{-42: 42, 1: -1}^42"), SV("{:}^42"), input);

    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV("{-42: 42, 1: -1}     "), SV("{:21}"), input);
    check(SV("{-42: 42, 1: -1}*****"), SV("{:*<21}"), input);
    check(SV("__{-42: 42, 1: -1}___"), SV("{:_^21}"), input);
    check(SV("#####{-42: 42, 1: -1}"), SV("{:#>21}"), input);

    check(SV("{-42: 42, 1: -1}     "), SV("{:{}}"), input, 21);
    check(SV("{-42: 42, 1: -1}*****"), SV("{:*<{}}"), input, 21);
    check(SV("__{-42: 42, 1: -1}___"), SV("{:_^{}}"), input, 21);
    check(SV("#####{-42: 42, 1: -1}"), SV("{:#>{}}"), input, 21);

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
    check(SV("__-42: 42, 1: -1___"), SV("{:_^19n}"), input);

    // *** type ***
    check(SV("__{-42: 42, 1: -1}___"), SV("{:_^21m}"), input); // the m type does the same as the default.
    check_exception("Type s requires character type as formatting argument", SV("{:s}"), input);
    check_exception("Type ?s requires character type as formatting argument", SV("{:?s}"), input);

    for (basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("s")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Only underlying has a format-spec
    check(SV("{-42: 42   , 1: -1     }"), SV("{::10}"), input);
    check(SV("{-42: 42***, 1: -1*****}"), SV("{::*<10}"), input);
    check(SV("{_-42: 42__, __1: -1___}"), SV("{::_^10}"), input);
    check(SV("{###-42: 42, #####1: -1}"), SV("{::#>10}"), input);

    check(SV("{-42: 42   , 1: -1     }"), SV("{::{}}"), input, 10);
    check(SV("{-42: 42***, 1: -1*****}"), SV("{::*<{}}"), input, 10);
    check(SV("{_-42: 42__, __1: -1___}"), SV("{::_^{}}"), input, 10);
    check(SV("{###-42: 42, #####1: -1}"), SV("{::#>{}}"), input, 10);

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
    for (basic_string_view<CharT> fmt : fmt_invalid_nested_types<CharT>("")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Both have a format-spec
    check(SV("^^{###-42: 42, #####1: -1}^^^"), SV("{:^^29:#>10}"), input);
    check(SV("^^{###-42: 42, #####1: -1}^^^"), SV("{:^^{}:#>10}"), input, 29);
    check(SV("^^{###-42: 42, #####1: -1}^^^"), SV("{:^^{}:#>{}}"), input, 29, 10);

    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}:#>10}"), input);
    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}:#>{}}"), input, 29);
}

//
// Pointer
//

template <class CharT, class TestFunction, class ExceptionTest>
void test_pointer(TestFunction check, ExceptionTest check_exception) {
    unordered_map<const void*, nullptr_t> input{{0, 0}};

    check(SV("{0x0: 0x0}"), SV("{}"), input);
    check(SV("{0x0: 0x0}^42"), SV("{}^42"), input);
    check(SV("{0x0: 0x0}^42"), SV("{:}^42"), input);

    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV("{0x0: 0x0}     "), SV("{:15}"), input);
    check(SV("{0x0: 0x0}*****"), SV("{:*<15}"), input);
    check(SV("__{0x0: 0x0}___"), SV("{:_^15}"), input);
    check(SV("#####{0x0: 0x0}"), SV("{:#>15}"), input);

    check(SV("{0x0: 0x0}     "), SV("{:{}}"), input, 15);
    check(SV("{0x0: 0x0}*****"), SV("{:*<{}}"), input, 15);
    check(SV("__{0x0: 0x0}___"), SV("{:_^{}}"), input, 15);
    check(SV("#####{0x0: 0x0}"), SV("{:#>{}}"), input, 15);

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
    check(SV("__0x0: 0x0___"), SV("{:_^13n}"), input);

    // *** type ***
    check(SV("__{0x0: 0x0}___"), SV("{:_^15m}"), input); // the m type does the same as the default.
    check_exception("Type s requires character type as formatting argument", SV("{:s}"), input);
    check_exception("Type ?s requires character type as formatting argument", SV("{:?s}"), input);

    for (basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("s")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Only underlying has a format-spec
    check(SV("{0x0: 0x0     }"), SV("{::13}"), input);
    check(SV("{0x0: 0x0*****}"), SV("{::*<13}"), input);
    check(SV("{__0x0: 0x0___}"), SV("{::_^13}"), input);
    check(SV("{#####0x0: 0x0}"), SV("{::#>13}"), input);

    check(SV("{0x0: 0x0     }"), SV("{::{}}"), input, 13);
    check(SV("{0x0: 0x0*****}"), SV("{::*<{}}"), input, 13);
    check(SV("{__0x0: 0x0___}"), SV("{::_^{}}"), input, 13);
    check(SV("{#####0x0: 0x0}"), SV("{::#>{}}"), input, 13);

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
    for (basic_string_view<CharT> fmt : fmt_invalid_nested_types<CharT>("")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Both have a format-spec
    check(SV("^^{###0x0: 0x0}^^^"), SV("{:^^18:#>11}"), input);
    check(SV("^^{###0x0: 0x0}^^^"), SV("{:^^{}:#>11}"), input, 18);
    check(SV("^^{###0x0: 0x0}^^^"), SV("{:^^{}:#>{}}"), input, 18, 11);

    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}:#>11}"), input);
    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}:#>{}}"), input, 18);
}

//
// String
//

template <class CharT, class TestFunction, class ExceptionTest>
void test_string(TestFunction check, ExceptionTest check_exception) {
    map<basic_string<CharT>, basic_string<CharT>> input{{STR("hello"), STR("HELLO")}, {STR("world"), STR("WORLD")}};

    check(SV(R"({"hello": "HELLO", "world": "WORLD"})"), SV("{}"), input);
    check(SV(R"({"hello": "HELLO", "world": "WORLD"}^42)"), SV("{}^42"), input);
    check(SV(R"({"hello": "HELLO", "world": "WORLD"}^42)"), SV("{:}^42"), input);

    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV(R"({"hello": "HELLO", "world": "WORLD"}     )"), SV("{:41}"), input);
    check(SV(R"({"hello": "HELLO", "world": "WORLD"}*****)"), SV("{:*<41}"), input);
    check(SV(R"(__{"hello": "HELLO", "world": "WORLD"}___)"), SV("{:_^41}"), input);
    check(SV(R"(#####{"hello": "HELLO", "world": "WORLD"})"), SV("{:#>41}"), input);

    check(SV(R"({"hello": "HELLO", "world": "WORLD"}     )"), SV("{:{}}"), input, 41);
    check(SV(R"({"hello": "HELLO", "world": "WORLD"}*****)"), SV("{:*<{}}"), input, 41);
    check(SV(R"(__{"hello": "HELLO", "world": "WORLD"}___)"), SV("{:_^{}}"), input, 41);
    check(SV(R"(#####{"hello": "HELLO", "world": "WORLD"})"), SV("{:#>{}}"), input, 41);

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
    check(SV(R"(__"hello": "HELLO", "world": "WORLD"___)"), SV("{:_^39n}"), input);

    // *** type ***
    check(SV(R"(__{"hello": "HELLO", "world": "WORLD"}___)"), SV("{:_^41m}"), input);
    check_exception("Type s requires character type as formatting argument", SV("{:s}"), input);
    check_exception("Type ?s requires character type as formatting argument", SV("{:?s}"), input);

    for (basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("s")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Only underlying has a format-spec
    check(SV(R"({"hello": "HELLO"     , "world": "WORLD"     })"), SV("{::21}"), input);
    check(SV(R"({"hello": "HELLO"*****, "world": "WORLD"*****})"), SV("{::*<21}"), input);
    check(SV(R"({__"hello": "HELLO"___, __"world": "WORLD"___})"), SV("{::_^21}"), input);
    check(SV(R"({#####"hello": "HELLO", #####"world": "WORLD"})"), SV("{::#>21}"), input);

    check(SV(R"({"hello": "HELLO"     , "world": "WORLD"     })"), SV("{::{}}"), input, 21);
    check(SV(R"({"hello": "HELLO"*****, "world": "WORLD"*****})"), SV("{::*<{}}"), input, 21);
    check(SV(R"({__"hello": "HELLO"___, __"world": "WORLD"___})"), SV("{::_^{}}"), input, 21);
    check(SV(R"({#####"hello": "HELLO", #####"world": "WORLD"})"), SV("{::#>{}}"), input, 21);

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
    for (basic_string_view<CharT> fmt : fmt_invalid_nested_types<CharT>("")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Both have a format-spec

    check(SV(R"(^^{#####"hello": "HELLO", #####"world": "WORLD"}^^^)"), SV("{:^^51:#>21}"), input);
    check(SV(R"(^^{#####"hello": "HELLO", #####"world": "WORLD"}^^^)"), SV("{:^^{}:#>21}"), input, 51);
    check(SV(R"(^^{#####"hello": "HELLO", #####"world": "WORLD"}^^^)"), SV("{:^^{}:#>{}}"), input, 51, 21);

    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}:#>21}"), input);
    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}:#>{}}"), input, 51);
}

//
// Handle
//

template <class CharT, class TestFunction, class ExceptionTest>
void test_status(TestFunction check, ExceptionTest check_exception) {
    unordered_multimap<status, status> input{{status::foobar, status::foo}, {status::foobar, status::bar}};

    check(SV("{0xaa55: 0xaaaa, 0xaa55: 0x5555}"), SV("{}"), input);
    check(SV("{0xaa55: 0xaaaa, 0xaa55: 0x5555}^42"), SV("{}^42"), input);
    check(SV("{0xaa55: 0xaaaa, 0xaa55: 0x5555}^42"), SV("{:}^42"), input);

    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV("{0xaa55: 0xaaaa, 0xaa55: 0x5555}     "), SV("{:37}"), input);
    check(SV("{0xaa55: 0xaaaa, 0xaa55: 0x5555}*****"), SV("{:*<37}"), input);
    check(SV("__{0xaa55: 0xaaaa, 0xaa55: 0x5555}___"), SV("{:_^37}"), input);
    check(SV("#####{0xaa55: 0xaaaa, 0xaa55: 0x5555}"), SV("{:#>37}"), input);

    check(SV("{0xaa55: 0xaaaa, 0xaa55: 0x5555}     "), SV("{:{}}"), input, 37);
    check(SV("{0xaa55: 0xaaaa, 0xaa55: 0x5555}*****"), SV("{:*<{}}"), input, 37);
    check(SV("__{0xaa55: 0xaaaa, 0xaa55: 0x5555}___"), SV("{:_^{}}"), input, 37);
    check(SV("#####{0xaa55: 0xaaaa, 0xaa55: 0x5555}"), SV("{:#>{}}"), input, 37);

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
    check(SV("__0xaa55: 0xaaaa, 0xaa55: 0x5555___"), SV("{:_^35n}"), input);

    // *** type ***
    check(
        SV("__{0xaa55: 0xaaaa, 0xaa55: 0x5555}___"), SV("{:_^37}"), input); // the m type does the same as the default.
    check_exception("Type s requires character type as formatting argument", SV("{:s}"), input);
    check_exception("Type ?s requires character type as formatting argument", SV("{:?s}"), input);

    // Underlying can't have a format-spec
}

//
// Adaptor
//

class adaptor {
    using adaptee = map<int, int>;

public:
    using key_type    = adaptee::key_type;
    using mapped_type = adaptee::mapped_type;
    using iterator    = adaptee::iterator;

    iterator begin() {
        return data_.begin();
    }
    iterator end() {
        return data_.end();
    }

    explicit adaptor(map<int, int>&& data) : data_(move(data)) {}

private:
    adaptee data_;
};

static_assert(format_kind<adaptor> == range_format::map);

template <class CharT, class TestFunction, class ExceptionTest>
void test_adaptor(TestFunction check, ExceptionTest check_exception) {
    test_int<CharT>(check, check_exception, adaptor{map<int, int>{{1, -1}, {42, -42}, {-42, 42}}});
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
    test_int<CharT>(check, check_exception);
    test_floating_point<CharT>(check, check_exception);
    test_pointer<CharT>(check, check_exception);
    test_string<CharT>(check, check_exception);

    test_status<CharT>(check, check_exception);

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
