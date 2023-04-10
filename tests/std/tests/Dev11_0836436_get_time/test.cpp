// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <ios>
#include <locale>
#include <sstream>
#include <streambuf>
#include <string>
#include <tuple>

using namespace std;

// DevDiv-821672 "<locale>: visual studio.net 2013 time libraries buggy (%x %X) - time_get"
// DevDiv-836436 "<iomanip>: get_time()'s AM/PM parsing is broken"
// DevDiv-872926 "<locale>: time_get::get parsing format string gets tm::tm_hour wrong [libcxx]"
// VSO-1259138/GH-2618 "<xloctime>: get_time does not return correct year in tm.tm_year if year is 1"

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
void test_locale_chinese();
void test_invalid_argument();
void test_buffer_resizing();
void test_gh_2618();
void test_gh_2848();

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
    test_locale_chinese();
    test_invalid_argument();
    test_buffer_resizing();
    test_gh_2618();
    test_gh_2848();
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
            istringstream iss("20200609");
            ios_base::iostate err = Bit;
            tm t{};
            const string fmt("%Y%m%d");
            use_facet<time_get<char>>(iss.getloc())
                .get(Iter(iss.rdbuf()), Iter(), iss, err, &t, fmt.c_str(), fmt.c_str() + fmt.size());
            assert(t.tm_mon == 5);
            assert(t.tm_mday == 9);
            assert(t.tm_year == 120);
        }

        {
            istringstream iss("20201213");
            ios_base::iostate err = Bit;
            tm t{};
            const string fmt("%Y%m%d");
            use_facet<time_get<char>>(iss.getloc())
                .get(Iter(iss.rdbuf()), Iter(), iss, err, &t, fmt.c_str(), fmt.c_str() + fmt.size());
            assert(t.tm_mon == 11);
            assert(t.tm_mday == 13);
            assert(t.tm_year == 120);
        }

        {
            istringstream iss("2020112");
            ios_base::iostate err = Bit;
            tm t{};
            const string fmt("%Y%m%d");
            use_facet<time_get<char>>(iss.getloc())
                .get(Iter(iss.rdbuf()), Iter(), iss, err, &t, fmt.c_str(), fmt.c_str() + fmt.size());
            assert(t.tm_mon == 10);
            assert(t.tm_mday == 2);
            assert(t.tm_year == 120);
        }

        {
            istringstream iss("2020061125");
            ios_base::iostate err = Bit;
            tm t{};
            const string fmt("%Y%m%d");
            use_facet<time_get<char>>(iss.getloc())
                .get(Iter(iss.rdbuf()), Iter(), iss, err, &t, fmt.c_str(), fmt.c_str() + fmt.size());
            assert(t.tm_mon == 5);
            assert(t.tm_mday == 11);
            assert(t.tm_year == 120);
        }

        {
            istringstream iss("2020120625119");
            ios_base::iostate err = Bit;
            tm t{};
            const string fmt("%Y12%m25%d");
            use_facet<time_get<char>>(iss.getloc())
                .get(Iter(iss.rdbuf()), Iter(), iss, err, &t, fmt.c_str(), fmt.c_str() + fmt.size());
            assert(t.tm_mon == 5);
            assert(t.tm_mday == 11);
            assert(t.tm_year == 120);
        }

        {
            istringstream iss("2020092Text");
            ios_base::iostate err = Bit;
            tm t{};
            const string fmt("%Y%m%d");
            use_facet<time_get<char>>(iss.getloc())
                .get(Iter(iss.rdbuf()), Iter(), iss, err, &t, fmt.c_str(), fmt.c_str() + fmt.size());
            assert(t.tm_mon == 8);
            assert(t.tm_mday == 2);
            assert(t.tm_year == 120);
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
            istringstream iss("20200609");
            tm t = {};
            const string fmt("%Y%m%d");
            iss >> get_time(&t, fmt.c_str());
            assert(!iss.fail());
            assert(t.tm_mon == 5);
            assert(t.tm_mday == 9);
            assert(t.tm_year == 120);
        }

        {
            istringstream iss("2020112");
            tm t = {};
            const string fmt("%Y%m%d");
            iss >> get_time(&t, fmt.c_str());
            assert(!iss.fail());
            assert(t.tm_mon == 10);
            assert(t.tm_mday == 2);
            assert(t.tm_year == 120);
        }

        {
            istringstream iss("2020061125");
            tm t = {};
            const string fmt("%Y%m%d");
            iss >> get_time(&t, fmt.c_str());
            assert(!iss.fail());
            assert(t.tm_mon == 5);
            assert(t.tm_mday == 11);
            assert(t.tm_year == 120);
        }

        {
            istringstream iss("2020124");
            tm t = {};
            const string fmt("%Y%d%m");
            iss >> get_time(&t, fmt.c_str());
            assert(!iss.fail());
            assert(t.tm_mon == 3);
            assert(t.tm_mday == 12);
            assert(t.tm_year == 120);
        }

        {
            istringstream iss("2020104Text");
            tm t = {};
            const string fmt("%Y%d%m");
            iss >> get_time(&t, fmt.c_str());
            assert(!iss.fail());
            assert(t.tm_mon == 3);
            assert(t.tm_mday == 10);
            assert(t.tm_year == 120);
        }

        {
            istringstream iss("202000000000000923");
            tm t = {};
            const string fmt("%Y%m%d");
            iss >> get_time(&t, fmt.c_str());
            assert(iss.fail());
        }

        {
            istringstream iss("202000000000000923");
            ios_base::iostate err = Bit;
            tm t{};
            const string fmt("%Y%m%d");
            use_facet<time_get<char>>(iss.getloc())
                .get(Iter(iss.rdbuf()), Iter(), iss, err, &t, fmt.c_str(), fmt.c_str() + fmt.size());
            assert(err == ios_base::failbit);
        }

        {
            // This case should fail
            istringstream iss("2011-D-18");
            ios_base::iostate err = Bit;
            tm t{};
            const string fmt("%Y-%b-%d");
            use_facet<time_get<char>>(iss.getloc())
                .get(Iter(iss.rdbuf()), Iter(), iss, err, &t, fmt.c_str(), fmt.c_str() + fmt.size());
            assert(err == ios_base::failbit);
        }

        {
            // This case should fail
            istringstream iss("2018-M-18");
            tm t = {};
            const string fmt("%Y-%b-%d");
            iss >> get_time(&t, fmt.c_str());
            assert(iss.fail());
        }

        {
            // GH-1071 should not fail when format is longer than the stream
            istringstream iss("2020");
            ios_base::iostate err = Bit;
            tm t{};
            const string fmt("%Y%m%d");
            use_facet<time_get<char>>(iss.getloc())
                .get(Iter(iss.rdbuf()), Iter(), iss, err, &t, fmt.c_str(), fmt.c_str() + fmt.size());
            assert(err == ios_base::eofbit);
            assert(t.tm_mon == 0);
            assert(t.tm_mday == 0);
            assert(t.tm_year == 120);
        }

        {
            // GH-1071 should not fail when format is longer than the stream
            istringstream iss("2020-sep");
            tm t = {};
            const string fmt("%Y-%b-%d");
            iss >> get_time(&t, fmt.c_str());
            assert(!iss.fail());
            assert(iss.eof());
            assert(t.tm_mon == 8);
            assert(t.tm_mday == 0);
            assert(t.tm_year == 120);
        }

        {
            // GH-1071 should not fail when format is longer than the stream
            istringstream iss("Current time is 3:8");
            tm t = {};
            const string fmt("Current time is %H:%M:%S");
            iss >> get_time(&t, fmt.c_str());
            assert(!iss.fail());
            assert(iss.eof());
            assert(t.tm_hour == 3);
            assert(t.tm_min == 8);
            assert(t.tm_sec == 0);
        }

        {
            // Should fail if EOF while not parsing specifier (N4878 [locale.time.get.members]/8.3).
            tm t{};
            istringstream iss("4");
            iss >> get_time(&t, "42");
            assert(iss.rdstate() == (ios_base::eofbit | ios_base::failbit));
        }

        {
            // Trailing % should not be treated as a literal (N4878 [locale.time.get.members]/8.4).
            tm t{};
            istringstream iss("%");
            iss >> get_time(&t, "%");
            assert(iss.fail());
        }

        {
            // % with modifier but no specifier is also incomplete.
            tm t{};
            istringstream iss("%E");
            iss >> get_time(&t, "%E");
            assert(iss.fail());
        }

        {
            // Literal match is case-insensitive (N4878 [locale.time.get.members]/8.6).
            tm t{};
            istringstream iss("aBc");
            iss >> get_time(&t, "AbC");
            assert(iss);
        }

        {
            // GH-1606: reads too many leading zeros
            istringstream iss("19700405T000006");
            tm t{};
            iss >> get_time(&t, "%Y%m%dT%H%M%S");
            assert(iss);

            printf("Expected hour 0, min 0, sec 6\n");
            printf("     Got hour %d, min %d, sec %d\n", t.tm_hour, t.tm_min, t.tm_sec);

            assert(t.tm_year == 70);
            assert(t.tm_mon == 3);
            assert(t.tm_mday == 5);
            assert(t.tm_hour == 0);
            assert(t.tm_min == 0);
            assert(t.tm_sec == 6);
        }

        {
            // strptime specification: "leading zeros are permitted but not required"
            tm t{};
            istringstream{" 7 4"} >> get_time(&t, "%m%d");
            assert(t.tm_mon == 6);
            assert(t.tm_mday == 4);
        }
    }
}

