// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <locale>, part 2
#define TEST_NAME "<locale>, part 2"

#include "tdefs.h"
#include <locale>
#include <sstream>
#include <string>
#include <wchar.h>

#define ADD_FACET(loc, pfac) STD locale(loc, pfac)
#define HAS_FACET(loc, Fac)  STD has_facet<Fac>(loc)
#define USE_FACET(loc, Fac)  STD use_facet<Fac>(loc)

STD locale loc;

void test_codecvt() { // test codecvt<wchar_t, char, mbstate_t>
    typedef STD codecvt<wchar_t, char, CSTD mbstate_t> Myfac;
    const Myfac* pf               = &USE_FACET(loc, Myfac);
    STD locale loc_byname         = ADD_FACET(loc, (new STD codecvt_byname<wchar_t, char, CSTD mbstate_t>("C")));
    const STD codecvt_base* pbase = pf;
    STD locale::id* pid           = &pf->id;
    Myfac::intern_type* pin       = (wchar_t*) nullptr;
    Myfac::extern_type* pex       = (char*) nullptr;
    Myfac::state_type* pst        = (CSTD mbstate_t*) nullptr;

    pbase = pbase; // to quiet diagnostics;
    pid   = pid;
    pin   = pin;
    pex   = pex;
    pst   = pst;

    const wchar_t ibuf[] = {L'a', L'b', L'c'};
    const wchar_t* inext;
    char obuf[32];
    char* onext;
    static CSTD mbstate_t st0; // initial state
    CSTD mbstate_t st;
    STD codecvt_base::result ans;

    CHECK(-1 <= pf->encoding());
    CHECK(1 <= pf->max_length());

    st    = st0;
    inext = nullptr;
    onext = nullptr;
    ans   = pf->out(st, ibuf, ibuf + 3, inext, obuf, obuf + sizeof(obuf), onext);
    if (ans == STD codecvt_base::noconv) { // no conversion, acceptable
        CHECK_INT(ans, STD codecvt_base::noconv);
        CHECK(pf->always_noconv());
        CHECK_PTR(inext, ibuf);
        CHECK_PTR(onext, obuf);

        const char bibuf[] = "abc";
        st                 = st0;
        CHECK_INT(pf->length(st, bibuf, bibuf + 3, 10), 3);
    } else { // converted, should succeed
        CHECK_INT(ans, STD codecvt_base::ok);
        CHECK(!pf->always_noconv());
        CHECK_PTR(inext, ibuf + 3);

        st                 = st0;
        const char* binext = nullptr;
        wchar_t* wonext    = nullptr;
        wchar_t wobuf[10];
        ans = pf->in(st, obuf, onext, binext, wobuf, wobuf + 10, wonext);
        CHECK_PTR(wonext, wobuf + 3);
        CHECK_WMEM(wobuf, L"abc", 3);

        st = st0;
        CHECK_INT(pf->length(st, obuf, onext, onext - obuf), onext - obuf);
    }

    onext = nullptr;
    ans   = pf->unshift(st, obuf, obuf + 10, onext);
    CHECK(ans == STD codecvt_base::noconv || ans == STD codecvt_base::ok);
    CHECK_PTR(onext, obuf);
}

