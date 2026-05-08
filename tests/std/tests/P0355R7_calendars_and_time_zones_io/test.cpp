// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <istream>
#include <iterator>
#include <limits>
#include <locale>
#include <ratio>
#include <source_location>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <timezone_data.hpp>

using namespace std;
using namespace std::chrono;

constexpr auto intmax_max = numeric_limits<intmax_t>::max();

template <class CharT, class Rep, class Period>
bool test_duration_basic_out(const duration<Rep, Period>& d, const CharT* expected) {
    basic_ostringstream<CharT> ss;

    ss << d;
    return ss.str() == expected;
}

#define WIDEN(TYPE, STR) get<const TYPE*>(pair{STR, L##STR})

template <class CharT>
bool test_duration_locale_out() {
    basic_ostringstream<CharT> ss;
    const duration<double> d{0.140625};
    ss.precision(3);
    ss << d;
    ss.setf(ios_base::scientific, ios_base::floatfield);
    ss << ' ' << d;

    basic_string<CharT> expected = WIDEN(CharT, "0.141s 1.406e-01s");

#ifdef _DEBUG
#define DEFAULT_IDL_SETTING 2
#else
#define DEFAULT_IDL_SETTING 0
#endif

#if !defined(_DLL) || _ITERATOR_DEBUG_LEVEL == DEFAULT_IDL_SETTING
    // When linking dynamically, user-defined facets are incompatible with non-default _ITERATOR_DEBUG_LEVEL settings.
    struct comma : numpunct<CharT> {
        CharT do_decimal_point() const {
            return ',';
        }
    };

    ss.imbue(locale(ss.getloc(), new comma));
    ss << ' ' << d;
    expected += WIDEN(CharT, " 1,406e-01s");
#endif // !defined(_DLL) || _ITERATOR_DEBUG_LEVEL == DEFAULT_IDL_SETTING

    return ss.str() == expected;
}

void test_duration_output() {
    using LongRatio = ratio<intmax_max - 1, intmax_max>;
    assert(test_duration_basic_out(duration<int, atto>{1}, "1as"));
    assert(test_duration_basic_out(duration<int, femto>{2}, "2fs"));
    assert(test_duration_basic_out(duration<int, pico>{3}, "3ps"));
    assert(test_duration_basic_out(duration<int, nano>{42}, "42ns"));
    assert(test_duration_basic_out(duration<int, micro>{42}, "42us"));
    assert(test_duration_basic_out(duration<int, milli>{42}, "42ms"));
    assert(test_duration_basic_out(duration<int, centi>{42}, "42cs"));
    assert(test_duration_basic_out(duration<int, deci>{42}, "42ds"));
    assert(test_duration_basic_out(duration<int, ratio<1>>{42}, "42s"));
    assert(test_duration_basic_out(duration<int, deca>{42}, "42das"));
    assert(test_duration_basic_out(duration<int, hecto>{42}, "42hs"));
    assert(test_duration_basic_out(duration<int, kilo>{42}, "42ks"));
    assert(test_duration_basic_out(duration<int, mega>{42}, "42Ms"));
    assert(test_duration_basic_out(duration<int, giga>{42}, "42Gs"));
    assert(test_duration_basic_out(duration<int, tera>{42}, "42Ts"));
    assert(test_duration_basic_out(duration<int, peta>{42}, "42Ps"));
    assert(test_duration_basic_out(duration<int, exa>{42}, "42Es"));
    assert(test_duration_basic_out(duration<int, ratio<60>>{42}, "42min"));
    assert(test_duration_basic_out(duration<int, ratio<60 * 60>>{42}, "42h"));
    assert(test_duration_basic_out(duration<int, ratio<60 * 60 * 24>>{42}, "42d"));

    assert(test_duration_basic_out(duration<int, ratio<2>>{24}, "24[2]s"));
    assert(test_duration_basic_out(duration<int, ratio<3, 1>>{24}, "24[3]s"));
    assert(test_duration_basic_out(duration<int, ratio<3, 7>>{24}, "24[3/7]s"));
    assert(test_duration_basic_out(duration<int, ratio<1, 2>>{24}, "24[1/2]s"));
    assert(test_duration_basic_out(duration<int, ratio<22, 7>>{24}, "24[22/7]s"));
    assert(test_duration_basic_out(duration<int, ratio<53, 101>>{24}, "24[53/101]s"));
    assert(test_duration_basic_out(duration<int, ratio<201, 2147483647>>{24}, "24[201/2147483647]s"));
    assert(test_duration_basic_out(duration<int, LongRatio>{24}, "24[9223372036854775806/9223372036854775807]s"));

    assert(test_duration_basic_out(duration<double>{0.140625}, "0.140625s"));
    assert(test_duration_locale_out<char>());

    assert(test_duration_basic_out(duration<int, atto>{1}, L"1as"));
    assert(test_duration_basic_out(duration<int, femto>{2}, L"2fs"));
    assert(test_duration_basic_out(duration<int, pico>{3}, L"3ps"));
    assert(test_duration_basic_out(duration<int, nano>{42}, L"42ns"));
    assert(test_duration_basic_out(duration<int, micro>{42}, L"42us"));
    assert(test_duration_basic_out(duration<int, milli>{42}, "42ms"));
    assert(test_duration_basic_out(duration<int, centi>{42}, L"42cs"));
    assert(test_duration_basic_out(duration<int, deci>{42}, L"42ds"));
    assert(test_duration_basic_out(duration<int, ratio<1>>{42}, L"42s"));
    assert(test_duration_basic_out(duration<int, deca>{42}, L"42das"));
    assert(test_duration_basic_out(duration<int, hecto>{42}, L"42hs"));
    assert(test_duration_basic_out(duration<int, kilo>{42}, L"42ks"));
    assert(test_duration_basic_out(duration<int, mega>{42}, L"42Ms"));
    assert(test_duration_basic_out(duration<int, giga>{42}, L"42Gs"));
    assert(test_duration_basic_out(duration<int, tera>{42}, L"42Ts"));
    assert(test_duration_basic_out(duration<int, peta>{42}, L"42Ps"));
    assert(test_duration_basic_out(duration<int, exa>{42}, L"42Es"));
    assert(test_duration_basic_out(duration<int, ratio<60>>{42}, L"42min"));
    assert(test_duration_basic_out(duration<int, ratio<60 * 60>>{42}, L"42h"));
    assert(test_duration_basic_out(duration<int, ratio<60 * 60 * 24>>{42}, L"42d"));

    assert(test_duration_basic_out(duration<int, ratio<2>>{24}, L"24[2]s"));
    assert(test_duration_basic_out(duration<int, ratio<3, 1>>{24}, L"24[3]s"));
    assert(test_duration_basic_out(duration<int, ratio<3, 7>>{24}, L"24[3/7]s"));
    assert(test_duration_basic_out(duration<int, ratio<1, 2>>{24}, L"24[1/2]s"));
    assert(test_duration_basic_out(duration<int, ratio<22, 7>>{24}, L"24[22/7]s"));
    assert(test_duration_basic_out(duration<int, ratio<53, 101>>{24}, L"24[53/101]s"));
    assert(test_duration_basic_out(duration<int, ratio<201, 2147483647>>{24}, L"24[201/2147483647]s"));
    assert(test_duration_basic_out(duration<int, LongRatio>{24}, L"24[9223372036854775806/9223372036854775807]s"));

    assert(test_duration_basic_out(duration<double>{0.140625}, L"0.140625s"));
    assert(test_duration_locale_out<wchar_t>());
}


