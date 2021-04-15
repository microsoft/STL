// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <chrono>
#include <concepts>
#include <format>
#include <iostream>
#include <stdio.h>
#include <string_view>
#include <type_traits>
#include <utility>

using namespace std;
using namespace chrono;

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
#define STR(Literal)                  TYPED_LITERAL(CharT, Literal)

template <typename CharT>
struct testing_callbacks {
    _Fmt_align expected_alignment = _Fmt_align::_None;
    basic_string_view<CharT> expected_fill;
    int expected_width                   = -1;
    size_t expected_dynamic_width        = static_cast<size_t>(-1);
    bool expected_auto_dynamic_width     = false;
    int expected_precision               = -1;
    size_t expected_dynamic_precision    = static_cast<size_t>(-1);
    bool expected_auto_dynamic_precision = false;
    vector<_Chrono_specs<CharT>>& expected_chrono_specs;
    size_t curr_index = 0;

    void _On_align(_Fmt_align aln) {
        assert(aln == expected_alignment);
    }
    void _On_fill(basic_string_view<CharT> str_view) {
        assert(str_view == expected_fill);
    }
    void _On_width(int width) {
        assert(width == expected_width);
    }
    void _On_dynamic_width(size_t id) {
        assert(id == expected_dynamic_width);
    }
    void _On_dynamic_width(_Auto_id_tag) {
        assert(expected_auto_dynamic_width);
    }
    void _On_precision(int pre) {
        assert(pre == expected_precision);
    }
    void _On_dynamic_precision(size_t id) {
        assert(id == expected_dynamic_precision);
    }
    void _On_dynamic_precision(_Auto_id_tag) {
        assert(expected_auto_dynamic_precision);
    }
    void _On_conversion_spec(char mod, CharT type) {
        assert(mod == expected_chrono_specs[curr_index]._Modifier);
        assert(static_cast<char>(type) == expected_chrono_specs[curr_index]._Type);
        assert(expected_chrono_specs[curr_index]._Lit_char == CharT{0}); // not set
        ++curr_index;
    }
    void _On_lit_char(CharT ch) {
        assert(ch == expected_chrono_specs[curr_index]._Lit_char);
        assert(expected_chrono_specs[curr_index]._Modifier == '\0'); // not set
        assert(expected_chrono_specs[curr_index]._Type == '\0'); // not set
        ++curr_index;
    }
};

template <typename CharT, typename callback_type>
void test_parse_helper(const CharT* (*func)(const CharT*, const CharT*, callback_type&&), basic_string_view<CharT> view,
    bool err_expected                                                  = false,
    typename basic_string_view<CharT>::size_type expected_end_position = basic_string_view<CharT>::npos,
    callback_type&& callbacks                                          = {}) {
    try {
        auto end = func(view.data(), view.data() + view.size(), move(callbacks));
        if (expected_end_position != basic_string_view<CharT>::npos) {
            assert(end == view.data() + expected_end_position);
        }
        assert(!err_expected);
    } catch (const format_error&) {
        assert(err_expected);
    }
}

template <typename CharT>
bool test_parse_conversion_spec() {
    auto parse_conv_spec_fn = _Parse_conversion_specs<CharT, testing_callbacks<CharT>>;
    using view_typ          = basic_string_view<CharT>;
    using chrono_spec       = _Chrono_specs<CharT>;

    view_typ s0(TYPED_LITERAL(CharT, "B"));
    view_typ s1(TYPED_LITERAL(CharT, "Ec"));
    view_typ s2(TYPED_LITERAL(CharT, "Od"));
    view_typ s3(TYPED_LITERAL(CharT, "E"));
    view_typ s4(TYPED_LITERAL(CharT, ""));
    view_typ s5(TYPED_LITERAL(CharT, "}"));
    view_typ s6(TYPED_LITERAL(CharT, "E}"));

    vector<chrono_spec> v0{{._Type = 'B'}};
    test_parse_helper(parse_conv_spec_fn, s0, false, view_typ::npos, {.expected_chrono_specs = v0});

    vector<chrono_spec> v1{{._Modifier = 'E', ._Type = 'c'}};
    test_parse_helper(parse_conv_spec_fn, s1, false, view_typ::npos, {.expected_chrono_specs = v1});

    vector<chrono_spec> v2{{._Modifier = 'O', ._Type = 'd'}};
    test_parse_helper(parse_conv_spec_fn, s2, false, view_typ::npos, {.expected_chrono_specs = v2});

    vector<chrono_spec> v{};
    test_parse_helper(parse_conv_spec_fn, s3, true, view_typ::npos, {.expected_chrono_specs = v});
    test_parse_helper(parse_conv_spec_fn, s4, true, view_typ::npos, {.expected_chrono_specs = v});
    test_parse_helper(parse_conv_spec_fn, s5, true, view_typ::npos, {.expected_chrono_specs = v});
    test_parse_helper(parse_conv_spec_fn, s6, true, view_typ::npos, {.expected_chrono_specs = v});

    return true;
}