void test_ctype() { // test ctype<wchar_t>
    typedef STD ctype<wchar_t> Myfac;
    const Myfac* pf             = &USE_FACET(loc, Myfac);
    STD locale loc_byname       = ADD_FACET(loc, new STD ctype_byname<wchar_t>("C"));
    const STD ctype_base* pbase = pf;
    Myfac::char_type* pc        = (wchar_t*) nullptr;
    STD locale::id* pid         = &pf->id;
    STD ctype_base::mask mbuf[10];
    wchar_t r2d2[] = {L'R', L'2', L'd', L'2', L'\0'};
    char r2d2_b[]  = "R2d2";
    const char* pend_b;
    const wchar_t* pend;

    pid   = pid; // to quiet diagnostics
    pc    = pc;
    pbase = pbase;

    CHECK(pf->is(STD ctype_base::alnum, L'a'));
    pend = pf->is(r2d2, r2d2 + 4, mbuf);
    CHECK_PTR(pend, r2d2 + 4);
    CHECK_INT(mbuf[1] & STD ctype_base::digit, STD ctype_base::digit);
    pend = pf->scan_is(STD ctype_base::lower, r2d2, r2d2 + 4);
    CHECK_PTR(pend, r2d2 + 2);
    pend = pf->scan_not(STD ctype_base::alpha, r2d2, r2d2 + 4);
    CHECK_PTR(pend, r2d2 + 1);

    CHECK_INT(pf->toupper(L'a'), L'A');
    pend = pf->toupper(r2d2, r2d2 + 4);
    CHECK_PTR(pend, r2d2 + 4);
    CHECK_INT(r2d2[2], L'D');
    CHECK_INT(pf->tolower(L'A'), L'a');
    pend = pf->tolower(r2d2, r2d2 + 4);
    CHECK_PTR(pend, r2d2 + 4);
    CHECK_INT(r2d2[2], L'd');

    CHECK_INT(pf->widen('a'), L'a');
    pend_b = pf->widen(r2d2_b, r2d2_b + 4, r2d2);
    CHECK_PTR(pend_b, r2d2_b + 4);
    CHECK_INT(r2d2[0], L'R');
    CHECK_INT(pf->narrow(L'a', '\0'), 'a');
    r2d2_b[0] = L'.';
    pend      = pf->narrow(r2d2, r2d2 + 4, 'x', r2d2_b);
    CHECK_PTR(pend, r2d2 + 4);
    CHECK_INT(r2d2_b[0], 'R');

    CHECK(STD isalnum(L'a', loc));
    CHECK(STD isalpha(L'a', loc));
    CHECK(!STD isblank(L'a', loc));
    CHECK(!STD iscntrl(L'a', loc));
    CHECK(!STD isdigit(L'a', loc));
    CHECK(STD isgraph(L'a', loc));
    CHECK(STD islower(L'a', loc));
    CHECK(STD isprint(L'a', loc));
    CHECK(!STD ispunct(L'a', loc));
    CHECK(!STD isspace(L'a', loc));
    CHECK(!STD isupper(L'a', loc));
    CHECK(STD isxdigit(L'a', loc));
}

struct Myxnpunct : public STD numpunct<wchar_t> { // specify numeric punctuation
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

    virtual STD wstring do_truename() const { // return name for true
        return L"yes";
    }