void test_locale_russian() {
    // Russian January in different cases (expanded, abbreviated, mixed cases)
    assert(read_date_locale(L"2020-\x042f\x043d\x0432\x0430\x0440\x044c-05", "ru-RU") == make_tuple(5, 0, 120));
    assert(read_date_locale(L"2020-\x044f\x043d\x0432-05", "ru-RU") == make_tuple(5, 0, 120));
    assert(read_date_locale(L"2020-\x044f\x043d\x0412\x0410\x0440\x042c-05", "ru-RU") == make_tuple(5, 0, 120));
    assert(read_date_locale(L"2020-\x042f\x041d\x0412-05", "ru-RU") == make_tuple(5, 0, 120));

    // Russian February in different cases (expanded, abbreviated, mixed cases)
    assert(read_date_locale(L"2020-\x0424\x0435\x0432\x0440\x0430\x043b\x044c-15", "ru-RU") == make_tuple(15, 1, 120));
    assert(read_date_locale(L"2020-\x0444\x0435\x0432-15", "ru-RU") == make_tuple(15, 1, 120));
    assert(read_date_locale(L"2020-\x0444\x0435\x0412\x0440\x0410\x043b\x044c-15", "ru-RU") == make_tuple(15, 1, 120));
    assert(read_date_locale(L"2020-\x0424\x0435\x0412-15", "ru-RU") == make_tuple(15, 1, 120));

    // Russian March in different cases (expanded, abbreviated, mixed cases)
    assert(read_date_locale(L"2020-\x041c\x0430\x0440\x0442-25", "ru-RU") == make_tuple(25, 2, 120));
    assert(read_date_locale(L"2020-\x043c\x0430\x0440-25", "ru-RU") == make_tuple(25, 2, 120));
    assert(read_date_locale(L"2020-\x041c\x0430\x0420\x0442-25", "ru-RU") == make_tuple(25, 2, 120));
    assert(read_date_locale(L"2020-\x041c\x0430\x0420-25", "ru-RU") == make_tuple(25, 2, 120));

    // Russian April in different cases (expanded, abbreviated, mixed cases)
    assert(read_date_locale(L"2020-\x0410\x043f\x0440\x0435\x043b\x044c-05", "ru-RU") == make_tuple(5, 3, 120));
    assert(read_date_locale(L"2020-\x0430\x043f\x0440-05", "ru-RU") == make_tuple(5, 3, 120));
    assert(read_date_locale(L"2020-\x0410\x043f\x0420\x0415\x043b\x044c-05", "ru-RU") == make_tuple(5, 3, 120));
    assert(read_date_locale(L"2020-\x0430\x041f\x0420-05", "ru-RU") == make_tuple(5, 3, 120));

    // Russian May in different cases (expanded, mixed cases)
    // Expanded and abbreviated versions are identical
    assert(read_date_locale(L"2020-\x041c\x0430\x0439-15", "ru-RU") == make_tuple(15, 4, 120));
    assert(read_date_locale(L"2020-\x043c\x0410\x0419-15", "ru-RU") == make_tuple(15, 4, 120));

    // Russian June in different cases (expanded, abbreviated, mixed cases)
    assert(read_date_locale(L"2020-\x0418\x044e\x043d\x044c-25", "ru-RU") == make_tuple(25, 5, 120));
    assert(read_date_locale(L"2020-\x0438\x044e\x043d-25", "ru-RU") == make_tuple(25, 5, 120));
    assert(read_date_locale(L"2020-\x0418\x044e\x041d\x042c-25", "ru-RU") == make_tuple(25, 5, 120));
    assert(read_date_locale(L"2020-\x0438\x042e\x041d-25", "ru-RU") == make_tuple(25, 5, 120));

    // Russian July in different cases (expanded, abbreviated, mixed cases)
    assert(read_date_locale(L"2020-\x0418\x044e\x043b\x044c-12", "ru-RU") == make_tuple(12, 6, 120));
    assert(read_date_locale(L"2020-\x0438\x044e\x043b-12", "ru-RU") == make_tuple(12, 6, 120));
    assert(read_date_locale(L"2020-\x0418\x044e\x041b\x044c-12", "ru-RU") == make_tuple(12, 6, 120));
    assert(read_date_locale(L"2020-\x0418\x044e\x041b-12", "ru-RU") == make_tuple(12, 6, 120));

    // Russian August in different cases (expanded, abbreviated, mixed cases)
    assert(read_date_locale(L"2020-\x0410\x0432\x0433\x0443\x0441\x0442-02", "ru-RU") == make_tuple(2, 7, 120));
    assert(read_date_locale(L"2020-\x0430\x0432\x0433-02", "ru-RU") == make_tuple(2, 7, 120));
    assert(read_date_locale(L"2020-\x0410\x0432\x0433\x0423\x0421\x0442-02", "ru-RU") == make_tuple(2, 7, 120));
    assert(read_date_locale(L"2020-\x0430\x0412\x0413-02", "ru-RU") == make_tuple(2, 7, 120));

    // Russian September in different cases (expanded, abbreviated, mixed cases)
    assert(read_date_locale(L"2020-\x0421\x0435\x043d\x0442\x044f\x0431\x0440\x044c-21", "ru-RU")
           == make_tuple(21, 8, 120));
    assert(read_date_locale(L"2020-\x0441\x0435\x043d-21", "ru-RU") == make_tuple(21, 8, 120));
    assert(read_date_locale(L"2020-\x0421\x0435\x043d\x0442\x044f\x0411\x0440\x044c-21", "ru-RU")
           == make_tuple(21, 8, 120));
    assert(read_date_locale(L"2020-\x0441\x0415\x041d-21", "ru-RU") == make_tuple(21, 8, 120));

    // Russian October in different cases (expanded, abbreviated, mixed cases)
    assert(read_date_locale(L"2020-\x041e\x043a\x0442\x044f\x0431\x0440\x044c-01", "ru-RU") == make_tuple(1, 9, 120));
    assert(read_date_locale(L"2020-\x043e\x043a\x0442-01", "ru-RU") == make_tuple(1, 9, 120));
    assert(read_date_locale(L"2020-\x041e\x043a\x0442\x044f\x0411\x0440\x044c-01", "ru-RU") == make_tuple(1, 9, 120));
    assert(read_date_locale(L"2020-\x043e\x041a\x0442-01", "ru-RU") == make_tuple(1, 9, 120));

    // Russian November in different cases (expanded, abbreviated, mixed cases)
    assert(read_date_locale(L"2020-\x041d\x043e\x044f\x0431\x0440\x044c-09", "ru-RU") == make_tuple(9, 10, 120));
    assert(read_date_locale(L"2020-\x043d\x043e\x044f-09", "ru-RU") == make_tuple(9, 10, 120));
    assert(read_date_locale(L"2020-\x041d\x043e\x044f\x0411\x0440\x044c-09", "ru-RU") == make_tuple(9, 10, 120));
    assert(read_date_locale(L"2020-\x043d\x041e\x042f-09", "ru-RU") == make_tuple(9, 10, 120));

    // Russian December in different cases (expanded, abbreviated, mixed cases)
    assert(read_date_locale(L"2020-\x0414\x0435\x043a\x0430\x0431\x0440\x044c-31", "ru-RU") == make_tuple(31, 11, 120));
    assert(read_date_locale(L"2020-\x0434\x0435\x043a-31", "ru-RU") == make_tuple(31, 11, 120));
    assert(read_date_locale(L"2020-\x0414\x0435\x043a\x0430\x0411\x0440\x044c-31", "ru-RU") == make_tuple(31, 11, 120));
    assert(read_date_locale(L"2020-\x0434\x0415\x043a-31", "ru-RU") == make_tuple(31, 11, 120));
}

