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
// * std/utilities/format/format.tuple/format.functions.tests.h
// * std/utilities/format/format.tuple/format.functions.format.pass.cpp
// * std/utilities/format/format.tuple/format.functions.vformat.pass.cpp

#include <cassert>
#include <concepts>
#include <cstddef>
#include <format>
#include <iterator>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include <test_format_support.hpp>

using namespace std;

#define CSTR(Str) TYPED_LITERAL(CharT, Str)
#define STR(Str)  basic_string(CSTR(Str))
#define SV(Str)   basic_string_view(CSTR(Str))

enum class color { black, red, gold };

template <class CharT>
struct std::formatter<color, CharT> : std::formatter<basic_string_view<CharT>, CharT> {
    static constexpr basic_string_view<CharT> color_names[] = {SV("black"), SV("red"), SV("gold")};
    auto format(color c, auto& ctx) const {
        return formatter<basic_string_view<CharT>, CharT>::format(color_names[static_cast<int>(c)], ctx);
    }
};

//
// Generic tests for a tuple and pair with two elements.
//
template <class CharT, class TestFunction, class ExceptionTest, class TupleOrPair>
void test_tuple_or_pair_int_int(TestFunction check, ExceptionTest check_exception, TupleOrPair&& input) {
    check(SV("(42, 99)"), SV("{}"), input);
    check(SV("(42, 99)^42"), SV("{}^42"), input);
    check(SV("(42, 99)^42"), SV("{:}^42"), input);

    // *** align-fill & width ***
    check(SV("(42, 99)     "), SV("{:13}"), input);
    check(SV("(42, 99)*****"), SV("{:*<13}"), input);
    check(SV("__(42, 99)___"), SV("{:_^13}"), input);
    check(SV("#####(42, 99)"), SV("{:#>13}"), input);

    check(SV("(42, 99)     "), SV("{:{}}"), input, 13);
    check(SV("(42, 99)*****"), SV("{:*<{}}"), input, 13);
    check(SV("__(42, 99)___"), SV("{:_^{}}"), input, 13);
    check(SV("#####(42, 99)"), SV("{:#>{}}"), input, 13);

    check_exception("The format string contains an invalid escape sequence", SV("{:}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{:{<}"), input);
    check_exception("The fill option contains an invalid value", SV("{::<}"), input);

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

    // *** type ***
    check(SV("__42: 99___"), SV("{:_^11m}"), input);
    check(SV("__42, 99___"), SV("{:_^11n}"), input);

    for (CharT c : SV("aAbBcdeEfFgGopPsxX?")) {
        check_exception("The format specifier should consume the input or end with a '}'",
            basic_string_view{STR("{:") + c + STR("}")}, input);
    }
}

template <class CharT, class TestFunction, class ExceptionTest, class TupleOrPair>
void test_tuple_or_pair_int_string(TestFunction check, ExceptionTest check_exception, TupleOrPair&& input) {
    check(SV("(42, \"hello\")"), SV("{}"), input);
    check(SV("(42, \"hello\")^42"), SV("{}^42"), input);
    check(SV("(42, \"hello\")^42"), SV("{:}^42"), input);

    // *** align-fill & width ***
    check(SV("(42, \"hello\")     "), SV("{:18}"), input);
    check(SV("(42, \"hello\")*****"), SV("{:*<18}"), input);
    check(SV("__(42, \"hello\")___"), SV("{:_^18}"), input);
    check(SV("#####(42, \"hello\")"), SV("{:#>18}"), input);

    check(SV("(42, \"hello\")     "), SV("{:{}}"), input, 18);
    check(SV("(42, \"hello\")*****"), SV("{:*<{}}"), input, 18);
    check(SV("__(42, \"hello\")___"), SV("{:_^{}}"), input, 18);
    check(SV("#####(42, \"hello\")"), SV("{:#>{}}"), input, 18);

    check_exception("The format string contains an invalid escape sequence", SV("{:}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{:{<}"), input);
    check_exception("The fill option contains an invalid value", SV("{::<}"), input);

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

    // *** type ***
    check(SV("__42: \"hello\"___"), SV("{:_^16m}"), input);
    check(SV("__42, \"hello\"___"), SV("{:_^16n}"), input);

    for (CharT c : SV("aAbBcdeEfFgGopPsxX?")) {
        check_exception("The format specifier should consume the input or end with a '}'",
            basic_string_view{STR("{:") + c + STR("}")}, input);
    }
}

