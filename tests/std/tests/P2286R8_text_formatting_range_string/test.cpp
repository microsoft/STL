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
// * std/utilities/format/format.range/format.range.fmtstr/format.functions.tests.h
// * std/utilities/format/format.range/format.range.fmtstr/format.functions.format.pass.cpp
// * std/utilities/format/format.range/format.range.fmtstr/format.functions.vformat.pass.cpp

#include <algorithm>
#include <array>
#include <charconv>
#include <concepts>
#include <cstddef>
#include <cstring>
#include <format>
#include <iterator>
#include <list>
#include <ranges>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
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


//
// Types
//

template <class Container>
class test_range_format_string {
public:
    explicit test_range_format_string(Container str) : str_(move(str)) {}

    Container::const_iterator begin() const {
        return str_.begin();
    }
    Container::const_iterator end() const {
        return str_.end();
    }

private:
    Container str_;
};

template <class Container>
constexpr range_format std::format_kind<test_range_format_string<Container>> = range_format::string;

template <class Container>
class test_range_format_debug_string {
public:
    explicit test_range_format_debug_string(Container str) : str_(move(str)) {}

    Container::const_iterator begin() const {
        return str_.begin();
    }
    Container::const_iterator end() const {
        return str_.end();
    }

private:
    Container str_;
};

template <class Container>
constexpr range_format std::format_kind<test_range_format_debug_string<Container>> = range_format::debug_string;

//
// String
//