void test_locale_german() {
    // German January in different cases (expanded, abbreviated, mixed cases)
    assert(read_date_locale(L"2020-\x004a\x0061\x006e\x0075\x0061\x0072-05", "de-DE") == make_tuple(5, 0, 120));
    assert(read_date_locale(L"2020-\x004a\x0061\x006e-05", "de-DE") == make_tuple(5, 0, 120));
    assert(read_date_locale(L"2020-\x004a\x0061\x006e\x0055\x0041\x0072-05", "de-DE") == make_tuple(5, 0, 120));
    assert(read_date_locale(L"2020-\x006a\x0041\x004e-05", "de-DE") == make_tuple(5, 0, 120));

    // German February in different cases (expanded, abbreviated, mixed cases)
    assert(read_date_locale(L"2020-\x0046\x0065\x0062\x0072\x0075\x0061\x0072-15", "de-DE") == make_tuple(15, 1, 120));
    assert(read_date_locale(L"2020-\x0046\x0065\x0062-15", "de-DE") == make_tuple(15, 1, 120));
    assert(read_date_locale(L"2020-\x0046\x0065\x0062\x0072\x0055\x0061\x0072-15", "de-DE") == make_tuple(15, 1, 120));
    assert(read_date_locale(L"2020-\x0066\x0045\x0062-15", "de-DE") == make_tuple(15, 1, 120));

    // German March in different cases (expanded, abbreviated, mixed cases)
    assert(read_date_locale(L"2020-\x004d\x00e4\x0072\x007a-25", "de-DE") == make_tuple(25, 2, 120));
    assert(read_date_locale(L"2020-\x004d\x0072\x007a-25", "de-DE") == make_tuple(25, 2, 120));
    assert(read_date_locale(L"2020-\x006d\x00e4\x0052\x005a-25", "de-DE") == make_tuple(25, 2, 120));
    assert(read_date_locale(L"2020-\x006d\x0052\x005a-25", "de-DE") == make_tuple(25, 2, 120));

    // German April in different cases (expanded, abbreviated, mixed cases)
    assert(read_date_locale(L"2020-\x0041\x0070\x0072\x0069\x006c-05", "de-DE") == make_tuple(5, 3, 120));
    assert(read_date_locale(L"2020-\x0041\x0070\x0072-05", "de-DE") == make_tuple(5, 3, 120));
    assert(read_date_locale(L"2020-\x0061\x0070\x0052\x0069\x004c-05", "de-DE") == make_tuple(5, 3, 120));
    assert(read_date_locale(L"2020-\x0061\x0050\x0052-05", "de-DE") == make_tuple(5, 3, 120));

    // German May in different cases (expanded, mixed cases)
    // Expanded and abbreviated versions are identical
    assert(read_date_locale(L"2020-\x004d\x0061\x0069-15", "de-DE") == make_tuple(15, 4, 120));
    assert(read_date_locale(L"2020-\x006d\x0041\x0069-15", "de-DE") == make_tuple(15, 4, 120));

    // German June in different cases (expanded, abbreviated, mixed cases)
    assert(read_date_locale(L"2020-\x004a\x0075\x006e\x0069-25", "de-DE") == make_tuple(25, 5, 120));
    assert(read_date_locale(L"2020-\x004a\x0075\x006e-25", "de-DE") == make_tuple(25, 5, 120));
    assert(read_date_locale(L"2020-\x006a\x0055\x004e\x0069-25", "de-DE") == make_tuple(25, 5, 120));
    assert(read_date_locale(L"2020-\x006a\x0055\x004e-25", "de-DE") == make_tuple(25, 5, 120));

    // German July in different cases (expanded, mixed cases)
    // Expanded and abbreviated are identical
    assert(read_date_locale(L"2020-\x004a\x0075\x006c\x0069-12", "de-DE") == make_tuple(12, 6, 120));
    assert(read_date_locale(L"2020-\x004a\x0075\x004c\x0069-12", "de-DE") == make_tuple(12, 6, 120));

    // German August in different cases (expanded, abbreviated, mixed cases)
    assert(read_date_locale(L"2020-\x0041\x0075\x0067\x0075\x0073\x0074-02", "de-DE") == make_tuple(2, 7, 120));
    assert(read_date_locale(L"2020-\x0041\x0075\x0067-02", "de-DE") == make_tuple(2, 7, 120));
    assert(read_date_locale(L"2020-\x0061\x0075\x0047\x0075\x0053\x0074-02", "de-DE") == make_tuple(2, 7, 120));
    assert(read_date_locale(L"2020-\x0061\x0055\x0047-02", "de-DE") == make_tuple(2, 7, 120));

    // German September in different cases (expanded, abbreviated, mixed cases)
    assert(read_date_locale(L"2020-\x0053\x0065\x0070\x0074\x0065\x006d\x0062\x0065\x0072-21", "de-DE")
           == make_tuple(21, 8, 120));
    assert(read_date_locale(L"2020-\x0053\x0065\x0070-21", "de-DE") == make_tuple(21, 8, 120));
    assert(read_date_locale(L"2020-\x0073\x0045\x0070\x0054\x0065\x004d\x0062\x0065\x0072-21", "de-DE")
           == make_tuple(21, 8, 120));
    assert(read_date_locale(L"2020-\x0053\x0065\x0070-21", "de-DE") == make_tuple(21, 8, 120));
    assert(read_date_locale(L"2020-\x0073\x0045\x0050-21", "de-DE") == make_tuple(21, 8, 120));

    // German October in different cases (expanded, abbreviated, mixed cases)
    assert(read_date_locale(L"2020-\x004f\x006b\x0074\x006f\x0062\x0065\x0072-01", "de-DE") == make_tuple(1, 9, 120));
    assert(read_date_locale(L"2020-\x004f\x006b\x0074-01", "de-DE") == make_tuple(1, 9, 120));
    assert(read_date_locale(L"2020-\x006f\x004b\x0074\x006f\x0042\x0065\x0052-01", "de-DE") == make_tuple(1, 9, 120));
    assert(read_date_locale(L"2020-\x006f\x004b\x0074-01", "de-DE") == make_tuple(1, 9, 120));

    // German November in different cases (expanded, abbreviated, mixed cases)
    assert(read_date_locale(L"2020-\x004e\x006f\x0076\x0065\x006d\x0062\x0065\x0072-09", "de-DE")
           == make_tuple(9, 10, 120));
    assert(read_date_locale(L"2020-\x004e\x006f\x0076-09", "de-DE") == make_tuple(9, 10, 120));
    assert(read_date_locale(L"2020-\x006e\x006f\x0056\x0065\x006d\x0042\x0065\x0052-09", "de-DE")
           == make_tuple(9, 10, 120));
    assert(read_date_locale(L"2020-\x006e\x004f\x0056-09", "de-DE") == make_tuple(9, 10, 120));

    // German December in different cases (expanded, abbreviated, mixed cases)
    assert(read_date_locale(L"2020-\x0044\x0065\x007a\x0065\x006d\x0062\x0065\x0072-31", "de-DE")
           == make_tuple(31, 11, 120));
    assert(read_date_locale(L"2020-\x0044\x0065\x007a-31", "de-DE") == make_tuple(31, 11, 120));
    assert(read_date_locale(L"2020-\x0064\x0065\x005a\x0065\x004d\x0062\x0045\x0072-31", "de-DE")
           == make_tuple(31, 11, 120));
    assert(read_date_locale(L"2020-\x0064\x0045\x005a-31", "de-DE") == make_tuple(31, 11, 120));
}

