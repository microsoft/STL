// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <iomanip>
#include <ios>
#include <locale>
#include <sstream>
#include <streambuf>
#include <string>
#include <time.h>
#include <tuple>

using namespace std;

// DevDiv-821672 "<locale>: visual studio.net 2013 time libraries buggy (%x %X) - time_get"
// DevDiv-836436 "<iomanip>: get_time()'s AM/PM parsing is broken"
// DevDiv-872926 "<locale>: time_get::get parsing format string gets tm::tm_hour wrong [libcxx]"

tm helper(const char* const s, const char* const fmt) {
    tm t{};

    istringstream iss(s);

    iss >> get_time(&t, fmt);

    assert(static_cast<bool>(iss));

    return t;
}

tm helper_locale(const wchar_t* const s, const wchar_t* const fmt, const char* _loc) {
    tm t{};

    wstringstream wss(s);
    wss.imbue(locale(_loc));
    wss >> get_time(&t, fmt);

    assert(static_cast<bool>(wss));

    return t;
}

int read_hour(const char* const s) {
    const auto t = helper(s, "%I %p");

    // %I The hour (12-hour clock) [01,12]; leading zeros are permitted but not required.
    // %p The locale's equivalent of a.m or p.m.

    // int tm_hour; // hours since midnight - [0, 23]

    return t.tm_hour;
}

tuple<int, int, int> read_date(const char* const s) {
    const auto t = helper(s, "%x");

    // %x The date, using the locale's date format.
    // "%d / %m / %y"
    // %d The day of the month [01,31]; leading zeros are permitted but not required.
    // %m The month number [01,12]; leading zeros are permitted but not required.
    // %y The year within century. When a century is not otherwise specified,
    //    values in the range [69,99] shall refer to years 1969 to 1999 inclusive, and
    //    values in the range [00,68] shall refer to years 2000 to 2068 inclusive;
    //    leading zeros shall be permitted but shall not be required.

    // int tm_mday; // day of the month - [1, 31]
    // int tm_mon;  // months since January - [0, 11]
    // int tm_year; // years since 1900

    return make_tuple(t.tm_mday, t.tm_mon, t.tm_year);
}

tuple<int, int, int> read_date_locale(const wchar_t* const s, const char* _loc) {
    const auto t = helper_locale(s, L"%Y-%b-%d", _loc);

    // %x The date, using the locale's date format.
    // "%d / %m / %y"
    // %d The day of the month [01,31]; leading zeros are permitted but not required.
    // %m The month number [01,12]; leading zeros are permitted but not required.
    // %y The year within century. When a century is not otherwise specified,
    //    values in the range [69,99] shall refer to years 1969 to 1999 inclusive, and
    //    values in the range [00,68] shall refer to years 2000 to 2068 inclusive;
    //    leading zeros shall be permitted but shall not be required.

    // int tm_mday; // day of the month - [1, 31]
    // int tm_mon;  // months since January - [0, 11]
    // int tm_year; // years since 1900

    return make_tuple(t.tm_mday, t.tm_mon, t.tm_year);
}


tuple<int, int, int> read_time(const char* const s) {
    const auto t = helper(s, "%X");

    // %X The time, using the locale's time format.
    // "%H : %M : %S"
    // %H The hour (24-hour clock) [00,23]; leading zeros are permitted but not required.
    // %M The minute [00,59]; leading zeros are permitted but not required.
    // %S The seconds [00,60]; leading zeros are permitted but not required.

    // int tm_hour; // hours since midnight - [0, 23]
    // int tm_min;  // minutes after the hour - [0, 59]
    // int tm_sec;  // seconds after the minute - [0, 60]

    return make_tuple(t.tm_hour, t.tm_min, t.tm_sec);
}

void test_640278();
void test_990695();
void test_locale_russian();
void test_locale_german();
void test_locale_chines();

