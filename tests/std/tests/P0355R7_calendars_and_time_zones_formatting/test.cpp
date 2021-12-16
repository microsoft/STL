// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <chrono>
#include <clocale>
#include <concepts>
#include <format>
#include <iostream>
#include <locale>
#include <sstream>
#include <stdio.h>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <timezone_data.hpp>

using namespace std;
using namespace chrono;

template <typename CharT>
[[nodiscard]] constexpr const CharT* choose_literal(const char* const str, const wchar_t* const wstr) noexcept {
    if constexpr (is_same_v<CharT, char>) {
        return str;
    } else {
        return wstr;
    }
}

#define STR(Literal) (choose_literal<CharT>(Literal, L##Literal))

// Test against IDL mismatch between the DLL which stores the locale and the code which uses it.
#ifdef _DEBUG
#define DEFAULT_IDL_SETTING 2
#else
#define DEFAULT_IDL_SETTING 0
#endif

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
    bool expected_localized              = false;
    vector<_Chrono_spec<CharT>>& expected_chrono_specs;
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
    void _On_localized() {
        assert(expected_localized);
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
    using chrono_spec       = _Chrono_spec<CharT>;

    view_typ s0(STR("B"));
    view_typ s1(STR("Ec"));
    view_typ s2(STR("Od"));
    view_typ s3(STR("E"));
    view_typ s4(STR(""));
    view_typ s5(STR("}"));
    view_typ s6(STR("E}"));

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
    using chrono_spec                 = _Chrono_spec<CharT>;

    view_typ s0(STR("%Oe"));
    view_typ s1(STR("lit"));
    view_typ s2(STR("%H:%M}"));
    view_typ s3(STR("6%H}"));
    view_typ s4(STR("*<6hi"));
    view_typ s5(STR("*^4.4%ymm"));
    view_typ s6(STR("%H%"));
    view_typ s7(STR("%H%}"));
    view_typ s8(STR("%nB%tC%%D"));
    view_typ s9(STR("L"));
    view_typ s10(STR("L%F"));

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
            .expected_fill         = view_typ(STR("*")),
            .expected_width        = 6,
            .expected_chrono_specs = v4});

    vector<chrono_spec> v5{{._Type = 'y'}, {._Lit_char = 'm'}, {._Lit_char = 'm'}};
    test_parse_helper(parse_chrono_format_specs_fn, s5, false, s5.size(),
        {.expected_alignment       = _Fmt_align::_Center,
            .expected_fill         = view_typ(STR("*")),
            .expected_width        = 4,
            .expected_precision    = 4,
            .expected_chrono_specs = v5});

    vector<chrono_spec> v{{._Type = 'H'}}; // we don't throw a format_error until we parse the %H
    test_parse_helper(parse_chrono_format_specs_fn, s6, true, view_typ::npos, {.expected_chrono_specs = v});
    test_parse_helper(parse_chrono_format_specs_fn, s7, true, view_typ::npos, {.expected_chrono_specs = v});

    vector<chrono_spec> v_empty{};
    test_parse_helper(parse_chrono_format_specs_fn, s9, false, view_typ::npos,
        {.expected_localized = true, .expected_chrono_specs = v_empty});

    vector<chrono_spec> v6{{._Type = 'F'}};
    test_parse_helper(parse_chrono_format_specs_fn, s10, false, view_typ::npos,
        {.expected_localized = true, .expected_chrono_specs = v6});


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

template <class Arg, class CharT>
void empty_braces_helper(
    const Arg& val, const CharT* const expected, const same_as<const CharT*> auto... alternatives) {
    // N4885 [time.format]/6: "If the chrono-specs is omitted, the chrono object is formatted
    // as if by streaming it to std::ostringstream os and copying os.str() through the output iterator
    // of the context with additional padding and adjustments as specified by the format specifiers."
    const auto result = format(STR("{}"), val);
    assert(((result == expected) || ... || (result == alternatives)));

    basic_ostringstream<CharT> stream;
    stream << val;
    assert(stream.str() == result);
    assert(stream);
}

template <typename CharT>
void test_duration_formatter() {
    empty_braces_helper(seconds{5}, STR("5s"));
    empty_braces_helper(minutes{7}, STR("7min"));
    empty_braces_helper(hours{9}, STR("9h"));
    empty_braces_helper(days{2}, STR("2d"));
    empty_braces_helper(-seconds{5}, STR("-5s"));
    empty_braces_helper(duration<int, ratio<3, 1>>{40}, STR("40[3]s"));
    empty_braces_helper(duration<int, ratio<3, 7>>{40}, STR("40[3/7]s"));

    assert(format(STR("{:%T}"), 4083007ms) == STR("01:08:03.007"));
    assert(format(STR("{:%T}"), -4083007ms) == STR("-01:08:03.007"));

    assert(format(STR("{:%T %j %q %Q}"), days{4} + 30min) == STR("00:30:00 4 min 5790"));
    assert(format(STR("{:%T %j %q %Q}"), -days{4} - 30min) == STR("-00:30:00 4 min 5790"));
    assert(format(STR("{:%T %j}"), days{4} + 23h + 30min) == STR("23:30:00 4"));
    assert(format(STR("{:%T %j}"), -days{4} - 23h - 30min) == STR("-23:30:00 4"));
    assert(format(STR("{:%T %j}"), duration<float, days::period>{1.55f}) == STR("13:11:59 1"));
    assert(format(STR("{:%T %j}"), duration<float, days::period>{-1.55f}) == STR("-13:11:59 1"));
}