template <typename CharT>
bool test_parse_chrono_format_specs() {
    auto parse_chrono_format_specs_fn = _Parse_chrono_format_specs<CharT, testing_callbacks<CharT>>;
    using view_typ                    = basic_string_view<CharT>;
    using chrono_spec                 = _Chrono_specs<CharT>;

    view_typ s0(TYPED_LITERAL(CharT, "%Oe"));
    view_typ s1(TYPED_LITERAL(CharT, "lit"));
    view_typ s2(TYPED_LITERAL(CharT, "%H:%M}"));
    view_typ s3(TYPED_LITERAL(CharT, "6%H}"));
    view_typ s4(TYPED_LITERAL(CharT, "*<6hi"));
    view_typ s5(TYPED_LITERAL(CharT, "*^4.4%ymm"));
    view_typ s6(TYPED_LITERAL(CharT, "%H%"));
    view_typ s7(TYPED_LITERAL(CharT, "%H%}"));
    view_typ s8(TYPED_LITERAL(CharT, "%nB%tC%%D"));

    vector<chrono_spec> v0{{._Modifier = 'O', ._Type = 'e'}};
    test_parse_helper(parse_chrono_format_specs_fn, s0, false, s0.size(), {.expected_chrono_specs = v0});

    vector<chrono_spec> v1{{._Lit_char = 'l'}, {._Lit_char = 'i'}, {._Lit_char = 't'}};
    test_parse_helper(parse_chrono_format_specs_fn, s1, true, s1.size(), {.expected_chrono_specs = v1});

    vector<chrono_spec> v2{{._Type = 'H'}, {._Lit_char = ':'}, {._Type = 'M'}};
    test_parse_helper(parse_chrono_format_specs_fn, s2, false, s2.size() - 1, {.expected_chrono_specs = v2});

    vector<chrono_spec> v3{{._Type = 'H'}};
    test_parse_helper(
        parse_chrono_format_specs_fn, s3, false, s3.size() - 1, {.expected_width = 6, .expected_chrono_specs = v3});

    vector<chrono_spec> v8{{._Lit_char = '\n'}, {._Lit_char = 'B'}, {._Lit_char = '\t'}, {._Lit_char = 'C'},
        {._Lit_char = '%'}, {._Lit_char = 'D'}};
    test_parse_helper(parse_chrono_format_specs_fn, s8, false, s8.size(), {.expected_chrono_specs = v8});

    vector<chrono_spec> v4{{._Lit_char = 'h'}, {._Lit_char = 'i'}};
    test_parse_helper(parse_chrono_format_specs_fn, s4, true, s4.size(),
        {.expected_alignment       = _Fmt_align::_Left,
            .expected_fill         = view_typ(TYPED_LITERAL(CharT, "*")),
            .expected_width        = 6,
            .expected_chrono_specs = v4});

    vector<chrono_spec> v5{{._Type = 'y'}, {._Lit_char = 'm'}, {._Lit_char = 'm'}};
    test_parse_helper(parse_chrono_format_specs_fn, s5, false, s5.size(),
        {.expected_alignment       = _Fmt_align::_Center,
            .expected_fill         = view_typ(TYPED_LITERAL(CharT, "*")),
            .expected_width        = 4,
            .expected_precision    = 4,
            .expected_chrono_specs = v5});

    vector<chrono_spec> v{{._Type = 'H'}}; // we don't throw a format_error until we parse the %H
    test_parse_helper(parse_chrono_format_specs_fn, s6, true, view_typ::npos, {.expected_chrono_specs = v});
    test_parse_helper(parse_chrono_format_specs_fn, s7, true, view_typ::npos, {.expected_chrono_specs = v});

    return true;
}