int main() {
    assert(read_hour("12 AM") == 0);
    assert(read_hour("12 am") == 0);

    assert(read_hour("1 AM") == 1);
    assert(read_hour("1 am") == 1);
    assert(read_hour("1 aM") == 1);
    assert(read_hour("1 Am") == 1);

    assert(read_hour("2 AM") == 2);
    assert(read_hour("2 am") == 2);

    assert(read_hour("11 AM") == 11);
    assert(read_hour("11 am") == 11);

    assert(read_hour("12 PM") == 12);
    assert(read_hour("12 pm") == 12);

    assert(read_hour("1 PM") == 13);
    assert(read_hour("1 pm") == 13);
    assert(read_hour("1 Pm") == 13);
    assert(read_hour("1 pM") == 13);

    assert(read_hour("2 PM") == 14);
    assert(read_hour("2 pm") == 14);

    assert(read_hour("11 PM") == 23);
    assert(read_hour("11 pm") == 23);

    assert(read_date("22 / 4 / 77") == make_tuple(22, /*NOTE DIFFERENCE:*/ 3, 77));

    assert(read_date("22 / 4 / 11") == make_tuple(22, /*NOTE DIFFERENCE:*/ 3, /*NOTE DIFFERENCE:*/ 111));

    assert(read_time("15 : 47 : 58") == make_tuple(15, 47, 58));

    assert(read_time("15 : 47 : 60") == make_tuple(15, 47, 60));

    test_640278();
    test_990695();
    test_locale_russian();
    test_locale_german();
    test_locale_chines();
}

typedef istreambuf_iterator<char> Iter;

// DevDiv-640278 "<locale>: time_get::do_get_year() thinks the world will end in 2035"
void test_year(const string& str, const ios_base::iostate expected_err, const int expected_tm_year) {
    istringstream iss(str);
    ios_base::iostate err = ios_base::goodbit;
    tm t{};
    use_facet<time_get<char>>(iss.getloc()).get_year(Iter(iss.rdbuf()), Iter(), iss, err, &t);
    assert(err == expected_err);
    assert(t.tm_year == expected_tm_year);
}

void test_640278() {
    test_year("", ios_base::eofbit | ios_base::failbit, 0);

    test_year("xyz", ios_base::failbit, 0);

    test_year("00", ios_base::eofbit, 100);
    test_year("01", ios_base::eofbit, 101);
    test_year("47", ios_base::eofbit, 147);
    test_year("68", ios_base::eofbit, 168);
    test_year("69", ios_base::eofbit, 69);
    test_year("83", ios_base::eofbit, 83);
    test_year("98", ios_base::eofbit, 98);
    test_year("99", ios_base::eofbit, 99);
    test_year("1776", ios_base::eofbit, -124);
    test_year("1899", ios_base::eofbit, -1);
    test_year("1900", ios_base::eofbit, 0);
    test_year("1901", ios_base::eofbit, 1);
    test_year("1955", ios_base::eofbit, 55);
    test_year("1985", ios_base::eofbit, 85);
    test_year("2014", ios_base::eofbit, 114);
    test_year("2161", ios_base::eofbit, 261);

    test_year("2013 frozen", ios_base::goodbit, 113);
}