template <typename CharT>
void test_clock_formatter() {
    empty_braces_helper(sys_seconds{}, STR("1970-01-01 00:00:00"));
    empty_braces_helper(sys_days{}, STR("1970-01-01"));
    empty_braces_helper(utc_seconds{}, STR("1970-01-01 00:00:00"));
    empty_braces_helper(tai_seconds{}, STR("1958-01-01 00:00:00"));
    empty_braces_helper(gps_seconds{}, STR("1980-01-06 00:00:00"));
    empty_braces_helper(file_time<seconds>{}, STR("1601-01-01 00:00:00"));
    empty_braces_helper(local_seconds{}, STR("1970-01-01 00:00:00"));

    assert(format(STR("{:%Z %z %Oz %Ez}"), sys_seconds{}) == STR("UTC +0000 +00:00 +00:00"));
    assert(format(STR("{:%Z %z %Oz %Ez}"), sys_days{}) == STR("UTC +0000 +00:00 +00:00"));
    assert(format(STR("{:%Z %z %Oz %Ez}"), utc_seconds{}) == STR("UTC +0000 +00:00 +00:00"));
    assert(format(STR("{:%Z %z %Oz %Ez}"), tai_seconds{}) == STR("TAI +0000 +00:00 +00:00"));
    assert(format(STR("{:%Z %z %Oz %Ez}"), gps_seconds{}) == STR("GPS +0000 +00:00 +00:00"));
    assert(format(STR("{:%Z %z %Oz %Ez}"), file_time<seconds>{}) == STR("UTC +0000 +00:00 +00:00"));
    throw_helper(STR("{:%Z %z %Oz %Ez}"), local_seconds{});

    assert(format(STR("{:%g %G %U %V %W}"), sys_days{2010y / January / 1}) == STR("09 2009 00 53 00"));
    assert(format(STR("{:%g %G %U %V %W}"), sys_days{2010y / January / 2}) == STR("09 2009 00 53 00"));
    assert(format(STR("{:%g %G %U %V %W}"), sys_days{2010y / January / 3}) == STR("09 2009 01 53 00"));
    assert(format(STR("{:%g %G %U %V %W}"), sys_days{2010y / January / 4}) == STR("10 2010 01 01 01"));
    assert(format(STR("{:%g %G %U %V %W}"), sys_days{2010y / January / 5}) == STR("10 2010 01 01 01"));
    assert(format(STR("{:%g %G %U %V %W}"), sys_days{2010y / January / 6}) == STR("10 2010 01 01 01"));
    assert(format(STR("{:%g %G %U %V %W}"), sys_days{2010y / January / 7}) == STR("10 2010 01 01 01"));
    assert(format(STR("{:%g %G %U %V %W}"), sys_days{2010y / May / 1}) == STR("10 2010 17 17 17"));
    assert(format(STR("{:%g %G %U %V %W}"), sys_days{2010y / May / 2}) == STR("10 2010 18 17 17"));
    assert(format(STR("{:%g %G %U %V %W}"), sys_days{2010y / May / 3}) == STR("10 2010 18 18 18"));
    assert(format(STR("{:%g %G %U %V %W}"), sys_days{2010y / December / 25}) == STR("10 2010 51 51 51"));
    assert(format(STR("{:%g %G %U %V %W}"), sys_days{2010y / December / 26}) == STR("10 2010 52 51 51"));
    assert(format(STR("{:%g %G %U %V %W}"), sys_days{2010y / December / 27}) == STR("10 2010 52 52 52"));
    assert(format(STR("{:%g %G %U %V %W}"), sys_days{2010y / December / 28}) == STR("10 2010 52 52 52"));
    assert(format(STR("{:%g %G %U %V %W}"), sys_days{2010y / December / 29}) == STR("10 2010 52 52 52"));
    assert(format(STR("{:%g %G %U %V %W}"), sys_days{2010y / December / 30}) == STR("10 2010 52 52 52"));
    assert(format(STR("{:%g %G %U %V %W}"), sys_days{2010y / December / 31}) == STR("10 2010 52 52 52"));

    assert(format(STR("{:%S}"), utc_clock::from_sys(get_tzdb().leap_seconds.front().date()) - 1s) == STR("60"));
    assert(format(STR("{:%F %T}"), utc_clock::from_sys(get_tzdb().leap_seconds.front().date()))
           == STR("1972-07-01 00:00:00"));
    assert(format(STR("{:%F %T}"), utc_clock::from_sys(sys_days{January / 9 / 2014} + 12h + 35min + 34s))
           == STR("2014-01-09 12:35:34"));
    assert(format(STR("{:%F %T}"), utc_clock::from_sys(get_tzdb().leap_seconds.front().date()) - 500ms)
           == STR("1972-06-30 23:59:60.500"));

    // Test an ordinary day.
    const auto utc_2021_05_04 = utc_clock::from_sys(sys_days{2021y / May / 4});

    // This is both the last day of a leap year (366th day) and the day of a leap second insertion.
    const auto utc_2016_12_31 = utc_clock::from_sys(sys_days{2016y / December / 31});

    for (const auto& h : {0h, 1h, 7h, 22h, 23h}) { // Accelerate testing; 24 * 60 * 61 iterations would be a lot.
        for (const auto& m : {0min, 1min, 7min, 58min, 59min}) {
            for (const auto& s : {0s, 1s, 7s, 58s, 59s, 60s}) {
                if (s != 60s) {
                    assert(format(STR("{:%F %T}"), utc_2021_05_04 + h + m + s)
                           == format(STR("2021-05-04 {:02}:{:02}:{:02}"), h.count(), m.count(), s.count()));
                }

                if ((h == 23h && m == 59min) || s != 60s) {
                    assert(format(STR("{:%F %T}"), utc_2016_12_31 + h + m + s)
                           == format(STR("2016-12-31 {:02}:{:02}:{:02}"), h.count(), m.count(), s.count()));
                }
            }
        }
    }

    empty_braces_helper(file_clock::from_utc(utc_2016_12_31), STR("2016-12-31 00:00:00"));
    empty_braces_helper(file_clock::from_utc(utc_2016_12_31) + 24h, STR("2017-01-01 00:00:00"));
    empty_braces_helper(file_clock::from_utc(utc_2021_05_04), STR("2021-05-04 00:00:00"));
    // TRANSITION: Test a leap second insertion after 2018-06 when there is one
}

