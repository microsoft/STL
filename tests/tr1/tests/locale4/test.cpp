// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <locale>, part 4
#define TEST_NAME "<locale>, part 4"

#include "tdefs.h"
#include <locale>
#include <sstream>
#include <string>
#include <time.h>
#include <wchar.h>

#define ADD_FACET(loc, pfac) STD locale(loc, pfac)
#define HAS_FACET(loc, Fac)  STD has_facet<Fac>(loc)
#define USE_FACET(loc, Fac)  STD use_facet<Fac>(loc)

STD locale loc;

void test_collate() { // test collate<wchar_t>
    typedef STD collate<wchar_t> Myfac;
    const Myfac* pf          = &USE_FACET(loc, Myfac);
    STD locale loc_byname    = ADD_FACET(loc, new STD collate_byname<wchar_t>("C"));
    STD locale::id* pid      = &pf->id;
    Myfac::char_type* pc     = (wchar_t*) nullptr;
    Myfac::string_type* pstr = (STD wstring*) nullptr;
    const STD wstring abc(L"abc"), abd(L"abd");
    STD wstring xabc = pf->transform(&abc[0], &abc[0] + 3);
    STD wstring xabd = pf->transform(&abd[0], &abd[0] + 3);

    pid  = pid; // to quiet diagnostics
    pc   = pc;
    pstr = pstr;

    CHECK(pf->compare(&abc[0], &abc[0] + 3, &abd[0], &abd[0] + 3));
    CHECK(!pf->compare(&abc[0], &abc[0] + 3, &abc[0], &abc[0] + 3));
    CHECK(xabc < xabd);

    CHECK(loc(abc, abd));
    CHECK(!loc(abc, abc));
}

void test_messages() { // test messages<wchar_t>
    typedef STD messages<wchar_t> Myfac;
    const Myfac* pf                  = &USE_FACET(loc, Myfac);
    STD locale loc_byname            = ADD_FACET(loc, new STD messages_byname<wchar_t>("C"));
    const STD messages_base* pbase   = pf;
    STD locale::id* pid              = &pf->id;
    Myfac::char_type* pc             = (wchar_t*) nullptr;
    Myfac::string_type* pstr         = (STD wstring*) nullptr;
    STD messages_base::catalog* pcat = (int*) nullptr;

    pbase = pbase; // to quiet diagnostics;
    pid   = pid;
    pc    = pc;
    pstr  = pstr;
    pcat  = pcat;

    STD wstring dflt(L"default");
    STD messages_base::catalog cat = pf->open(STD string("abc"), loc);
    STD wstring msg                = pf->get(cat, 0, 0, dflt);
    CHECK_WSTR(msg.c_str(), dflt.c_str());
    pf->close(cat);
}

struct Myxmpunct : public STD moneypunct<wchar_t, false> { // specify money punctuation
protected:
    virtual wchar_t do_decimal_point() const { // return decimal point
        return L'_';
    }

    virtual wchar_t do_thousands_sep() const { // return thousands separator
        return L';';
    }

    virtual STD string do_grouping() const { // return grouping rule
        return "\2";
    }

    virtual STD wstring do_curr_symbol() const { // return currency symbol
        return L"@@";
    }

    virtual STD wstring do_positive_sign() const { // return positive sign
        return L"+";
    }

    virtual STD wstring do_negative_sign() const { // return negative sign
        return L"-";
    }

    virtual int do_frac_digits() const { // return number of fraction digits
        return 4;
    }

    virtual pattern do_neg_format() const { // return pattern for negative format
        static STD money_base::pattern pat = {
            {STD money_base::sign, STD money_base::symbol, STD money_base::value, STD money_base::none}};
        return pat;
    }

    virtual pattern do_pos_format() const { // return pattern for positive format
        static STD money_base::pattern pat = {
            {STD money_base::sign, STD money_base::none, STD money_base::symbol, STD money_base::value}};
        return pat;
    }
};

struct Myxctype2 : public STD ctype<wchar_t> { // get protected members
    Myxctype2() { // default construct
    }

    virtual wchar_t do_widen(char ch) const { // widen a character
        if (ch == '-') {
            return L'@';
        } else if (ch == '0') {
            return L'A';
        } else if (ch == '1') {
            return L'b';
        } else {
            return (wchar_t) ch;
        }
    }

