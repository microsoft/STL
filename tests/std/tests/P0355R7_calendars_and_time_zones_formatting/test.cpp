// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <chrono>
#include <concepts>
#include <format>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
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

template <class CharT, class... Args>
void throw_helper(const basic_string_view<CharT> fmt, const Args&... vals) {
    try {
        (void) format(fmt, vals...);
        assert(false);
    } catch (const format_error&) {
    }
}

template <class CharT, class... Args>
void throw_helper(const CharT* fmt, const Args&... vals) {
    throw_helper(basic_string_view<CharT>{fmt}, vals...);
}

template <class CharT, class... Args>
void stream_helper(const CharT* expect, const Args&... vals) {
    basic_ostringstream<CharT> stream;
    (stream << ... << vals);
    assert(stream.str() == expect);
    assert(stream);
}

template <class Arg, class CharT>
void empty_braces_helper(const Arg& val, const CharT* const expected) {
    // N4885 [time.format]/6: "If the chrono-specs is omitted, the chrono object is formatted
    // as if by streaming it to std::ostringstream os and copying os.str() through the output iterator
    // of the context with additional padding and adjustments as specified by the format specifiers."
    assert(format(STR("{}"), val) == expected);

    stream_helper(expected, val);
}

// FIXME: TEMPORARY CODE FOR WRITING TESTS, REMOVE BEFORE MERGING
template <class Str>
constexpr void print(Str str) {
    if constexpr (is_same_v<Str, string>) {
        cout << "res: " << str << "\n";
    } else {
        wcout << "res: " << str << "\n";
    }
}