template <typename CharT>
void test_day_formatter() {
    using view_typ = basic_string_view<CharT>;
    using str_typ  = basic_string<CharT>;

    view_typ s0(STR("{:%d}"));
    view_typ s1(STR("{:%e}"));
    view_typ s2(STR("{:%Od}"));
    view_typ s3(STR("{:%Oe}"));
    view_typ s4(STR("{}"));
    view_typ s5(STR("{:=>8}"));
    view_typ s6(STR("{:lit}"));
    view_typ s7(STR("{:%d days}"));
    view_typ s8(STR("{:*^6%dmm}"));

    str_typ a0(STR("27"));
    str_typ a1(STR("05"));
    str_typ a2(STR(" 5"));
    str_typ a3(STR("50 is not a valid day"));
    str_typ a4(STR("======27"));
    str_typ a5(STR("======05"));
    str_typ a6(STR("lit27"));
    str_typ a7(STR("27 days"));
    str_typ a8(STR("*27mm*"));

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

    // Op <<
    empty_braces_helper(day{0}, STR("00 is not a valid day"));
    empty_braces_helper(day{27}, STR("27"));
    empty_braces_helper(day{200}, STR("200 is not a valid day"));
}

template <typename CharT>
void test_month_formatter() {
    empty_braces_helper(month{1}, STR("Jan"));
    empty_braces_helper(month{12}, STR("Dec"));
    empty_braces_helper(month{0}, STR("0 is not a valid month"));
    empty_braces_helper(month{20}, STR("20 is not a valid month"));

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
}

template <typename CharT>
void test_year_formatter() {
    empty_braces_helper(year{0}, STR("0000"));
    empty_braces_helper(year{-200}, STR("-0200"));
    empty_braces_helper(year{121}, STR("0121"));

    assert(format(STR("{:%Y %y%C}"), year{1912}) == STR("1912 1219"));
    assert(format(STR("{:%Y %y%C}"), year{-1912}) == STR("-1912 88-20"));
    assert(format(STR("{:%Y %y%C}"), year{-200}) == STR("-0200 00-02"));
    assert(format(STR("{:%Y %y%C}"), year{200}) == STR("0200 0002"));
    // TRANSITION, add tests for EY Oy Ey EC

    empty_braces_helper(year{1900}, STR("1900"));
    empty_braces_helper(year{2000}, STR("2000"));
    empty_braces_helper(year{-32768}, STR("-32768 is not a valid year"));
}

template <typename CharT>
void test_weekday_formatter() {
    weekday invalid{10};
    empty_braces_helper(weekday{3}, STR("Wed"));
    empty_braces_helper(invalid, STR("10 is not a valid weekday"));

    assert(format(STR("{:%a %A}"), weekday{6}) == STR("Sat Saturday"));
    assert(format(STR("{:%u %w}"), weekday{6}) == STR("6 6"));
    assert(format(STR("{:%u %w}"), weekday{0}) == STR("7 0"));
}

template <typename CharT>
void test_weekday_indexed_formatter() {
    weekday_indexed invalid1{Tuesday, 10};
    weekday_indexed invalid2{weekday{10}, 3};
    weekday_indexed invalid3{weekday{14}, 9};
    empty_braces_helper(weekday_indexed{Monday, 1}, STR("Mon[1]"));
    empty_braces_helper(invalid1, STR("Tue[10 is not a valid index]"));
    empty_braces_helper(invalid2, STR("10 is not a valid weekday[3]"));
    empty_braces_helper(invalid3, STR("14 is not a valid weekday[9 is not a valid index]"));

    assert(format(STR("{:%a %A}"), weekday_indexed{Monday, 2}) == STR("Mon Monday"));
    assert(format(STR("{:%u %w}"), weekday_indexed{Tuesday, 3}) == STR("2 2"));
    assert(format(STR("{:%u %w}"), weekday_indexed{Sunday, 4}) == STR("7 0"));
    assert(format(STR("{:%u %w}"), weekday_indexed{Sunday, 10}) == STR("7 0"));
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
    empty_braces_helper(January / 16, STR("Jan/16"));
    empty_braces_helper(month{13} / day{40}, STR("13 is not a valid month/40 is not a valid day"));

    assert(format(STR("{:%B %d}"), June / 17) == STR("June 17"));
    throw_helper(STR("{:%Y}"), June / 17);
    assert(format(STR("{:%B}"), June / day{40}) == STR("June"));
    throw_helper(STR("{:%B}"), month{13} / 17);

    assert(format(STR("{:%j}"), January / 5) == STR("005"));
    assert(format(STR("{:%j}"), February / 5) == STR("036"));
    assert(format(STR("{:%j}"), February / 28) == STR("059"));
    assert(format(STR("{:%j}"), February / 29) == STR("060"));
    throw_helper(STR("{:%j}"), March / 1);
}