template <class CharT, class TestFunction, class ExceptionTest>
void test_string(TestFunction check, ExceptionTest check_exception, auto&& input) {
    check(SV("hello"), SV("{}"), input);
    check(SV("hello^42"), SV("{}^42"), input);
    check(SV("hello^42"), SV("{:}^42"), input);

    // *** align-fill & width ***
    check(SV("hello     "), SV("{:10}"), input);
    check(SV("hello*****"), SV("{:*<10}"), input);
    check(SV("__hello___"), SV("{:_^10}"), input);
    check(SV(":::::hello"), SV("{::>10}"), input);

    check(SV("hello     "), SV("{:{}}"), input, 10);
    check(SV("hello*****"), SV("{:*<{}}"), input, 10);
    check(SV("__hello___"), SV("{:_^{}}"), input, 10);
    check(SV(":::::hello"), SV("{::>{}}"), input, 10);

    check_exception("The format string contains an invalid escape sequence", SV("{:}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{:{<}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:-}"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:#}"), input);

    // *** zero-padding ***
    check_exception("The width option should not have a leading zero", SV("{:0}"), input);

    // *** precision ***
    check(SV("hel"), SV("{:.3}"), input);
    check(SV("hel"), SV("{:.{}}"), input, 3);

    check(SV("hel  "), SV("{:5.3}"), input);
    check(SV("hel  "), SV("{:{}.{}}"), input, 5, 3);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:L}"), input);

    // *** type ***
    check(SV("hello"), SV("{:s}"), input);
    check(SV("\"hello\""), SV("{:?}"), input);
    for (basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("s?")) {
        check_exception("The type option contains an invalid value for a string formatting argument", fmt, input);
    }
}

template <class CharT, class TestFunction, class ExceptionTest>
void test_string(TestFunction check, ExceptionTest check_exception) {
    // libc++ uses different containers for contiguous and non-contiguous ranges.
    basic_string<CharT> input = STR("hello");
    test_string<CharT>(check, check_exception, test_range_format_string<basic_string<CharT>>{input});
    test_string<CharT>(check, check_exception, test_range_format_string<basic_string_view<CharT>>{input});
    test_string<CharT>(
        check, check_exception, test_range_format_string<list<CharT>>{list<CharT>{input.begin(), input.end()}});
}

//
// String range
//

template <class CharT, class TestFunction, class ExceptionTest>
void test_range_string(TestFunction check, ExceptionTest check_exception, auto&& input) {
    check(SV(R"([Hello, world])"), SV("{}"), input);
    check(SV(R"([Hello, world]^42)"), SV("{}^42"), input);
    check(SV(R"([Hello, world]^42)"), SV("{:}^42"), input);

    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV(R"([Hello, world]     )"), SV("{:19}"), input);
    check(SV(R"([Hello, world]*****)"), SV("{:*<19}"), input);
    check(SV(R"(__[Hello, world]___)"), SV("{:_^19}"), input);
    check(SV(R"(#####[Hello, world])"), SV("{:#>19}"), input);

    check(SV(R"([Hello, world]     )"), SV("{:{}}"), input, 19);
    check(SV(R"([Hello, world]*****)"), SV("{:*<{}}"), input, 19);
    check(SV(R"(__[Hello, world]___)"), SV("{:_^{}}"), input, 19);
    check(SV(R"(#####[Hello, world])"), SV("{:#>{}}"), input, 19);

    check_exception("The format string contains an invalid escape sequence", SV("{:}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{:{<}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:-}"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:#}"), input);

    // *** zero-padding ***
    check_exception("The width option should not have a leading zero", SV("{:0}"), input);

    // *** precision ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:.}"), input);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:L}"), input);

    // *** n
    check(SV(R"(_Hello, world_)"), SV("{:_^14n}"), input);

    // *** type ***
    check_exception("Type m requires a pair or a tuple with two elements", SV("{:m}"), input);
    check_exception("Type s requires character type as formatting argument", SV("{:s}"), input);
    check_exception("Type ?s requires character type as formatting argument", SV("{:?s}"), input);

    for (basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("s")) {
        check_exception("The format specifier should consume the input or end with a '}'", fmt, input);
    }

    // ***** Only underlying has a format-spec
    check(SV(R"([Hello   , world   ])"), SV("{::8}"), input);
    check(SV(R"([Hello***, world***])"), SV("{::*<8}"), input);
    check(SV(R"([_Hello__, _world__])"), SV("{::_^8}"), input);
    check(SV(R"([:::Hello, :::world])"), SV("{:::>8}"), input);

    check(SV(R"([Hello   , world   ])"), SV("{::{}}"), input, 8);
    check(SV(R"([Hello***, world***])"), SV("{::*<{}}"), input, 8);
    check(SV(R"([_Hello__, _world__])"), SV("{::_^{}}"), input, 8);
    check(SV(R"([:::Hello, :::world])"), SV("{:::>{}}"), input, 8);

    check_exception("The format string contains an invalid escape sequence", SV("{::}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{::{<}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::-}"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::#}"), input);

    // *** zero-padding ***
    check_exception("The width option should not have a leading zero", SV("{::05}"), input);

    // *** precision ***
    check(SV(R"([Hel, wor])"), SV("{::.3}"), input);

    check(SV(R"([Hel, wor])"), SV("{::.{}}"), input, 3);

    check_exception("The precision option does not contain a value or an argument index", SV("{::.}"), input);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::L}"), input);

    // *** type ***
    for (basic_string_view<CharT> fmt : fmt_invalid_nested_types<CharT>("s?")) {
        check_exception("The type option contains an invalid value for a string formatting argument", fmt, input);
    }

    // ***** Both have a format-spec
    check(SV(R"(^^[:::Hello, :::world]^^^)"), SV("{:^^25::>8}"), input);
    check(SV(R"(^^[:::Hello, :::world]^^^)"), SV("{:^^{}::>8}"), input, 25);
    check(SV(R"(^^[:::Hello, :::world]^^^)"), SV("{:^^{}::>{}}"), input, 25, 8);

    check(SV(R"(^^[:::Hello, :::world]^^^)"), SV("{:^^25::>8}"), input);
    check(SV(R"(^^[:::Hello, :::world]^^^)"), SV("{:^^{}::>8}"), input, 25);
    check(SV(R"(^^[:::Hello, :::world]^^^)"), SV("{:^^{}::>{}}"), input, 25, 8);

    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}::>8}"), input);
    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}::>{}}"), input, 25);
}

