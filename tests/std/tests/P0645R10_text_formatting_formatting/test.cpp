// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <climits>
#include <cstdio>
#include <exception>
#include <format>
#include <iterator>
#include <limits>
#include <list>
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
        (void) format(fmt, vals...);
        assert(false);
    } catch (const format_error&) {
    }
}

template <class charT>
struct move_only_back_inserter {
    back_insert_iterator<basic_string<charT>> it;
    using difference_type = ptrdiff_t;

    bool moved_from = false;

    move_only_back_inserter() = default;
    explicit move_only_back_inserter(basic_string<charT>& str) : it{str} {}

    move_only_back_inserter(const move_only_back_inserter&) = delete;
    move_only_back_inserter& operator=(const move_only_back_inserter&) = delete;

    move_only_back_inserter(move_only_back_inserter&& other) : it(other.it) {
        assert(!exchange(other.moved_from, true));
    }
    move_only_back_inserter& operator=(move_only_back_inserter&& other) {
        assert(!exchange(other.moved_from, true));
        it         = other.it;
        moved_from = false;
        return *this;
    }

    move_only_back_inserter& operator++() {
        assert(!moved_from);
        ++it;
        return *this;
    }

    decltype(auto) operator++(int) {
        assert(!moved_from);
        return it++;
    }

    decltype(auto) operator*() {
        assert(!moved_from);
        return *it;
    }
};

template <class charT>
move_only_back_inserter(basic_string<charT>&) -> move_only_back_inserter<charT>;

// tests for format with no format args or replacement fields
template <class charT>
void test_simple_formatting() {
    basic_string<charT> output_string;

    vformat_to(back_insert_iterator{output_string}, locale::classic(), STR("f"), make_testing_format_args<charT>());
    assert(output_string == STR("f"));

    output_string.clear();
    format_to(move_only_back_inserter{output_string}, STR("{} {} {} {} {} {} {} {} {}"), true, charT{'a'}, 0, 0u, 0.0,
        STR("s"), basic_string_view{STR("sv")}, nullptr, static_cast<void*>(nullptr));
    assert(output_string == STR("true a 0 0 0 s sv 0x0 0x0"));

    output_string.clear();
    format_to(move_only_back_inserter{output_string}, STR("{:} {:} {:} {:} {:} {:} {:} {:} {:}"), true, charT{'a'}, 0,
        0u, 0.0, STR("s"), basic_string_view{STR("sv")}, nullptr, static_cast<void*>(nullptr));
    assert(output_string == STR("true a 0 0 0 s sv 0x0 0x0"));

    output_string.clear();
    format_to_n(move_only_back_inserter{output_string}, 300, STR("{} {} {} {} {} {} {} {} {}"), true, charT{'a'}, 0, 0u,
        0.0, STR("s"), basic_string_view{STR("sv")}, nullptr, static_cast<void*>(nullptr));
    assert(output_string == STR("true a 0 0 0 s sv 0x0 0x0"));

    output_string.clear();
    vformat_to(
        back_insert_iterator{output_string}, locale::classic(), STR("format"), make_testing_format_args<charT>());
    assert(output_string == STR("format"));

    output_string.clear();
    format_to(back_insert_iterator{output_string}, STR("format"));
    assert(output_string == STR("format"));

    output_string.clear();
    format_to(back_insert_iterator{output_string}, locale::classic(), STR("format"));
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

    output_string.clear();
    format_to(back_insert_iterator{output_string}, STR("{}"), STR("f"));
    assert(output_string == STR("f"));

    output_string.clear();
    format_to(back_insert_iterator{output_string}, locale::classic(), STR("{}"), STR("f"));
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
        (void) _Write_aligned(back_inserter(output_string), 2, specs, _Fmt_align::_Left, writer);
        return output_string;
    };

    assert(tester() == STR("AB"));

    specs._Width = 1;
    assert(tester() == STR("AB"));


    specs._Width     = 5;
    specs._Alignment = _Fmt_align::_Left;
    assert(tester() == STR("AB   "));

    specs._Alignment = _Fmt_align::_Right;
    assert(tester() == STR("   AB"));

    specs._Alignment = _Fmt_align::_Center;
    assert(tester() == STR(" AB  "));


    specs._Alignment = _Fmt_align::_Left;
    specs._Fill[0]   = {'*'};
    assert(tester() == STR("AB***"));

    specs._Alignment = _Fmt_align::_Right;
    assert(tester() == STR("***AB"));

    specs._Alignment = _Fmt_align::_Center;
    assert(tester() == STR("*AB**"));
}