template <class CharT, class TestFunction, class TupleOrPair>
void test_escaping(TestFunction check, TupleOrPair&& input) {
    static_assert(same_as<remove_cvref_t<decltype(get<0>(input))>, CharT>);
    static_assert(same_as<remove_cvref_t<decltype(get<1>(input))>, basic_string<CharT>>);

    check(SV(R"(('*', ""))"), SV("{}"), input);

    // Char
    get<0>(input) = CharT('\t');
    check(SV(R"(('\t', ""))"), SV("{}"), input);
    get<0>(input) = CharT('\n');
    check(SV(R"(('\n', ""))"), SV("{}"), input);
    get<0>(input) = CharT('\0');
    check(SV(R"(('\u{0}', ""))"), SV("{}"), input);

    // String
    if constexpr (is_same_v<CharT, wchar_t>) {
        get<0>(input) = L'*';
        get<1>(input) = L"hell\u00d6"; // U+00D6 LATIN CAPITAL LETTER O WITH DIAERESIS
        check(L"('*', \"hell\u00d6\")"sv, L"{}"sv, input);
        check(L"#('*', \"hell\u00d6\")#"sv, L"{:#^16}"sv, input);

        get<1>(input) = L"hell\uff2f"; // U+FF2F FULLWIDTH LATIN CAPITAL LETTER O
        check(L"('*', \"hell\uff2f\")"sv, L"{}"sv, input);
        check(L"('*', \"hell\uff2f\")#"sv, L"{:#^16}"sv, input);
    }
}

//
// pair tests
//

template <class CharT, class TestFunction, class ExceptionTest>
void test_pair_int_int(TestFunction check, ExceptionTest check_exception) {
    test_tuple_or_pair_int_int<CharT>(check, check_exception, make_pair(42, 99));
}

template <class CharT, class TestFunction, class ExceptionTest>
void test_pair_int_string(TestFunction check, ExceptionTest check_exception) {
    test_tuple_or_pair_int_string<CharT>(check, check_exception, make_pair(42, SV("hello")));
    test_tuple_or_pair_int_string<CharT>(check, check_exception, make_pair(42, STR("hello")));
    test_tuple_or_pair_int_string<CharT>(check, check_exception, make_pair(42, CSTR("hello")));
}

//
// tuple tests
//

template <class CharT, class TestFunction, class ExceptionTest>
void test_tuple_int(TestFunction check, ExceptionTest check_exception) {
    auto input = make_tuple(42);

    check(SV("(42)"), SV("{}"), input);
    check(SV("(42)^42"), SV("{}^42"), input);
    check(SV("(42)^42"), SV("{:}^42"), input);

    // *** align-fill & width ***
    check(SV("(42)     "), SV("{:9}"), input);
    check(SV("(42)*****"), SV("{:*<9}"), input);
    check(SV("__(42)___"), SV("{:_^9}"), input);
    check(SV("#####(42)"), SV("{:#>9}"), input);

    check(SV("(42)     "), SV("{:{}}"), input, 9);
    check(SV("(42)*****"), SV("{:*<{}}"), input, 9);
    check(SV("__(42)___"), SV("{:_^{}}"), input, 9);
    check(SV("#####(42)"), SV("{:#>{}}"), input, 9);

    check_exception("The format string contains an invalid escape sequence", SV("{:}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{:{<}"), input);
    check_exception("The fill option contains an invalid value", SV("{::<}"), input);

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

    // *** type ***
    check_exception("Type m requires a pair or a tuple with two elements", SV("{:m}"), input);
    check(SV("__42___"), SV("{:_^7n}"), input);

    for (CharT c : SV("aAbBcdeEfFgGopPsxX?")) {
        check_exception("The format specifier should consume the input or end with a '}'",
            basic_string_view{STR("{:") + c + STR("}")}, input);
    }
}