template <class CharT, class TestFunction, class ExceptionTest>
void test_range_string(TestFunction check, ExceptionTest check_exception) {
    // libc++ uses different containers for contiguous and non-contiguous ranges.
    array input{STR("Hello"), STR("world")};
    test_range_string<CharT>(check, check_exception,
        array{test_range_format_string<basic_string<CharT>>{input[0]},
            test_range_format_string<basic_string<CharT>>{input[1]}});
    test_range_string<CharT>(check, check_exception,
        array{test_range_format_string<basic_string_view<CharT>>{input[0]},
            test_range_format_string<basic_string_view<CharT>>{input[1]}});
    test_range_string<CharT>(check, check_exception,
        array{test_range_format_string<list<CharT>>{list<CharT>{input[0].begin(), input[0].end()}},
            test_range_format_string<list<CharT>>{list<CharT>{input[1].begin(), input[1].end()}}});
    test_range_string<CharT>(check, check_exception,
        list{test_range_format_string<list<CharT>>{list<CharT>{input[0].begin(), input[0].end()}},
            test_range_format_string<list<CharT>>{list<CharT>{input[1].begin(), input[1].end()}}});
}

//
// Debug string
//

template <class CharT, class TestFunction, class ExceptionTest>
void test_debug_string(TestFunction check, ExceptionTest check_exception, auto&& input) {
    check(SV("\"hello\""), SV("{}"), input);
    check(SV("\"hello\"^42"), SV("{}^42"), input);
    check(SV("\"hello\"^42"), SV("{:}^42"), input);

    // *** align-fill & width ***
    check(SV("\"hello\"     "), SV("{:12}"), input);
    check(SV("\"hello\"*****"), SV("{:*<12}"), input);
    check(SV("__\"hello\"___"), SV("{:_^12}"), input);
    check(SV(":::::\"hello\""), SV("{::>12}"), input);

    check(SV("\"hello\"     "), SV("{:{}}"), input, 12);
    check(SV("\"hello\"*****"), SV("{:*<{}}"), input, 12);
    check(SV("__\"hello\"___"), SV("{:_^{}}"), input, 12);
    check(SV(":::::\"hello\""), SV("{::>{}}"), input, 12);

    check_exception("The format string contains an invalid escape sequence", SV("{:}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{:{<}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:-}"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:#}"), input);

    // *** zero-padding ***
    check_exception("The width option should not have a leading zero", SV("{:0}"), input);

    // *** precision ***
    check(SV("\"he"), SV("{:.3}"), input);
    check(SV("\"he"), SV("{:.{}}"), input, 3);

    check(SV("\"he  "), SV("{:5.3}"), input);
    check(SV("\"he  "), SV("{:{}.{}}"), input, 5, 3);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:L}"), input);

    // *** type ***
    check(SV("\"hello\""), SV("{:s}"), input); // escape overrides the type option s
    check(SV("\"hello\""), SV("{:?}"), input);
    for (basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("s?")) {
        check_exception("The type option contains an invalid value for a string formatting argument", fmt, input);
    }
}

template <class CharT, class TestFunction, class ExceptionTest>
void test_debug_string(TestFunction check, ExceptionTest check_exception) {
    // libc++ uses different containers for contiguous and non-contiguous ranges.
    basic_string<CharT> input = STR("hello");
    test_debug_string<CharT>(check, check_exception, test_range_format_debug_string<basic_string<CharT>>{input});
    test_debug_string<CharT>(check, check_exception, test_range_format_debug_string<basic_string_view<CharT>>{input});
    test_debug_string<CharT>(
        check, check_exception, test_range_format_debug_string<list<CharT>>{list<CharT>{input.begin(), input.end()}});
}

//
// Debug string range
//

template <class CharT, class TestFunction, class ExceptionTest>
void test_range_debug_string(TestFunction check, ExceptionTest check_exception, auto&& input) {
    // ***** underlying has no format-spec

    // *** align-fill & width ***
    check(SV(R"(["Hello", "world"]     )"), SV("{:23}"), input);
    check(SV(R"(["Hello", "world"]*****)"), SV("{:*<23}"), input);
    check(SV(R"(__["Hello", "world"]___)"), SV("{:_^23}"), input);
    check(SV(R"(#####["Hello", "world"])"), SV("{:#>23}"), input);

    check(SV(R"(["Hello", "world"]     )"), SV("{:{}}"), input, 23);
    check(SV(R"(["Hello", "world"]*****)"), SV("{:*<{}}"), input, 23);
    check(SV(R"(__["Hello", "world"]___)"), SV("{:_^{}}"), input, 23);
    check(SV(R"(#####["Hello", "world"])"), SV("{:#>{}}"), input, 23);

    check_exception("The format string contains an invalid escape sequence", SV("{:}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{:{<}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{:-}"), input);

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
    check(SV(R"(["Hello"   , "world"   ])"), SV("{::10}"), input);
    check(SV(R"(["Hello"***, "world"***])"), SV("{::*<10}"), input);
    check(SV(R"([_"Hello"__, _"world"__])"), SV("{::_^10}"), input);
    check(SV(R"([:::"Hello", :::"world"])"), SV("{:::>10}"), input);

    check(SV(R"(["Hello"   , "world"   ])"), SV("{::{}}"), input, 10);
    check(SV(R"(["Hello"***, "world"***])"), SV("{::*<{}}"), input, 10);
    check(SV(R"([_"Hello"__, _"world"__])"), SV("{::_^{}}"), input, 10);
    check(SV(R"([:::"Hello", :::"world"])"), SV("{:::>{}}"), input, 10);

    check_exception("The format string contains an invalid escape sequence", SV("{::}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{::{<}"), input);

    // *** sign ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::-}"), input);

    // *** alternate form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::#}"), input);

    // *** zero-padding ***
    check_exception("The width option should not have a leading zero", SV("{::05}"), input);

    // *** precision ***
    check(SV(R"(["He, "wo])"), SV("{::.3}"), input);

    check(SV(R"(["He, "wo])"), SV("{::.{}}"), input, 3);

    check_exception("The precision option does not contain a value or an argument index", SV("{::.}"), input);

    // *** locale-specific form ***
    check_exception("The format specifier should consume the input or end with a '}'", SV("{::L}"), input);

    // *** type ***
    for (basic_string_view<CharT> fmt : fmt_invalid_nested_types<CharT>("s?")) {
        check_exception("The type option contains an invalid value for a string formatting argument", fmt, input);
    }

    // ***** Both have a format-spec
    check(SV(R"(^^[:::"Hello", :::"world"]^^^)"), SV("{:^^29::>10}"), input);
    check(SV(R"(^^[:::"Hello", :::"world"]^^^)"), SV("{:^^{}::>10}"), input, 29);
    check(SV(R"(^^[:::"Hello", :::"world"]^^^)"), SV("{:^^{}::>{}}"), input, 29, 10);

    check(SV(R"(^^[:::"Hello", :::"world"]^^^)"), SV("{:^^29::>10}"), input);
    check(SV(R"(^^[:::"Hello", :::"world"]^^^)"), SV("{:^^{}::>10}"), input, 29);
    check(SV(R"(^^[:::"Hello", :::"world"]^^^)"), SV("{:^^{}::>{}}"), input, 29, 10);

    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}::>10}"), input);
    check_exception(
        "The argument index value is too large for the number of arguments supplied", SV("{:^^{}::>{}}"), input, 29);
}