// DevDiv-990695 "<locale>: time_get should ignore ios_base::iostate's initial value"
void test_990695() {
    for (int k = 0; k < 2; ++k) {
        const auto Bit = k == 0 ? ios_base::goodbit : ios_base::failbit;

        {
            istringstream iss("1983");
            ios_base::iostate err = Bit;
            tm t{};
            use_facet<time_get<char>>(iss.getloc()).get_year(Iter(iss.rdbuf()), Iter(), iss, err, &t);
            assert(t.tm_year == 83);
        }

        {
            istringstream iss("20");
            ios_base::iostate err = Bit;
            tm t{};
            use_facet<time_get<char>>(iss.getloc()).get(Iter(iss.rdbuf()), Iter(), iss, err, &t, 'C', '\0');
            assert(t.tm_year == 100);
        }

        {
            istringstream iss("7");
            ios_base::iostate err = Bit;
            tm t{};
            use_facet<time_get<char>>(iss.getloc()).get(Iter(iss.rdbuf()), Iter(), iss, err, &t, 'I', '\0');
            assert(t.tm_hour == 7);
        }

        {
            istringstream iss("4");
            ios_base::iostate err = Bit;
            tm t{};
            use_facet<time_get<char>>(iss.getloc()).get(Iter(iss.rdbuf()), Iter(), iss, err, &t, 'm', '\0');
            assert(t.tm_mon == 3);
        }

        {
            istringstream iss("14");
            ios_base::iostate err = Bit;
            tm t{};
            use_facet<time_get<char>>(iss.getloc()).get(Iter(iss.rdbuf()), Iter(), iss, err, &t, 'y', '\0');
            assert(t.tm_year == 114);
        }

        {
            istringstream iss("10 31 2014");
            ios_base::iostate err = Bit;
            tm t{};
            const string fmt("%m %d %Y");
            use_facet<time_get<char>>(iss.getloc())
                .get(Iter(iss.rdbuf()), Iter(), iss, err, &t, fmt.c_str(), fmt.c_str() + fmt.size());
            assert(t.tm_mon == 9);
            assert(t.tm_mday == 31);
            assert(t.tm_year == 114);
        }

        {
            istringstream iss("sep 31 2014");
            ios_base::iostate err = Bit;
            tm t{};
            const string fmt("%b %d %Y");
            use_facet<time_get<char>>(iss.getloc())
                .get(Iter(iss.rdbuf()), Iter(), iss, err, &t, fmt.c_str(), fmt.c_str() + fmt.size());
            assert(t.tm_mon == 8);
            assert(t.tm_mday == 31);
            assert(t.tm_year == 114);
        }

        {
            istringstream iss("aUG 14 2020");
            ios_base::iostate err = Bit;
            tm t{};
            const string fmt("%b %d %Y");
            use_facet<time_get<char>>(iss.getloc())
                .get(Iter(iss.rdbuf()), Iter(), iss, err, &t, fmt.c_str(), fmt.c_str() + fmt.size());
            assert(t.tm_mon == 7);
            assert(t.tm_mday == 14);
            assert(t.tm_year == 120);
        }

        {
            istringstream iss("feBRuArY 02 1991");
            ios_base::iostate err = Bit;
            tm t{};
            const string fmt("%b %d %Y");
            use_facet<time_get<char>>(iss.getloc())
                .get(Iter(iss.rdbuf()), Iter(), iss, err, &t, fmt.c_str(), fmt.c_str() + fmt.size());
            assert(t.tm_mon == 1);
            assert(t.tm_mday == 2);
            assert(t.tm_year == 91);
        }

        {
            istringstream iss("19 SKIP_THIS sEpTemBER SKIP_THIS 2005");
            ios_base::iostate err = Bit;
            tm t{};
            const string fmt("%d SKIP_THIS %b SKIP_THIS %Y");
            use_facet<time_get<char>>(iss.getloc())
                .get(Iter(iss.rdbuf()), Iter(), iss, err, &t, fmt.c_str(), fmt.c_str() + fmt.size());
            assert(t.tm_mon == 8);
            assert(t.tm_mday == 19);
            assert(t.tm_year == 105);
        }

        {
            istringstream iss("2011-D-18");
            ios_base::iostate err = Bit;
            tm t{};
            const string fmt("%Y-%b-%d");
            use_facet<time_get<char>>(iss.getloc())
                .get(Iter(iss.rdbuf()), Iter(), iss, err, &t, fmt.c_str(), fmt.c_str() + fmt.size());
            assert(err == ios_base::failbit);
        }

        {
            std::tm t = {};
            std::istringstream ss("2018-M-18");
            ss >> std::get_time(&t, "%Y-%b-%d");
            assert(ss.fail());
        }
    }
}

