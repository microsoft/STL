// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <assert.h>
#include <format>
#include <iterator>
#include <limits>
#include <locale>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

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

// Test against IDL mismatch between the DLL which stores the locale and the code which uses it.
#ifdef _DEBUG
#define DEFAULT_IDL_SETTING 2
#else
#define DEFAULT_IDL_SETTING 0
#endif

template <class charT, class... Args>
auto make_testing_format_args(Args&&... vals) {
    if constexpr (is_same_v<charT, wchar_t>) {
        return make_wformat_args(forward<Args>(vals)...);
    } else {
        return make_format_args(forward<Args>(vals)...);
    }
}

template <class charT, class... Args>
void throw_helper(const charT* fmt, const Args&... vals) {
    try {
        format(fmt, vals...);
        assert(false);
    } catch (const format_error&) {
    }
}


// tests for format with no format args or replacement fields
template <class charT>
void test_simple_formatting() {
    basic_string<charT> output_string;

    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("f"), make_testing_format_args<charT>());
    assert(output_string == STR("f"));

    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("format"), make_testing_format_args<charT>());
    assert(output_string == STR("format"));

    assert(format(STR("f")) == STR("f"));
    assert(format(STR("format")) == STR("format"));
    assert(format(locale::classic(), STR("f")) == STR("f"));
    assert(format(locale::classic(), STR("format")) == STR("format"));
}

template <class charT>
void test_escaped_curls() {
    basic_string<charT> output_string;

    // test escaped opening curls
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("{{"), make_testing_format_args<charT>());
    assert(output_string == STR("{"));

    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("{{{{"), make_testing_format_args<charT>());
    assert(output_string == STR("{{"));

    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("x{{"), make_testing_format_args<charT>());
    assert(output_string == STR("x{"));

    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("{{ {{"), make_testing_format_args<charT>());
    assert(output_string == STR("{ {"));

    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("x{{x"), make_testing_format_args<charT>());
    assert(output_string == STR("x{x"));

    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("{{x"), make_testing_format_args<charT>());
    assert(output_string == STR("{x"));

    // tests escaped closing curls
    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("}}"), make_testing_format_args<charT>());
    assert(output_string == STR("}"));

    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("}}}}"), make_testing_format_args<charT>());
    assert(output_string == STR("}}"));

    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("x}}"), make_testing_format_args<charT>());
    assert(output_string == STR("x}"));

    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("}} }}"), make_testing_format_args<charT>());
    assert(output_string == STR("} }"));

    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("x}}x"), make_testing_format_args<charT>());
    assert(output_string == STR("x}x"));

    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("}}x"), make_testing_format_args<charT>());
    assert(output_string == STR("}x"));
}