void test_locale_chinese() {
    // Chinese letters don't have distinct upper and lower cases

    // January in Chinese (expanded and abbreviated)
    assert(read_date_locale(L"2020-\x4e00\x6708-05", "zh-CN") == make_tuple(5, 0, 120));
    assert(read_date_locale(L"2020-\x0031\x6708-05", "zh-CN") == make_tuple(5, 0, 120));

    // February in Chinese (expanded and abbreviated)
    assert(read_date_locale(L"2020-\x4e8c\x6708-15", "zh-CN") == make_tuple(15, 1, 120));
    assert(read_date_locale(L"2020-\x0032\x6708-15", "zh-CN") == make_tuple(15, 1, 120));

    // March in Chinese (expanded and abbreviated)
    assert(read_date_locale(L"2020-\x4e09\x6708-25", "zh-CN") == make_tuple(25, 2, 120));
    assert(read_date_locale(L"2020-\x0033\x6708-25", "zh-CN") == make_tuple(25, 2, 120));

    // April in Chinese (expanded and abbreviated)
    assert(read_date_locale(L"2020-\x56db\x6708-05", "zh-CN") == make_tuple(5, 3, 120));
    assert(read_date_locale(L"2020-\x0034\x6708-05", "zh-CN") == make_tuple(5, 3, 120));

    // May in Chinese (expanded and abbreviated)
    assert(read_date_locale(L"2020-\x4e94\x6708-15", "zh-CN") == make_tuple(15, 4, 120));
    assert(read_date_locale(L"2020-\x0035\x6708-15", "zh-CN") == make_tuple(15, 4, 120));

    // June in Chinese (expanded and abbreviated)
    assert(read_date_locale(L"2020-\x516d\x6708-25", "zh-CN") == make_tuple(25, 5, 120));
    assert(read_date_locale(L"2020-\x0036\x6708-25", "zh-CN") == make_tuple(25, 5, 120));

    // July in Chinese (expanded and abbreviated)
    assert(read_date_locale(L"2020-\x4e03\x6708-12", "zh-CN") == make_tuple(12, 6, 120));
    assert(read_date_locale(L"2020-\x0037\x6708-12", "zh-CN") == make_tuple(12, 6, 120));

    // August in Chinese (expanded and abbreviated)
    assert(read_date_locale(L"2020-\x516b\x6708-02", "zh-CN") == make_tuple(2, 7, 120));
    assert(read_date_locale(L"2020-\x0038\x6708-02", "zh-CN") == make_tuple(2, 7, 120));

    // September in Chinese (expanded and abbreviated)
    assert(read_date_locale(L"2020-\x4e5d\x6708-21", "zh-CN") == make_tuple(21, 8, 120));
    assert(read_date_locale(L"2020-\x0039\x6708-21", "zh-CN") == make_tuple(21, 8, 120));

    // October in Chinese (expanded and abbreviated)
    assert(read_date_locale(L"2020-\x5341\x6708-01", "zh-CN") == make_tuple(1, 9, 120));
    assert(read_date_locale(L"2020-\x0031\x0030\x6708-01", "zh-CN") == make_tuple(1, 9, 120));

    // November in Chinese (expanded and abbreviated)
    assert(read_date_locale(L"2020-\x5341\x4e00\x6708-09", "zh-CN") == make_tuple(9, 10, 120));
    assert(read_date_locale(L"2020-\x0031\x0031\x6708-09", "zh-CN") == make_tuple(9, 10, 120));

    // December in Chinese (expanded and abbreviated)
    assert(read_date_locale(L"2020-\x5341\x4e8c\x6708-31", "zh-CN") == make_tuple(31, 11, 120));
    assert(read_date_locale(L"2020-\x0031\x0032\x6708-31", "zh-CN") == make_tuple(31, 11, 120));
}