void test_locale_russian() {
    assert(read_date_locale(L"2020-Январь-05", "ru_RU.UTF-8") == make_tuple(5, /*NOTE DIFFERENCE:*/ 0, 120));
    assert(read_date_locale(L"2020-янв-05", "ru_RU.UTF-8") == make_tuple(5, /*NOTE DIFFERENCE:*/ 0, 120));
    assert(read_date_locale(L"2020-янВАрЬ-05", "ru_RU.UTF-8") == make_tuple(5, /*NOTE DIFFERENCE:*/ 0, 120));

    assert(read_date_locale(L"2020-Февраль-15", "ru_RU.UTF-8") == make_tuple(15, /*NOTE DIFFERENCE:*/ 1, 120));
    assert(read_date_locale(L"2020-фев-15", "ru_RU.UTF-8") == make_tuple(15, /*NOTE DIFFERENCE:*/ 1, 120));
    assert(read_date_locale(L"2020-феВрАль-15", "ru_RU.UTF-8") == make_tuple(15, /*NOTE DIFFERENCE:*/ 1, 120));

    assert(read_date_locale(L"2020-Март-25", "ru_RU.UTF-8") == make_tuple(25, /*NOTE DIFFERENCE:*/ 2, 120));
    assert(read_date_locale(L"2020-мар-25", "ru_RU.UTF-8") == make_tuple(25, /*NOTE DIFFERENCE:*/ 2, 120));
    assert(read_date_locale(L"2020-МаРт-25", "ru_RU.UTF-8") == make_tuple(25, /*NOTE DIFFERENCE:*/ 2, 120));

    assert(read_date_locale(L"2020-Апрель-05", "ru_RU.UTF-8") == make_tuple(5, /*NOTE DIFFERENCE:*/ 3, 120));
    assert(read_date_locale(L"2020-апр-05", "ru_RU.UTF-8") == make_tuple(5, /*NOTE DIFFERENCE:*/ 3, 120));
    assert(read_date_locale(L"2020-АпРЕль-05", "ru_RU.UTF-8") == make_tuple(5, /*NOTE DIFFERENCE:*/ 3, 120));

    assert(read_date_locale(L"2020-Май-15", "ru_RU.UTF-8") == make_tuple(15, /*NOTE DIFFERENCE:*/ 4, 120));
    assert(read_date_locale(L"2020-мАЙ-15", "ru_RU.UTF-8") == make_tuple(15, /*NOTE DIFFERENCE:*/ 4, 120));

    assert(read_date_locale(L"2020-Июнь-25", "ru_RU.UTF-8") == make_tuple(25, /*NOTE DIFFERENCE:*/ 5, 120));
    assert(read_date_locale(L"2020-июн-25", "ru_RU.UTF-8") == make_tuple(25, /*NOTE DIFFERENCE:*/ 5, 120));
    assert(read_date_locale(L"2020-ИюНЬ-25", "ru_RU.UTF-8") == make_tuple(25, /*NOTE DIFFERENCE:*/ 5, 120));

    assert(read_date_locale(L"2020-Июль-12", "ru_RU.UTF-8") == make_tuple(12, /*NOTE DIFFERENCE:*/ 6, 120));
    assert(read_date_locale(L"2020-июл-12", "ru_RU.UTF-8") == make_tuple(12, /*NOTE DIFFERENCE:*/ 6, 120));
    assert(read_date_locale(L"2020-ИюЛь-12", "ru_RU.UTF-8") == make_tuple(12, /*NOTE DIFFERENCE:*/ 6, 120));

    assert(read_date_locale(L"2020-Август-02", "ru_RU.UTF-8") == make_tuple(2, /*NOTE DIFFERENCE:*/ 7, 120));
    assert(read_date_locale(L"2020-авг-02", "ru_RU.UTF-8") == make_tuple(2, /*NOTE DIFFERENCE:*/ 7, 120));
    assert(read_date_locale(L"2020-АвгУСт-02", "ru_RU.UTF-8") == make_tuple(2, /*NOTE DIFFERENCE:*/ 7, 120));

    assert(read_date_locale(L"2020-Сентябрь-21", "ru_RU.UTF-8") == make_tuple(21, /*NOTE DIFFERENCE:*/ 8, 120));
    assert(read_date_locale(L"2020-сен-21", "ru_RU.UTF-8") == make_tuple(21, /*NOTE DIFFERENCE:*/ 8, 120));
    assert(read_date_locale(L"2020-СентяБрь-21", "ru_RU.UTF-8") == make_tuple(21, /*NOTE DIFFERENCE:*/ 8, 120));

    assert(read_date_locale(L"2020-Октябрь-01", "ru_RU.UTF-8") == make_tuple(1, /*NOTE DIFFERENCE:*/ 9, 120));
    assert(read_date_locale(L"2020-окт-01", "ru_RU.UTF-8") == make_tuple(1, /*NOTE DIFFERENCE:*/ 9, 120));
    assert(read_date_locale(L"2020-ОктяБрь-01", "ru_RU.UTF-8") == make_tuple(1, /*NOTE DIFFERENCE:*/ 9, 120));

    assert(read_date_locale(L"2020-Ноябрь-09", "ru_RU.UTF-8") == make_tuple(9, /*NOTE DIFFERENCE:*/ 10, 120));
    assert(read_date_locale(L"2020-ноя-09", "ru_RU.UTF-8") == make_tuple(9, /*NOTE DIFFERENCE:*/ 10, 120));
    assert(read_date_locale(L"2020-НояБрь-09", "ru_RU.UTF-8") == make_tuple(9, /*NOTE DIFFERENCE:*/ 10, 120));

    assert(read_date_locale(L"2020-Декабрь-31", "ru_RU.UTF-8") == make_tuple(31, /*NOTE DIFFERENCE:*/ 11, 120));
    assert(read_date_locale(L"2020-дек-31", "ru_RU.UTF-8") == make_tuple(31, /*NOTE DIFFERENCE:*/ 11, 120));
    assert(read_date_locale(L"2020-ДекаБрь-31", "ru_RU.UTF-8") == make_tuple(31, /*NOTE DIFFERENCE:*/ 11, 120));
}