template <class CharT, class TestFunction, class ExceptionTest>
void test_tuple_int_string_color(TestFunction check, ExceptionTest check_exception) {
    const auto input = make_tuple(42, SV("hello"), color::red);

    check(SV("(42, \"hello\", \"red\")"), SV("{}"), input);
    check(SV("(42, \"hello\", \"red\")^42"), SV("{}^42"), input);
    check(SV("(42, \"hello\", \"red\")^42"), SV("{:}^42"), input);

    // *** align-fill & width ***
    check(SV("(42, \"hello\", \"red\")     "), SV("{:25}"), input);
    check(SV("(42, \"hello\", \"red\")*****"), SV("{:*<25}"), input);
    check(SV("__(42, \"hello\", \"red\")___"), SV("{:_^25}"), input);
    check(SV("#####(42, \"hello\", \"red\")"), SV("{:#>25}"), input);

    check(SV("(42, \"hello\", \"red\")     "), SV("{:{}}"), input, 25);
    check(SV("(42, \"hello\", \"red\")*****"), SV("{:*<{}}"), input, 25);
    check(SV("__(42, \"hello\", \"red\")___"), SV("{:_^{}}"), input, 25);
    check(SV("#####(42, \"hello\", \"red\")"), SV("{:#>{}}"), input, 25);

    check_exception("The format string contains an invalid escape sequence", SV("{:}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{:{<}"), input);
    check_exception("The fill option contains an invalid value", SV("{::<}"), input);

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

    // *** type ***
    check_exception("Type m requires a pair or a tuple with two elements", SV("{:m}"), input);
    check(SV("__42, \"hello\", \"red\"___"), SV("{:_^23n}"), input);

    for (CharT c : SV("aAbBcdeEfFgGopPsxX?")) {
        check_exception("The format specifier should consume the input or end with a '}'",
            basic_string_view{STR("{:") + c + STR("}")}, input);
    }
}

template <class CharT, class TestFunction, class ExceptionTest>
void test_tuple_int_int(TestFunction check, ExceptionTest check_exception) {
    test_tuple_or_pair_int_int<CharT>(check, check_exception, make_tuple(42, 99));
}

template <class CharT, class TestFunction, class ExceptionTest>
void test_tuple_int_string(TestFunction check, ExceptionTest check_exception) {
    test_tuple_or_pair_int_string<CharT>(check, check_exception, make_tuple(42, SV("hello")));
    test_tuple_or_pair_int_string<CharT>(check, check_exception, make_tuple(42, STR("hello")));
    test_tuple_or_pair_int_string<CharT>(check, check_exception, make_tuple(42, CSTR("hello")));
}

template <class CharT, class TestFunction, class ExceptionTest, class Nested>
void test_nested(TestFunction check, ExceptionTest check_exception, Nested&& input) {
    // N4971 [format.formatter.spec]/2
    //   A debug-enabled specialization of formatter additionally provides a
    //   public, constexpr, non-static member function set_debug_format()
    //   which modifies the state of the formatter to be as if the type of the
    //   std-format-spec parsed by the last call to parse were ?.
    // pair and tuple are not debug-enabled specializations so the
    // set_debug_format is not propagated. The paper
    //   P2733 Fix handling of empty specifiers in format
    // addressed this.

    check(SV("(42, (\"hello\", \"red\"))"), SV("{}"), input);
    check(SV("(42, (\"hello\", \"red\"))^42"), SV("{}^42"), input);
    check(SV("(42, (\"hello\", \"red\"))^42"), SV("{:}^42"), input);

    // *** align-fill & width ***
    check(SV("(42, (\"hello\", \"red\"))     "), SV("{:27}"), input);
    check(SV("(42, (\"hello\", \"red\"))*****"), SV("{:*<27}"), input);
    check(SV("__(42, (\"hello\", \"red\"))___"), SV("{:_^27}"), input);
    check(SV("#####(42, (\"hello\", \"red\"))"), SV("{:#>27}"), input);

    check(SV("(42, (\"hello\", \"red\"))     "), SV("{:{}}"), input, 27);
    check(SV("(42, (\"hello\", \"red\"))*****"), SV("{:*<{}}"), input, 27);
    check(SV("__(42, (\"hello\", \"red\"))___"), SV("{:_^{}}"), input, 27);
    check(SV("#####(42, (\"hello\", \"red\"))"), SV("{:#>{}}"), input, 27);

    check_exception("The format string contains an invalid escape sequence", SV("{:}<}"), input);
    check_exception("The fill option contains an invalid value", SV("{:{<}"), input);
    check_exception("The fill option contains an invalid value", SV("{::<}"), input);

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

    // *** type ***
    check(SV("__42: (\"hello\", \"red\")___"), SV("{:_^25m}"), input);
    check(SV("__42, (\"hello\", \"red\")___"), SV("{:_^25n}"), input);

    for (CharT c : SV("aAbBcdeEfFgGopPsxX?")) {
        check_exception("The format specifier should consume the input or end with a '}'",
            basic_string_view{STR("{:") + c + STR("}")}, input);
    }
}