void test_invalid_parameter_handler(const wchar_t* const expression, const wchar_t* const function,
    const wchar_t* const file, const unsigned int line, const uintptr_t reserved) {
    (void) expression;
    (void) reserved;

    static int num_called = 0;
    if (++num_called > 10) {
        wprintf(
            L"Test Failed: Invalid parameter handler was called over 10 times by %s in %s:%u\n", function, file, line);
        exit(1);
    }
}

void test_invalid_argument() {
#ifndef _M_CEE_PURE
    _set_invalid_parameter_handler(test_invalid_parameter_handler);

    time_t t = time(nullptr);
    tm currentTime;
    localtime_s(&currentTime, &t);

    {
        wstringstream wss;
        wss << put_time(&currentTime, L"%Y-%m-%d-%H-%M-%s");
        assert(wss.rdstate() == ios_base::badbit);
    }

    {
        stringstream ss;
        ss << put_time(&currentTime, "%Y-%m-%d-%H-%M-%s");
        assert(ss.rdstate() == ios_base::badbit);
    }
#endif // _M_CEE_PURE
}

void test_buffer_resizing() {
    time_t t = time(nullptr);
    tm currentTime;
    localtime_s(&currentTime, &t);

    {
        wstringstream wss;
        wss.imbue(locale("ja-JP"));
        wss << put_time(&currentTime, L"%c");
        assert(wss.rdstate() == ios_base::goodbit);
    }

    {
        stringstream ss;
        ss.imbue(locale("ja-JP"));
        ss << put_time(&currentTime, "%c");
        assert(ss.rdstate() == ios_base::goodbit);
    }
}