void test_locale_german() {
    assert(read_date_locale(L"2020-Januar-05", "de_DE.utf-8") == make_tuple(5, /*NOTE DIFFERENCE:*/ 0, 120));
    assert(read_date_locale(L"2020-Jan-05", "de_DE.utf-8") == make_tuple(5, /*NOTE DIFFERENCE:*/ 0, 120));
    assert(read_date_locale(L"2020-JanUAr-05", "de_DE.utf-8") == make_tuple(5, /*NOTE DIFFERENCE:*/ 0, 120));

    assert(read_date_locale(L"2020-Februar-15", "de_DE.utf-8") == make_tuple(15, /*NOTE DIFFERENCE:*/ 1, 120));
    assert(read_date_locale(L"2020-Feb-15", "de_DE.utf-8") == make_tuple(15, /*NOTE DIFFERENCE:*/ 1, 120));
    assert(read_date_locale(L"2020-FebrUar-15", "de_DE.utf-8") == make_tuple(15, /*NOTE DIFFERENCE:*/ 1, 120));

    assert(read_date_locale(L"2020-März-25", "de_DE.utf-8") == make_tuple(25, /*NOTE DIFFERENCE:*/ 2, 120));
    assert(read_date_locale(L"2020-mäRZ-25", "de_DE.utf-8") == make_tuple(25, /*NOTE DIFFERENCE:*/ 2, 120));

    assert(read_date_locale(L"2020-April-05", "de_DE.utf-8") == make_tuple(5, /*NOTE DIFFERENCE:*/ 3, 120));
    assert(read_date_locale(L"2020-Apr-05", "de_DE.utf-8") == make_tuple(5, /*NOTE DIFFERENCE:*/ 3, 120));
    assert(read_date_locale(L"2020-apRiL-05", "de_DE.utf-8") == make_tuple(5, /*NOTE DIFFERENCE:*/ 3, 120));

    assert(read_date_locale(L"2020-Mai-15", "de_DE.utf-8") == make_tuple(15, /*NOTE DIFFERENCE:*/ 4, 120));
    assert(read_date_locale(L"2020-mAi-15", "de_DE.utf-8") == make_tuple(15, /*NOTE DIFFERENCE:*/ 4, 120));

    assert(read_date_locale(L"2020-Juni-25", "de_DE.utf-8") == make_tuple(25, /*NOTE DIFFERENCE:*/ 5, 120));
    assert(read_date_locale(L"2020-jUNi-25", "de_DE.utf-8") == make_tuple(25, /*NOTE DIFFERENCE:*/ 5, 120));
    assert(read_date_locale(L"2020-jUNi-25", "de_DE.utf-8") == make_tuple(25, /*NOTE DIFFERENCE:*/ 5, 120));

    assert(read_date_locale(L"2020-Juli-12", "de_DE.utf-8") == make_tuple(12, /*NOTE DIFFERENCE:*/ 6, 120));
    assert(read_date_locale(L"2020-JuLi-12", "de_DE.utf-8") == make_tuple(12, /*NOTE DIFFERENCE:*/ 6, 120));
    assert(read_date_locale(L"2020-jUli-12", "de_DE.utf-8") == make_tuple(12, /*NOTE DIFFERENCE:*/ 6, 120));

    assert(read_date_locale(L"2020-August-02", "de_DE.utf-8") == make_tuple(2, /*NOTE DIFFERENCE:*/ 7, 120));
    assert(read_date_locale(L"2020-Aug-02", "de_DE.utf-8") == make_tuple(2, /*NOTE DIFFERENCE:*/ 7, 120));
    assert(read_date_locale(L"2020-auGuSt-02", "de_DE.utf-8") == make_tuple(2, /*NOTE DIFFERENCE:*/ 7, 120));

    assert(read_date_locale(L"2020-September-21", "de_DE.utf-8") == make_tuple(21, /*NOTE DIFFERENCE:*/ 8, 120));
    assert(read_date_locale(L"2020-Sep-21", "de_DE.utf-8") == make_tuple(21, /*NOTE DIFFERENCE:*/ 8, 120));
    assert(read_date_locale(L"2020-sEpTeMber-21", "de_DE.utf-8") == make_tuple(21, /*NOTE DIFFERENCE:*/ 8, 120));

    assert(read_date_locale(L"2020-Oktober-01", "de_DE.utf-8") == make_tuple(1, /*NOTE DIFFERENCE:*/ 9, 120));
    assert(read_date_locale(L"2020-Okt-01", "de_DE.utf-8") == make_tuple(1, /*NOTE DIFFERENCE:*/ 9, 120));
    assert(read_date_locale(L"2020-oKtoBeR-01", "de_DE.utf-8") == make_tuple(1, /*NOTE DIFFERENCE:*/ 9, 120));

    assert(read_date_locale(L"2020-November-09", "de_DE.utf-8") == make_tuple(9, /*NOTE DIFFERENCE:*/ 10, 120));
    assert(read_date_locale(L"2020-Nov-09", "de_DE.utf-8") == make_tuple(9, /*NOTE DIFFERENCE:*/ 10, 120));
    assert(read_date_locale(L"2020-noVemBeR-09", "de_DE.utf-8") == make_tuple(9, /*NOTE DIFFERENCE:*/ 10, 120));

    assert(read_date_locale(L"2020-Dezember-31", "de_DE.utf-8") == make_tuple(31, /*NOTE DIFFERENCE:*/ 11, 120));
    assert(read_date_locale(L"2020-Dez-31", "de_DE.utf-8") == make_tuple(31, /*NOTE DIFFERENCE:*/ 11, 120));
    assert(read_date_locale(L"2020-deZeMbEr-31", "de_DE.utf-8") == make_tuple(31, /*NOTE DIFFERENCE:*/ 11, 120));
}