template <class CharT, class TestFunction, class ExceptionTest>
void run_tests(TestFunction check, ExceptionTest check_exception) {
    test_pair_int_int<CharT>(check, check_exception);
    test_pair_int_string<CharT>(check, check_exception);

    test_tuple_int<CharT>(check, check_exception);
    test_tuple_int_int<CharT>(check, check_exception);
    test_tuple_int_string<CharT>(check, check_exception);
    test_tuple_int_string_color<CharT>(check, check_exception);

    test_nested<CharT>(check, check_exception, make_pair(42, make_pair(SV("hello"), color::red)));
    test_nested<CharT>(check, check_exception, make_pair(42, make_tuple(SV("hello"), color::red)));
    test_nested<CharT>(check, check_exception, make_tuple(42, make_pair(SV("hello"), color::red)));
    test_nested<CharT>(check, check_exception, make_tuple(42, make_tuple(SV("hello"), color::red)));

    test_escaping<CharT>(check, make_pair(CharT('*'), STR("")));
    test_escaping<CharT>(check, make_tuple(CharT('*'), STR("")));

    // Test const ref-qualified types.
    check(SV("(42)"), SV("{}"), tuple<int>{42});
    check(SV("(42)"), SV("{}"), tuple<const int>{42});

    int answer = 42;
    check(SV("(42)"), SV("{}"), tuple<int&>{answer});
    check(SV("(42)"), SV("{}"), tuple<const int&>{answer});

    check(SV("(42)"), SV("{}"), tuple<int&&>{42});
    check(SV("(42)"), SV("{}"), tuple<const int&&>{42});
}

template <class>
struct format_context_for_impl {};

template <>
struct format_context_for_impl<char> {
    using type = format_context;
};

template <>
struct format_context_for_impl<wchar_t> {
    using type = wformat_context;
};

template <class CharT>
using format_context_for = format_context_for_impl<CharT>::type;

auto test_format = []<class CharT, class... Args>(basic_string_view<CharT> expected,
                       type_identity_t<basic_format_string<CharT, Args...>> fmt, Args&&... args) {
    auto out = format(fmt, forward<Args>(args)...);
    assert(out == expected);
};

auto test_format_exception = []<class CharT, class... Args>(string_view, basic_string_view<CharT>, Args&&...) {
    // After P2216 most exceptions thrown by format become ill-formed.
    // Therefore this test does nothing.
    // A basic ill-formed test is done in format.verify.cpp
    // The exceptions are tested by other functions that don't use the basic-format-string as fmt argument.
};

auto test_vformat = []<class CharT, class... Args>(
                        basic_string_view<CharT> expected, basic_string_view<CharT> fmt, Args&&... args) {
    auto out = vformat(fmt, make_format_args<format_context_for<CharT>>(args...));
    assert(out == expected);
};

auto test_vformat_exception = []<class CharT, class... Args>([[maybe_unused]] string_view what,
                                  [[maybe_unused]] basic_string_view<CharT> fmt, [[maybe_unused]] Args&&... args) {
    try {
        (void) vformat(fmt, make_format_args<format_context_for<CharT>>(args...));
        assert(false);
    } catch (const format_error&) {
    }
};

// Also test that functions taking non-constructible basic_format_context specializations can be well-formed,
// despite the fact that they can't actually be called.

template <class CharT>
void test_unconstructible_format_context_for_raw_ptr(basic_format_context<CharT*, CharT>& ctx) { // COMPILE-ONLY
    formatter<tuple<basic_string<CharT>>, CharT> tuple_formatter;
    tuple_formatter.format(make_tuple(basic_string<CharT>(STR("42"))), ctx);
}

template <class CharT>
void test_unconstructible_format_context_for_back_inserter(
    basic_format_context<back_insert_iterator<basic_string<CharT>>, CharT>& ctx) { // COMPILE-ONLY
    formatter<tuple<basic_string<CharT>>, CharT> tuple_formatter;
    tuple_formatter.format(make_tuple(basic_string<CharT>(STR("42"))), ctx);
}

int main() {
    run_tests<char>(test_format, test_format_exception);
    run_tests<char>(test_vformat, test_vformat_exception);

    run_tests<wchar_t>(test_format, test_format_exception);
    run_tests<wchar_t>(test_vformat, test_vformat_exception);

    (void) &test_unconstructible_format_context_for_raw_ptr<char>;
    (void) &test_unconstructible_format_context_for_raw_ptr<wchar_t>;

    (void) &test_unconstructible_format_context_for_back_inserter<char>;
    (void) &test_unconstructible_format_context_for_back_inserter<wchar_t>;
}