template <class CharT, class TestFunction, class ExceptionTest>
void test_range_debug_string(TestFunction check, ExceptionTest check_exception) {
    // libc++ uses different containers for contiguous and non-contiguous ranges.
    array input{STR("Hello"), STR("world")};
    test_range_debug_string<CharT>(check, check_exception,
        array{test_range_format_debug_string<basic_string<CharT>>{input[0]},
            test_range_format_debug_string<basic_string<CharT>>{input[1]}});
    test_range_debug_string<CharT>(check, check_exception,
        array{test_range_format_debug_string<basic_string_view<CharT>>{input[0]},
            test_range_format_debug_string<basic_string_view<CharT>>{input[1]}});
    test_range_debug_string<CharT>(check, check_exception,
        array{test_range_format_debug_string<list<CharT>>{list<CharT>{input[0].begin(), input[0].end()}},
            test_range_format_debug_string<list<CharT>>{list<CharT>{input[1].begin(), input[1].end()}}});
    test_range_debug_string<CharT>(check, check_exception,
        list{test_range_format_debug_string<list<CharT>>{list<CharT>{input[0].begin(), input[0].end()}},
            test_range_format_debug_string<list<CharT>>{list<CharT>{input[1].begin(), input[1].end()}}});
}

//
// Driver
//

template <class CharT, class TestFunction, class ExceptionTest>
void format_tests(TestFunction check, ExceptionTest check_exception) {
    test_string<CharT>(check, check_exception);
    test_range_string<CharT>(check, check_exception);

    test_debug_string<CharT>(check, check_exception);
    test_range_debug_string<CharT>(check, check_exception);
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