void test_locale_chines() {
    assert(read_date_locale(L"2020-一月-05", "chinese") == make_tuple(5, /*NOTE DIFFERENCE:*/ 0, 120));
    assert(read_date_locale(L"2020-二月-15", "chinese") == make_tuple(15, /*NOTE DIFFERENCE:*/ 1, 120));
    assert(read_date_locale(L"2020-三月-25", "chinese") == make_tuple(25, /*NOTE DIFFERENCE:*/ 2, 120));
    assert(read_date_locale(L"2020-四月-05", "chinese") == make_tuple(5, /*NOTE DIFFERENCE:*/ 3, 120));
    assert(read_date_locale(L"2020-五月-15", "chinese") == make_tuple(15, /*NOTE DIFFERENCE:*/ 4, 120));
    assert(read_date_locale(L"2020-六月-25", "chinese") == make_tuple(25, /*NOTE DIFFERENCE:*/ 5, 120));
    assert(read_date_locale(L"2020-七月-12", "chinese") == make_tuple(12, /*NOTE DIFFERENCE:*/ 6, 120));
    assert(read_date_locale(L"2020-八月-02", "chinese") == make_tuple(2, /*NOTE DIFFERENCE:*/ 7, 120));
    assert(read_date_locale(L"2020-九月-21", "chinese") == make_tuple(21, /*NOTE DIFFERENCE:*/ 8, 120));
    assert(read_date_locale(L"2020-十月-01", "chinese") == make_tuple(1, /*NOTE DIFFERENCE:*/ 9, 120));
    assert(read_date_locale(L"2020-十一月-09", "chinese") == make_tuple(9, /*NOTE DIFFERENCE:*/ 10, 120));
    assert(read_date_locale(L"2020-十二月-31", "chinese") == make_tuple(31, /*NOTE DIFFERENCE:*/ 11, 120));
}