template <typename CharT>
void test_month_day_last_formatter() {
    empty_braces_helper(February / last, STR("Feb/last"));

    assert(format(STR("{:%B}"), June / last) == STR("June"));
    assert(format(STR("{:%d}"), June / last) == STR("30"));
    throw_helper(STR("{:%d}"), February / last);
    throw_helper(STR("{:%B}"), month{13} / last);

    assert(format(STR("{:%j}"), January / last) == STR("031"));
    throw_helper(STR("{:%j}"), February / last);
    throw_helper(STR("{:%j}"), April / last);
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

    assert(format(STR("{:%u}"), invalid1) == STR("5"));
    throw_helper(STR("{:%u}"), invalid2);
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

    assert(format(STR("{:%u}"), invalid2) == STR("5"));
    throw_helper(STR("{:%u}"), invalid1);
}

template <typename CharT>
void test_year_month_formatter() {
    empty_braces_helper(1444y / October, STR("1444/Oct"));

    assert(format(STR("{:%Y %B}"), 2000y / July) == STR("2000 July"));
    throw_helper(STR("{:%d}"), 2000y / July);

    throw_helper(STR("{:%g}"), 2005y / month{0});
    throw_helper(STR("{:%G}"), 2005y / month{0});
    throw_helper(STR("{:%g}"), 2005y / January);
    throw_helper(STR("{:%G}"), 2005y / January);
    assert(format(STR("{:%g %G}"), 2005y / February) == STR("05 2005"));
    assert(format(STR("{:%g %G}"), 2005y / November) == STR("05 2005"));
    throw_helper(STR("{:%g}"), 2005y / December);
    throw_helper(STR("{:%G}"), 2005y / December);
    throw_helper(STR("{:%g}"), 2005y / month{13});
    throw_helper(STR("{:%G}"), 2005y / month{13});
    throw_helper(STR("{:%g}"), year{-32768} / November);
    throw_helper(STR("{:%G}"), year{-32768} / November);
}

template <typename CharT>
void test_year_month_day_formatter() {
    year_month_day invalid{year{1234}, month{0}, day{31}};
    empty_braces_helper(year_month_day{year{1900}, month{2}, day{1}}, STR("1900-02-01"));
    empty_braces_helper(invalid, STR("1234-00-31 is not a valid date"));

    assert(format(STR("{:%Y %b %d}"), year_month_day{year{1234}, month{5}, day{6}}) == STR("1234 May 06"));
    assert(format(STR("{:%F %D}"), invalid) == STR("1234-00-31 00/31/34"));
    assert(format(STR("{:%a %A}"), year_month_day{year{1900}, month{1}, day{4}}) == STR("Thu Thursday"));
    assert(format(STR("{:%u %w}"), year_month_day{year{1900}, month{1}, day{4}}) == STR("4 4"));
    throw_helper(STR("{:%u}"), invalid);

    assert(format(STR("{:%j}"), 1900y / January / 4) == STR("004"));
    assert(format(STR("{:%j}"), 1900y / May / 7) == STR("127"));
    assert(format(STR("{:%j}"), 2000y / May / 7) == STR("128"));
    throw_helper(STR("{:%j}"), invalid);

    assert(format(STR("{:%g %G %U %V %W}"), 2010y / January / 1) == STR("09 2009 00 53 00"));
    assert(format(STR("{:%g %G %U %V %W}"), 2010y / January / 2) == STR("09 2009 00 53 00"));
    assert(format(STR("{:%g %G %U %V %W}"), 2010y / January / 3) == STR("09 2009 01 53 00"));
    assert(format(STR("{:%g %G %U %V %W}"), 2010y / January / 4) == STR("10 2010 01 01 01"));
    assert(format(STR("{:%g %G %U %V %W}"), 2010y / January / 5) == STR("10 2010 01 01 01"));
    assert(format(STR("{:%g %G %U %V %W}"), 2010y / January / 6) == STR("10 2010 01 01 01"));
    assert(format(STR("{:%g %G %U %V %W}"), 2010y / January / 7) == STR("10 2010 01 01 01"));
    assert(format(STR("{:%g %G %U %V %W}"), 2010y / May / 1) == STR("10 2010 17 17 17"));
    assert(format(STR("{:%g %G %U %V %W}"), 2010y / May / 2) == STR("10 2010 18 17 17"));
    assert(format(STR("{:%g %G %U %V %W}"), 2010y / May / 3) == STR("10 2010 18 18 18"));
    assert(format(STR("{:%g %G %U %V %W}"), 2010y / December / 25) == STR("10 2010 51 51 51"));
    assert(format(STR("{:%g %G %U %V %W}"), 2010y / December / 26) == STR("10 2010 52 51 51"));
    assert(format(STR("{:%g %G %U %V %W}"), 2010y / December / 27) == STR("10 2010 52 52 52"));
    assert(format(STR("{:%g %G %U %V %W}"), 2010y / December / 28) == STR("10 2010 52 52 52"));
    assert(format(STR("{:%g %G %U %V %W}"), 2010y / December / 29) == STR("10 2010 52 52 52"));
    assert(format(STR("{:%g %G %U %V %W}"), 2010y / December / 30) == STR("10 2010 52 52 52"));
    assert(format(STR("{:%g %G %U %V %W}"), 2010y / December / 31) == STR("10 2010 52 52 52"));
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

    throw_helper(STR("{:%u}"), invalid);

    assert(format(STR("{:%j}"), 1900y / January / last) == STR("031"));
    assert(format(STR("{:%j}"), 1900y / February / last) == STR("059"));
    assert(format(STR("{:%j}"), 2000y / February / last) == STR("060"));
    throw_helper(STR("{:%j}"), year{1900} / month{13} / last);

    assert(format(STR("{:%g %G %U %V %W}"), 2010y / May / last) == STR("10 2010 22 22 22"));
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

    assert(format(STR("{:%u}"), invalid1) == STR("5"));
    throw_helper(STR("{:%u}"), invalid2);

    assert(format(STR("{:%j}"), 1900y / January / Tuesday[2]) == STR("009"));
    throw_helper(STR("{:%j}"), invalid1);

    assert(format(STR("{:%g %G %U %V %W}"), 2010y / May / Monday[5]) == STR("10 2010 22 22 22"));
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

    assert(format(STR("{:%u}"), invalid2) == STR("5"));
    throw_helper(STR("{:%u}"), invalid1);

    assert(format(STR("{:%j}"), 1900y / January / Tuesday[last]) == STR("030"));
    throw_helper(STR("{:%j}"), invalid1);

    assert(format(STR("{:%g %G %U %V %W}"), 2010y / May / Monday[last]) == STR("10 2010 22 22 22"));
}