template <typename CharT>
void test_clock_formatter() {
    stream_helper(STR("1970-01-01 00:00:00"), sys_seconds{});
    stream_helper(STR("1970-01-01"), sys_days{});
    stream_helper(STR("1970-01-01 00:00:00"), utc_seconds{});
    stream_helper(STR("1958-01-01 00:00:00"), tai_seconds{});
    stream_helper(STR("1980-01-06 00:00:00"), gps_seconds{});
    stream_helper(STR("1601-01-01 00:00:00"), file_time<seconds>{});
    stream_helper(STR("1970-01-01 00:00:00"), local_seconds{});

    assert(format(STR("{:%Z %z %Oz %Ez}"), sys_seconds{}) == STR("UTC +0000 +00:00 +00:00"));
    assert(format(STR("{:%Z %z %Oz %Ez}"), sys_days{}) == STR("UTC +0000 +00:00 +00:00"));
    assert(format(STR("{:%Z %z %Oz %Ez}"), utc_seconds{}) == STR("UTC +0000 +00:00 +00:00"));
    assert(format(STR("{:%Z %z %Oz %Ez}"), tai_seconds{}) == STR("TAI +0000 +00:00 +00:00"));
    assert(format(STR("{:%Z %z %Oz %Ez}"), gps_seconds{}) == STR("GPS +0000 +00:00 +00:00"));
    assert(format(STR("{:%Z %z %Oz %Ez}"), file_time<seconds>{}) == STR("UTC +0000 +00:00 +00:00"));
    throw_helper(STR("{:%Z %z %Oz %Ez}"), local_seconds{});

    assert(format(STR("{:%S}"), utc_clock::from_sys(get_tzdb().leap_seconds.front().date()) - 1s) == STR("60"));
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
    throw_helper(STR("{:%Od}"), day{40});
    throw_helper(STR("{:%Ed}"), day{40});
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
void test_weekday_formatter() {
    weekday invalid{10};
    assert(format(STR("{}"), weekday{3}) == STR("Wed"));
    stream_helper(STR("Wed"), weekday{3});
    stream_helper(STR("10 is not a valid weekday"), invalid);

    assert(format(STR("{:%a %A}"), weekday{6}) == STR("Sat Saturday"));
    assert(format(STR("{:%u %w}"), weekday{6}) == STR("6 6"));
    assert(format(STR("{:%u %w}"), weekday{0}) == STR("7 0"));
}

template <typename CharT>
void test_weekday_indexed_formatter() {
    weekday_indexed invalid1{Tuesday, 10};
    weekday_indexed invalid2{weekday{10}, 3};
    weekday_indexed invalid3{weekday{14}, 9};
    assert(format(STR("{}"), weekday_indexed{Monday, 1}) == STR("Mon[1]"));
    stream_helper(STR("Mon[1]"), weekday_indexed{Monday, 1});
    stream_helper(STR("Tue[10 is not a valid index]"), invalid1);
    stream_helper(STR("10 is not a valid weekday[3]"), invalid2);
    stream_helper(STR("14 is not a valid weekday[9 is not a valid index]"), invalid3);

    assert(format(STR("{:%a %A}"), weekday_indexed{Monday, 2}) == STR("Mon Monday"));
    assert(format(STR("{:%u %w}"), weekday_indexed{Tuesday, 3}) == STR("2 2"));
    assert(format(STR("{:%u %w}"), weekday_indexed{Sunday, 4}) == STR("7 0"));
}

template <typename CharT>
void test_weekday_last_formatter() {
    constexpr weekday_last invalid{weekday{10}};

    empty_braces_helper(Wednesday[last], STR("Wed[last]"));
    empty_braces_helper(invalid, STR("10 is not a valid weekday[last]"));

    assert(format(STR("{:%a %A %u %w}"), Saturday[last]) == STR("Sat Saturday 6 6"));
    assert(format(STR("{:%a %A %u %w}"), Sunday[last]) == STR("Sun Sunday 7 0"));
}

template <typename CharT>
void test_month_day_formatter() {
    stream_helper(STR("Jan/16"), January / 16);
    stream_helper(STR("13 is not a valid month/40 is not a valid day"), month{13} / day{40});

    assert(format(STR("{:%B %d}"), June / 17) == STR("June 17"));
    throw_helper(STR("{:%Y}"), June / 17);
}

template <typename CharT>
void test_month_day_last_formatter() {
    stream_helper(STR("Feb/last"), February / last);

    assert(format(STR("{:%B}"), June / last) == STR("June"));
    assert(format(STR("{:%d}"), June / last) == STR("30"));
    throw_helper(STR("{:%d}"), February / last);
}

template <typename CharT>
void test_month_weekday_formatter() {
    constexpr month_weekday mwd1 = August / Tuesday[3];
    constexpr month_weekday mwd2 = December / Sunday[4];

    constexpr month_weekday invalid1 = March / Friday[9];
    constexpr month_weekday invalid2 = March / weekday{8}[2];
    constexpr month_weekday invalid3 = month{20} / Friday[2];
    constexpr month_weekday invalid4 = month{20} / weekday{8}[9];

    empty_braces_helper(mwd1, STR("Aug/Tue[3]"));
    empty_braces_helper(mwd2, STR("Dec/Sun[4]"));

    empty_braces_helper(invalid1, STR("Mar/Fri[9 is not a valid index]"));
    empty_braces_helper(invalid2, STR("Mar/8 is not a valid weekday[2]"));
    empty_braces_helper(invalid3, STR("20 is not a valid month/Fri[2]"));
    empty_braces_helper(invalid4, STR("20 is not a valid month/8 is not a valid weekday[9 is not a valid index]"));

    assert(format(STR("{:%b %B %h %m %a %A %u %w}"), mwd1) == STR("Aug August Aug 08 Tue Tuesday 2 2"));
    assert(format(STR("{:%b %B %h %m %a %A %u %w}"), mwd2) == STR("Dec December Dec 12 Sun Sunday 7 0"));
}

template <typename CharT>
void test_month_weekday_last_formatter() {
    constexpr month_weekday_last mwdl1 = August / Tuesday[last];
    constexpr month_weekday_last mwdl2 = December / Sunday[last];

    constexpr month_weekday_last invalid1 = March / weekday{8}[last];
    constexpr month_weekday_last invalid2 = month{20} / Friday[last];
    constexpr month_weekday_last invalid3 = month{20} / weekday{8}[last];

    empty_braces_helper(mwdl1, STR("Aug/Tue[last]"));
    empty_braces_helper(mwdl2, STR("Dec/Sun[last]"));

    empty_braces_helper(invalid1, STR("Mar/8 is not a valid weekday[last]"));
    empty_braces_helper(invalid2, STR("20 is not a valid month/Fri[last]"));
    empty_braces_helper(invalid3, STR("20 is not a valid month/8 is not a valid weekday[last]"));

    assert(format(STR("{:%b %B %h %m %a %A %u %w}"), mwdl1) == STR("Aug August Aug 08 Tue Tuesday 2 2"));
    assert(format(STR("{:%b %B %h %m %a %A %u %w}"), mwdl2) == STR("Dec December Dec 12 Sun Sunday 7 0"));
}

template <typename CharT>
void test_year_month_formatter() {
    stream_helper(STR("1444/Oct"), 1444y / October);

    assert(format(STR("{:%Y %B}"), 2000y / July) == STR("2000 July"));
    throw_helper(STR("{:%d}"), 2000y / July);
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
void test_year_month_day_last_formatter() {
    constexpr year_month_day_last ymdl1 = 2021y / April / last;
    constexpr year_month_day_last ymdl2 = 2004y / February / last;

    constexpr year_month_day_last invalid = 1999y / month{20} / last;

    empty_braces_helper(ymdl1, STR("2021/Apr/last"));
    empty_braces_helper(ymdl2, STR("2004/Feb/last"));

    empty_braces_helper(invalid, STR("1999/20 is not a valid month/last"));

    constexpr auto fmt = STR("{:%D %F, %Y %C %y, %b %B %h %m, %d %e, %a %A %u %w}");
    assert(format(fmt, ymdl1) == STR("04/30/21 2021-04-30, 2021 20 21, Apr April Apr 04, 30 30, Fri Friday 5 5"));
    assert(format(fmt, ymdl2) == STR("02/29/04 2004-02-29, 2004 20 04, Feb February Feb 02, 29 29, Sun Sunday 7 0"));
}

template <typename CharT>
void test_year_month_weekday_formatter() {
    constexpr year_month_weekday ymwd1 = 2021y / April / Friday[5];
    constexpr year_month_weekday ymwd2 = 2004y / February / Sunday[5];

    constexpr year_month_weekday invalid1 = 2015y / March / Friday[9];
    constexpr year_month_weekday invalid2 = 2015y / March / weekday{8}[2];
    constexpr year_month_weekday invalid3 = 2015y / month{20} / Friday[2];
    constexpr year_month_weekday invalid4 = 2015y / month{20} / weekday{8}[9];

    empty_braces_helper(ymwd1, STR("2021/Apr/Fri[5]"));
    empty_braces_helper(ymwd2, STR("2004/Feb/Sun[5]"));

    empty_braces_helper(invalid1, STR("2015/Mar/Fri[9 is not a valid index]"));
    empty_braces_helper(invalid2, STR("2015/Mar/8 is not a valid weekday[2]"));
    empty_braces_helper(invalid3, STR("2015/20 is not a valid month/Fri[2]"));
    empty_braces_helper(invalid4, STR("2015/20 is not a valid month/8 is not a valid weekday[9 is not a valid index]"));

    constexpr auto fmt = STR("{:%D %F, %Y %C %y, %b %B %h %m, %d %e, %a %A %u %w}");
    assert(format(fmt, ymwd1) == STR("04/30/21 2021-04-30, 2021 20 21, Apr April Apr 04, 30 30, Fri Friday 5 5"));
    assert(format(fmt, ymwd2) == STR("02/29/04 2004-02-29, 2004 20 04, Feb February Feb 02, 29 29, Sun Sunday 7 0"));
}

template <typename CharT>
void test_year_month_weekday_last_formatter() {
    constexpr year_month_weekday_last ymwdl1 = 2021y / April / Friday[last];
    constexpr year_month_weekday_last ymwdl2 = 2004y / February / Sunday[last];

    constexpr year_month_weekday_last invalid1 = 2015y / March / weekday{8}[last];
    constexpr year_month_weekday_last invalid2 = 2015y / month{20} / Friday[last];
    constexpr year_month_weekday_last invalid3 = 2015y / month{20} / weekday{8}[last];

    empty_braces_helper(ymwdl1, STR("2021/Apr/Fri[last]"));
    empty_braces_helper(ymwdl2, STR("2004/Feb/Sun[last]"));

    empty_braces_helper(invalid1, STR("2015/Mar/8 is not a valid weekday[last]"));
    empty_braces_helper(invalid2, STR("2015/20 is not a valid month/Fri[last]"));
    empty_braces_helper(invalid3, STR("2015/20 is not a valid month/8 is not a valid weekday[last]"));

    constexpr auto fmt = STR("{:%D %F, %Y %C %y, %b %B %h %m, %d %e, %a %A %u %w}");
    assert(format(fmt, ymwdl1) == STR("04/30/21 2021-04-30, 2021 20 21, Apr April Apr 04, 30 30, Fri Friday 5 5"));
    assert(format(fmt, ymwdl2) == STR("02/29/04 2004-02-29, 2004 20 04, Feb February Feb 02, 29 29, Sun Sunday 7 0"));
}

template <typename CharT>
void test_hh_mm_ss_formatter() {
    stream_helper(STR("-01:08:03.007"), hh_mm_ss{-4083007ms});
    stream_helper(STR("01:08:03.007"), hh_mm_ss{4083007ms});
    stream_helper(STR("18:15:45.123"), hh_mm_ss{65745123ms});
    stream_helper(STR("18:15:45"), hh_mm_ss{65745s});

    assert(format(STR("{:%H %I %M %S %r %R %T %p}"), hh_mm_ss{13h + 14min + 15351ms})
           == STR("13 01 14 15.351 13:14:15 13:14 13:14:15.351 PM"));

    assert(format(STR("{:%H %I %M %S %r %R %T %p}"), hh_mm_ss{-13h - 14min - 15351ms})
           == STR("-13 01 14 15.351 13:14:15 13:14 13:14:15.351 PM"));

    throw_helper(STR("{}"), hh_mm_ss{24h});
    throw_helper(STR("{}"), hh_mm_ss{-24h});
    assert(format(STR("{:%M %S}"), hh_mm_ss{27h + 12min + 30s}) == STR("12 30"));
}

void test_exception_classes() {
    { // N4885 [time.zone.exception.nonexist]/4
        string s;

        try {
            (void) zoned_time{"America/New_York", local_days{Sunday[2] / March / 2016} + 2h + 30min};
        } catch (const nonexistent_local_time& e) {
            s = e.what();
        }

        assert(s
               == "2016-03-13 02:30:00 is in a gap between\n"
                  "2016-03-13 02:00:00 EST and\n"
                  "2016-03-13 03:00:00 EDT which are both equivalent to\n"
                  "2016-03-13 07:00:00 UTC");
    }

    { // N4885 [time.zone.exception.ambig]/4
        string s;

        try {
            (void) zoned_time{"America/New_York", local_days{Sunday[1] / November / 2016} + 1h + 30min};
        } catch (const ambiguous_local_time& e) {
            s = e.what();
        }

        assert(s
               == "2016-11-06 01:30:00 is ambiguous. It could be\n"
                  "2016-11-06 01:30:00 EDT == 2016-11-06 05:30:00 UTC or\n"
                  "2016-11-06 01:30:00 EST == 2016-11-06 06:30:00 UTC");
    }
}

int main() {
    test_parse_conversion_spec<char>();
    test_parse_conversion_spec<wchar_t>();

    test_parse_chrono_format_specs<char>();
    test_parse_chrono_format_specs<wchar_t>();

    test_clock_formatter<char>();
    test_clock_formatter<wchar_t>();

    test_day_formatter<char>();
    test_day_formatter<wchar_t>();

    test_month_formatter<char>();
    test_month_formatter<wchar_t>();

    test_year_formatter<char>();
    test_year_formatter<wchar_t>();

    test_weekday_formatter<char>();
    test_weekday_formatter<wchar_t>();

    test_weekday_indexed_formatter<char>();
    test_weekday_indexed_formatter<wchar_t>();

    test_weekday_last_formatter<char>();
    test_weekday_last_formatter<wchar_t>();

    test_month_day_formatter<char>();
    test_month_day_formatter<wchar_t>();

    test_month_day_last_formatter<char>();
    test_month_day_last_formatter<wchar_t>();

    test_month_weekday_formatter<char>();
    test_month_weekday_formatter<wchar_t>();

    test_month_weekday_last_formatter<char>();
    test_month_weekday_last_formatter<wchar_t>();

    test_year_month_formatter<char>();
    test_year_month_formatter<wchar_t>();

    test_year_month_day_formatter<char>();
    test_year_month_day_formatter<wchar_t>();

    test_year_month_day_last_formatter<char>();
    test_year_month_day_last_formatter<wchar_t>();

    test_year_month_weekday_formatter<char>();
    test_year_month_weekday_formatter<wchar_t>();

    test_year_month_weekday_last_formatter<char>();
    test_year_month_weekday_last_formatter<wchar_t>();

    test_hh_mm_ss_formatter<char>();
    test_hh_mm_ss_formatter<wchar_t>();

    test_exception_classes();
}