    virtual const char* do_widen(const char* first, const char* last,
        wchar_t* dest) const { // widen a character sequence
        for (; first != last; ++first, ++dest) {
            *dest = do_widen(*first);
        }
        return first;
    }

    virtual char do_narrow(wchar_t ch, char) const { // narrow a character
        if (ch == L'P') {
            return '%';
        } else if (ch == L'K') {
            return ':';
        } else if (ch == L'Z') {
            return 'H';
        } else {
            return (char) ch;
        }
    }

    virtual const wchar_t* do_narrow(
        const wchar_t* first, const wchar_t* last, char, char* dest) const { // narrow a character sequence
        for (; first != last; ++first, ++dest) {
            *dest = do_narrow(*first, '\0');
        }
        return first;
    }
};

void test_money_get() { // test money_get<wchar_t, istreambuf_iterator<wchar_t> >
    typedef STD istreambuf_iterator<wchar_t> Myiter;
    typedef STD money_get<wchar_t, Myiter> Myfac;
    typedef STD moneypunct<wchar_t, false> Mympunct;
    const Myfac* pf          = &USE_FACET(loc, Myfac);
    STD locale::id* pid      = &pf->id;
    Myfac::char_type* pc     = (wchar_t*) nullptr;
    Myfac::iter_type* pi     = (Myiter*) nullptr;
    Myfac::string_type* pstr = (STD wstring*) nullptr;

    pid  = pid; // to quiet diagnostics
    pc   = pc;
    pi   = pi;
    pstr = pstr;

    STD wistringstream istr;
    STD locale loc_mympunct  = ADD_FACET(loc, (Mympunct*) new Myxmpunct);
    STD ios_base::iostate st = STD ios_base::goodbit;
    Myiter iit0, iit;

    istr.imbue(loc_mympunct);

    long double units = 0.0L;
    st                = STD ios_base::goodbit;
    istr.str(L"@@1_0000 ");
    iit = pf->get(Myiter(istr), iit0, false, istr, st, units);
    CHECK_INT(*iit, L' ');
    CHECK(units == 10000.0L);

    STD wstring digits;
    st = STD ios_base::goodbit;
    istr.str(L"-@@1;23 ");
    iit = pf->get(Myiter(istr), iit0, false, istr, st, digits);
    CHECK_INT(*iit, L' ');
    CHECK_WSTR(digits.c_str(), L"-1230000");

    { // check for widen calls
        STD wistringstream istr2(L"-bAA2 ");
        STD locale loc_myctype = ADD_FACET(loc, new Myxctype2);
        istr2.imbue(loc_myctype);

        units = 0.0L;
        st    = STD ios_base::goodbit;
        iit   = pf->get(Myiter(istr2), iit0, false, istr2, st, units);
        CHECK_INT(*iit, L' ');
        CHECK_DOUBLE(units, -1002.0L);

        digits = L"";
        st     = STD ios_base::goodbit;
        istr2.str(L"-b23A ");
        iit = pf->get(Myiter(istr2), iit0, false, istr2, st, digits);
        CHECK_INT(*iit, L' ');
        CHECK_WSTR(digits.c_str(), L"@b23A");
    }
}

void test_money_put() { // test money_put<wchar_t, ostreambuf_iterator<wchar_t> >
    typedef STD ostreambuf_iterator<wchar_t> Myiter;
    typedef STD money_put<wchar_t, Myiter> Myfac;
    typedef STD moneypunct<wchar_t, false> Mympunct;
    const Myfac* pf          = &USE_FACET(loc, Myfac);
    STD locale::id* pid      = &pf->id;
    Myfac::char_type* pc     = (wchar_t*) nullptr;
    Myfac::iter_type* pi     = (Myiter*) nullptr;
    Myfac::string_type* pstr = (STD wstring*) nullptr;

    pid  = pid; // to quiet diagnostics
    pc   = pc;
    pi   = pi;
    pstr = pstr;

    STD wostringstream ostr;
    STD locale loc_mympunct = ADD_FACET(loc, (Mympunct*) new Myxmpunct);
    Myiter oit(ostr);

    ostr.imbue(loc_mympunct);

    oit = pf->put(oit, false, ostr, L'*', 10000.0L);
    CHECK_WSTR(ostr.str().c_str(), L"+1_0000");

    ostr.str(L"");
    oit = Myiter(ostr);
    ostr.setf(STD ios_base::showbase);
    ostr.width(14);
    oit = pf->put(oit, false, ostr, L'*', STD wstring(L"-1234567"));
    CHECK_WSTR(ostr.str().c_str(), L"**-@@1;23_4567");

    { // check for widen calls
        STD wostringstream ostr2;
        STD locale loc_myctype = ADD_FACET(loc, new Myxctype2);
        ostr2.imbue(loc_myctype);

        ostr2.str(L"");
        oit = Myiter(ostr2);
        ostr2.setf(STD ios_base::showbase);
        oit = pf->put(oit, false, ostr2, L'*', -10002.0L);
        CHECK_WSTR(ostr2.str().c_str(), L"-bAAA2");

        ostr2.str(L"");
        oit = Myiter(ostr2);
        ostr2.setf(STD ios_base::showbase);
        ostr2.width(14);
        oit = pf->put(oit, false, ostr2, L'*', STD wstring(L"@345Ab"));
        CHECK_WSTR(ostr2.str().c_str(), L"********-345Ab");
    }
}