template <class charT, class... Args>
void throw_helper(const basic_string_view<charT> fmt, const Args&... vals) {
    try {
        (void) format(fmt, vals...);
        assert(false);
    } catch (const format_error&) {
    }
}

template <class charT, class... Args>
void throw_helper(const charT* fmt, const Args&... vals) {
    throw_helper(basic_string_view<charT>{fmt}, vals...);
}

template <class charT, class... Args>
void stream_helper(const charT* expect, const Args&... vals) {
    basic_stringstream<charT> stream;
    (stream << ... << vals);
    assert(stream.str() == expect);
    assert(stream);
}

template <class Str>
constexpr void print(Str str) {
    if constexpr (is_same_v<Str, string>) {
        cout << "res: " << str << "\n";
    } else {
        wcout << "res: " << str << "\n";
    }
}

template <typename CharT>
void test_day_formatter() {
    using view_typ = basic_string_view<CharT>;
    using str_typ  = basic_string<CharT>;

    view_typ s0(TYPED_LITERAL(CharT, "{:%d}"));
    view_typ s1(TYPED_LITERAL(CharT, "{:%e}"));
    view_typ s2(TYPED_LITERAL(CharT, "{:%Od}"));
    view_typ s3(TYPED_LITERAL(CharT, "{:%Oe}"));
    view_typ s4(TYPED_LITERAL(CharT, "{}"));
    view_typ s5(TYPED_LITERAL(CharT, "{:=>8}"));
    view_typ s6(TYPED_LITERAL(CharT, "{:lit}"));
    view_typ s7(TYPED_LITERAL(CharT, "{:%d days}"));
    view_typ s8(TYPED_LITERAL(CharT, "{:*^6%dmm}"));

    str_typ a0(TYPED_LITERAL(CharT, "27"));
    str_typ a1(TYPED_LITERAL(CharT, "05"));
    str_typ a2(TYPED_LITERAL(CharT, " 5"));
    str_typ a3(TYPED_LITERAL(CharT, "50 is not a valid day"));
    str_typ a4(TYPED_LITERAL(CharT, "======27"));
    str_typ a5(TYPED_LITERAL(CharT, "======05"));
    str_typ a6(TYPED_LITERAL(CharT, "lit27"));
    str_typ a7(TYPED_LITERAL(CharT, "27 days"));
    str_typ a8(TYPED_LITERAL(CharT, "*27mm*"));

    // 2 digits
    day d0{27};
    auto res = format(s0, d0);
    assert(res == a0);
    res = format(s1, d0);
    assert(res == a0);

    // 1 digit
    day d1{5};
    res = format(s0, d1);
    assert(res == a1);
    res = format(s1, d1);
    assert(res == a2);

    // O modifier
    res = format(s2, d0);
    assert(res == a0);
    res = format(s3, d0);
    assert(res == a0);
    res = format(s2, d1);
    assert(res == a1);
    res = format(s3, d1);
    assert(res == a2);

    // [time.format]/6
    day d2{50};
    res = format(s4, d0);
    assert(res == a0);
    res = format(s4, d2);
    assert(res == a3);

    // width/align
    res = format(s5, d0);
    assert(res == a4);
    res = format(s5, d1);
    assert(res == a5);
    res = format(s5, d2);
    assert(res == a3);

    // chrono-spec must begin with conversion-spec
    throw_helper(s6, d0);

    // lit chars
    res = format(s7, d0);
    assert(res == a7);
    res = format(s8, d0);
    assert(res == a8);

    assert(format(STR("{:%d %d %d}"), day{27}) == STR("27 27 27"));
    assert(format(STR("{:%d}"), day{200}) == STR("200"));
    throw_helper(STR("{:%Ed}"), day{10});
    assert(format(STR("{}"), day{0}) == STR("00 is not a valid day"));

    // Op <<
    stream_helper(STR("00 is not a valid day"), day{0});
    stream_helper(STR("27"), day{27});
    stream_helper(STR("200 is not a valid day"), day{200});
}