void test_gh_2618() {
    // GH-2618 <xloctime>: get_time does not return correct year in tm.tm_year if year is 1
    auto TestTimeGetYear = [](const char* input, const int expected_y, const int expected_Y,
                               const int expected_get_year) {
        {
            tm time{};
            istringstream iss{input};
            iss >> get_time(&time, "%y");
            assert(time.tm_year + 1900 == expected_y);
        }

        {
            tm time{};
            istringstream iss{input};
            iss >> get_time(&time, "%Y");
            assert(time.tm_year + 1900 == expected_Y);
        }

        {
            tm time{};
            ios_base::iostate state{};
            istringstream iss{input};
            use_facet<time_get<char>>(iss.getloc()).get_year({iss}, {}, iss, state, &time);
            assert(time.tm_year + 1900 == expected_get_year);
        }
    };

    // 4-digit strings: 'y' should only read the first two digits, 'Y' and `get_year` should agree
    TestTimeGetYear("0001", 2000, 1, 1);
    TestTimeGetYear("0080", 2000, 80, 80);
    TestTimeGetYear("1995", 2019, 1995, 1995);
    TestTimeGetYear("2022", 2020, 2022, 2022);
    TestTimeGetYear("8522", 1985, 8522, 8522);

    // 3-digit strings: same as 4-digit
    TestTimeGetYear("001", 2000, 1, 1);
    TestTimeGetYear("080", 2008, 80, 80);
    TestTimeGetYear("995", 1999, 995, 995);

    // 2-digit strings: 'Y' should parse literally, `get_year` should behave as 'y'
    TestTimeGetYear("01", 2001, 1, 2001);
    TestTimeGetYear("80", 1980, 80, 1980);
    TestTimeGetYear("95", 1995, 95, 1995);
    TestTimeGetYear("22", 2022, 22, 2022);

    // 1-digit strings: same as 2-digit
    TestTimeGetYear("1", 2001, 1, 2001);
}

void test_gh_2848() {
    // GH-2848 <xloctime>: time_get::get can still assert 'istreambuf_iterator is not dereferenceable' when
    // the format is longer than the stream
    {
        const locale loc{locale::classic()};
        const auto& tmget{use_facet<time_get<char>>(loc)};
        ios_base::iostate err{ios_base::goodbit};
        tm when{};
        const string fmt{"%X"};
        istringstream iss{"3:04"};
        istreambuf_iterator<char> first{iss};
        const istreambuf_iterator<char> last{};
        tmget.get(first, last, iss, err, &when, fmt.data(), fmt.data() + fmt.size());
        assert(err == (ios_base::eofbit | ios_base::failbit));
    }

    {
        const locale loc{locale::classic()};
        const auto& tmget{use_facet<time_get<wchar_t>>(loc)};
        ios_base::iostate err{ios_base::goodbit};
        tm when{};
        const wstring fmt{L"%X"};
        wistringstream iss{L"3:04"};
        istreambuf_iterator<wchar_t> first{iss};
        const istreambuf_iterator<wchar_t> last{};
        tmget.get(first, last, iss, err, &when, fmt.data(), fmt.data() + fmt.size());
        assert(err == (ios_base::eofbit | ios_base::failbit));
    }
}