template <class CharT, class CStringOrStdString, class Parsable>
ios_base::iostate parse_state(const CharT* str, const CStringOrStdString& fmt, Parsable& p,
    type_identity_t<basic_string<CharT>*> abbrev = nullptr, minutes* offset = nullptr) {
    p = Parsable{};
    if (abbrev) {
        if constexpr (is_same_v<CharT, char>) {
            *abbrev = "!";
        } else {
            *abbrev = L"!";
        }
    }

    if (offset) {
        *offset = minutes::min();
    }

    basic_istringstream<CharT> sstr{str};
    if (abbrev) {
        if (offset) {
            sstr >> parse(fmt, p, *abbrev, *offset);
        } else {
            sstr >> parse(fmt, p, *abbrev);
        }
    } else {
        if (offset) {
            sstr >> parse(fmt, p, *offset);
        } else {
            sstr >> parse(fmt, p);
        }
    }

    return sstr.rdstate();
}

template <class CharT, class CStringOrStdString, class Parsable>
void test_parse(const CharT* str, const CStringOrStdString& fmt, Parsable& p,
    type_identity_t<basic_string<CharT>*> abbrev = nullptr, minutes* offset = nullptr,
    const source_location sl = source_location::current()) {
    const auto masked_state = parse_state(str, fmt, p, abbrev, offset) & ~ios_base::eofbit;
    const bool desirable    = masked_state == ios_base::goodbit;
    if (!desirable) {
        printf("test_parse() encountered a problem on line %u.\n", static_cast<unsigned int>(sl.line()));
    }
    assert(desirable);
}

template <class CharT, class CStringOrStdString, class Parsable>
void test_parse(const basic_string<CharT>& str, const CStringOrStdString& fmt, Parsable& p,
    type_identity_t<basic_string<CharT>*> abbrev = nullptr, minutes* offset = nullptr,
    const source_location sl = source_location::current()) {
    test_parse(str.c_str(), fmt, p, abbrev, offset, sl);
}

template <class CharT, class CStringOrStdString, class Parsable, class Wanted>
void want_value(const CharT* str, const CStringOrStdString& fmt, Parsable& p, const Wanted& wanted,
    const source_location sl = source_location::current()) {
    test_parse(str, fmt, p, nullptr, nullptr, sl);
    assert(p == wanted);
}

template <class CharT, class CStringOrStdString, class Parsable, class Wanted>
void want_value(const basic_string<CharT>& str, const CStringOrStdString& fmt, Parsable& p, const Wanted& wanted,
    const source_location sl = source_location::current()) {
    want_value(str.c_str(), fmt, p, wanted, sl);
}

template <class CharT, class CStringOrStdString, class Parsable>
void fail_parse(const CharT* str, const CStringOrStdString& fmt, Parsable& p,
    type_identity_t<basic_string<CharT>*> abbrev = nullptr, minutes* offset = nullptr,
    const source_location sl = source_location::current()) {
    const auto masked_state = parse_state(str, fmt, p, abbrev, offset) & ~ios_base::eofbit;
    const bool desirable    = masked_state != ios_base::goodbit;
    if (!desirable) {
        printf("fail_parse() encountered a problem on line %u.\n", static_cast<unsigned int>(sl.line()));
    }
    assert(desirable);
}

template <class CharT, class CStringOrStdString, class Parsable>
void fail_parse(const basic_string<CharT>& str, const CStringOrStdString& fmt, Parsable& p,
    type_identity_t<basic_string<CharT>*> abbrev = nullptr, minutes* offset = nullptr,
    const source_location sl = source_location::current()) {
    fail_parse(str.c_str(), fmt, p, abbrev, offset, sl);
}

template <class TimeType, class IntType = int>
void test_limits(const char* flag, const IntType min, const IntType max) {
    char buffer[24];
    TimeType value;
    auto conv_result = to_chars(begin(buffer), end(buffer), static_cast<make_signed_t<IntType>>(min) - 1);
    assert(conv_result.ec == errc{});
    assert(conv_result.ptr != end(buffer));
    *conv_result.ptr = '\0';
    fail_parse(buffer, flag, value);
    conv_result = to_chars(begin(buffer), end(buffer), max + 1);
    assert(conv_result.ec == errc{});
    assert(conv_result.ptr != end(buffer));
    *conv_result.ptr = '\0';
    fail_parse(buffer, flag, value);

    conv_result = to_chars(begin(buffer), end(buffer), min);
    assert(conv_result.ec == errc{});
    assert(conv_result.ptr != end(buffer));
    *conv_result.ptr = '\0';
    want_value(buffer, flag, value, TimeType{min});
    conv_result = to_chars(begin(buffer), end(buffer), max);
    assert(conv_result.ec == errc{});
    assert(conv_result.ptr != end(buffer));
    *conv_result.ptr = '\0';
    want_value(buffer, flag, value, TimeType{max});
}