void test_moneypunct() { // test moneypunct<wchar_t>
    typedef STD moneypunct<wchar_t, false> Myfac;
    STD locale loc_mypunct = ADD_FACET(loc, (Myfac*) new Myxmpunct);

    { // test local punctuation
        const Myfac* pf             = &USE_FACET(loc_mypunct, Myfac);
        const STD money_base* pbase = pf;
        STD locale::id* pid         = &pf->id;
        Myfac::char_type* pc        = (wchar_t*) nullptr;
        Myfac::string_type* pstr    = (STD wstring*) nullptr;
        STD money_base::part part   = STD money_base::none;
        STD money_base::pattern npat;

        pid   = pid; // to quiet diagnostics
        pc    = pc;
        pstr  = pstr;
        pbase = pbase;
        part  = part;

        CHECK_INT(pf->decimal_point(), L'_');
        CHECK_INT(pf->thousands_sep(), L';');
        CHECK_STR(pf->grouping().c_str(), "\2");
        CHECK_WSTR(pf->curr_symbol().c_str(), L"@@");
        CHECK_WSTR(pf->positive_sign().c_str(), L"+");
        CHECK_WSTR(pf->negative_sign().c_str(), L"-");
        CHECK_INT(pf->frac_digits(), 4);

        npat = pf->neg_format();
        CHECK_INT(npat.field[0], STD money_base::sign);
        CHECK_INT(npat.field[1], STD money_base::symbol);
        CHECK_INT(npat.field[2], STD money_base::value);
        CHECK_INT(npat.field[3], STD money_base::none);

        npat = pf->pos_format();
        CHECK_INT(npat.field[0], STD money_base::sign);
        CHECK_INT(npat.field[1], STD money_base::none);
        CHECK_INT(npat.field[2], STD money_base::symbol);
        CHECK_INT(npat.field[3], STD money_base::value);
        CHECK(!Myfac::intl);
    }

    { // test international punctuation
        typedef STD moneypunct<wchar_t, true> Myfac2;
        const Myfac2* pf = &USE_FACET(loc_mypunct, Myfac2);
        STD money_base::pattern npat;

        npat = pf->neg_format();
        CHECK_INT(npat.field[0], STD money_base::symbol);
        CHECK_INT(npat.field[1], STD money_base::sign);
        CHECK_INT(npat.field[2], STD money_base::none);
        CHECK_INT(npat.field[3], STD money_base::value);

        npat = pf->pos_format();
        CHECK_INT(npat.field[0], STD money_base::symbol);
        CHECK_INT(npat.field[1], STD money_base::sign);
        CHECK_INT(npat.field[2], STD money_base::none);
        CHECK_INT(npat.field[3], STD money_base::value);
        CHECK(Myfac2::intl);
    }
}