    virtual STD wstring do_falsename() const { // return name for false
        return L"no";
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
};

void test_num_get() { // test num_get<wchar_t, istreambuf_iterator<wchar_t> >
    typedef STD istreambuf_iterator<wchar_t> Myiter;
    typedef STD num_get<wchar_t, Myiter> Myfac;
    const Myfac* pf      = &USE_FACET(loc, Myfac);
    STD locale::id* pid  = &pf->id;
    Myfac::char_type* pc = (wchar_t*) nullptr;
    Myfac::iter_type* pi = (Myiter*) nullptr;

    pid = pid; // to quiet diagnostics
    pc  = pc;
    pi  = pi;

    STD wistringstream istr(L" 1 no -123 4;56 789 012 3_ 0;45 6_78e2 0 ");
    STD locale loc_mypunct   = ADD_FACET(loc, new Myxnpunct);
    STD ios_base::iostate st = STD ios_base::goodbit;
    Myiter iit0, iit(istr);

    istr.imbue(loc_mypunct);

    bool bo = false;
    iit     = pf->get(++iit, iit0, istr, st, bo);
    CHECK_INT(*iit, L' ');
    CHECK(bo);
    istr.setf(STD ios_base::boolalpha);
    iit = pf->get(++iit, iit0, istr, st, bo);
    CHECK_INT(*iit, L' ');
    CHECK(!bo);

    long lo = 0;
    iit     = pf->get(++iit, iit0, istr, st, lo);
    CHECK_INT(*iit, L' ');
    CHECK_INT(lo, -123);

    unsigned short ush = 0;
    iit                = pf->get(++iit, iit0, istr, st, ush);
    CHECK_INT(*iit, L' ');
    CHECK_INT(ush, 456);

    unsigned int ui = 0;
    iit             = pf->get(++iit, iit0, istr, st, ui);
    CHECK_INT(*iit, L' ');
    CHECK_INT(ui, 789);

    unsigned long ul = 0;
    iit              = pf->get(++iit, iit0, istr, st, ul);
    CHECK_INT(*iit, L' ');
    CHECK_INT(ul, 12);

    float fl = 0.0F;
    iit      = pf->get(++iit, iit0, istr, st, fl);
    CHECK_INT(*iit, L' ');
    CHECK(fl == 3.0f);

    double db = 0.0;
    iit       = pf->get(++iit, iit0, istr, st, db);
    CHECK_INT(*iit, L' ');
    CHECK(db == 45.0);

    long double ld = 0.0L;
    iit            = pf->get(++iit, iit0, istr, st, ld);
    CHECK_INT(*iit, L' ');
    CHECK(ld == 678.0L);

    void* pv = nullptr;
    iit      = pf->get(++iit, iit0, istr, st, pv);
    CHECK_INT(*iit, L' ');
    CHECK_PTR(pv, nullptr);

    { // check for widen calls
        STD wistringstream istr2(L" @3Ab @45bA ");
        STD locale loc_myctype = ADD_FACET(loc, new Myxctype2);

        st  = STD ios_base::goodbit;
        iit = Myiter(istr2);

        istr2.imbue(loc_myctype);

        lo  = 0;
        iit = pf->get(++iit, iit0, istr2, st, lo);
        CHECK_INT(*iit, L' ');
        CHECK_INT(lo, -301);

        fl  = 0.0F;
        iit = pf->get(++iit, iit0, istr2, st, fl);
        CHECK_INT(*iit, L' ');
        CHECK(fl == -4510.0f);
    }
}

void test_num_put() { // test num_put<wchar_t, ostreambuf_iterator<wchar_t> >
    typedef STD ostreambuf_iterator<wchar_t> Myiter;
    typedef STD num_put<wchar_t, Myiter> Myfac;
    const Myfac* pf      = &USE_FACET(loc, Myfac);
    STD locale::id* pid  = &pf->id;
    Myfac::char_type* pc = (wchar_t*) nullptr;
    Myfac::iter_type* pi = (Myiter*) nullptr;

    pid = pid; // to quiet diagnostics
    pc  = pc;
    pi  = pi;

    STD wostringstream ostr;
    STD locale loc_mypunct = ADD_FACET(loc, new Myxnpunct);
    Myiter oit(ostr);

    ostr.imbue(loc_mypunct);

    oit = pf->put(oit, ostr, L'*', false);
    ostr.setf(STD ios_base::boolalpha);
    oit = pf->put(oit, ostr, L'*', true);
    CHECK_WSTR(ostr.str().c_str(), L"0yes");

    ostr.str(L"");
    oit = Myiter(ostr);
    ostr.width(8);
    ostr.setf(STD ios_base::internal);
    oit = pf->put(oit, ostr, L'*', -1234L);
    CHECK_WSTR(ostr.str().c_str(), L"-**12;34");

    ostr.str(L"");
    oit = Myiter(ostr);
    ostr.width(8);
    ostr.setf(STD ios_base::left, STD ios_base::adjustfield);
    ostr.setf(STD ios_base::hex, STD ios_base::basefield);
    oit = pf->put(oit, ostr, L'*', 0xbeefLU);
    CHECK_WSTR(ostr.str().c_str(), L"be;ef***");

    ostr.str(L"");
    oit = Myiter(ostr);
    ostr.setf(STD ios_base::left, STD ios_base::adjustfield);
    oit = pf->put(oit, ostr, L'*', 1234.5);
    CHECK_WSTR(ostr.str().c_str(), L"12;34_5");

    ostr.str(L"");
    oit = Myiter(ostr);
    ostr.setf(STD ios_base::left, STD ios_base::adjustfield);
    ostr.setf(STD ios_base::scientific, STD ios_base::floatfield);
    oit = pf->put(oit, ostr, L'*', -6789.0L);

    CHECK_WSTR(ostr.str().c_str(), L"-6_789000e+03");

    ostr.str(L"");
    oit = Myiter(ostr);
    oit = pf->put(oit, ostr, L'*', (void*) &oit);
    CHECK(0 < ostr.str().size());

    { // check for widen calls
        STD wostringstream ostr2;
        STD locale loc_myctype = ADD_FACET(loc, new Myxctype2);

        ostr2.imbue(loc_myctype);

        ostr2.str(L"");
        oit = Myiter(ostr2);
        oit = pf->put(oit, ostr2, L'*', -301L);
        CHECK_WSTR(ostr2.str().c_str(), L"@3Ab");

        ostr2.str(L"");
        oit = Myiter(ostr2);
        oit = pf->put(oit, ostr2, L'*', -4501.0F);
        CHECK_WSTR(ostr2.str().c_str(), L"@45Ab");
    }
}

void test_numpunct() { // test numpunct<wchar_t>
    typedef STD numpunct<wchar_t> Myfac;
    STD locale loc_mypunct   = ADD_FACET(loc, new Myxnpunct);
    const Myfac* pf          = &USE_FACET(loc_mypunct, Myfac);
    STD locale::id* pid      = &pf->id;
    Myfac::char_type* pc     = (wchar_t*) nullptr;
    Myfac::string_type* pstr = (STD wstring*) nullptr;

    pid  = pid; // to quiet diagnostics
    pc   = pc;
    pstr = pstr;

    CHECK_INT(pf->decimal_point(), L'_');
    CHECK_INT(pf->thousands_sep(), L';');
    CHECK_STR(pf->grouping().c_str(), "\2");
    CHECK_WSTR(pf->truename().c_str(), L"yes");
    CHECK_WSTR(pf->falsename().c_str(), L"no");
}

void test_main() { // test basic workings of locale definitions
    test_codecvt();
    test_ctype();
    test_num_get();
    test_num_put();
    test_numpunct();
}