void parse_seconds() {
    seconds time;
    want_value("1", "%S", time, 1s);
    want_value("12", "%S", time, 12s);
    want_value("234", "%S4", time, 23s);
    want_value("0345", "%3S5", time, 34s);
    want_value(" 456", "%3S6", time, 45s);
    want_value("99", "%S", time, 99s); // not out-of-range for duration

    milliseconds time_ms;
    want_value("12.543", "%S", time_ms, 12s + 543ms);
    assert(time_ms == seconds{12} + milliseconds{543});
    want_value("01.234", "%S", time_ms, 1'234ms);
    want_value(" 1.234", "%S", time_ms, 1'234ms);
    want_value("1.234", "%S", time_ms, 1'234ms);
    want_value("1. 234", "%S 234", time_ms, 1s); // Flag should consume "1.".
    want_value("1 .234", "%S .234", time_ms, 1s); // Flag should consume "1".
    want_value("12..345", "%S.345", time_ms, 12s); // Flag should consume "12.".
    fail_parse("1.2345", "%6S", time_ms); // would truncate
    want_value("1.2340", "%6S", time_ms, 1'234ms);

    duration<int64_t, atto> time_atto;
    test_parse("0.400000000000000002", "%S", time_atto);
    assert((time_atto == duration<int64_t, deci>{4} + duration<int64_t, atto>{2}));

    duration<float, ratio<1, 25>> time_float;
    test_parse("0.33", "%S", time_float);
    assert((time_float == duration<float, ratio<1, 25>>{8.25f}));

    fail_parse("1.2 1.3", "%S %S", time_ms);
    fail_parse("1.2 2.2", "%S %S", time_ms);
}

void parse_minutes() {
    seconds time;
    want_value("1", "%M", time, 1min);
    want_value("12", "%M", time, 12min);
    want_value("234", "%M4", time, 23min);
    want_value("0345", "%3M5", time, 34min);
    want_value(" 456", "%3M6", time, 45min);
    want_value("99", "%M", time, 99min); // not out-of-range for duration
}

void parse_hours() {
    seconds time;

    fail_parse("0", "%I", time);
    want_value("1", "%I", time, 1h);
    want_value("11", "%I", time, 11h);
    want_value("12", "%I", time, 0h); // assumes a.m.
    fail_parse("13", "%I", time);

    fail_parse("0", "%OI", time);
    want_value("1", "%OI", time, 1h);
    want_value("11", "%OI", time, 11h);
    want_value("12", "%OI", time, 0h); // assumes a.m.
    fail_parse("13", "%OI", time);

    want_value("110", "%I0", time, 11h);
    want_value("0011", "%3I1", time, 1h);
    want_value("010", "%4I", time, 10h);
    want_value(" 12", "%4I", time, 0h); // assumes A.M. with no %p flag

    want_value("1", "%H", time, 1h);
    want_value("12", "%H", time, 12h);
    want_value("234", "%H4", time, 23h);
    want_value("0123", "%3H3", time, 12h);
    want_value(" 234", "%3H4", time, 23h);
    want_value("30", "%H", time, 30h); // not out-of-range for duration

    // any permutation of %I, %p, and %H should be valid, as long as they're consistent
    want_value("8 pm 20", "%I %p %H", time, 20h);
    want_value("8 20 pm", "%I %H %p", time, 20h);
    want_value("pm 8 20", "%p %I %H", time, 20h);
    want_value("pm 20 8", "%p %H %I", time, 20h);
    want_value("20 pm 8", "%H %p %I", time, 20h);
    want_value("20 8 pm", "%H %I %p", time, 20h);

    // inconsistent
    fail_parse("8 am 20", "%I %p %H", time);
    fail_parse("8 20 am", "%I %H %p", time);
    fail_parse("am 8 20", "%p %I %H", time);
    fail_parse("am 20 8", "%p %H %I", time);
    fail_parse("20 am 8", "%H %p %I", time);
    fail_parse("20 8 am", "%H %I %p", time);
}

void parse_other_duration() {
    seconds time;
    want_value("42", "%j", time, days{42});
    duration<int32_t, micro> time_micro; // maximum ~35.8 minutes
    fail_parse("1", "%j", time_micro);
    want_value("400", "%j", time, days{400}); // not out-of-range for duration

    want_value(" 1:23:42", "%T", time, 1h + 23min + 42s);
    want_value("01:23:42", "%T", time, 1h + 23min + 42s);

    want_value("11: 2:42", "%T", time, 11h + 2min + 42s);
    want_value("11:02:42", "%T", time, 11h + 2min + 42s);

    want_value("12:34: 4", "%T", time, 12h + 34min + 4s);
    want_value("12:34:04", "%T", time, 12h + 34min + 4s);
    want_value("00:34:04", "%T", time, 34min + 4s);

    milliseconds time_milli;
    want_value("12:34:56.789", "%T", time_milli, 12h + 34min + 56s + 789ms);

    // locale's time representation %X (== "%H : %M : %S")
    want_value("12:34:04", "%X", time, 12h + 34min + 4s);

    // floating-point representations, parsing precision controlled by duration::period
    duration<float, milli> df;
    test_parse("9.125", "%S", df);
    assert(df.count() == 9125.0f);

    duration<double, milli> dd;
    test_parse("1.875", "%S", dd);
    assert(dd.count() == 1875.0);
}

void parse_time_zone() {
    seconds time;
    minutes offset;
    test_parse("-0430", "%z", time, nullptr, &offset);
    assert(offset == -(4h + 30min));
    test_parse("+0430", "%z", time, nullptr, &offset);
    assert(offset == (4h + 30min));
    test_parse("0430", "%z", time, nullptr, &offset);
    assert(offset == (4h + 30min));
    test_parse("11", "%z", time, nullptr, &offset);
    assert(offset == 11h);

    fail_parse("4", "%z", time, nullptr, &offset);
    fail_parse("043", "%z", time, nullptr, &offset);
    fail_parse("!0430", "%z", time, nullptr, &offset);

    test_parse("-04:30", "%Ez", time, nullptr, &offset);
    assert(offset == -(4h + 30min));
    test_parse("+04:30", "%Ez", time, nullptr, &offset);
    assert(offset == (4h + 30min));
    test_parse("04:30", "%Ez", time, nullptr, &offset);
    assert(offset == (4h + 30min));
    test_parse("4:30", "%Ez", time, nullptr, &offset);
    assert(offset == (4h + 30min));
    test_parse("11", "%Ez", time, nullptr, &offset);
    assert(offset == 11h);
    test_parse("4", "%Ez", time, nullptr, &offset);
    assert(offset == 4h);

    fail_parse("!4", "%Ez", time, nullptr, &offset);
    // %Ez matches "04", leaving "30" in the stream.
    fail_parse("0430 meow", "%Ez meow", time, nullptr, &offset);
    fail_parse("04:", "%Ez", time, nullptr, &offset);
    fail_parse("04:3", "%Ez", time, nullptr, &offset);

    string tz_name;
    test_parse("UTC", "%Z", time, &tz_name);
    assert(tz_name == "UTC");
    test_parse("Etc/GMT+11", "%Z", time, &tz_name);
    assert(tz_name == "Etc/GMT+11");
    test_parse("Etc/GMT-4", "%Z", time, &tz_name);
    assert(tz_name == "Etc/GMT-4");
    test_parse("Asia/Hong_Kong", "%Z", time, &tz_name);
    assert(tz_name == "Asia/Hong_Kong");

    fail_parse("Not_valid! 00", "%Z %H", time, &tz_name);
    test_parse("Valid_Tz! 07", "%Z! %H", time, &tz_name);
    assert(tz_name == "Valid_Tz");
    assert(time == 7h);
}

void parse_calendar_types_basic() {
    // basic day tests
    day d;
    want_value("1", "%d", d, day{1});
    want_value("23", "%d", d, day{23});
    want_value("012", "%d2", d, day{1});
    want_value(" 23", "%d3", d, day{2});

    test_limits<day, unsigned int>("%d", 1, 31);

    want_value("1", "%e", d, day{1});
    want_value("23", "%e", d, day{23});
    want_value("012", "%e2", d, day{1});
    want_value(" 23", "%e3", d, day{2});

    // basic weekday tests
    weekday wd;

    want_value("Mon", "%a", wd, Monday);
    want_value("wedNesday", "%A", wd, Wednesday);

    want_value("1", "%w", wd, Monday); // 0-based, Sunday=0
    want_value("1", "%u", wd, Monday); // ISO 1-based, Monday=1
    want_value("7", "%u", wd, Sunday);

    test_limits<weekday, unsigned int>("%w", 0, 6);
    test_limits<weekday, unsigned int>("%u", 1, 7);

    // basic month tests
    month m;
    want_value("Apr", "%b", m, April);
    want_value("deCeMbeR", "%b", m, December);
    want_value("September", "%B", m, September);
    want_value("February", "%h", m, February);

    want_value("3", "%m", m, March);
    want_value("11", "%m", m, November);
    want_value("110", "%m0", m, November);
    want_value("0011", "%3m1", m, January);
    want_value("010", "%4m", m, October);
    want_value(" 12", "%4m", m, December);

    test_limits<month, unsigned int>("%m", 1, 12);

    // basic year tests
    year y;
    want_value("1777", "%Y", y, 1777y);
    want_value("07 17", "%y %C", y, 1707y);
    want_value("18 077", "%C %3y", y, 1877y);

    // interpretation of two-digit year by itself
    want_value("00", "%y", y, 2000y);
    want_value("68", "%y", y, 2068y);
    want_value("69", "%y", y, 1969y);
    want_value("99", "%y", y, 1999y);

    // negative century
    want_value("-1 95", "%C %y", y, -95y);
    want_value("-2 00", "%C %y", y, -200y);

    // [time.parse] example
    want_value("-20 76", "%3C %y", y, -1976y);

    // check consistency, or lack thereof
    want_value("1887 18 87", "%Y %C %y", y, 1887y);
    fail_parse("1888 18 87", "%Y %C %y", y);
    fail_parse("1887 19 87", "%Y %C %y", y);
    fail_parse("1887 18 88", "%Y %C %y", y);

    // basic month_day tests
    month_day md;
    want_value("1 Jan 1", "%j %b %d", md, January / 1d);
    want_value("32 Feb 1", "%j %b %d", md, February / 1d);
    want_value("59 Feb 28", "%j %b %d", md, February / 28d);
    fail_parse("0", "%j", md);
    fail_parse("60", "%j", md); // could be Feb 29 or Mar 1

    want_value("January 1", "%b %d", md, January / 1d);
    want_value("January 31", "%b %d", md, January / 31d);

    want_value("February 1", "%b %d", md, February / 1d);
    want_value("February 29", "%b %d", md, February / 29d);

    want_value("March 1", "%b %d", md, March / 1d);
    want_value("March 31", "%b %d", md, March / 31d);

    want_value("April 1", "%b %d", md, April / 1d);
    want_value("April 30", "%b %d", md, April / 30d);

    want_value("May 1", "%b %d", md, May / 1d);
    want_value("May 31", "%b %d", md, May / 31d);

    want_value("June 1", "%b %d", md, June / 1d);
    want_value("June 30", "%b %d", md, June / 30d);

    want_value("July 1", "%b %d", md, July / 1d);
    want_value("July 31", "%b %d", md, July / 31d);

    want_value("August 1", "%b %d", md, August / 1d);
    want_value("August 31", "%b %d", md, August / 31d);

    want_value("September 1", "%b %d", md, September / 1d);
    want_value("September 30", "%b %d", md, September / 30d);

    want_value("October 1", "%b %d", md, October / 1d);
    want_value("October 31", "%b %d", md, October / 31d);

    want_value("November 1", "%b %d", md, November / 1d);
    want_value("November 30", "%b %d", md, November / 30d);

    want_value("December 1", "%b %d", md, December / 1d);
    want_value("December 31", "%b %d", md, December / 31d);

    // not ambiguous with year
    year_month_day ymd;
    want_value("60 2004-02-29", "%j %F", ymd, 2004y / February / 29d);

    // basic year_month_day tests
    // different ways of specifying year
    want_value("12-01-1997", "%d-%m-%Y", ymd, 12d / January / 1997y);
    want_value("12-01-19 97", "%d-%m-%C %y", ymd, 12d / January / 1997y);
    want_value("12-01-97", "%d-%m-%y", ymd, 12d / January / 1997y);

    // basic %D test
    want_value("07/04/76 17", "%D %C", ymd, July / 4d / 1776y);
    // locale's date representation %x (== "%m / %d / %y")
    want_value("07/04/76 17", "%x %C", ymd, 4d / July / 1776y);
    want_value("12/10/15 18", "%x %C", ymd, 10d / December / 1815y);

    // day-of-year tests, leap and non-leap years
    want_value("60 2001-03-01", "%j %F", ymd, 2001y / March / 1d);
    want_value("61 2004-03-01", "%j %F", ymd, 2004y / March / 1d);
    want_value("90 2001-03-31", "%j %F", ymd, 2001y / March / last);
    want_value("91 2004-03-31", "%j %F", ymd, 2004y / March / last);

    want_value("91 2001-04-01", "%j %F", ymd, 2001y / April / 1d);
    want_value("92 2004-04-01", "%j %F", ymd, 2004y / April / 1d);
    want_value("120 2001-04-30", "%j %F", ymd, 2001y / April / last);
    want_value("121 2004-04-30", "%j %F", ymd, 2004y / April / last);

    want_value("121 2001-05-01", "%j %F", ymd, 2001y / May / 1d);
    want_value("122 2004-05-01", "%j %F", ymd, 2004y / May / 1d);
    want_value("151 2001-05-31", "%j %F", ymd, 2001y / May / last);
    want_value("152 2004-05-31", "%j %F", ymd, 2004y / May / last);

    want_value("152 2001-06-01", "%j %F", ymd, 2001y / June / 1d);
    want_value("153 2004-06-01", "%j %F", ymd, 2004y / June / 1d);
    want_value("181 2001-06-30", "%j %F", ymd, 2001y / June / last);
    want_value("182 2004-06-30", "%j %F", ymd, 2004y / June / last);

    want_value("182 2001-07-01", "%j %F", ymd, 2001y / July / 1d);
    want_value("183 2004-07-01", "%j %F", ymd, 2004y / July / 1d);
    want_value("212 2001-07-31", "%j %F", ymd, 2001y / July / last);
    want_value("213 2004-07-31", "%j %F", ymd, 2004y / July / last);

    want_value("213 2001-08-01", "%j %F", ymd, 2001y / August / 1d);
    want_value("214 2004-08-01", "%j %F", ymd, 2004y / August / 1d);
    want_value("243 2001-08-31", "%j %F", ymd, 2001y / August / last);
    want_value("244 2004-08-31", "%j %F", ymd, 2004y / August / last);

    want_value("244 2001-09-01", "%j %F", ymd, 2001y / September / 1d);
    want_value("245 2004-09-01", "%j %F", ymd, 2004y / September / 1d);
    want_value("273 2001-09-30", "%j %F", ymd, 2001y / September / last);
    want_value("274 2004-09-30", "%j %F", ymd, 2004y / September / last);

    want_value("274 2001-10-01", "%j %F", ymd, 2001y / October / 1d);
    want_value("275 2004-10-01", "%j %F", ymd, 2004y / October / 1d);
    want_value("304 2001-10-31", "%j %F", ymd, 2001y / October / last);
    want_value("305 2004-10-31", "%j %F", ymd, 2004y / October / last);

    want_value("305 2001-11-01", "%j %F", ymd, 2001y / November / 1d);
    want_value("306 2004-11-01", "%j %F", ymd, 2004y / November / 1d);
    want_value("334 2001-11-30", "%j %F", ymd, 2001y / November / last);
    want_value("335 2004-11-30", "%j %F", ymd, 2004y / November / last);

    want_value("335 2001-12-01", "%j %F", ymd, 2001y / December / 1d);
    want_value("336 2004-12-01", "%j %F", ymd, 2004y / December / 1d);
    want_value("365 2001-12-31", "%j %F", ymd, 2001y / December / last);
    want_value("366 2004-12-31", "%j %F", ymd, 2004y / December / last);

    want_value("82 1882", "%j %Y", ymd, 23d / March / 1882y);
    fail_parse("366 2001", "%j %Y", ymd);
    fail_parse("367 2004", "%j %Y", ymd);

    // Check consistency between date and day-of-week
    test_parse("Wed 2000-03-01", "%a %F", ymd);
    fail_parse("Mon 2000-03-01", "%a %F", ymd);

    // For %F, width is applied only to the year
    want_value("12345-06-07", "%5F", ymd, 7d / June / 12345y);
    fail_parse("12345-00006-07", "%5F", ymd);
    fail_parse("12345-06-00007", "%5F", ymd);
    fail_parse("12345-00006-00007", "%5F", ymd);
}

void parse_iso_week_date() {
    year_month_day ymd;
    want_value("2005-W52-6", "%G-W%V-%u", ymd, 2005y / December / 31d);
    want_value("2005-W52-7", "%G-W%V-%u", ymd, 2006y / January / 1d);
    want_value("2006-W01-1", "%G-W%V-%u", ymd, 2006y / January / 2d);
    fail_parse("2006-W00-1", "%G-W%V-%u", ymd);

    want_value("2007-W52-7", "%G-W%V-%u", ymd, 2007y / December / 30d);
    want_value("2008-W01-1", "%G-W%V-%u", ymd, 2007y / December / 31d);
    want_value("2008-W01-2", "%G-W%V-%u", ymd, 2008y / January / 1d);

    fail_parse("05-W52-6", "%g-W%V-%u", ymd); // no century

    year_month_day ref{2005y / December / 31d};
    fail_parse("2005-W52-6 19", "%G-W%V-%u %C", ymd); // inconsistent century
    want_value("2005-W52-6 20", "%G-W%V-%u %C", ymd, ref); // consistent century
    want_value("05-W52-6 20", "%g-W%V-%u %C", ymd, ref);

    fail_parse("2005-W52-6 2004", "%G-W%V-%u %Y", ymd); // inconsistent year
    want_value("2005-W52-6 2005", "%G-W%V-%u %Y", ymd, ref); // consistent year
    want_value("05-W52-6 2005", "%g-W%V-%u %Y", ymd, ref);
    want_value("2005-W52-6 05", "%G-W%V-%u %y", ymd, ref);
    want_value("05-W52-6 05", "%g-W%V-%u %y", ymd, ref);

    ref = 2007y / December / 31d;
    fail_parse("2008-W01-1 2008", "%G-W%V-%u %Y", ymd); // inconsistent year (!)
    want_value("2008-W01-1 2007", "%G-W%V-%u %Y", ymd, ref); // consistent year
    want_value("08-W01-1 2007", "%g-W%V-%u %Y", ymd, ref);
    want_value("2008-W01-1 07", "%G-W%V-%u %y", ymd, ref);
    want_value("08-W01-1 07", "%g-W%V-%u %y", ymd, ref);

    // ISO and Gregorian years in different centuries
    want_value("1699-W53-5 1700", "%G-W%V-%u %Y", ymd, 1d / January / 1700y);
    fail_parse("1699-W54-5 1700", "%G-W%V-%u %Y", ymd);
    fail_parse("1699-W53-5 00", "%G-W%V-%u %y", ymd); // inconsistent %y (== 2000)
    fail_parse("99-W53-5 16 00", "%g-W%V-%u %C %y", ymd); // inconsistent %C+%y year (== 1600)
    fail_parse("99-W53-5 17 00", "%g-W%V-%u %C %y", ymd); // inconsistent %C+%g ISO year (== 1700)

    // This is expected to parse successfully. Even though %C+%g would give the wrong year,
    // as above we don't try to use that for the ISO year when %G is present.
    want_value("1699 99-W53-5 17 00", "%G %g-W%V-%u %C %y", ymd, 1d / January / 1700y);
    fail_parse("1699 98-W53-5 17 00", "%G %g-W%V-%u %C %y", ymd);
}

void parse_other_week_date() {
    year_month_day ymd;
    // Year begins on Sunday.
    want_value("2017-01-0", "%Y-%U-%w", ymd, 2017y / January / 1d);
    want_value("2017-00-0", "%Y-%W-%w", ymd, 2017y / January / 1d);
    want_value("2017-53-0", "%Y-%U-%w", ymd, 2017y / December / 31d);

    fail_parse("2017/-1/0", "%Y/%W/%w", ymd);
    fail_parse("2017/-1/0", "%Y/%U/%w", ymd);
    fail_parse("2017-54-0", "%Y-%W-%w", ymd);
    fail_parse("2017-54-0", "%Y-%U-%w", ymd);
    fail_parse("2018-00-0", "%Y-%U-%w", ymd); // refers to 31 Dec. 2017
    fail_parse("2017-53-1", "%Y-%U-%w", ymd); // refers to 01 Jan. 2018

    // Year begins on Monday.
    want_value("2018-00-1", "%Y-%U-%w", ymd, 2018y / January / 1d);
    want_value("2018-01-1", "%Y-%W-%w", ymd, 2018y / January / 1d);
    want_value("2018-53-1", "%Y-%W-%w", ymd, 2018y / December / 31d);

    // Year begins on Tuesday.
    want_value("2019-00-2", "%Y-%U-%w", ymd, 2019y / January / 1d);
    want_value("2019-00-2", "%Y-%W-%w", ymd, 2019y / January / 1d);

    // Year begins on Wednesday.
    want_value("2020-00-3", "%Y-%U-%w", ymd, 2020y / January / 1d);
    want_value("2020-00-3", "%Y-%W-%w", ymd, 2020y / January / 1d);

    // Year begins on Thursday.
    want_value("2015-00-4", "%Y-%U-%w", ymd, 2015y / January / 1d);
    want_value("2015-00-4", "%Y-%W-%w", ymd, 2015y / January / 1d);

    // Year begins on Friday.
    want_value("2016-00-5", "%Y-%U-%w", ymd, 2016y / January / 1d);
    want_value("2016-00-5", "%Y-%W-%w", ymd, 2016y / January / 1d);

    // Year begins on Saturday.
    want_value("2022-00-6", "%Y-%U-%w", ymd, 2022y / January / 1d);
    want_value("2022-00-6", "%Y-%W-%w", ymd, 2022y / January / 1d);
}

void parse_incomplete() {
    // Parsing should fail if the input is insufficient to supply all fields of the format string, even if the input is
    // sufficient to supply all fields of the parsable.
    // Check both explicit and shorthand format strings, since the code path is different.
    year_month ym;
    assert(parse_state("2021-01", "%Y-%m-%d", ym) == (ios_base::eofbit | ios_base::failbit));
    assert(parse_state("2022-02", "%F", ym) == (ios_base::eofbit | ios_base::failbit));
    assert(parse_state("2021-", "%Y-%m-%d", ym) == (ios_base::eofbit | ios_base::failbit));
    assert(parse_state("2022-", "%F", ym) == (ios_base::eofbit | ios_base::failbit));

    seconds time;
    fail_parse("01:59", "%H:%M:%S", time);
    fail_parse("03:23", "%T", time);
    fail_parse("04", "%R", time);

    // Check for parsing of whitespace fields after other fields.  More whitespace tests below.
    test_parse("15:19", "%H:%M%t", time);
    test_parse("15:19", "%R%t", time);
    fail_parse("15:19", "%H:%M%n", time);
    fail_parse("15:19", "%R%n", time);

    // However, it is OK to omit seconds from the format when parsing a duration to seconds precision.
    test_parse("05:24", "%H:%M", time);
    test_parse("06:25", "%R", time);
}

void parse_whitespace() {
    seconds time;
    fail_parse("ab", "a%nb", time);
    test_parse("a b", "a%nb", time);
    fail_parse("a  b", "a%nb", time);
    fail_parse("a", "a%n", time);
    test_parse("a ", "a%n", time);

    test_parse("ab", "a%tb", time);
    test_parse("a b", "a%tb", time);
    fail_parse("a  b", "a%tb", time);
    test_parse("a", "a%t", time);
    test_parse("a ", "a%t", time);

    test_parse("a", "a ", time);
    test_parse("", "", time);
    test_parse("", " ", time);
    test_parse("", "%t", time);
    fail_parse("", "%n", time);
}

tzdb copy_tzdb() {
    const auto& my_tzdb = get_tzdb_list().front();
    vector<time_zone> zones;
    vector<time_zone_link> links;
    transform(my_tzdb.zones.begin(), my_tzdb.zones.end(), back_inserter(zones),
        [](const auto& tz) { return time_zone{_Secret_time_zone_construct_tag{}, tz.name()}; });
    transform(my_tzdb.links.begin(), my_tzdb.links.end(), back_inserter(links), [](const auto& link) {
        return time_zone_link{_Secret_time_zone_link_construct_tag{}, link.name(), link.target()};
    });

    return {my_tzdb.version, move(zones), move(links), my_tzdb.leap_seconds, my_tzdb._All_ls_positive};
}

void test_gh_1952() {
    // GH-1952 <chrono>: parse ignores subseconds when the underlying type supports it
    const auto time_str{"2021-06-02T17:51:05.696028Z"};
    const auto fmt{"%FT%TZ"};
    const auto utc_ref = clock_cast<utc_clock>(sys_days{2021y / June / 2d} + 17h + 51min + 5s + 696028us);

    utc_time<microseconds> utc_parsed;
    want_value(time_str, fmt, utc_parsed, utc_ref);

    sys_time<microseconds> sys_parsed;
    want_value(time_str, fmt, sys_parsed, clock_cast<system_clock>(utc_ref));

    file_time<microseconds> file_parsed;
    want_value(time_str, fmt, file_parsed, clock_cast<file_clock>(utc_ref));

    local_time<microseconds> local_parsed;
    test_parse(time_str, fmt, local_parsed);
    assert(local_parsed.time_since_epoch() == clock_cast<system_clock>(utc_ref).time_since_epoch());

    const auto elapsed_leap = get_leap_second_info(utc_ref).elapsed;
    const auto tai_offset   = 10s + elapsed_leap;
    tai_time<microseconds> tai_parsed;
    test_parse(time_str, fmt, tai_parsed);
    assert(tai_parsed + tai_offset == clock_cast<tai_clock>(utc_ref));

    const auto gps_offset = -9s + elapsed_leap;
    gps_time<microseconds> gps_parsed;
    test_parse(time_str, fmt, gps_parsed);
    assert(gps_parsed + gps_offset == clock_cast<gps_clock>(utc_ref));
}

void parse_timepoints() {
    sys_seconds ref = sys_days{2020y / October / 29d} + 19h + 1min + 42s;
    sys_seconds st;
    utc_seconds ut;
    file_time<seconds> ft;
    local_seconds lt;

    test_parse("thu oct 29 19:01:42 2020", "%c", st);
    test_parse("thu oct 29 19:01:42 2020", "%c", ut);
    test_parse("thu oct 29 19:01:42 2020", "%c", ft);
    test_parse("thu oct 29 19:01:42 2020", "%c", lt);

    assert(st == ref);
    assert(ut == clock_cast<utc_clock>(ref));
    assert(ft == clock_cast<file_clock>(ref));
    assert(lt.time_since_epoch() == ref.time_since_epoch());

    minutes offset;
    test_parse("thu oct 29 19:01:42 2020 0430", "%c %z", st, nullptr, &offset);
    assert(st == ref - offset);
    assert(offset == 4h + 30min);

    test_parse("thu oct 29 19:01:42 2020 0430", "%c %z", ut, nullptr, &offset);
    assert(ut == clock_cast<utc_clock>(ref) - offset);
    assert(offset == 4h + 30min);

    test_parse("thu oct 29 19:01:42 2020 0430", "%c %z", ft, nullptr, &offset);
    assert(ft == clock_cast<file_clock>(ref) - offset);
    assert(offset == 4h + 30min);

    test_parse("thu oct 29 19:01:42 2020 0430", "%c %z", lt, nullptr, &offset);
    assert(lt.time_since_epoch() == ref.time_since_epoch());
    assert(offset == 4h + 30min);

    // N4878 [time.clock.tai]/1:
    // The clock tai_clock measures seconds since 1958-01-01 00:00:00 and is offset 10s ahead of UTC at this date.
    // That is, 1958-01-01 00:00:00 TAI is equivalent to 1957-12-31 23:59:50 UTC. Leap seconds are not inserted into
    // TAI. Therefore every time a leap second is inserted into UTC, UTC shifts another second with respect to TAI.
    // For example by 2000-01-01 there had been 22 positive and 0 negative leap seconds inserted so 2000-01-01
    // 00:00:00 UTC is equivalent to 2000-01-01 00:00:32 TAI (22s plus the initial 10s offset).

    ref = sys_days{1957y / December / 31d} + days{1} - 10s;
    tai_seconds tt;
    want_value("wed jan  1 00:00:00 1958", "%c", tt, clock_cast<tai_clock>(ref));

    ref = sys_days{2000y / January / 1d};
    want_value("sat jan  1 00:00:32 2000", "%c", tt, clock_cast<tai_clock>(ref));
    test_parse("sat jan  1 00:00:32 2000 0430", "%c %z", tt, nullptr, &offset);
    assert(tt == clock_cast<tai_clock>(ref) - offset);
    assert(offset == 4h + 30min);

    // N4878 [time.clock.gps]/1:
    // The clock gps_clock measures seconds since the first Sunday of January, 1980 00:00:00 UTC. Leap seconds are
    // not inserted into GPS. Therefore every time a leap second is inserted into UTC, UTC shifts another second
    // with respect to GPS. Aside from the offset from 1958y/January/1 to 1980y/January/Sunday[1], GPS is behind TAI
    // by 19s due to the 10s offset between 1958 and 1970 and the additional 9 leap seconds inserted between 1970
    // and 1980.

    gps_seconds gt;
    ref = sys_days{1980y / January / 6d};
    test_parse("sun jan  6 00:00:00 1980 0430", "%c %z", gt, nullptr, &offset);
    assert(gt == clock_cast<gps_clock>(ref) - offset);
    assert(offset == 4h + 30min);
    want_value("sun jan  6 00:00:00 1980", "%c", gt, clock_cast<gps_clock>(ref));
    test_parse("sun jan  6 00:00:19 1980", "%c", tt);
    assert(gt == clock_cast<gps_clock>(tt));

    seconds time;
    want_value(" 1:23:42 am", "%r", time, 1h + 23min + 42s);
    want_value("2000-01-02 01:23:42 pm", "%F %r", st, sys_days{2000y / January / 2d} + (13h + 23min + 42s));

    want_value("11: 2:42 am", "%r", time, 11h + 2min + 42s);
    want_value("2000-01-02 11:02:42 pm", "%F %r", st, sys_days{2000y / January / 2d} + (23h + 2min + 42s));

    want_value("12:34: 4 am", "%r", time, 34min + 4s);
    want_value("2000-01-02 12:34:04 pm", "%F %r", st, sys_days{2000y / January / 2d} + (12h + 34min + 4s));


    want_value(" 3:14 2000-01-02", "%R %F", st, sys_days{2000y / January / 2d} + (3h + 14min));
    want_value("03:14 2000-01-02", "%R %F", st, sys_days{2000y / January / 2d} + (3h + 14min));

    want_value("11: 3 2000-01-02", "%R %F", st, sys_days{2000y / January / 2d} + (11h + 3min));
    want_value("11:03 2000-01-02", "%R %F", st, sys_days{2000y / January / 2d} + (11h + 3min));

    want_value("00:42 2000-01-02", "%R %F", st, sys_days{2000y / January / 2d} + (42min));
    want_value("12:42 2000-01-02", "%R %F", st, sys_days{2000y / January / 2d} + (12h + 42min));

    // Historical leap seconds don't allow complete testing, because they've all been positive and there haven't been
    // any since 2016 (as of 2021).
    {
        auto my_tzdb   = copy_tzdb();
        auto& leap_vec = my_tzdb.leap_seconds;
        leap_vec.erase(leap_vec.begin() + 27, leap_vec.end());
        leap_vec.emplace_back(sys_days{1d / January / 2020y}, false, leap_vec.back()._Elapsed());
        leap_vec.emplace_back(sys_days{1d / January / 2022y}, true, leap_vec.back()._Elapsed());
        my_tzdb._All_ls_positive = false;
        get_tzdb_list()._Emplace_front(move(my_tzdb));
    }

    utc_seconds ut_ref = utc_clock::from_sys(sys_days{1d / July / 1972y}) - 1s; // leap second insertion
    want_value("fri june 30 23:59:60 1972", "%c", ut, ut_ref);

    // Test a later leap second, where the accumulated offset is greater than 1s.
    ut_ref = utc_clock::from_sys(sys_days{1d / July / 1992y}) - 1s;
    want_value("tue june 30 23:59:60 1992", "%c", ut, ut_ref);

    // not leap-second aware
    fail_parse("fri june 30 23:59:60 1972", "%c", st);
    fail_parse("fri june 30 23:59:60 1972", "%c", tt);
    fail_parse("fri june 30 23:59:60 1972", "%c", gt);
    fail_parse("fri june 30 23:59:60 1972", "%c", ft);

    fail_parse("sat june 30 23:59:60 1973", "%c", ut); // not a leap second insertion

    // the last leap second insertion that file_clock is not aware of
    test_parse("sat dec 31 23:59:59 2016", "%c", ut);
    want_value("sat dec 31 23:59:59 2016", "%c", ft, clock_cast<file_clock>(ut));

    test_parse("sat dec 31 23:59:60 2016", "%c", ut);
    fail_parse("sat dec 31 23:59:60 2016", "%c", ft);

    test_parse("sun jan 01 00:00:00 2017", "%c", ut);
    want_value("sun jan 01 00:00:00 2017", "%c", ft, clock_cast<file_clock>(ut));

    ref = sys_days{1d / January / 2020y} - 1s; // negative leap second, UTC time doesn't exist
    fail_parse("tue dec 31 23:59:59 2019", "%c", ut);
    fail_parse("tue dec 31 23:59:59 2019", "%c", ft);

    want_value("tue dec 31 23:59:59 2019", "%c", st, ref);

    test_parse("tue dec 31 23:59:59 2019", "%c", lt); // Not UTC, might be valid depending on the time zone.
    assert(lt.time_since_epoch() == ref.time_since_epoch());

    // Initially, TAI - UTC == 37s.
    test_parse("tue dec 31 23:59:59 2019", "%c", tt);
    test_parse("tue dec 31 23:59:22 2019", "%c", ut);
    assert(tt == clock_cast<tai_clock>(ut));

    test_parse("wed jan 01 00:00:35 2020", "%c", tt);
    test_parse("tue dec 31 23:59:58 2019", "%c", ut);
    assert(tt == clock_cast<tai_clock>(ut));

    test_parse("wed jan 01 00:00:36 2020", "%c", tt);
    test_parse("wed jan 01 00:00:00 2020", "%c", ut);
    assert(tt == clock_cast<tai_clock>(ut));

    // Initially, GPS - UTC == 18s
    test_parse("tue dec 31 23:59:59 2019", "%c", gt);
    test_parse("tue dec 31 23:59:41 2019", "%c", ut);
    assert(gt == clock_cast<gps_clock>(ut));

    test_parse("wed jan 01 00:00:16 2020", "%c", gt);
    test_parse("tue dec 31 23:59:58 2019", "%c", ut);
    assert(gt == clock_cast<gps_clock>(ut));

    test_parse("wed jan 01 00:00:17 2020", "%c", gt);
    test_parse("wed jan 01 00:00:00 2020", "%c", ut);
    assert(gt == clock_cast<gps_clock>(ut));


    ut_ref = utc_clock::from_sys(sys_days{1d / January / 2022y}) - 1s; // leap second insertion
    want_value("fri dec 31 23:59:60 2021", "%c", ut, ut_ref);
    want_value("fri dec 31 23:59:60 2021", "%c", ft, clock_cast<file_clock>(ut_ref));


    // GH-1606: reads too many leading zeros
    want_value("19700405T000006", "%Y%m%dT%H%M%S", st, sys_days{5d / April / 1970y} + 6s);

    // GH-1280 tests
    year_month_day ymd;
    want_value("20200609", "%Y%m%d", ymd, 9d / June / 2020y);

    want_value("20201213", "%Y%m%d", ymd, 13d / December / 2020y);

    want_value("2020112", "%Y%m%d", ymd, 2d / November / 2020y);

    want_value("2020061125", "%Y%m%d", ymd, 11d / June / 2020y);

    want_value("2020120625119", "%Y%m%d", ymd, 6d / December / 2020y);

    want_value("2020092Text", "%Y%m%d", ymd, 2d / September / 2020y);

    want_value("20200609", "%Y%m%d", ymd, 9d / June / 2020y);

    want_value("2020112", "%Y%m%d", ymd, 2d / November / 2020y);

    want_value("2020061125", "%Y%m%d", ymd, 11d / June / 2020y);

    want_value("2020124", "%Y%m%d", ymd, 4d / December / 2020y);

    want_value("2020104Text", "%Y%m%d", ymd, 4d / October / 2020y);

    fail_parse("202000000000000923", "%Y%m%d", ymd);
    fail_parse("202000000000000923", "%Y%m%d", ymd);

    // time_point out-of-range tests
    fail_parse("1887-12-22 00:00:-1", "%F %T", st);
    fail_parse("1887-12-22 00:00:60", "%F %T", st);
    fail_parse("1887-12-22 00:-1:00", "%F %T", st);
    fail_parse("1887-12-22 00:60:00", "%F %T", st);
    fail_parse("1887-12-22 -1:00:00", "%F %T", st);
    fail_parse("1887-12-22 24:00:00", "%F %T", st);

    want_value("1912-06-23 00:00:00", "%F %T", st, sys_days{23d / June / 1912y});
    want_value("1912-06-23 23:59:59", "%F %T", st, sys_days{23d / June / 1912y} + 23h + 59min + 59s);

    // GH-1938: parsing timepoint with year and day-of-year
    want_value("1882.82", "%Y.%j", st, sys_days{23d / March / 1882y});

    test_gh_1952();

    // GH-2698: 00:00:60 incorrectly parsed
    fail_parse("2021-08-28 00:00:60", "%F %T", ut);

    fail_parse("2017-01-01 05:29:60", "%F %T", ut);
    want_value(
        "2017-01-01 05:29:60 +05:30", "%F %T %Ez", ut, clock_cast<utc_clock>(sys_days{1d / January / 2017y}) - 1s);
}

void parse_modified_maximum_characters() {
    const string hundred_digits(100, '1');
    const string seventy_zeroes(70, '0');

    {
        day d{7};
        want_value(hundred_digits, "%d", d, day{11});
        fail_parse(seventy_zeroes + "22", "%d", d); // 0 is not a valid day of the month
        fail_parse(hundred_digits, "%100d", d); // overflow
        want_value(seventy_zeroes + "22", "%100d", d, day{22});

        want_value(hundred_digits, "%e", d, day{11});
        fail_parse(seventy_zeroes + "22", "%e", d); // 0 is not a valid day of the month
        fail_parse(hundred_digits, "%100e", d); // overflow
        want_value(seventy_zeroes + "22", "%100e", d, day{22});
    }

    {
        hours h{33h};
        want_value(hundred_digits, "%H", h, 11h);
        want_value(seventy_zeroes + "22", "%H", h, 0h);
        fail_parse(hundred_digits, "%100H", h); // overflow
        want_value(seventy_zeroes + "22", "%100H", h, 22h);

        want_value(hundred_digits, "%I", h, 11h);
        fail_parse(seventy_zeroes + "7", "%I", h); // 0 is not a valid 12-hour clock
        fail_parse(hundred_digits, "%100I", h); // overflow
        want_value(seventy_zeroes + "7", "%100I", h, 7h);
    }

    {
        days ds{22};
        want_value(hundred_digits, "%j", ds, days{111});
        want_value(seventy_zeroes + "1729", "%j", ds, days{0});
        fail_parse(hundred_digits, "%100j", ds); // overflow
        want_value(seventy_zeroes + "1729", "%100j", ds, days{1729});
    }

    {
        year_month_day ymd{1970y / January / 1d};
        want_value("2030 " + hundred_digits, "%Y %j", ymd, 2030y / April / 21d);
        fail_parse("2030 " + seventy_zeroes + "124", "%Y %j", ymd); // 0 is not a valid day of the year
        fail_parse("2030 " + hundred_digits, "%Y %100j", ymd); // overflow
        want_value("2030 " + seventy_zeroes + "124", "%Y %100j", ymd, 2030y / May / 4d);

        fail_parse(hundred_digits + "-02-03", "%100F", ymd); // overflow
        want_value(seventy_zeroes + "1969-07-20", "%100F", ymd, 1969y / July / 20d);

        want_value("2015 4 " + hundred_digits, "%Y %u %U", ymd, 2015y / March / 19d);
        want_value("2015 4 " + seventy_zeroes + "6", "%Y %u %U", ymd, 2015y / January / 1d);
        fail_parse("2015 4 " + hundred_digits, "%Y %u %100U", ymd); // overflow
        want_value("2015 4 " + seventy_zeroes + "6", "%Y %u %100U", ymd, 2015y / February / 12d);

        want_value("2015 4 " + hundred_digits, "%Y %u %W", ymd, 2015y / March / 19d);
        want_value("2015 4 " + seventy_zeroes + "6", "%Y %u %W", ymd, 2015y / January / 1d);
        fail_parse("2015 4 " + hundred_digits, "%Y %u %100W", ymd); // overflow
        want_value("2015 4 " + seventy_zeroes + "6", "%Y %u %100W", ymd, 2015y / February / 12d);

        want_value("4 03 19 " + hundred_digits, "%u %V %C %g", ymd, 1911y / January / 19d);
        want_value("4 03 19 " + seventy_zeroes + "33", "%u %V %C %g", ymd, 1900y / January / 18d);
        fail_parse("4 03 19 " + hundred_digits, "%u %V %C %100g", ymd); // overflow
        want_value("4 03 19 " + seventy_zeroes + "55", "%u %V %C %100g", ymd, 1955y / January / 20d);

        want_value("4 03 " + hundred_digits, "%u %V %G", ymd, 1111y / January / 19d);
        want_value("4 03 " + seventy_zeroes + "2030", "%u %V %G", ymd, 0y / January / 20d);
        fail_parse("4 03 " + hundred_digits, "%u %V %100G", ymd); // overflow
        want_value("4 03 " + seventy_zeroes + "2030", "%u %V %100G", ymd, 2030y / January / 17d);

        want_value("4 2015 " + hundred_digits, "%u %G %V", ymd, 2015y / March / 12d);
        fail_parse("4 2015 " + seventy_zeroes + "33", "%u %G %V", ymd); // 0 is not a valid ISO week
        fail_parse("4 2015 " + hundred_digits, "%u %G %100V", ymd); // overflow
        want_value("4 2015 " + seventy_zeroes + "33", "%u %G %100V", ymd, 2015y / August / 13d);
    }

    {
        month mo{January};
        want_value(hundred_digits, "%m", mo, November);
        fail_parse(seventy_zeroes + "12", "%m", mo); // 0 is not a valid month
        fail_parse(hundred_digits, "%100m", mo); // overflow
        want_value(seventy_zeroes + "12", "%100m", mo, December);
    }

    {
        minutes mi{22min};
        want_value(hundred_digits, "%M", mi, 11min);
        want_value(seventy_zeroes + "55", "%M", mi, 0min);
        fail_parse(hundred_digits, "%100M", mi); // overflow
        want_value(seventy_zeroes + "55", "%100M", mi, 55min);
    }

    {
        seconds s{22s};
        want_value(hundred_digits, "%S", s, 11s);
        want_value(seventy_zeroes + "55", "%S", s, 0s);
        fail_parse(hundred_digits, "%100S", s); // overflow
        want_value(seventy_zeroes + "55", "%100S", s, 55s);
    }

    {
        weekday wd{Thursday};
        want_value(hundred_digits, "%u", wd, Monday);
        fail_parse(seventy_zeroes + "2", "%u", wd); // 0 is not a valid weekday for %u
        fail_parse(hundred_digits, "%100u", wd); // overflow
        want_value(seventy_zeroes + "2", "%100u", wd, Tuesday);

        want_value(hundred_digits, "%w", wd, Monday);
        want_value(seventy_zeroes + "2", "%w", wd, Sunday);
        fail_parse(hundred_digits, "%100w", wd); // overflow
        want_value(seventy_zeroes + "2", "%100w", wd, Tuesday);
    }

    {
        year y{1970y};
        want_value(hundred_digits, "%y", y, 2011y);
        want_value(seventy_zeroes + "55", "%y", y, 2000y);
        fail_parse(hundred_digits, "%100y", y); // overflow
        want_value(seventy_zeroes + "55", "%100y", y, 2055y);

        want_value(hundred_digits, "%Y", y, 1111y);
        want_value(seventy_zeroes + "2026", "%Y", y, 0y);
        fail_parse(hundred_digits, "%100Y", y); // overflow
        want_value(seventy_zeroes + "2026", "%100Y", y, 2026y);

        want_value("33 " + hundred_digits, "%y %C", y, 1133y);
        want_value("33 " + seventy_zeroes + "22", "%y %C", y, 33y);
        fail_parse("33 " + hundred_digits, "%y %100C", y); // overflow
        want_value("33 " + seventy_zeroes + "22", "%y %100C", y, 2233y);
    }
}

template <class CharT, class CStringOrStdString>
void test_io_manipulator() {
    seconds time;
    want_value(WIDEN(CharT, "12"), CStringOrStdString{WIDEN(CharT, "%S")}, time, 12s);
    want_value(WIDEN(CharT, "12"), CStringOrStdString{WIDEN(CharT, "%M")}, time, 12min);
    want_value(WIDEN(CharT, "30"), CStringOrStdString{WIDEN(CharT, "%H")}, time, 30h);
    want_value(WIDEN(CharT, " 1:23:42"), CStringOrStdString{WIDEN(CharT, "%T")}, time, 1h + 23min + 42s);
    basic_string<CharT> tz_name;
    test_parse(WIDEN(CharT, "Etc/GMT+11"), CStringOrStdString{WIDEN(CharT, "%Z")}, time, &tz_name);
    assert(tz_name == WIDEN(CharT, "Etc/GMT+11"));
    fail_parse(WIDEN(CharT, "Not_valid! 00"), CStringOrStdString{WIDEN(CharT, "%Z %H")}, time, &tz_name);

    weekday wd;
    want_value(WIDEN(CharT, "wedNesday"), CStringOrStdString{WIDEN(CharT, "%A")}, wd, Wednesday);

    month m;
    want_value(WIDEN(CharT, "deCeMbeR"), CStringOrStdString{WIDEN(CharT, "%b")}, m, December);

    sys_seconds st;
    want_value(WIDEN(CharT, "oct 29 19:01:42 2020"), CStringOrStdString{WIDEN(CharT, "%c")}, st,
        sys_days{2020y / October / 29d} + 19h + 1min + 42s);

    fail_parse(WIDEN(CharT, "ab"), CStringOrStdString{WIDEN(CharT, "a%nb")}, time);
    test_parse(WIDEN(CharT, "a b"), CStringOrStdString{WIDEN(CharT, "a%nb")}, time);
    fail_parse(WIDEN(CharT, "a  b"), CStringOrStdString{WIDEN(CharT, "a%nb")}, time);
}

void test_lwg_3536() {
    // LWG-3536, "Should chrono::from_stream() assign zero to duration for failure?"
    minutes mm{20};

    {
        istringstream iss{"2:2:30"};
        iss >> parse("%H:%M:%S", mm);
        assert(iss.fail());
        assert(mm == 20min);
    }

    {
        istringstream iss{"June"};
        iss >> parse("%B", mm);
        assert(iss.fail());
        assert(mm == 20min);
    }

    {
        istringstream iss{""};
        iss >> parse("%B", mm);
        assert(iss.fail());
        assert(mm == 20min);
    }
}

namespace lwg_3956 {
    struct has_adl_from_stream {
        int value = 0;

        template <class CharT, class Traits, class... ArgTypes>
        friend basic_istream<CharT, Traits>& from_stream(
            basic_istream<CharT, Traits>& istr, const CharT*, has_adl_from_stream& parsed, ArgTypes&&...) {
            parsed.value = 42;
            return istr;
        }
    };

    struct has_no_adl_from_stream {
        operator year&() &;
    };

    template <class... ArgTypes>
    concept can_parse = requires(ArgTypes&&... args) { parse(forward<ArgTypes>(args)...); };

    static_assert(can_parse<const char*, has_adl_from_stream&>);
    static_assert(can_parse<const string&, has_adl_from_stream&>);
    static_assert(can_parse<const wchar_t*, has_adl_from_stream&>);
    static_assert(can_parse<const wstring&, has_adl_from_stream&>);

    static_assert(!can_parse<const char*, has_no_adl_from_stream&>);
    static_assert(!can_parse<const string&, has_no_adl_from_stream&>);
    static_assert(!can_parse<const wchar_t*, has_no_adl_from_stream&>);
    static_assert(!can_parse<const wstring&, has_no_adl_from_stream&>);
} // namespace lwg_3956

void test_lwg_3956() {
    {
        lwg_3956::has_adl_from_stream parsed{};
        test_parse("", "", parsed);
        assert(parsed.value == 42);
    }
    {
        lwg_3956::has_adl_from_stream parsed{};
        test_parse(L"", L"", parsed);
        assert(parsed.value == 42);
    }
}

void test_all_parse() {
    parse_seconds();
    parse_minutes();
    parse_hours();
    parse_other_duration();
    parse_time_zone();
    parse_calendar_types_basic();
    parse_iso_week_date();
    parse_other_week_date();
    parse_incomplete();
    parse_whitespace();
    parse_timepoints();
    parse_modified_maximum_characters();
    test_io_manipulator<char, const char*>();
    test_io_manipulator<wchar_t, const wchar_t*>();
    test_io_manipulator<char, string>();
    test_io_manipulator<wchar_t, wstring>();
    test_lwg_3536();
    test_lwg_3956();
}

void test() {
    test_duration_output();
    test_all_parse();
}

int main() {
    run_tz_test([] { test(); });
}