void test_time_get() { // test time_get<wchar_t, istreambuf_iterator<wchar_t> >
    typedef STD istreambuf_iterator<wchar_t> Myiter;
    typedef STD time_get<wchar_t, Myiter> Myfac;
    const Myfac* pf       = &USE_FACET(loc, Myfac);
    STD locale loc_byname = ADD_FACET(loc, (new STD time_get_byname<wchar_t, STD istreambuf_iterator<wchar_t>>("C")));
    const STD time_base* pbase = pf;
    STD locale::id* pid        = &pf->id;
    Myfac::char_type* pc       = (wchar_t*) nullptr;
    Myfac::iter_type* pi       = (Myiter*) nullptr;

    STD time_base::dateorder order_tab[] = {
        STD time_base::no_order, STD time_base::dmy, STD time_base::mdy, STD time_base::ymd, STD time_base::ydm};

    pbase = pbase; // to quiet diagnostics;
    pid   = pid;
    pc    = pc;
    pi    = pi;

    STD time_base::dateorder order = pf->date_order();
    CHECK(order == order_tab[0] || order == order_tab[1] || order == order_tab[2] || order == order_tab[3]
          || order == order_tab[4]);

    STD wistringstream istr;
    STD ios_base::iostate st = STD ios_base::goodbit;
    Myiter iit0, iit(istr);
    CSTD tm tvec{};

    istr.str(L"Monday Tue ");
    st  = STD ios_base::goodbit;
    iit = pf->get_weekday(Myiter(istr), iit0, istr, st, &tvec);
    CHECK_INT(*iit, L' ');
    CHECK_INT(tvec.tm_wday, 1);
    iit = pf->get_weekday(++iit, iit0, istr, st, &tvec);
    CHECK_INT(*iit, L' ');
    CHECK_INT(tvec.tm_wday, 2);

    istr.str(L"October Nov ");
    st  = STD ios_base::goodbit;
    iit = pf->get_monthname(Myiter(istr), iit0, istr, st, &tvec);
    CHECK_INT(*iit, L' ');
    CHECK_INT(tvec.tm_mon, 9);
    iit = pf->get_monthname(++iit, iit0, istr, st, &tvec);
    CHECK_INT(*iit, L' ');
    CHECK_INT(tvec.tm_mon, 10);

    istr.str(L"1978 ");
    st  = STD ios_base::goodbit;
    iit = pf->get_year(Myiter(istr), iit0, istr, st, &tvec);
    CHECK_INT(*iit, L' ');
    CHECK_INT(tvec.tm_year, 78);

    // test time and date against time_put output
    typedef STD ostreambuf_iterator<wchar_t> Myoiter;
    typedef STD time_put<wchar_t, Myoiter> Myofac;
    const Myofac* pof = &USE_FACET(loc, Myofac);
    STD wostringstream ostr;
    CSTD tm tvec0{};
    CSTD tm tvecx{};

    tvecx.tm_sec   = 15;
    tvecx.tm_min   = 55;
    tvecx.tm_hour  = 6;
    tvecx.tm_mday  = 2;
    tvecx.tm_mon   = 11;
    tvecx.tm_year  = 79;
    tvecx.tm_isdst = -1;
    CSTD mktime(&tvecx);

    pof->put(Myoiter(ostr), ostr, L'*', &tvecx, L'X', L'\0');
    ostr << L' ';
    istr.str(ostr.str());
    tvec = tvec0;

    st  = STD ios_base::goodbit;
    iit = pf->get_time(Myiter(istr), iit0, istr, st, &tvec);
    CHECK_INT(*iit, L' ');
    CHECK_INT(tvec.tm_sec, 15);
    CHECK_INT(tvec.tm_min, 55);
    CHECK_INT(tvec.tm_hour, 6);

    ostr.str(L"");
    pof->put(Myoiter(ostr), ostr, L'*', &tvecx, L'x', L'\0');
    ostr << L' ';
    istr.str(ostr.str());
    tvec = tvec0;

    st  = STD ios_base::goodbit;
    iit = pf->get_date(Myiter(istr), iit0, istr, st, &tvec);
    CHECK_INT(*iit, L' ');
    CHECK_INT(tvec.tm_mday, 2);
    CHECK_INT(tvec.tm_mon, 11);
    CHECK_INT(tvec.tm_year, 79);

    tvec = tvec0;
    istr.str(L"Mon ");
    st  = STD ios_base::goodbit;
    iit = pf->get(Myiter(istr), iit0, istr, st, &tvec, 'a');
    CHECK_INT(*iit, L' ');
    CHECK_INT(tvec.tm_wday, 1);

    tvec = tvec0;
    istr.str(L"October ");
    st  = STD ios_base::goodbit;
    iit = pf->get(Myiter(istr), iit0, istr, st, &tvec, 'b');
    CHECK_INT(*iit, L' ');
    CHECK_INT(tvec.tm_mon, 9);

    tvec = tvec0;
    istr.str(L"  Mon ");
    st                 = STD ios_base::goodbit;
    const wchar_t* fmt = L" %a";
    iit                = pf->get(Myiter(istr), iit0, istr, st, &tvec, fmt, fmt + CSTD wcslen(fmt));
    CHECK_INT(*iit, L' ');
    CHECK_INT(tvec.tm_wday, 1);

    tvec = tvec0;
    istr.str(L"  October ");
    st  = STD ios_base::goodbit;
    fmt = L" %b";
    iit = pf->get(Myiter(istr), iit0, istr, st, &tvec, fmt, fmt + CSTD wcslen(fmt));
    CHECK_INT(*iit, L' ');
    CHECK_INT(tvec.tm_mon, 9);

    tvec = tvec0;
    istr.str(L"December 02 06:55:15 1979 ");
    st  = STD ios_base::goodbit;
    fmt = L" %c";
    iit = pf->get(Myiter(istr), iit0, istr, st, &tvec, fmt, fmt + CSTD wcslen(fmt));
    CHECK_INT(*iit, L' ');
    CHECK_INT(tvec.tm_year, 79);

    { // check for narrow/widen calls
        STD wistringstream istr2(L"12K34K56 ");
        STD locale loc_myctype = ADD_FACET(loc, new Myxctype2);
        istr2.imbue(loc_myctype);

        st  = STD ios_base::goodbit;
        iit = pf->get_time(Myiter(istr2), iit0, istr2, st, &tvec);
        CHECK_INT(*iit, L' ');
        CHECK_INT(tvec.tm_sec, 56);
        CHECK_INT(tvec.tm_min, 34);
        CHECK_INT(tvec.tm_hour, 12);
    }
}

