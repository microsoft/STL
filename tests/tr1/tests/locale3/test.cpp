// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <locale>, part 3
#define TEST_NAME "<locale>, part 3"

#include "tdefs.h"
#include <iomanip> // for get/put_money
#include <iterator>
#include <locale>
#include <sstream>
#include <string>
#include <time.h>

#define ADD_FACET(loc, pfac) STD locale(loc, pfac)
#define HAS_FACET(loc, Fac)  STD has_facet<Fac>(loc)
#define USE_FACET(loc, Fac)  STD use_facet<Fac>(loc)

STD locale loc;

void test_collate() { // test collate<char>
    typedef STD collate<char> Myfac;
    const Myfac* pf       = &USE_FACET(loc, Myfac);
    STD locale loc_byname = ADD_FACET(loc, new STD collate_byname<char>("C"));

    STD locale loc_byname_str = ADD_FACET(loc, (new STD collate_byname<char>(STD string("C"))));

    STD locale::id* pid      = &pf->id;
    Myfac::char_type* pc     = (char*) nullptr;
    Myfac::string_type* pstr = (STD string*) nullptr;
    const STD string abc("abc"), abd("abd");
    STD string xabc = pf->transform(&abc[0], &abc[0] + 3);
    STD string xabd = pf->transform(&abd[0], &abd[0] + 3);

    pid  = pid; // to quiet diagnostics
    pc   = pc;
    pstr = pstr;

    CHECK(pf->compare(&abc[0], &abc[0] + 3, &abd[0], &abd[0] + 3));
    CHECK(!pf->compare(&abc[0], &abc[0] + 3, &abc[0], &abc[0] + 3));
    CHECK(xabc < xabd);

    CHECK(loc(abc, abd));
    CHECK(!loc(abc, abc));
}

void test_messages() { // test messages<char>
    typedef STD messages<char> Myfac;
    const Myfac* pf       = &USE_FACET(loc, Myfac);
    STD locale loc_byname = ADD_FACET(loc, new STD messages_byname<char>("C"));

    STD locale loc_byname_str = ADD_FACET(loc, (new STD messages_byname<char>(STD string("C"))));

    const STD messages_base* pbase   = pf;
    STD locale::id* pid              = &pf->id;
    Myfac::char_type* pc             = (char*) nullptr;
    Myfac::string_type* pstr         = (STD string*) nullptr;
    STD messages_base::catalog* pcat = (int*) nullptr;

    pbase = pbase; // to quiet diagnostics;
    pid   = pid;
    pc    = pc;
    pstr  = pstr;
    pcat  = pcat;

    STD string dflt("default");
    STD messages_base::catalog cat = pf->open(STD string("abc"), loc);
    STD string msg                 = pf->get(cat, 0, 0, dflt);
    CHECK_STR(msg.c_str(), dflt.c_str());
    pf->close(cat);
}

struct Myxmpunct : public STD moneypunct<char, false> { // specify money punctuation
protected:
    virtual char do_decimal_point() const { // return decimal point
        return '_';
    }

    virtual char do_thousands_sep() const { // return thousands separator
        return ';';
    }

    virtual STD string do_grouping() const { // return grouping rule
        return "\2";
    }

    virtual STD string do_curr_symbol() const { // return currency symbol
        return "@@";
    }

    virtual STD string do_positive_sign() const { // return positive sign
        return "+";
    }