template <typename CharT>
void test_month_formatter() {
    assert(format(STR("{}"), month{1}) == STR("Jan"));
    assert(format(STR("{}"), month{12}) == STR("Dec"));
    assert(format(STR("{}"), month{0}) == STR("0 is not a valid month"));
    assert(format(STR("{}"), month{20}) == STR("20 is not a valid month"));

    // Specs
    assert(format(STR("{:%b %h %B}"), month{1}) == STR("Jan Jan January"));
    assert(format(STR("{:%m %Om}"), month{1}) == STR("01 01"));

    // Out of bounds month
    assert(format(STR("{:%m}"), month{0}) == STR("00"));
    throw_helper(STR("{:%b}"), month{0});
    throw_helper(STR("{:%h}"), month{0});
    throw_helper(STR("{:%B}"), month{0});

    // Invalid specs
    throw_helper(STR("{:%A}"), month{1});
    throw_helper(STR("{:%.4}"), month{1});

    // Op <<
    stream_helper(STR("Jan"), month{1});
    stream_helper(STR("Dec"), month{12});
    stream_helper(STR("0 is not a valid month"), month{0});
    stream_helper(STR("20 is not a valid month"), month{20});
}

template <typename CharT>
void test_year_formatter() {
    assert(format(STR("{}"), year{0}) == STR("0000"));
    assert(format(STR("{}"), year{-200}) == STR("-0200"));
    assert(format(STR("{}"), year{121}) == STR("0121"));

    assert(format(STR("{:%Y %y%C}"), year{1912}) == STR("1912 1219"));
    assert(format(STR("{:%Y %y%C}"), year{-1912}) == STR("-1912 88-20"));
    // TRANSITION, add tests for EY Oy Ey EC

    stream_helper(STR("1900"), year{1900});
    stream_helper(STR("2000"), year{2000});
    stream_helper(STR("-32768 is not a valid year"), year{-32768});
}

template <typename CharT>
void test_year_month_day_formatter() {
    year_month_day invalid{year{1234}, month{0}, day{31}};
    assert(format(STR("{}"), year_month_day{year{1900}, month{2}, day{1}}) == STR("1900-02-01"));
    stream_helper(STR("1900-02-01"), year_month_day{year{1900}, month{2}, day{1}});
    stream_helper(STR("1234-00-31 is not a valid date"), invalid);

    assert(format(STR("{:%Y %b %d}"), year_month_day{year{1234}, month{5}, day{6}}) == STR("1234 May 06"));
    assert(format(STR("{:%F %D}"), invalid) == STR("1234-00-31 00/31/34"));
    assert(format(STR("{:%a %A}"), year_month_day{year{1900}, month{1}, day{4}}) == STR("Thu Thursday"));
    assert(format(STR("{:%u %w}"), year_month_day{year{1900}, month{1}, day{4}}) == STR("4 4"));
}

template <typename CharT>
void test_hh_mm_ss_formatter() {
    stream_helper(STR("-01:08:03.007"), hh_mm_ss{-4083007ms});
    stream_helper(STR("01:08:03.007"), hh_mm_ss{4083007ms});
    stream_helper(STR("18:15:45.123"), hh_mm_ss{65745123ms});
    stream_helper(STR("18:15:45"), hh_mm_ss{65745s});
}

int main() {
    test_parse_conversion_spec<char>();
    test_parse_conversion_spec<wchar_t>();

    test_parse_chrono_format_specs<char>();
    test_parse_chrono_format_specs<wchar_t>();

    test_day_formatter<char>();
    test_day_formatter<wchar_t>();

    test_month_formatter<char>();
    test_month_formatter<wchar_t>();

    test_year_formatter<char>();
    test_year_formatter<wchar_t>();

    test_year_month_day_formatter<char>();
    test_year_month_day_formatter<wchar_t>();

    test_hh_mm_ss_formatter<char>();
    test_hh_mm_ss_formatter<wchar_t>();
}