void test_time_put() { // test time_put<wchar_t, ostreambuf_iterator<wchar_t> >
    typedef STD ostreambuf_iterator<wchar_t> Myiter;
    typedef STD time_put<wchar_t, Myiter> Myfac;
    const Myfac* pf       = &USE_FACET(loc, Myfac);
    STD locale loc_byname = ADD_FACET(loc, (new STD time_put_byname<wchar_t, STD ostreambuf_iterator<wchar_t>>("C")));
    STD locale::id* pid   = &pf->id;
    Myfac::char_type* pc  = (wchar_t*) nullptr;
    Myfac::iter_type* pi  = (Myiter*) nullptr;

    pid = pid; // to quiet diagnostics;
    pc  = pc;
    pi  = pi;

    STD wostringstream ostr;
    CSTD tm tvec{};

    tvec.tm_sec   = 15;
    tvec.tm_min   = 55;
    tvec.tm_hour  = 6;
    tvec.tm_mday  = 2;
    tvec.tm_mon   = 11;
    tvec.tm_year  = 79;
    tvec.tm_isdst = -1;
    CSTD mktime(&tvec);

    pf->put(Myiter(ostr), ostr, L'*', &tvec, L'a', L'\0');
    CHECK_WSTR(ostr.str().c_str(), L"Sun");

    const wchar_t fmt[] = {L"%A %B %d %H:%M:%S %Y %%AM"};
    ostr.str(L"");
    pf->put(Myiter(ostr), ostr, L'*', &tvec, fmt, fmt + sizeof(fmt) / sizeof(wchar_t) - 1);
    CHECK_WSTR(ostr.str().c_str(), L"Sunday December 02 06:55:15 1979 %AM");

    { // check for narrow/widen calls
        STD wostringstream ostr2;
        STD locale loc_myctype = ADD_FACET(loc, new Myxctype2);
        ostr2.imbue(loc_myctype);

        pf->put(Myiter(ostr2), ostr2, L'*', &tvec, 'H', '\0');
        CHECK_WSTR(ostr2.str().c_str(), L"06");

        const wchar_t fmt2[] = {L"%A %B %d PZ:PM:PS %Y %%AM"};
        ostr2.str(L"");
        pf->put(Myiter(ostr2), ostr2, L'*', &tvec, fmt2, fmt2 + sizeof(fmt2) / sizeof(wchar_t) - 1);
        CHECK_WSTR(ostr2.str().c_str(), L"Sunday December 02 06:55:15 1979 %AM");
    }
}

void test_main() { // test basic workings of locale definitions
    test_collate();
    test_messages();
    test_money_get();
    test_money_put();
    test_moneypunct();
    test_time_get();
    test_time_put();
}