    virtual STD string do_negative_sign() const { // return negative sign
        return "-";
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

struct Myxctype2 : public STD ctype<char> { // get protected members
    Myxctype2() { // default construct
    }

    virtual char do_widen(char ch) const { // widen a character
        if (ch == '-') {
            return '@';
        } else if (ch == '0') {
            return 'A';
        } else if (ch == '1') {
            return 'b';
        } else {
            return ch;
        }
    }

    virtual const char* do_widen(const char* first, const char* last,
        char* dest) const { // widen a character sequence
        for (; first != last; ++first, ++dest) {
            *dest = do_widen(*first);
        }
        return first;
    }

    virtual char do_narrow(char ch, char) const { // narrow a character
        if (ch == 'P') {
            return '%';
        } else if (ch == 'K') {
            return ':';
        } else if (ch == 'Z') {
            return 'H';
        } else {
            return ch;
        }
    }

    virtual const char* do_narrow(
        const char* first, const char* last, char, char* dest) const { // narrow a character sequence
        for (; first != last; ++first, ++dest) {
            *dest = do_narrow(*first, '\0');
        }
        return first;
    }
};

void test_money_get() { // test money_get<char, istreambuf_iterator<char> >
    typedef STD istreambuf_iterator<char> Myiter;
    typedef STD money_get<char, Myiter> Myfac;
    const Myfac* pf          = &USE_FACET(loc, Myfac);
    STD locale::id* pid      = &pf->id;
    Myfac::char_type* pc     = (char*) nullptr;
    Myfac::iter_type* pi     = (Myiter*) nullptr;
    Myfac::string_type* pstr = (STD string*) nullptr;

    pid  = pid; // to quiet diagnostics
    pc   = pc;
    pi   = pi;
    pstr = pstr;

    STD istringstream istr;
    STD locale loc_mypunct   = ADD_FACET(loc, new Myxmpunct);
    STD ios_base::iostate st = STD ios_base::goodbit;
    Myiter iit0, iit;

    istr.imbue(loc_mypunct);

    long double units = 0.0L;
    st                = STD ios_base::goodbit;
    istr.str("@@1_0000 ");
    iit = pf->get(Myiter(istr), iit0, false, istr, st, units);
    CHECK_INT(*iit, ' ');
    CHECK(units == 10000.0L);

    STD string digits;
    st = STD ios_base::goodbit;
    istr.str("-@@1;23 ");
    iit = pf->get(Myiter(istr), iit0, false, istr, st, digits);
    CHECK_INT(*iit, ' ');
    CHECK_STR(digits.c_str(), "-1230000");

    units = 0.0L;
    istr.str("@@1_0000 ");
    istr >> STD get_money(units);
    CHECK(units == 10000.0L);

    digits = "";
    istr.str("-@@1;23 ");
    istr >> STD get_money(digits);
    CHECK_STR(digits.c_str(), "-1230000");

    { // check for widen calls
        STD istringstream istr2("-bAA2 ");
        STD locale loc_myctype = ADD_FACET(loc, new Myxctype2);
        istr2.imbue(loc_myctype);

        units = 0.0L;
        st    = STD ios_base::goodbit;
        iit   = pf->get(Myiter(istr2), iit0, false, istr2, st, units);
        CHECK_INT(*iit, ' ');
        CHECK_DOUBLE(units, -1002.0L);

        digits = "";
        st     = STD ios_base::goodbit;
        istr2.str("-b23A ");
        iit = pf->get(Myiter(istr2), iit0, false, istr2, st, digits);
        CHECK_INT(*iit, ' ');
        CHECK_STR(digits.c_str(), "@b23A");
    }
}

void test_money_put() { // test money_put<char, ostreambuf_iterator<char> >
    typedef STD ostreambuf_iterator<char> Myiter;
    typedef STD money_put<char, Myiter> Myfac;
    const Myfac* pf          = &USE_FACET(loc, Myfac);
    STD locale::id* pid      = &pf->id;
    Myfac::char_type* pc     = (char*) nullptr;
    Myfac::iter_type* pi     = (Myiter*) nullptr;
    Myfac::string_type* pstr = (STD string*) nullptr;

    pid  = pid; // to quiet diagnostics
    pc   = pc;
    pi   = pi;
    pstr = pstr;

    STD ostringstream ostr;
    STD locale loc_mypunct = ADD_FACET(loc, new Myxmpunct);
    Myiter oit(ostr);

    ostr.imbue(loc_mypunct);

    oit = pf->put(oit, false, ostr, '*', 10000.0L);
    CHECK_STR(ostr.str().c_str(), "+1_0000");

    ostr.str("");
    oit = Myiter(ostr);
    ostr.setf(STD ios_base::showbase);
    ostr.width(14);
    oit = pf->put(oit, false, ostr, '*', STD string("-1234567"));
    CHECK_STR(ostr.str().c_str(), "**-@@1;23_4567");

    ostr.str("");
    ostr << STD put_money(10000.0L);
    CHECK_STR(ostr.str().c_str(), "+@@1_0000");

    ostr.str("");
    ostr << STD put_money(STD string("-1234567"));
    CHECK_STR(ostr.str().c_str(), "-@@1;23_4567");

    { // check for widen calls
        STD ostringstream ostr2;
        STD locale loc_myctype = ADD_FACET(loc, new Myxctype2);
        ostr2.imbue(loc_myctype);

        ostr2.str("");
        oit = Myiter(ostr2);
        ostr2.setf(STD ios_base::showbase);
        oit = pf->put(oit, false, ostr2, '*', -10002.0L);
        CHECK_STR(ostr2.str().c_str(), "-bAAA2");

        ostr2.str("");
        oit = Myiter(ostr2);
        ostr2.setf(STD ios_base::showbase);
        ostr2.width(14);
        oit = pf->put(oit, false, ostr2, '*', STD string("@345Ab"));
        CHECK_STR(ostr2.str().c_str(), "********-345Ab");
    }
}

void test_moneypunct() { // test moneypunct<char, false>
    typedef STD moneypunct<char, false> Myfac;
    STD locale loc_mypunct = ADD_FACET(loc, new Myxmpunct);
    const Myfac* pf        = &USE_FACET(loc_mypunct, Myfac);
    STD locale loc_byname  = ADD_FACET(loc, (new STD moneypunct_byname<char, false>("C")));

    STD locale loc_byname_str = ADD_FACET(loc, (new STD moneypunct_byname<char, false>(STD string("C"))));

    const STD money_base* pbase = pf;
    STD locale::id* pid         = &pf->id;
    Myfac::char_type* pc        = (char*) nullptr;
    Myfac::string_type* pstr    = (STD string*) nullptr;
    STD money_base::part part   = STD money_base::space;
    STD money_base::pattern npat;

    pid   = pid; // to quiet diagnostics
    pc    = pc;
    pstr  = pstr;
    pbase = pbase;
    part  = part;

    CHECK_INT(pf->decimal_point(), '_');
    CHECK_INT(pf->thousands_sep(), ';');
    CHECK_STR(pf->grouping().c_str(), "\2");
    CHECK_STR(pf->curr_symbol().c_str(), "@@");
    CHECK_STR(pf->positive_sign().c_str(), "+");
    CHECK_STR(pf->negative_sign().c_str(), "-");
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

    { // test international punctuation
        typedef STD moneypunct<char, true> Myfac2;
        const Myfac2* pf2 = &USE_FACET(loc_mypunct, Myfac2);
        STD money_base::pattern npat2;

        npat2 = pf2->neg_format();
        CHECK_INT(npat2.field[0], STD money_base::symbol);
        CHECK_INT(npat2.field[1], STD money_base::sign);
        CHECK_INT(npat2.field[2], STD money_base::none);
        CHECK_INT(npat2.field[3], STD money_base::value);

        npat2 = pf2->pos_format();
        CHECK_INT(npat2.field[0], STD money_base::symbol);
        CHECK_INT(npat2.field[1], STD money_base::sign);
        CHECK_INT(npat2.field[2], STD money_base::none);
        CHECK_INT(npat2.field[3], STD money_base::value);
        CHECK(Myfac2::intl);
    }
}

void test_time_get() { // test time_get<char, istreambuf_iterator<char> >
    typedef STD istreambuf_iterator<char> Myiter;
    typedef STD time_get<char, Myiter> Myfac;
    const Myfac* pf       = &USE_FACET(loc, Myfac);
    STD locale loc_byname = ADD_FACET(loc, (new STD time_get_byname<char, STD istreambuf_iterator<char>>("C")));

    STD locale loc_byname_str =
        ADD_FACET(loc, (new STD time_get_byname<char, STD istreambuf_iterator<char>>(STD string("C"))));

    const STD time_base* pbase = pf;
    STD locale::id* pid        = &pf->id;
    Myfac::char_type* pc       = (char*) nullptr;
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

    STD istringstream istr;
    STD ios_base::iostate st = STD ios_base::goodbit;
    Myiter iit0, iit(istr);
    CSTD tm tvec{};

    istr.str("Monday Tue ");
    st  = STD ios_base::goodbit;
    iit = pf->get_weekday(Myiter(istr), iit0, istr, st, &tvec);
    CHECK_INT(*iit, ' ');
    CHECK_INT(tvec.tm_wday, 1);
    iit = pf->get_weekday(++iit, iit0, istr, st, &tvec);
    CHECK_INT(*iit, ' ');
    CHECK_INT(tvec.tm_wday, 2);

    istr.str("October Nov ");
    st  = STD ios_base::goodbit;
    iit = pf->get_monthname(Myiter(istr), iit0, istr, st, &tvec);
    CHECK_INT(*iit, ' ');
    CHECK_INT(tvec.tm_mon, 9);
    iit = pf->get_monthname(++iit, iit0, istr, st, &tvec);
    CHECK_INT(*iit, ' ');
    CHECK_INT(tvec.tm_mon, 10);

    istr.str("1978 ");
    st  = STD ios_base::goodbit;
    iit = pf->get_year(Myiter(istr), iit0, istr, st, &tvec);
    CHECK_INT(*iit, ' ');
    CHECK_INT(tvec.tm_year, 78);

    // test time and date against time_put output
    typedef STD ostreambuf_iterator<char> Myoiter;
    typedef STD time_put<char, Myoiter> Myofac;
    const Myofac* pof = &USE_FACET(loc, Myofac);
    STD ostringstream ostr;
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

    pof->put(Myoiter(ostr), ostr, '*', &tvecx, 'X', '\0');
    ostr << ' ';
    istr.str(ostr.str());
    tvec = tvec0;

    st  = STD ios_base::goodbit;
    iit = pf->get_time(Myiter(istr), iit0, istr, st, &tvec);
    CHECK_INT(*iit, ' ');
    CHECK_INT(tvec.tm_sec, 15);
    CHECK_INT(tvec.tm_min, 55);
    CHECK_INT(tvec.tm_hour, 6);

    ostr.str("");
    pof->put(Myoiter(ostr), ostr, '*', &tvecx, 'x', '\0');
    ostr << ' ';
    istr.str(ostr.str());
    tvec = tvec0;

    st  = STD ios_base::goodbit;
    iit = pf->get_date(Myiter(istr), iit0, istr, st, &tvec);
    CHECK_INT(*iit, ' ');
    CHECK_INT(tvec.tm_mday, 2);
    CHECK_INT(tvec.tm_mon, 11);
    CHECK_INT(tvec.tm_year, 79);

    tvec = tvec0;
    istr.str("Mon ");
    st  = STD ios_base::goodbit;
    iit = pf->get(Myiter(istr), iit0, istr, st, &tvec, 'a');
    CHECK_INT(*iit, ' ');
    CHECK_INT(tvec.tm_wday, 1);

    tvec = tvec0;
    istr.str("October ");
    st  = STD ios_base::goodbit;
    iit = pf->get(Myiter(istr), iit0, istr, st, &tvec, 'b');
    CHECK_INT(*iit, ' ');
    CHECK_INT(tvec.tm_mon, 9);

    tvec = tvec0;
    istr.str("  Mon ");
    st              = STD ios_base::goodbit;
    const char* fmt = " %a";
    iit             = pf->get(Myiter(istr), iit0, istr, st, &tvec, fmt, fmt + CSTD strlen(fmt));
    CHECK_INT(*iit, ' ');
    CHECK_INT(tvec.tm_wday, 1);

    tvec = tvec0;
    istr.str("  October ");
    st  = STD ios_base::goodbit;
    fmt = " %b";
    iit = pf->get(Myiter(istr), iit0, istr, st, &tvec, fmt, fmt + CSTD strlen(fmt));
    CHECK_INT(*iit, ' ');
    CHECK_INT(tvec.tm_mon, 9);

    tvec = tvec0;
    istr.str("December 02 06:55:15 1979 ");
    st  = STD ios_base::goodbit;
    fmt = " %c";
    iit = pf->get(Myiter(istr), iit0, istr, st, &tvec, fmt, fmt + CSTD strlen(fmt));
    CHECK_INT(*iit, ' ');
    CHECK_INT(tvec.tm_year, 79);

    { // check for narrow/widen calls
        STD istringstream istr2("12K34K56 ");
        STD locale loc_myctype = ADD_FACET(loc, new Myxctype2);
        istr2.imbue(loc_myctype);

        st  = STD ios_base::goodbit;
        iit = pf->get_time(Myiter(istr2), iit0, istr2, st, &tvec);
        CHECK_INT(*iit, ' ');
        CHECK_INT(tvec.tm_sec, 56);
        CHECK_INT(tvec.tm_min, 34);
        CHECK_INT(tvec.tm_hour, 12);
    }
}

void test_time_put() { // test time_put<char, ostreambuf_iterator<char> >
    typedef STD ostreambuf_iterator<char> Myiter;
    typedef STD time_put<char, Myiter> Myfac;
    const Myfac* pf       = &USE_FACET(loc, Myfac);
    STD locale loc_byname = ADD_FACET(loc, (new STD time_put_byname<char, STD ostreambuf_iterator<char>>("C")));

    STD locale loc_byname_str =
        ADD_FACET(loc, (new STD time_put_byname<char, STD ostreambuf_iterator<char>>(STD string("C"))));

    STD locale::id* pid  = &pf->id;
    Myfac::char_type* pc = (char*) nullptr;
    Myfac::iter_type* pi = (Myiter*) nullptr;

    pid = pid; // to quiet diagnostics;
    pc  = pc;
    pi  = pi;

    STD ostringstream ostr;
    CSTD tm tvec{};

    tvec.tm_sec   = 15;
    tvec.tm_min   = 55;
    tvec.tm_hour  = 6;
    tvec.tm_mday  = 2;
    tvec.tm_mon   = 11;
    tvec.tm_year  = 79;
    tvec.tm_isdst = -1;
    CSTD mktime(&tvec);

    pf->put(Myiter(ostr), ostr, '*', &tvec, 'a', '\0');
    CHECK_STR(ostr.str().c_str(), "Sun");

    const char fmt[] = {"%A %B %d %H:%M:%S %Y %%AM"};
    ostr.str("");
    pf->put(Myiter(ostr), ostr, '*', &tvec, fmt, fmt + sizeof(fmt) - 1);
    CHECK_STR(ostr.str().c_str(), "Sunday December 02 06:55:15 1979 %AM");

    { // check for narrow/widen calls
        STD ostringstream ostr2;
        STD locale loc_myctype = ADD_FACET(loc, new Myxctype2);
        ostr2.imbue(loc_myctype);

        pf->put(Myiter(ostr2), ostr2, '*', &tvec, 'H', '\0');
        CHECK_STR(ostr2.str().c_str(), "06");

        const char fmt2[] = {"%A %B %d PZ:PM:PS %Y %%AM"};
        ostr2.str("");
        pf->put(Myiter(ostr2), ostr2, '*', &tvec, fmt2, fmt2 + sizeof(fmt2) / sizeof(char) - 1);
        CHECK_STR(ostr2.str().c_str(), "Sunday December 02 06:55:15 1979 %AM");
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