template <typename CharT>
void test_hh_mm_ss_formatter() {
    empty_braces_helper(hh_mm_ss{-4083007ms}, STR("-01:08:03.007"));
    empty_braces_helper(hh_mm_ss{4083007ms}, STR("01:08:03.007"));
    empty_braces_helper(hh_mm_ss{65745123ms}, STR("18:15:45.123"));
    empty_braces_helper(hh_mm_ss{65745s}, STR("18:15:45"));
    empty_braces_helper(hh_mm_ss{0.1ns}, STR("00:00:00.000000000"));
    empty_braces_helper(hh_mm_ss{1.45ns}, STR("00:00:00.000000001"));
    empty_braces_helper(hh_mm_ss{1.56ns}, STR("00:00:00.000000001"));
    empty_braces_helper(hh_mm_ss{1e+8ns}, STR("00:00:00.100000000"));
    empty_braces_helper(hh_mm_ss{999'999.9us}, STR("00:00:00.999999"));
    empty_braces_helper(hh_mm_ss{59'999'999.9us}, STR("00:00:59.999999"));
    empty_braces_helper(hh_mm_ss{3'599'999'999.9us}, STR("00:59:59.999999"));
    empty_braces_helper(hh_mm_ss{86'399'999'999.9us}, STR("23:59:59.999999"));

    assert(format(STR("{:%H %I %M %S %r %R %T %p}"), hh_mm_ss{13h + 14min + 15351ms})
           == STR("13 01 14 15.351 01:14:15 PM 13:14 13:14:15.351 PM"));

    assert(format(STR("{:%H %I %M %S %r %R %T %p}"), hh_mm_ss{-13h - 14min - 15351ms})
           == STR("-13 01 14 15.351 01:14:15 PM 13:14 13:14:15.351 PM"));

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
                      "2016-03-13 07:00:00 UTC"
               || s
                      == "2016-03-13 02:30:00 is in a gap between\n"
                         "2016-03-13 02:00:00 GMT-5 and\n"
                         "2016-03-13 03:00:00 GMT-4 which are both equivalent to\n"
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
                      "2016-11-06 01:30:00 EST == 2016-11-06 06:30:00 UTC"
               || s
                      == "2016-11-06 01:30:00 is ambiguous. It could be\n"
                         "2016-11-06 01:30:00 GMT-4 == 2016-11-06 05:30:00 UTC or\n"
                         "2016-11-06 01:30:00 GMT-5 == 2016-11-06 06:30:00 UTC");
    }
}

template <typename CharT>
void test_information_classes() {
    const tzdb& database = get_tzdb();

    const time_zone* const sydney_tz = database.locate_zone(Sydney::Tz_name);
    assert(sydney_tz != nullptr);

    const time_zone* const la_tz = database.locate_zone(LA::Tz_name);
    assert(la_tz != nullptr);

    const sys_info sys1 = sydney_tz->get_info(Sydney::Std_1.begin() + days{1});
    const sys_info sys2 = sydney_tz->get_info(Sydney::Day_2.begin() + days{1});
    const sys_info sys3 = la_tz->get_info(LA::Std_1.begin() + days{1});
    const sys_info sys4 = la_tz->get_info(LA::Day_2.begin() + days{1});

    const local_info loc1 = sydney_tz->get_info(Sydney::Std_1.local_begin() + days{1});
    const local_info loc2 = sydney_tz->get_info(Sydney::Day_2.local_begin() + days{1});
    const local_info loc3 = la_tz->get_info(LA::Std_1.local_begin() + days{1});
    const local_info loc4 = la_tz->get_info(LA::Day_2.local_begin() + days{1});

    const local_info ambiguous1 = sydney_tz->get_info(Sydney::Std_1.local_begin());
    const local_info ambiguous2 = la_tz->get_info(LA::Std_1.local_begin());

    const local_info nonexistent1 = sydney_tz->get_info(Sydney::Std_1.local_end());
    const local_info nonexistent2 = la_tz->get_info(LA::Std_1.local_end());

    // N4885 [time.zone.info.sys]/7: "Effects: Streams out the sys_info object r in an unspecified format."
    // N4885 [time.zone.info.local]/3: "Effects: Streams out the local_info object r in an unspecified format."

    empty_braces_helper(sys1,
        STR("begin: 2020-04-04 16:00:00, end: 2020-10-03 16:00:00, "
            "offset: 36000s, save: 0min, abbrev: AEST"),
        STR("begin: 2020-04-04 16:00:00, end: 2020-10-03 16:00:00, "
            "offset: 36000s, save: 0min, abbrev: GMT+10"));
    empty_braces_helper(sys2,
        STR("begin: 2020-10-03 16:00:00, end: 2021-04-03 16:00:00, "
            "offset: 39600s, save: 60min, abbrev: AEDT"),
        STR("begin: 2020-10-03 16:00:00, end: 2021-04-03 16:00:00, "
            "offset: 39600s, save: 60min, abbrev: GMT+11"));
    empty_braces_helper(sys3,
        STR("begin: 2020-11-01 09:00:00, end: 2021-03-14 10:00:00, "
            "offset: -28800s, save: 0min, abbrev: PST"),
        STR("begin: 2020-11-01 09:00:00, end: 2021-03-14 10:00:00, "
            "offset: -28800s, save: 0min, abbrev: GMT-8"));
    empty_braces_helper(sys4,
        STR("begin: 2021-03-14 10:00:00, end: 2021-11-07 09:00:00, "
            "offset: -25200s, save: 60min, abbrev: PDT"),
        STR("begin: 2021-03-14 10:00:00, end: 2021-11-07 09:00:00, "
            "offset: -25200s, save: 60min, abbrev: GMT-7"));
    empty_braces_helper(loc1,
        STR("result: unique, "
            "first: (begin: 2020-04-04 16:00:00, end: 2020-10-03 16:00:00, "
            "offset: 36000s, save: 0min, abbrev: AEST)"),
        STR("result: unique, "
            "first: (begin: 2020-04-04 16:00:00, end: 2020-10-03 16:00:00, "
            "offset: 36000s, save: 0min, abbrev: GMT+10)"));
    empty_braces_helper(loc2,
        STR("result: unique, "
            "first: (begin: 2020-10-03 16:00:00, end: 2021-04-03 16:00:00, "
            "offset: 39600s, save: 60min, abbrev: AEDT)"),
        STR("result: unique, "
            "first: (begin: 2020-10-03 16:00:00, end: 2021-04-03 16:00:00, "
            "offset: 39600s, save: 60min, abbrev: GMT+11)"));
    empty_braces_helper(loc3,
        STR("result: unique, "
            "first: (begin: 2020-11-01 09:00:00, end: 2021-03-14 10:00:00, "
            "offset: -28800s, save: 0min, abbrev: PST)"),
        STR("result: unique, "
            "first: (begin: 2020-11-01 09:00:00, end: 2021-03-14 10:00:00, "
            "offset: -28800s, save: 0min, abbrev: GMT-8)"));
    empty_braces_helper(loc4,
        STR("result: unique, "
            "first: (begin: 2021-03-14 10:00:00, end: 2021-11-07 09:00:00, "
            "offset: -25200s, save: 60min, abbrev: PDT)"),
        STR("result: unique, "
            "first: (begin: 2021-03-14 10:00:00, end: 2021-11-07 09:00:00, "
            "offset: -25200s, save: 60min, abbrev: GMT-7)"));
    empty_braces_helper(ambiguous1,
        STR("result: ambiguous, "
            "first: (begin: 2019-10-05 16:00:00, end: 2020-04-04 16:00:00, "
            "offset: 39600s, save: 60min, abbrev: AEDT), "
            "second: (begin: 2020-04-04 16:00:00, end: 2020-10-03 16:00:00, "
            "offset: 36000s, save: 0min, abbrev: AEST)"),
        STR("result: ambiguous, "
            "first: (begin: 2019-10-05 16:00:00, end: 2020-04-04 16:00:00, "
            "offset: 39600s, save: 60min, abbrev: GMT+11), "
            "second: (begin: 2020-04-04 16:00:00, end: 2020-10-03 16:00:00, "
            "offset: 36000s, save: 0min, abbrev: GMT+10)"));
    empty_braces_helper(ambiguous2,
        STR("result: ambiguous, "
            "first: (begin: 2020-03-08 10:00:00, end: 2020-11-01 09:00:00, "
            "offset: -25200s, save: 60min, abbrev: PDT), "
            "second: (begin: 2020-11-01 09:00:00, end: 2021-03-14 10:00:00, "
            "offset: -28800s, save: 0min, abbrev: PST)"),
        STR("result: ambiguous, "
            "first: (begin: 2020-03-08 10:00:00, end: 2020-11-01 09:00:00, "
            "offset: -25200s, save: 60min, abbrev: GMT-7), "
            "second: (begin: 2020-11-01 09:00:00, end: 2021-03-14 10:00:00, "
            "offset: -28800s, save: 0min, abbrev: GMT-8)"));
    empty_braces_helper(nonexistent1,
        STR("result: nonexistent, "
            "first: (begin: 2020-04-04 16:00:00, end: 2020-10-03 16:00:00, "
            "offset: 36000s, save: 0min, abbrev: AEST), "
            "second: (begin: 2020-10-03 16:00:00, end: 2021-04-03 16:00:00, "
            "offset: 39600s, save: 60min, abbrev: AEDT)"),
        STR("result: nonexistent, "
            "first: (begin: 2020-04-04 16:00:00, end: 2020-10-03 16:00:00, "
            "offset: 36000s, save: 0min, abbrev: GMT+10), "
            "second: (begin: 2020-10-03 16:00:00, end: 2021-04-03 16:00:00, "
            "offset: 39600s, save: 60min, abbrev: GMT+11)"));
    empty_braces_helper(nonexistent2,
        STR("result: nonexistent, "
            "first: (begin: 2020-11-01 09:00:00, end: 2021-03-14 10:00:00, "
            "offset: -28800s, save: 0min, abbrev: PST), "
            "second: (begin: 2021-03-14 10:00:00, end: 2021-11-07 09:00:00, "
            "offset: -25200s, save: 60min, abbrev: PDT)"),
        STR("result: nonexistent, "
            "first: (begin: 2020-11-01 09:00:00, end: 2021-03-14 10:00:00, "
            "offset: -28800s, save: 0min, abbrev: GMT-8), "
            "second: (begin: 2021-03-14 10:00:00, end: 2021-11-07 09:00:00, "
            "offset: -25200s, save: 60min, abbrev: GMT-7)"));

    const auto sys1_str = format(STR("{:%z %Ez %Oz %Z}"), sys1);
    const auto sys2_str = format(STR("{:%z %Ez %Oz %Z}"), sys2);
    const auto sys3_str = format(STR("{:%z %Ez %Oz %Z}"), sys3);
    const auto sys4_str = format(STR("{:%z %Ez %Oz %Z}"), sys4);

    const auto loc1_str = format(STR("{:%z %Ez %Oz %Z}"), loc1);
    const auto loc2_str = format(STR("{:%z %Ez %Oz %Z}"), loc2);
    const auto loc3_str = format(STR("{:%z %Ez %Oz %Z}"), loc3);
    const auto loc4_str = format(STR("{:%z %Ez %Oz %Z}"), loc4);

    assert(sys1_str == STR("+1000 +10:00 +10:00 AEST") || sys1_str == STR("+1000 +10:00 +10:00 GMT+10"));
    assert(sys2_str == STR("+1100 +11:00 +11:00 AEDT") || sys2_str == STR("+1100 +11:00 +11:00 GMT+11"));
    assert(sys3_str == STR("-0800 -08:00 -08:00 PST") || sys3_str == STR("-0800 -08:00 -08:00 GMT-8"));
    assert(sys4_str == STR("-0700 -07:00 -07:00 PDT") || sys4_str == STR("-0700 -07:00 -07:00 GMT-7"));

    assert(loc1_str == STR("+1000 +10:00 +10:00 AEST") || loc1_str == STR("+1000 +10:00 +10:00 GMT+10"));
    assert(loc2_str == STR("+1100 +11:00 +11:00 AEDT") || loc2_str == STR("+1100 +11:00 +11:00 GMT+11"));
    assert(loc3_str == STR("-0800 -08:00 -08:00 PST") || loc3_str == STR("-0800 -08:00 -08:00 GMT-8"));
    assert(loc4_str == STR("-0700 -07:00 -07:00 PDT") || loc4_str == STR("-0700 -07:00 -07:00 GMT-7"));

    throw_helper(STR("{:%z}"), ambiguous1);
    throw_helper(STR("{:%z}"), ambiguous2);
    throw_helper(STR("{:%z}"), nonexistent1);
    throw_helper(STR("{:%z}"), nonexistent2);

    throw_helper(STR("{:%Z}"), ambiguous1);
    throw_helper(STR("{:%Z}"), ambiguous2);
    throw_helper(STR("{:%Z}"), nonexistent1);
    throw_helper(STR("{:%Z}"), nonexistent2);

    // Additionally test zero and half-hour offsets.
    const time_zone* const utc_tz = database.locate_zone("Etc/UTC"sv);
    assert(utc_tz != nullptr);

    const time_zone* const kolkata_tz = database.locate_zone("Asia/Kolkata"sv);
    assert(kolkata_tz != nullptr);

    const sys_info sys5 = utc_tz->get_info(sys_days{2021y / January / 1});
    const sys_info sys6 = kolkata_tz->get_info(sys_days{2021y / January / 1});

    assert(format(STR("{:%z %Ez %Oz}"), sys5) == STR("+0000 +00:00 +00:00"));
    assert(format(STR("{:%z %Ez %Oz}"), sys6) == STR("+0530 +05:30 +05:30"));
}

template <typename CharT>
void test_local_time_format_formatter() {
    constexpr local_seconds t{local_days{2021y / April / 19} + 1h + 2min + 3s};
    const string abbrev{"Meow"};
    constexpr seconds offset{17h + 29min};

    const auto ltf = local_time_format(t, &abbrev, &offset);

    throw_helper(STR("{:%Z}"), local_time_format(t, nullptr, &offset));
    throw_helper(STR("{:%z}"), local_time_format(t, &abbrev, nullptr));

    assert(format(STR("{:%Z %z %Oz %Ez}"), ltf) == STR("Meow +1729 +17:29 +17:29"));

    // Doesn't appear in the Standard, but allowed by N4885 [global.functions]/2.
    // Implements N4885 [time.zone.zonedtime.nonmembers]/2 for zoned_time.
    empty_braces_helper(ltf, STR("2021-04-19 01:02:03 Meow"));

    assert(format(STR("{:%c, %x, %X}"), ltf) == STR("04/19/21 01:02:03, 04/19/21, 01:02:03"));
    assert(format(STR("{:%D %F, %Y %C %y, %b %B %h %m, %d %e, %a %A %u %w}"), ltf)
           == STR("04/19/21 2021-04-19, 2021 20 21, Apr April Apr 04, 19 19, Mon Monday 1 1"));
    assert(format(STR("{:%H %I %M %S, %r, %R %T %p}"), ltf) == STR("01 01 02 03, 01:02:03 AM, 01:02 01:02:03 AM"));
    assert(format(STR("{:%g %G %U %V %W}"), ltf) == STR("21 2021 16 16 16"));
}

template <typename CharT>
void test_zoned_time_formatter() {
    constexpr sys_seconds t{sys_days{2021y / April / 19} + 15h + 16min + 17s};

    const zoned_time zt{LA::Tz_name, t};

    empty_braces_helper(zt, STR("2021-04-19 08:16:17 PDT"), STR("2021-04-19 08:16:17 GMT-7"));

    assert(format(STR("{:%c, %x, %X}"), zt) == STR("04/19/21 08:16:17, 04/19/21, 08:16:17"));
    assert(format(STR("{:%D %F, %Y %C %y, %b %B %h %m, %d %e, %a %A %u %w}"), zt)
           == STR("04/19/21 2021-04-19, 2021 20 21, Apr April Apr 04, 19 19, Mon Monday 1 1"));
    assert(format(STR("{:%H %I %M %S, %r, %R %T %p}"), zt) == STR("08 08 16 17, 08:16:17 AM, 08:16 08:16:17 AM"));
    assert(format(STR("{:%g %G %U %V %W}"), zt) == STR("21 2021 16 16 16"));
}

template <typename CharT>
void test_locale() {
    assert(format(locale{"zh-CN"}, STR("{:^22L%Y %B %d %A}"), 2021y / June / 16d)
           == STR(" 2021 \u516D\u6708 16 \u661F\u671F\u4E09  "));


    locale loc("de-DE");

    assert(format(loc, STR("{:%S}"), 42ms) == STR("00.042"));
    assert(format(loc, STR("{:L%S}"), 42ms) == STR("00,042"));

    auto stream = [=](auto value) {
        basic_ostringstream<CharT> os;
        os.imbue(loc);
        os << value;
        return os.str();
    };
    assert(stream(month{May}) == STR("Mai"));
    assert(stream(weekday{Tuesday}) == STR("Di"));
    assert(stream(weekday_indexed{Tuesday[3]}) == STR("Di[3]"));
    assert(stream(weekday_indexed{Tuesday[42]}) == STR("Di[42 is not a valid index]"));
    assert(stream(weekday_last{Tuesday}) == STR("Di[last]"));
    assert(stream(month_day{May, day{4}}) == STR("Mai/04"));
    assert(stream(month_day_last{May}) == STR("Mai/last"));
    assert(stream(month_weekday{May / Tuesday[4]}) == STR("Mai/Di[4]"));
    assert(stream(month_weekday_last{May / Tuesday[last]}) == STR("Mai/Di[last]"));
    assert(stream(year_month{2021y / May}) == STR("2021/Mai"));
    assert(stream(year_month_day_last{2021y / May / last}) == STR("2021/Mai/last"));
    assert(stream(year_month_weekday{2021y / May / Tuesday[4]}) == STR("2021/Mai/Di[4]"));
    assert(stream(year_month_weekday_last{2021y / May / Tuesday[last]}) == STR("2021/Mai/Di[last]"));
}

void test() {
    test_parse_conversion_spec<char>();
    test_parse_conversion_spec<wchar_t>();

    test_parse_chrono_format_specs<char>();
    test_parse_chrono_format_specs<wchar_t>();

    test_duration_formatter<char>();
    test_duration_formatter<wchar_t>();

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

    test_information_classes<char>();
    test_information_classes<wchar_t>();

    test_local_time_format_formatter<char>();
    test_local_time_format_formatter<wchar_t>();

    test_zoned_time_formatter<char>();
    test_zoned_time_formatter<wchar_t>();

#if !defined(_DLL) || _ITERATOR_DEBUG_LEVEL == DEFAULT_IDL_SETTING
    test_locale<wchar_t>();
#ifndef _MSVC_INTERNAL_TESTING // TRANSITION, the Windows version on Contest VMs doesn't always understand ".UTF-8"
    assert(setlocale(LC_ALL, ".UTF-8") != nullptr);
    test_locale<char>();
#endif // _MSVC_INTERNAL_TESTING
#endif // !defined(_DLL) || _ITERATOR_DEBUG_LEVEL == DEFAULT_IDL_SETTING
}

int main() {
    run_tz_test([] { test(); });
}