template <class charT>
void test_simple_replacement_field() {
    basic_string<charT> output_string;

    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(STR("f")));
    assert(output_string == STR("f"));


    assert(format(STR("{}"), STR("f")) == STR("f"));
    assert(format(locale::classic(), STR("{}"), STR("f")) == STR("f"));

    // Test string_view
    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("{}"),
        make_testing_format_args<charT>(basic_string_view<charT>{STR("f")}));
    assert(output_string == STR("f"));

    // Test bool
    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(true));
    assert(output_string == STR("true"));

    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(false));
    assert(output_string == STR("false"));

    // Test char
    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>('a'));
    assert(output_string == STR("a"));

    // Test const void*
    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("{}"),
        make_testing_format_args<charT>(static_cast<const void*>(nullptr)));
    assert(output_string == STR("0x0"));

    // Test void*
    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("{}"),
        make_testing_format_args<charT>(static_cast<void*>(nullptr)));
    assert(output_string == STR("0x0"));

    // Test nullptr
    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(nullptr));
    assert(output_string == STR("0x0"));

    // Test signed integers
    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(1234));
    assert(output_string == STR("1234"));

    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(1234ll));
    assert(output_string == STR("1234"));

    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(INT_MIN));
    assert(output_string == STR("-2147483648"));

    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(INT_MAX));
    assert(output_string == STR("2147483647"));

    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(LLONG_MAX));
    assert(output_string == STR("9223372036854775807"));

    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(LLONG_MIN));
    assert(output_string == STR("-9223372036854775808"));

    // Test unsigned integers
    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(1234u));
    assert(output_string == STR("1234"));

    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(1234ull));
    assert(output_string == STR("1234"));

    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(UINT_MAX));
    assert(output_string == STR("4294967295"));

    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(ULLONG_MAX));
    assert(output_string == STR("18446744073709551615"));

    // Test float
    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(12.34f));
    assert(output_string == STR("12.34"));

    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(FLT_MAX));
    assert(output_string == STR("3.4028235e+38"));

    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(-FLT_MAX));
    assert(output_string == STR("-3.4028235e+38"));

    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(FLT_MIN));
    assert(output_string == STR("1.1754944e-38"));

    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("{}"),
        make_testing_format_args<charT>(FLT_EPSILON));
    assert(output_string == STR("1.1920929e-07"));

    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("{}"),
        make_testing_format_args<charT>(FLT_TRUE_MIN));
    assert(output_string == STR("1e-45"));

    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("{}"),
        make_testing_format_args<charT>(numeric_limits<float>::infinity()));
    assert(output_string == STR("inf"));

    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("{}"),
        make_testing_format_args<charT>(-numeric_limits<float>::infinity()));
    assert(output_string == STR("-inf"));

    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("{}"),
        make_testing_format_args<charT>(numeric_limits<float>::quiet_NaN()));
    assert(output_string == STR("nan"));

    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(0.f));
    assert(output_string == STR("0"));

    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(-0.f));
    assert(output_string == STR("-0"));

    // Test double
    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(12.34));
    assert(output_string == STR("12.34"));

    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(DBL_MAX));
    assert(output_string == STR("1.7976931348623157e+308"));

    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(-DBL_MAX));
    assert(output_string == STR("-1.7976931348623157e+308"));

    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(DBL_MIN));
    assert(output_string == STR("2.2250738585072014e-308"));

    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("{}"),
        make_testing_format_args<charT>(DBL_EPSILON));
    assert(output_string == STR("2.220446049250313e-16"));

    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("{}"),
        make_testing_format_args<charT>(DBL_TRUE_MIN));
    assert(output_string == STR("5e-324"));

    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("{}"),
        make_testing_format_args<charT>(numeric_limits<double>::infinity()));
    assert(output_string == STR("inf"));

    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("{}"),
        make_testing_format_args<charT>(-numeric_limits<double>::infinity()));
    assert(output_string == STR("-inf"));

    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("{}"),
        make_testing_format_args<charT>(numeric_limits<double>::quiet_NaN()));
    assert(output_string == STR("nan"));

    output_string.clear();
    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(0.0));
    assert(output_string == STR("0"));

    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("{}"), make_testing_format_args<charT>(-0.0));
    assert(output_string == STR("-0"));
}

template <class charT>
void test_multiple_replacement_fields() {
    basic_string<charT> output_string;

    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("{0} {0}"),
        make_testing_format_args<charT>(STR("f")));
    assert(output_string == STR("f f"));
}

template <class charT>
void test_fill_and_align() {
    auto writer = [](auto out) {
        out++ = 'A';
        out++ = 'B';
        return out;
    };

    _Basic_format_specs<charT> specs;

    auto tester = [&] {
        basic_string<charT> output_string;
        _Write_aligned(back_inserter(output_string), 2, specs, _Align::_Left, writer);
        return output_string;
    };

    assert(tester() == STR("AB"));

    specs._Width = 1;
    assert(tester() == STR("AB"));


    specs._Width     = 5;
    specs._Alignment = _Align::_Left;
    assert(tester() == STR("AB   "));

    specs._Alignment = _Align::_Right;
    assert(tester() == STR("   AB"));

    specs._Alignment = _Align::_Center;
    assert(tester() == STR(" AB  "));


    specs._Alignment = _Align::_Left;
    specs._Fill[0]   = {'*'};
    assert(tester() == STR("AB***"));

    specs._Alignment = _Align::_Right;
    assert(tester() == STR("***AB"));

    specs._Alignment = _Align::_Center;
    assert(tester() == STR("*AB**"));
}