template <class charT, class Integral>
void test_integral_specs() {
    assert(format(STR("{:}"), Integral{0}) == STR("0"));

    // Sign
    assert(format(STR("{: }"), Integral{0}) == STR(" 0"));
    assert(format(STR("{:+}"), Integral{0}) == STR("+0"));
    assert(format(STR("{:-}"), Integral{0}) == STR("0"));

    if constexpr (is_signed_v<Integral>) {
        assert(format(STR("{: }"), Integral{-1}) == STR("-1"));
        assert(format(STR("{:+}"), Integral{-1}) == STR("-1"));
        assert(format(STR("{:-}"), Integral{-1}) == STR("-1"));
    }

    assert(format(STR("{: 3}"), Integral{1}) == STR("  1"));
    assert(format(STR("{:+3}"), Integral{1}) == STR(" +1"));
    assert(format(STR("{:-3}"), Integral{1}) == STR("  1"));

    // Alternate form
    assert(format(STR("{:#}"), Integral{0}) == STR("0"));
    assert(format(STR("{:#d}"), Integral{0}) == STR("0"));
    assert(format(STR("{:#c}"), Integral{'a'}) == STR("a"));

    assert(format(STR("{:#b}"), Integral{0}) == STR("0b0"));
    assert(format(STR("{:#B}"), Integral{0}) == STR("0B0"));

    assert(format(STR("{:#o}"), Integral{0}) == STR("0"));
    assert(format(STR("{:#o}"), Integral{1}) == STR("01"));

    assert(format(STR("{:#x}"), Integral{0}) == STR("0x0"));
    assert(format(STR("{:#X}"), Integral{0}) == STR("0X0"));
    assert(format(STR("{:#x}"), Integral{255}) == STR("0xff"));
    assert(format(STR("{:#X}"), Integral{255}) == STR("0XFF"));

    assert(format(STR("{:+#6x}"), Integral{255}) == STR(" +0xff"));

    if constexpr (is_signed_v<Integral>) {
        assert(format(STR("{:#o}"), Integral{-1}) == STR("-01"));
        assert(format(STR("{:#x}"), Integral{-255}) == STR("-0xff"));
        assert(format(STR("{:#X}"), Integral{-255}) == STR("-0XFF"));
    }

    if constexpr (is_same_v<Integral, long long>) {
        assert(format(STR("{:b}"), numeric_limits<long long>::min())
               == STR("-1000000000000000000000000000000000000000000000000000000000000000"));
    }

    // Leading zero
    assert(format(STR("{:0}"), Integral{0}) == STR("0"));
    assert(format(STR("{:03}"), Integral{0}) == STR("000"));
    assert(format(STR("{:+03}"), Integral{0}) == STR("+00"));
    assert(format(STR("{:<03}"), Integral{0}) == STR("0  "));
    assert(format(STR("{:>03}"), Integral{0}) == STR("  0"));
    assert(format(STR("{:+#06X}"), Integral{5}) == STR("+0X005"));

    // Width
    assert(format(STR("{:3}"), Integral{0}) == STR("  0"));

    // Precision
    throw_helper(STR("{:.1}"), Integral{0});

    // Locale
#if !defined(_DLL) || _ITERATOR_DEBUG_LEVEL == DEFAULT_IDL_SETTING
    assert(format(locale{"en-US"}, STR("{:L}"), Integral{0}) == STR("0"));
    assert(format(locale{"en-US"}, STR("{:L}"), Integral{100}) == STR("100"));
    assert(format(locale{"en-US"}, STR("{:L}"), Integral{1'000}) == STR("1,000"));
    assert(format(locale{"en-US"}, STR("{:L}"), Integral{10'000}) == STR("10,000"));
    assert(format(locale{"en-US"}, STR("{:L}"), Integral{100'000}) == STR("100,000"));
    assert(format(locale{"en-US"}, STR("{:L}"), Integral{1'000'000}) == STR("1,000,000"));
    assert(format(locale{"en-US"}, STR("{:L}"), Integral{10'000'000}) == STR("10,000,000"));
    assert(format(locale{"en-US"}, STR("{:L}"), Integral{100'000'000}) == STR("100,000,000"));

    assert(format(locale{"en-US"}, STR("{:Lx}"), Integral{0x123'abc}) == STR("123,abc"));
    assert(format(locale{"en-US"}, STR("{:6L}"), Integral{1'000}) == STR(" 1,000"));

    assert(format(locale{"hi-IN"}, STR("{:L}"), Integral{10'000'000}) == STR("1,00,00,000"));
    assert(format(locale{"hi-IN"}, STR("{:L}"), Integral{100'000'000}) == STR("10,00,00,000"));

    assert(format(locale{"hi-IN"}, STR("{:Lx}"), Integral{0x123'abc}) == STR("1,23,abc"));
#endif // !defined(_DLL) || _ITERATOR_DEBUG_LEVEL == DEFAULT_IDL_SETTING

    // Type
    assert(format(STR("{:b}"), Integral{0}) == STR("0"));
    assert(format(STR("{:b}"), Integral{100}) == STR("1100100"));

    assert(format(STR("{:d}"), Integral{100}) == STR("100"));

    throw_helper(STR("{:c}"), Integral{numeric_limits<charT>::max()} + 1);
    if constexpr (is_signed_v<Integral>) {
        throw_helper(STR("{:c}"), Integral{numeric_limits<charT>::min()} - 1);
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


    assert(format(STR("{:#.2g}"), 0.5) == STR("0.50"));
    assert(format(STR("{:#.1g}"), 0.5) == STR("0.5"));
    assert(format(STR("{:#.3g}"), 0.5) == STR("0.500"));
    assert(format(STR("{:#.3g}"), 0.05) == STR("0.0500"));
    assert(format(STR("{:#.3g}"), 0.0005) == STR("0.000500"));
    assert(format(STR("{:#.3g}"), 0.00005) == STR("5.00e-05"));
    assert(format(STR("{:#.2g}"), 0.0999) == STR("0.10"));
    assert(format(STR("{:#.3g}"), 0.000470) == STR("0.000470"));

    assert(format(STR("{:#} {:#}"), inf, nan) == STR("inf nan"));
    assert(format(STR("{:#a} {:#a}"), inf, nan) == STR("inf nan"));
    assert(format(STR("{:#A} {:#A}"), inf, nan) == STR("INF NAN"));
    assert(format(STR("{:#e} {:#e}"), inf, nan) == STR("inf nan"));
    assert(format(STR("{:#E} {:#E}"), inf, nan) == STR("INF NAN"));
    assert(format(STR("{:#f} {:#f}"), inf, nan) == STR("inf nan"));
    assert(format(STR("{:#F} {:#F}"), inf, nan) == STR("INF NAN"));
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

    // Test type specifiers together with precision beyond _Max_precision
    if constexpr (is_same_v<charT, char>) {
        charT buffer[2048];

        string_view expected{buffer, to_chars(begin(buffer), end(buffer), value, chars_format::general, 2000).ptr};
        assert(format("{:.2000}", value) == expected);

        expected = {buffer, to_chars(begin(buffer), end(buffer), value, chars_format::hex, 2000).ptr};
        assert(format("{:.2000a}", value) == expected);

        expected = {buffer, to_chars(begin(buffer), end(buffer), value, chars_format::scientific, 2000).ptr};
        assert(format("{:.2000e}", value) == expected);

        expected = {buffer, to_chars(begin(buffer), end(buffer), value, chars_format::fixed, 2000).ptr};
        assert(format("{:.2000f}", value) == expected);

        expected = {buffer, to_chars(begin(buffer), end(buffer), value, chars_format::general, 2000).ptr};
        assert(format("{:.2000g}", value) == expected);

        // Ensure that we behave correctly regarding natural numbers that do not need a decimal point
        expected = {buffer, to_chars(begin(buffer), end(buffer), 1.0, chars_format::general, 2000).ptr};
        assert(format("{:.2000}", 1.0) == expected);

        expected = {buffer, to_chars(begin(buffer), end(buffer), 1.0, chars_format::hex, 2000).ptr};
        assert(format("{:.2000a}", 1.0) == expected);

        expected = {buffer, to_chars(begin(buffer), end(buffer), 1.0, chars_format::scientific, 2000).ptr};
        assert(format("{:.2000e}", 1.0) == expected);

        expected = {buffer, to_chars(begin(buffer), end(buffer), 1.0, chars_format::fixed, 2000).ptr};
        assert(format("{:.2000f}", 1.0) == expected);

        expected = {buffer, to_chars(begin(buffer), end(buffer), 1.0, chars_format::general, 2000).ptr};
        assert(format("{:.2000g}", 1.0) == expected);

        // Ensure that we behave correctly regarding natural numbers that do not need a decimal point but we require it
        // via #
        assert(format("{:#}", 1.0) == string("1."));
        assert(format("{:#a}", 1.0) == string("1.p+0"));
        assert(format("{:#e}", 1.0) == string("1.000000e+00"));
        assert(format("{:#f}", 1.0) == string("1.000000"));
        assert(format("{:#g}", 1.0) == string("1.00000"));
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

    assert(format(locale{"de-DE"}, STR("{:Lf}"), Float{0}) == STR("0,000000"));
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

    assert(format(STR("{:a} {:a}"), inf, nan) == STR("inf nan"));
    assert(format(STR("{:A} {:A}"), inf, nan) == STR("INF NAN"));
    assert(format(STR("{:e} {:e}"), inf, nan) == STR("inf nan"));
    assert(format(STR("{:E} {:E}"), inf, nan) == STR("INF NAN"));
    assert(format(STR("{:f} {:f}"), inf, nan) == STR("inf nan"));
    assert(format(STR("{:F} {:F}"), inf, nan) == STR("INF NAN"));
    assert(format(STR("{:g} {:g}"), inf, nan) == STR("inf nan"));
    assert(format(STR("{:G} {:G}"), inf, nan) == STR("INF NAN"));
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
    test_integral_specs<charT, int>();
    test_integral_specs<charT, unsigned int>();
    test_integral_specs<charT, long long>();
    test_integral_specs<charT, unsigned long long>();
    test_bool_specs<charT>();
    test_char_specs<charT>();
    test_float_specs<charT, float>();
    test_float_specs<charT, double>();
    test_float_specs<charT, long double>();
    test_pointer_specs<charT>();
    test_string_specs<charT>();
}

template <class charT, class... Args>
void test_size_helper(const size_t expected_size, const basic_string_view<charT> fmt, const Args&... args) {
    assert(formatted_size(fmt, args...) == expected_size);
    assert(formatted_size(locale::classic(), fmt, args...) == expected_size);

    const auto signed_size = static_cast<ptrdiff_t>(expected_size);
    basic_string<charT> str;
    {
        str.resize(expected_size);
        const auto res = format_to_n(str.begin(), signed_size, fmt, args...);
        assert(res.size == signed_size);
        assert(res.out - str.begin() == signed_size);
        assert(res.out == str.end());
        assert(format(fmt, args...) == str);

        basic_string<charT> locale_str;
        locale_str.resize(expected_size);
        format_to_n(locale_str.begin(), signed_size, locale::classic(), fmt, args...);
        assert(str == locale_str);
        assert(locale_str.size() == expected_size);
    }
    basic_string<charT> half_str;
    {
        const auto half_size = expected_size / 2;
        half_str.resize(half_size);
        const auto res = format_to_n(half_str.begin(), static_cast<ptrdiff_t>(half_size), fmt, args...);
        assert(res.size == signed_size);
        assert(static_cast<size_t>(res.out - half_str.begin()) == half_size);
        assert(res.out == half_str.end());
    }
    assert(str.starts_with(half_str));
}

template <class charT>
void test_size() {
    test_size_helper<charT>(3, STR("{}"), 123);
    test_size_helper<charT>(6, STR("{}"), 3.1415);
    test_size_helper<charT>(8, STR("{:8}"), STR("scully"));
}

// The libfmt_ tests are derived from tests in
// libfmt, Copyright (c) 2012 - present, Victor Zverovich
// See NOTICE.txt for more information.

template <class charT>
void libfmt_formatter_test_escape() {
    assert(format(STR("{{")) == STR("{"));
    assert(format(STR("before {{")) == STR("before {"));
    assert(format(STR("{{ after")) == STR("{ after"));
    assert(format(STR("before {{ after")) == STR("before { after"));
    assert(format(STR("}}")) == STR("}"));
    assert(format(STR("before }}")) == STR("before }"));
    assert(format(STR("}} after")) == STR("} after"));
    assert(format(STR("before }} after")) == STR("before } after"));
    assert(format(STR("{{}}")) == STR("{}"));
    assert(format(STR("{{{0}}}"), 42) == STR("{42}"));
}

template <class charT>
void libfmt_formatter_test_args_in_different_position() {
    assert(format(STR("{0}"), 42) == STR("42"));
    assert(format(STR("before {0}"), 42) == STR("before 42"));
    assert(format(STR("{0} after"), 42) == STR("42 after"));
    assert(format(STR("before {0} after"), 42) == STR("before 42 after"));
    assert(format(STR("{0} = {1}"), STR("answer"), 42) == STR("answer = 42"));
    assert(format(STR("{1} is the {0}"), STR("answer"), 42) == STR("42 is the answer"));
    assert(format(STR("{0}{1}{0}"), STR("abra"), STR("cad")) == STR("abracadabra"));
}

template <class charT>
void libfmt_formatter_test_auto_arg_index() {
    assert(format(STR("{}{}{}"), 'a', 'b', 'c') == STR("abc"));
    throw_helper(STR("{0}{}"), 'a', 'b');
    throw_helper(STR("{}{0}"), 'a', 'b');
    assert(format(STR("{:.{}}"), 1.2345, 2) == STR("1.2"));
    throw_helper(STR("{0}:.{}"), 1.2345, 2);
    throw_helper(STR("{:.{0}}"), 1.2345, 2);
    throw_helper(STR("{}"));
}

template <class charT>
void libfmt_formatter_test_left_align() {
    assert(format(STR("{0:<4}"), 42) == STR("42  "));
    assert(format(STR("{0:<4o}"), 042) == STR("42  "));
    assert(format(STR("{0:<4x}"), 0x42) == STR("42  "));
    assert(format(STR("{0:<5}"), -42) == STR("-42  "));
    assert(format(STR("{0:<5}"), 42u) == STR("42   "));
    assert(format(STR("{0:<5}"), -42l) == STR("-42  "));
    assert(format(STR("{0:<5}"), 42ul) == STR("42   "));
    assert(format(STR("{0:<5}"), -42ll) == STR("-42  "));
    assert(format(STR("{0:<5}"), 42ull) == STR("42   "));
    assert(format(STR("{0:<5}"), -42.0) == STR("-42  "));
    assert(format(STR("{0:<5}"), -42.0l) == STR("-42  "));
    assert(format(STR("{0:<5}"), 'c') == STR("c    "));
    assert(format(STR("{0:<5}"), STR("abc")) == STR("abc  "));
    assert(format(STR("{0:<8}"), reinterpret_cast<void*>(0xface)) == STR("0xface  "));
}

template <class charT>
void libfmt_formatter_test_right_align() {
    assert(format(STR("{0:>4}"), 42) == STR("  42"));
    assert(format(STR("{0:>4o}"), 042) == STR("  42"));
    assert(format(STR("{0:>4x}"), 0x42) == STR("  42"));
    assert(format(STR("{0:>5}"), -42) == STR("  -42"));
    assert(format(STR("{0:>5}"), 42u) == STR("   42"));
    assert(format(STR("{0:>5}"), -42l) == STR("  -42"));
    assert(format(STR("{0:>5}"), 42ul) == STR("   42"));
    assert(format(STR("{0:>5}"), -42ll) == STR("  -42"));
    assert(format(STR("{0:>5}"), 42ull) == STR("   42"));
    assert(format(STR("{0:>5}"), -42.0) == STR("  -42"));
    assert(format(STR("{0:>5}"), -42.0l) == STR("  -42"));
    assert(format(STR("{0:>5}"), 'c') == STR("    c"));
    assert(format(STR("{0:>5}"), STR("abc")) == STR("  abc"));
    assert(format(STR("{0:>8}"), reinterpret_cast<void*>(0xface)) == STR("  0xface"));
}

template <class charT>
void libfmt_formatter_test_center_align() {
    assert(format(STR("{0:^5}"), 42) == STR(" 42  "));
    assert(format(STR("{0:^5o}"), 042) == STR(" 42  "));
    assert(format(STR("{0:^5x}"), 0x42) == STR(" 42  "));
    assert(format(STR("{0:^5}"), -42) == STR(" -42 "));
    assert(format(STR("{0:^5}"), 42u) == STR(" 42  "));
    assert(format(STR("{0:^5}"), -42l) == STR(" -42 "));
    assert(format(STR("{0:^5}"), 42ul) == STR(" 42  "));
    assert(format(STR("{0:^5}"), -42ll) == STR(" -42 "));
    assert(format(STR("{0:^5}"), 42ull) == STR(" 42  "));
    assert(format(STR("{0:^5}"), -42.0) == STR(" -42 "));
    assert(format(STR("{0:^5}"), -42.0l) == STR(" -42 "));
    assert(format(STR("{0:^5}"), 'c') == STR("  c  "));
    assert(format(STR("{0:^6}"), STR("abc")) == STR(" abc  "));
    assert(format(STR("{0:^8}"), reinterpret_cast<void*>(0xface)) == STR(" 0xface "));
}

template <class charT>
void libfmt_formatter_test_fill() {
    throw_helper(STR("{0:{<5}"), 'c');
    throw_helper(STR("{0:{<5}}"), 'c');
    assert(format(STR("{0:*>4}"), 42) == STR("**42"));
    assert(format(STR("{0:*>5}"), -42) == STR("**-42"));
    assert(format(STR("{0:*>5}"), 42u) == STR("***42"));
    assert(format(STR("{0:*>5}"), -42l) == STR("**-42"));
    assert(format(STR("{0:*>5}"), 42ul) == STR("***42"));
    assert(format(STR("{0:*>5}"), -42ll) == STR("**-42"));
    assert(format(STR("{0:*>5}"), 42ull) == STR("***42"));
    assert(format(STR("{0:*>5}"), -42.0) == STR("**-42"));
    assert(format(STR("{0:*>5}"), -42.0l) == STR("**-42"));
    assert(format(STR("{0:*<5}"), 'c') == STR("c****"));
    assert(format(STR("{0:*<5}"), STR("abc")) == STR("abc**"));
    assert(format(STR("{0:*>8}"), reinterpret_cast<void*>(0xface)) == STR("**0xface"));
    assert(format(STR("{:}="), STR("meow")) == STR("meow="));
    assert(format(basic_string_view<charT>(STR("{:\0>4}"), 6), '*') == basic_string<charT>(STR("\0\0\0*"), 4));
    throw_helper(STR("{:\x80\x80\x80\x80\x80>}"), 0);
}

template <class charT>
void libfmt_formatter_test_plus_sign() {
    assert(format(STR("{0:+}"), 42) == STR("+42"));
    assert(format(STR("{0:+}"), -42) == STR("-42"));
    assert(format(STR("{0:+}"), 42) == STR("+42"));
    assert(format(STR("{0:+}"), 42u) == STR("+42")); // behavior differs from libfmt, but conforms
    assert(format(STR("{0:+}"), 42l) == STR("+42"));
    assert(format(STR("{0:+}"), 42ul) == STR("+42")); // behavior differs from libfmt, but conforms
    assert(format(STR("{0:+}"), 42ll) == STR("+42"));
    assert(format(STR("{0:+}"), 42ull) == STR("+42")); // behavior differs from libfmt, but conforms
    assert(format(STR("{0:+}"), 42.0) == STR("+42"));
    assert(format(STR("{0:+}"), 42.0l) == STR("+42"));
    throw_helper(STR("{0:+"), 'c');
    throw_helper(STR("{0:+}"), 'c');
    throw_helper(STR("{0:+}"), STR("abc"));
    throw_helper(STR("{0:+}"), reinterpret_cast<void*>(0x42));
}

template <class charT>
void libfmt_formatter_test_minus_sign() {
    assert(format(STR("{0:-}"), 42) == STR("42"));
    assert(format(STR("{0:-}"), -42) == STR("-42"));
    assert(format(STR("{0:-}"), 42) == STR("42"));
    assert(format(STR("{0:-}"), 42u) == STR("42")); // behavior differs from libfmt, but conforms
    assert(format(STR("{0:-}"), 42l) == STR("42"));
    assert(format(STR("{0:-}"), 42ul) == STR("42")); // behavior differs from libfmt, but conforms
    assert(format(STR("{0:-}"), 42ll) == STR("42"));
    assert(format(STR("{0:-}"), 42ull) == STR("42")); // behavior differs from libfmt, but conforms
    assert(format(STR("{0:-}"), 42.0) == STR("42"));
    assert(format(STR("{0:-}"), 42.0l) == STR("42"));
    throw_helper(STR("{0:-"), 'c');
    throw_helper(STR("{0:-}"), 'c');
    throw_helper(STR("{0:-}"), STR("abc"));
    throw_helper(STR("{0:-}"), reinterpret_cast<void*>(0x42));
}

template <class charT>
void libfmt_formatter_test_space_sign() {
    assert(format(STR("{0: }"), 42) == STR(" 42"));
    assert(format(STR("{0: }"), -42) == STR("-42"));
    assert(format(STR("{0: }"), 42) == STR(" 42"));
    assert(format(STR("{0: }"), 42u) == STR(" 42")); // behavior differs from libfmt, but conforms
    assert(format(STR("{0: }"), 42l) == STR(" 42"));
    assert(format(STR("{0: }"), 42ul) == STR(" 42")); // behavior differs from libfmt, but conforms
    assert(format(STR("{0: }"), 42ll) == STR(" 42"));
    assert(format(STR("{0: }"), 42ull) == STR(" 42")); // behavior differs from libfmt, but conforms
    assert(format(STR("{0: }"), 42.0) == STR(" 42"));
    assert(format(STR("{0: }"), 42.0l) == STR(" 42"));
    throw_helper(STR("{0: "), 'c');
    throw_helper(STR("{0: }"), 'c');
    throw_helper(STR("{0: }"), STR("abc"));
    throw_helper(STR("{0: }"), reinterpret_cast<void*>(0x42));
}

template <class charT>
void libfmt_formatter_test_hash_flag() {
    assert(format(STR("{0:#}"), 42) == STR("42"));
    assert(format(STR("{0:#}"), -42) == STR("-42"));
    assert(format(STR("{0:#b}"), 42) == STR("0b101010"));
    assert(format(STR("{0:#B}"), 42) == STR("0B101010"));
    assert(format(STR("{0:#b}"), -42) == STR("-0b101010"));
    assert(format(STR("{0:#x}"), 0x42) == STR("0x42"));
    assert(format(STR("{0:#X}"), 0x42) == STR("0X42"));
    assert(format(STR("{0:#x}"), -0x42) == STR("-0x42"));
    assert(format(STR("{0:#o}"), 0) == STR("0"));
    assert(format(STR("{0:#o}"), 042) == STR("042"));
    assert(format(STR("{0:#o}"), -042) == STR("-042"));
    assert(format(STR("{0:#}"), 42u) == STR("42"));
    assert(format(STR("{0:#x}"), 0x42u) == STR("0x42"));
    assert(format(STR("{0:#o}"), 042u) == STR("042"));

    assert(format(STR("{0:#}"), -42l) == STR("-42"));
    assert(format(STR("{0:#x}"), 0x42l) == STR("0x42"));
    assert(format(STR("{0:#x}"), -0x42l) == STR("-0x42"));
    assert(format(STR("{0:#o}"), 042l) == STR("042"));
    assert(format(STR("{0:#o}"), -042l) == STR("-042"));
    assert(format(STR("{0:#}"), 42ul) == STR("42"));
    assert(format(STR("{0:#x}"), 0x42ul) == STR("0x42"));
    assert(format(STR("{0:#o}"), 042ul) == STR("042"));

    assert(format(STR("{0:#}"), -42ll) == STR("-42"));
    assert(format(STR("{0:#x}"), 0x42ll) == STR("0x42"));
    assert(format(STR("{0:#x}"), -0x42ll) == STR("-0x42"));
    assert(format(STR("{0:#o}"), 042ll) == STR("042"));
    assert(format(STR("{0:#o}"), -042ll) == STR("-042"));
    assert(format(STR("{0:#}"), 42ull) == STR("42"));
    assert(format(STR("{0:#x}"), 0x42ull) == STR("0x42"));
    assert(format(STR("{0:#o}"), 042ull) == STR("042"));

    assert(format(STR("{0:#}"), -42.0) == STR("-42.")); // behavior differs from libfmt, but conforms
    assert(format(STR("{0:#}"), -42.0l) == STR("-42.")); // behavior differs from libfmt, but conforms
    assert(format(STR("{:#.0e}"), 42.0) == STR("4.e+01"));
    assert(format(STR("{:#.0f}"), 0.01) == STR("0."));
    assert(format(STR("{:#.2g}"), 0.5) == STR("0.50"));
    assert(format(STR("{:#.0f}"), 0.5) == STR("0."));
    throw_helper(STR("{0:#"), 'c');
    throw_helper(STR("{0:#}"), 'c');
    throw_helper(STR("{0:#}"), STR("abc"));
    throw_helper(STR("{0:#}"), reinterpret_cast<void*>(0x42));
}

template <class charT>
void libfmt_formatter_test_zero_flag() {
    assert(format(STR("{0:0}"), 42) == STR("42"));
    assert(format(STR("{0:05}"), -42) == STR("-0042"));
    assert(format(STR("{0:05}"), 42u) == STR("00042"));
    assert(format(STR("{0:05}"), -42l) == STR("-0042"));
    assert(format(STR("{0:05}"), 42ul) == STR("00042"));
    assert(format(STR("{0:05}"), -42ll) == STR("-0042"));
    assert(format(STR("{0:05}"), 42ull) == STR("00042"));
    assert(format(STR("{0:07}"), -42.0) == STR("-000042"));
    assert(format(STR("{0:07}"), -42.0l) == STR("-000042"));
    throw_helper(STR("{0:0"), 'c');
    throw_helper(STR("{0:05}"), 'c');
    throw_helper(STR("{0:05}"), STR("abc"));
    throw_helper(STR("{0:05}"), reinterpret_cast<void*>(0x42));
}

template <class charT>
void libfmt_formatter_test_runtime_width() {
    throw_helper(STR("{0:{"), 0);
    throw_helper(STR("{0:{}"), 0);
    throw_helper(STR("{0:{?}}"), 0);
    throw_helper(STR("{0:{1}}"), 0);
    throw_helper(STR("{0:{0:}}"), 0);
    throw_helper(STR("{0:{1}}"), 0, -1);
    throw_helper(STR("{0:{1}}"), 0, (INT_MAX + 1u));
    throw_helper(STR("{0:{1}}"), 0, -1l);
    throw_helper(STR("{0:{1}}"), 0, (INT_MAX + 1ul));
    assert(format(STR("{0:{1}}"), 0, '0')
           == STR("                                               0")); // behavior differs from libfmt, but conforms
    throw_helper(STR("{0:{1}}"), 0, 0.0);

    assert(format(STR("{0:{1}}"), -42, 4) == STR(" -42"));
    assert(format(STR("{0:{1}}"), 42u, 5) == STR("   42"));
    assert(format(STR("{0:{1}}"), -42l, 6) == STR("   -42"));
    assert(format(STR("{0:{1}}"), 42ul, 7) == STR("     42"));
    assert(format(STR("{0:{1}}"), -42ll, 6) == STR("   -42"));
    assert(format(STR("{0:{1}}"), 42ull, 7) == STR("     42"));
    assert(format(STR("{0:{1}}"), -1.23, 8) == STR("   -1.23"));
    assert(format(STR("{0:{1}}"), -1.23l, 9) == STR("    -1.23"));
    assert(format(STR("{0:{1}}"), reinterpret_cast<void*>(0xcafe), 10)
           == STR("0xcafe    ")); // behavior differs from libfmt, but conforms
    assert(format(STR("{0:{1}}"), 'x', 11) == STR("x          "));
    assert(format(STR("{0:{1}}"), STR("str"), 12) == STR("str         "));
}

template <class charT>
void libfmt_formatter_test_runtime_precision() {
    throw_helper(STR("{0:.{"), 0);
    throw_helper(STR("{0:.{}"), 0);
    throw_helper(STR("{0:.{?}}"), 0);
    throw_helper(STR("{0:.{1}"), 0, 0);
    throw_helper(STR("{0:.{1}}"), 0);
    throw_helper(STR("{0:.{0:}}"), 0);
    throw_helper(STR("{0:.{1}}"), 0, -1);
    throw_helper(STR("{0:.{1}}"), 0, (INT_MAX + 1u));
    throw_helper(STR("{0:.{1}}"), 0, -1l);
    throw_helper(STR("{0:.{1}}"), 0, (INT_MAX + 1ul));
    throw_helper(STR("{0:.{1}}"), 0, '0');
    throw_helper(STR("{0:.{1}}"), 0, 0.0);
    throw_helper(STR("{0:.{1}}"), 42, 2);
    throw_helper(STR("{0:.{1}f}"), 42, 2);
    throw_helper(STR("{0:.{1}}"), 42u, 2);
    throw_helper(STR("{0:.{1}f}"), 42u, 2);
    throw_helper(STR("{0:.{1}}"), 42l, 2);
    throw_helper(STR("{0:.{1}f}"), 42l, 2);
    throw_helper(STR("{0:.{1}}"), 42ul, 2);
    throw_helper(STR("{0:.{1}f}"), 42ul, 2);
    throw_helper(STR("{0:.{1}}"), 42ll, 2);
    throw_helper(STR("{0:.{1}f}"), 42ll, 2);
    throw_helper(STR("{0:.{1}}"), 42ull, 2);
    throw_helper(STR("{0:.{1}f}"), 42ull, 2);
    throw_helper(STR("{0:3.{1}}"), 'x', 0);
    assert(format(STR("{0:.{1}}"), 1.2345, 2) == STR("1.2"));
    assert(format(STR("{1:.{0}}"), 2, 1.2345l) == STR("1.2"));
    throw_helper(STR("{0:.{1}}"), reinterpret_cast<void*>(0xcafe), 2);
    throw_helper(STR("{0:.{1}f}"), reinterpret_cast<void*>(0xcafe), 2);
    assert(format(STR("{0:.{1}}"), STR("str"), 2) == STR("st"));
}

template <class charT>
void test_locale_specific_formatting_without_locale() {
#ifndef _MSVC_INTERNAL_TESTING // TRANSITION, the Windows version on Contest VMs doesn't always understand ".UTF-8"
#if !defined(_DLL) || _ITERATOR_DEBUG_LEVEL == DEFAULT_IDL_SETTING
    locale loc("en-US.UTF-8");
    locale::global(loc);
    assert(format(STR("{:L}"), 12345) == STR("12,345"));
    locale::global(locale::classic());
#endif // !defined(_DLL) || _ITERATOR_DEBUG_LEVEL == DEFAULT_IDL_SETTING
#endif // _MSVC_INTERNAL_TESTING
}

template <class charT>
void test_slow_append_path() {
    const charT* const hello_world = STR("Hello world");

    // test format_to with a back_insert_iterator to a list, which will pick the slow path.
    list<charT> list_output;
    format_to(back_inserter(list_output), STR("{}"), hello_world);
    assert((basic_string<charT>{list_output.begin(), list_output.end()} == hello_world));

    // test format_to with a normal iterator to a string, which will also pick the _Copy_unchecked path.
    basic_string<charT> str;
    str.resize(char_traits<charT>::length(hello_world));
    format_to(str.begin(), STR("{}"), hello_world);
    assert(str == hello_world);
}

void test() {
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

    test_size<char>();
    test_size<wchar_t>();

    libfmt_formatter_test_escape<char>();
    libfmt_formatter_test_escape<wchar_t>();

    libfmt_formatter_test_args_in_different_position<char>();
    libfmt_formatter_test_args_in_different_position<wchar_t>();

    libfmt_formatter_test_auto_arg_index<char>();
    libfmt_formatter_test_auto_arg_index<wchar_t>();

    libfmt_formatter_test_left_align<char>();
    libfmt_formatter_test_left_align<wchar_t>();

    libfmt_formatter_test_right_align<char>();
    libfmt_formatter_test_right_align<wchar_t>();

    libfmt_formatter_test_center_align<char>();
    libfmt_formatter_test_center_align<wchar_t>();

    libfmt_formatter_test_fill<char>();
    libfmt_formatter_test_fill<wchar_t>();

    libfmt_formatter_test_plus_sign<char>();
    libfmt_formatter_test_plus_sign<wchar_t>();

    libfmt_formatter_test_minus_sign<char>();
    libfmt_formatter_test_minus_sign<wchar_t>();

    libfmt_formatter_test_space_sign<char>();
    libfmt_formatter_test_space_sign<wchar_t>();

    libfmt_formatter_test_hash_flag<char>();
    libfmt_formatter_test_hash_flag<wchar_t>();

    libfmt_formatter_test_zero_flag<char>();
    libfmt_formatter_test_zero_flag<wchar_t>();

    libfmt_formatter_test_runtime_width<char>();
    libfmt_formatter_test_runtime_width<wchar_t>();

    libfmt_formatter_test_runtime_precision<char>();
    libfmt_formatter_test_runtime_precision<wchar_t>();

    test_locale_specific_formatting_without_locale<char>();
    test_locale_specific_formatting_without_locale<wchar_t>();

    test_slow_append_path<char>();
    test_slow_append_path<wchar_t>();
}

int main() {
    test();
}