template <class charT, class integral>
void test_intergal_specs() {
    assert(format(STR("{:}"), integral{0}) == STR("0"));

    // Sign
    assert(format(STR("{: }"), integral{0}) == STR(" 0"));
    assert(format(STR("{:+}"), integral{0}) == STR("+0"));
    assert(format(STR("{:-}"), integral{0}) == STR("0"));

    if constexpr (is_signed_v<integral>) {
        assert(format(STR("{: }"), integral{-1}) == STR("-1"));
        assert(format(STR("{:+}"), integral{-1}) == STR("-1"));
        assert(format(STR("{:-}"), integral{-1}) == STR("-1"));
    }

    assert(format(STR("{: 3}"), integral{1}) == STR("  1"));
    assert(format(STR("{:+3}"), integral{1}) == STR(" +1"));
    assert(format(STR("{:-3}"), integral{1}) == STR("  1"));

    // Alternate form
    assert(format(STR("{:#}"), integral{0}) == STR("0"));
    assert(format(STR("{:#d}"), integral{0}) == STR("0"));
    assert(format(STR("{:#c}"), integral{'a'}) == STR("a"));

    assert(format(STR("{:#b}"), integral{0}) == STR("0b0"));
    assert(format(STR("{:#B}"), integral{0}) == STR("0B0"));

    assert(format(STR("{:#o}"), integral{0}) == STR("0"));
    assert(format(STR("{:#o}"), integral{1}) == STR("01"));

    assert(format(STR("{:#x}"), integral{0}) == STR("0x0"));
    assert(format(STR("{:#X}"), integral{0}) == STR("0X0"));
    assert(format(STR("{:#x}"), integral{255}) == STR("0xff"));
    assert(format(STR("{:#X}"), integral{255}) == STR("0XFF"));

    assert(format(STR("{:+#6x}"), integral{255}) == STR(" +0xff"));

    if constexpr (is_signed_v<integral>) {
        assert(format(STR("{:#o}"), integral{-1}) == STR("-01"));
        assert(format(STR("{:#x}"), integral{-255}) == STR("-0xff"));
        assert(format(STR("{:#X}"), integral{-255}) == STR("-0XFF"));
    }

    if constexpr (is_same_v<integral, long long>) {
        assert(format(STR("{:b}"), numeric_limits<long long>::min())
               == STR("-1000000000000000000000000000000000000000000000000000000000000000"));
    }

    // Leading zero
    assert(format(STR("{:0}"), integral{0}) == STR("0"));
    assert(format(STR("{:03}"), integral{0}) == STR("000"));
    assert(format(STR("{:+03}"), integral{0}) == STR("+00"));
    assert(format(STR("{:<03}"), integral{0}) == STR("0  "));
    assert(format(STR("{:>03}"), integral{0}) == STR("  0"));
    assert(format(STR("{:+#06X}"), integral{5}) == STR("+0X005"));

    // Width
    assert(format(STR("{:3}"), integral{0}) == STR("  0"));

    // Precision
    throw_helper(STR("{:.1}"), integral{0});

    // Locale
#if !defined(_DLL) || _ITERATOR_DEBUG_LEVEL == DEFAULT_IDL_SETTING
    assert(format(locale{"en-US"}, STR("{:L}"), integral{0}) == STR("0"));
    assert(format(locale{"en-US"}, STR("{:L}"), integral{100}) == STR("100"));
    assert(format(locale{"en-US"}, STR("{:L}"), integral{1'000}) == STR("1,000"));
    assert(format(locale{"en-US"}, STR("{:L}"), integral{10'000}) == STR("10,000"));
    assert(format(locale{"en-US"}, STR("{:L}"), integral{100'000}) == STR("100,000"));
    assert(format(locale{"en-US"}, STR("{:L}"), integral{1'000'000}) == STR("1,000,000"));
    assert(format(locale{"en-US"}, STR("{:L}"), integral{10'000'000}) == STR("10,000,000"));
    assert(format(locale{"en-US"}, STR("{:L}"), integral{100'000'000}) == STR("100,000,000"));

    assert(format(locale{"en-US"}, STR("{:Lx}"), integral{0x123'abc}) == STR("123,abc"));
    assert(format(locale{"en-US"}, STR("{:6L}"), integral{1'000}) == STR(" 1,000"));

    assert(format(locale{"hi-IN"}, STR("{:L}"), integral{10'000'000}) == STR("1,00,00,000"));
    assert(format(locale{"hi-IN"}, STR("{:L}"), integral{100'000'000}) == STR("10,00,00,000"));

    assert(format(locale{"hi-IN"}, STR("{:Lx}"), integral{0x123'abc}) == STR("1,23,abc"));
#endif // !defined(_DLL) || _ITERATOR_DEBUG_LEVEL == DEFAULT_IDL_SETTING

    // Type
    assert(format(STR("{:b}"), integral{0}) == STR("0"));
    assert(format(STR("{:b}"), integral{100}) == STR("1100100"));

    assert(format(STR("{:d}"), integral{100}) == STR("100"));

    throw_helper(STR("{:c}"), integral{numeric_limits<charT>::max()} + 1);
    if constexpr (is_signed_v<integral>) {
        throw_helper(STR("{:c}"), integral{numeric_limits<charT>::min()} - 1);
    }
}

template <class charT, class T>
void test_type(const charT* fmt, T val) {
    assert(format(fmt, val) == format(fmt, static_cast<int>(val)));
}

template <class charT>
void test_bool_specs() {
    assert(format(STR("{:}"), true) == STR("true"));
    assert(format(STR("{:}"), false) == STR("false"));

    // Sign
    throw_helper(STR("{: }"), true);
    throw_helper(STR("{:+}"), true);
    throw_helper(STR("{:-}"), true);

    // Alternate form
    throw_helper(STR("{:#}"), true);

    // Leading zero
    throw_helper(STR("{:0}"), true);

    // Width
    assert(format(STR("{:6}"), true) == STR("true  "));
    assert(format(STR("{:6}"), false) == STR("false "));

    // Precision
    throw_helper(STR("{:.5}"), true);

    // Locale
    assert(format(STR("{:L}"), true) == STR("true"));
    assert(format(STR("{:L}"), false) == STR("false"));
#if !defined(_DLL) || _ITERATOR_DEBUG_LEVEL == DEFAULT_IDL_SETTING
    assert(format(locale{"en-US"}, STR("{:L}"), true) == STR("true"));
    assert(format(locale{"en-US"}, STR("{:L}"), false) == STR("false"));

    struct my_bool : numpunct<charT> {
        virtual basic_string<charT> do_truename() const {
            return STR("yes");
        }

        virtual basic_string<charT> do_falsename() const {
            return STR("no");
        }
    };

    locale loc{locale::classic(), new my_bool};

    assert(format(loc, STR("{:L}"), true) == STR("yes"));
    assert(format(loc, STR("{:L}"), false) == STR("no"));
#endif // !defined(_DLL) || _ITERATOR_DEBUG_LEVEL == DEFAULT_IDL_SETTING

    // Type
    assert(format(STR("{:s}"), true) == STR("true"));
    throw_helper(STR("{:a}"), true);

    test_type(STR("{:b}"), true);
    test_type(STR("{:B}"), true);
    test_type(STR("{:c}"), true);
    test_type(STR("{:d}"), true);
    test_type(STR("{:o}"), true);
    test_type(STR("{:x}"), true);
    test_type(STR("{:X}"), true);

    test_type(STR("{:b}"), false);
    test_type(STR("{:B}"), false);
    test_type(STR("{:c}"), false);
    test_type(STR("{:d}"), false);
    test_type(STR("{:o}"), false);
    test_type(STR("{:x}"), false);
    test_type(STR("{:X}"), false);
}

template <class charT>
void test_char_specs() {
    assert(format(STR("{:}"), charT{'X'}) == STR("X"));

    // Sign
    throw_helper(STR("{: }"), charT{'X'});
    throw_helper(STR("{:+}"), charT{'X'});
    throw_helper(STR("{:-}"), charT{'X'});

    // Alternate form
    throw_helper(STR("{:#}"), charT{'X'});

    // Leading zero
    throw_helper(STR("{:0}"), charT{'X'});

    // Width
    assert(format(STR("{:3}"), charT{'X'}) == STR("X  "));

    // Precision
    throw_helper(STR("{:.5}"), charT{'X'});

    // Types
    assert(format(STR("{:c}"), charT{'X'}) == STR("X"));
    throw_helper(STR("{:a}"), charT{'X'});

    test_type(STR("{:b}"), charT{'X'});
    test_type(STR("{:B}"), charT{'X'});
    test_type(STR("{:c}"), charT{'X'});
    test_type(STR("{:d}"), charT{'X'});
    test_type(STR("{:o}"), charT{'X'});
    test_type(STR("{:x}"), charT{'X'});
    test_type(STR("{:X}"), charT{'X'});

    test_type(STR("{:+d}"), charT{'X'});
}

template <class charT, class Float>
void test_float_specs() {
    const Float inf = numeric_limits<Float>::infinity();
    const Float nan = numeric_limits<Float>::quiet_NaN();

    assert(format(STR("{:}"), Float{0}) == STR("0"));
    assert(format(STR("{:}"), inf) == STR("inf"));
    assert(format(STR("{:}"), nan) == STR("nan"));

    // Sign
    assert(format(STR("{: }"), Float{0}) == STR(" 0"));
    assert(format(STR("{:+}"), Float{0}) == STR("+0"));
    assert(format(STR("{:-}"), Float{0}) == STR("0"));

    assert(format(STR("{: }"), Float{-1}) == STR("-1"));
    assert(format(STR("{:+}"), Float{-1}) == STR("-1"));
    assert(format(STR("{:-}"), Float{-1}) == STR("-1"));

    assert(format(STR("{: 3}"), Float{1}) == STR("  1"));
    assert(format(STR("{:+3}"), Float{1}) == STR(" +1"));
    assert(format(STR("{:-3}"), Float{1}) == STR("  1"));

    assert(format(STR("{: }"), inf) == STR(" inf"));
    assert(format(STR("{:+}"), inf) == STR("+inf"));
    assert(format(STR("{:-}"), inf) == STR("inf"));

    assert(format(STR("{: }"), -inf) == STR("-inf"));
    assert(format(STR("{:+}"), -inf) == STR("-inf"));
    assert(format(STR("{:-}"), -inf) == STR("-inf"));

    assert(format(STR("{: }"), nan) == STR(" nan"));
    assert(format(STR("{:+}"), nan) == STR("+nan"));
    assert(format(STR("{:-}"), nan) == STR("nan"));

    // Alternate form
    assert(format(STR("{:#}"), Float{0}) == STR("0."));
    assert(format(STR("{:#a}"), Float{0}) == STR("0.p+0"));
    assert(format(STR("{:#A}"), Float{0}) == STR("0.P+0"));
    assert(format(STR("{:#.0e}"), Float{0}) == STR("0.e+00"));
    assert(format(STR("{:#.0E}"), Float{0}) == STR("0.E+00"));
    assert(format(STR("{:#.0f}"), Float{0}) == STR("0."));
    assert(format(STR("{:#.0F}"), Float{0}) == STR("0."));
    assert(format(STR("{:#g}"), Float{0}) == STR("0.00000"));
    assert(format(STR("{:#G}"), Float{0}) == STR("0.00000"));
    assert(format(STR("{:#g}"), Float{1.2}) == STR("1.20000"));
    assert(format(STR("{:#G}"), Float{1.2}) == STR("1.20000"));
    assert(format(STR("{:#g}"), Float{1'000'000}) == STR("1.00000e+06"));
    assert(format(STR("{:#g}"), Float{12.2}) == STR("12.2000"));
    assert(format(STR("{:#.0g}"), Float{0}) == STR("0."));
    assert(format(STR("{:#.0G}"), Float{0}) == STR("0."));

    assert(format(STR("{:#} {:#}"), inf, nan) == STR("inf nan"));
    assert(format(STR("{:#a} {:#a}"), inf, nan) == STR("inf nan"));
    assert(format(STR("{:#A} {:#A}"), inf, nan) == STR("INF NAN"));
    assert(format(STR("{:#e} {:#e}"), inf, nan) == STR("inf nan"));
    assert(format(STR("{:#E} {:#E}"), inf, nan) == STR("INF NAN"));
    assert(format(STR("{:#f} {:#f}"), inf, nan) == STR("inf nan"));
    assert(format(STR("{:#F} {:#F}"), inf, nan) == STR("inf nan"));
    assert(format(STR("{:#g} {:#g}"), inf, nan) == STR("inf nan"));
    assert(format(STR("{:#G} {:#G}"), inf, nan) == STR("INF NAN"));

    // Width
    assert(format(STR("{:3}"), Float{0}) == STR("  0"));
    assert(format(STR("{:#9G}"), Float{12.2}) == STR("  12.2000"));
    assert(format(STR("{:#12g}"), Float{1'000'000}) == STR(" 1.00000e+06"));

    // Precision
    Float value = 1234.52734375;
    assert(format(STR("{:.4}"), value) == STR("1235"));
    assert(format(STR("{:.1}"), inf) == STR("inf"));
    assert(format(STR("{:.1}"), nan) == STR("nan"));

    assert(format(STR("{:.4a}"), value) == STR("1.34a2p+10"));
    assert(format(STR("{:.4A}"), value) == STR("1.34A2P+10"));

    assert(format(STR("{:.4e}"), value) == STR("1.2345e+03"));
    assert(format(STR("{:.4E}"), value) == STR("1.2345E+03"));

    assert(format(STR("{:.4f}"), value) == STR("1234.5273"));
    assert(format(STR("{:.4F}"), value) == STR("1234.5273"));

    assert(format(STR("{:.4g}"), value) == STR("1235"));
    assert(format(STR("{:.4G}"), value) == STR("1235"));

    assert(format(STR("{:.3000f}"), -numeric_limits<Float>::max()).size()
           == 3002 + numeric_limits<Float>::max_exponent10 + 1);
    assert(format(STR("{:.3000f}"), -numeric_limits<Float>::denorm_min()).size() == 3003);
    assert(format(STR("{:#.3000g}"), -numeric_limits<Float>::max()).size() == 3002);
    assert(format(STR("{:#.3000g}"), -numeric_limits<Float>::denorm_min()).size()
           == 3007 - static_cast<int>(is_same_v<Float, float>));

    for (auto limits : {-numeric_limits<Float>::max(), -numeric_limits<Float>::denorm_min()}) {
        auto fixed3000 = format(STR("{:.3000f}"), limits);
        auto fixed1500 = format(STR("{:.1500f}"), limits);
        assert(fixed1500 == fixed3000.substr(0, fixed1500.size()));
        assert(all_of(fixed3000.begin() + static_cast<int>(fixed1500.size()), fixed3000.end(),
            [](auto ch) { return ch == charT{'0'}; }));
    }

    for (auto limits : {-numeric_limits<Float>::max(), -numeric_limits<Float>::denorm_min()}) {
        auto general3000 = format(STR("{:#.3000g}"), limits);
        auto general1500 = format(STR("{:#.3000g}"), limits);
        assert(general1500 == general3000.substr(0, general1500.size()));
        assert(all_of(general3000.begin() + static_cast<int>(general1500.size()), general3000.end(),
            [](auto ch) { return ch == charT{'0'}; }));
    }

    // Leading zero
    assert(format(STR("{:06}"), Float{0}) == STR("000000"));
    assert(format(STR("{:06}"), Float{1.2}) == STR("0001.2"));
    assert(format(STR("{:06}"), nan) == STR("   nan"));
    assert(format(STR("{:06}"), inf) == STR("   inf"));

    // Locale
#if !defined(_DLL) || _ITERATOR_DEBUG_LEVEL == DEFAULT_IDL_SETTING
    assert(format(locale{"en-US"}, STR("{:L}"), Float{0}) == STR("0"));
    assert(format(locale{"en-US"}, STR("{:Lf}"), Float{0}) == STR("0.000000"));
    assert(format(locale{"en-US"}, STR("{:L}"), Float{100}) == STR("100"));
    assert(format(locale{"en-US"}, STR("{:L}"), Float{100.2345}) == STR("100.2345"));
    assert(format(locale{"en-US"}, STR("{:.4Lf}"), value) == STR("1,234.5273"));
    assert(format(locale{"en-US"}, STR("{:#.4Lg}"), Float{0}) == STR("0.000"));
    assert(format(locale{"en-US"}, STR("{:L}"), nan) == STR("nan"));
    assert(format(locale{"en-US"}, STR("{:L}"), inf) == STR("inf"));

    assert(format(locale{"de_DE"}, STR("{:Lf}"), Float{0}) == STR("0,000000"));
#endif // !defined(_DLL) || _ITERATOR_DEBUG_LEVEL == DEFAULT_IDL_SETTING

    // Type
    assert(format(STR("{:a}"), value) == STR("1.34a1cp+10"));
    assert(format(STR("{:A}"), value) == STR("1.34A1CP+10"));

    assert(format(STR("{:e}"), value) == STR("1.234527e+03"));
    assert(format(STR("{:E}"), value) == STR("1.234527E+03"));

    assert(format(STR("{:f}"), value) == STR("1234.527344"));
    assert(format(STR("{:F}"), value) == STR("1234.527344"));

    assert(format(STR("{:g}"), value) == STR("1234.53"));
    assert(format(STR("{:G}"), value) == STR("1234.53"));
}

template <class charT>
void test_pointer_specs() {
    assert(format(STR("{:}"), nullptr) == STR("0x0"));

    // Sign
    throw_helper(STR("{: }"), nullptr);
    throw_helper(STR("{:+}"), nullptr);
    throw_helper(STR("{:-}"), nullptr);

    // Alternate form
    throw_helper(STR("{:#}"), nullptr);

    // Leading zero
    throw_helper(STR("{:0}"), nullptr);

    // Width
    assert(format(STR("{:5}"), nullptr) == STR("0x0  "));

    // Precision
    throw_helper(STR("{:.5}"), nullptr);

    // Locale
    throw_helper(STR("{:L}"), nullptr);

    // Types
    assert(format(STR("{:p}"), nullptr) == STR("0x0"));
    throw_helper(STR("{:a}"), nullptr);
}

template <class charT>
void test_string_specs() {
    auto cstr = STR("scully");
    auto view = basic_string_view{cstr};

    assert(format(STR("{:}"), cstr) == cstr);
    assert(format(STR("{:}"), view) == cstr);

    // Sign
    throw_helper(STR("{: }"), cstr);
    throw_helper(STR("{:+}"), cstr);
    throw_helper(STR("{:-}"), cstr);

    throw_helper(STR("{: }"), view);
    throw_helper(STR("{:+}"), view);
    throw_helper(STR("{:-}"), view);

    // Alternate form
    throw_helper(STR("{:#}"), cstr);
    throw_helper(STR("{:#}"), view);

    // Leading zero
    throw_helper(STR("{:0}"), cstr);
    throw_helper(STR("{:0}"), view);

    // Width
    assert(format(STR("{:8}"), cstr) == STR("scully  "));
    assert(format(STR("{:8}"), view) == STR("scully  "));

    // Precision
    assert(format(STR("{:.2}"), cstr) == STR("sc"));
    assert(format(STR("{:5.2}"), cstr) == STR("sc   "));
    assert(format(STR("{:5.2}"), cstr) == STR("sc   "));
    assert(format(STR("{:>5.2}"), cstr) == STR("   sc"));

    assert(format(STR("{:.2}"), view) == STR("sc"));
    assert(format(STR("{:5.2}"), view) == STR("sc   "));
    assert(format(STR("{:5.2}"), view) == STR("sc   "));
    assert(format(STR("{:>5.2}"), view) == STR("   sc"));

    // Locale
    throw_helper(STR("{:L}"), cstr);
    throw_helper(STR("{:L}"), view);

    // Types
    assert(format(STR("{:s}"), cstr) == cstr);
    throw_helper(STR("{:a}"), cstr);

    assert(format(STR("{:s}"), view) == cstr);
    throw_helper(STR("{:a}"), view);
}

template <class charT>
void test_spec_replacement_field() {
    test_intergal_specs<charT, int>();
    test_intergal_specs<charT, unsigned int>();
    test_intergal_specs<charT, long long>();
    test_intergal_specs<charT, unsigned long long>();
    test_bool_specs<charT>();
    test_char_specs<charT>();
    test_float_specs<charT, float>();
    test_float_specs<charT, double>();
    test_float_specs<charT, long double>();
    test_pointer_specs<charT>();
    test_string_specs<charT>();
}

int main() {
    test_simple_formatting<char>();
    test_simple_formatting<wchar_t>();

    test_escaped_curls<char>();
    test_escaped_curls<wchar_t>();

    test_simple_replacement_field<char>();
    test_simple_replacement_field<wchar_t>();

    test_multiple_replacement_fields<char>();
    test_multiple_replacement_fields<wchar_t>();

    test_fill_and_align<char>();
    test_fill_and_align<wchar_t>();

    test_spec_replacement_field<char>();
    test_spec_replacement_field<wchar_t>();

    return 0;
}
