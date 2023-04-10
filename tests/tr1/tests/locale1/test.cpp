// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <locale>, part 1
#define TEST_NAME "<locale>, part 1"

#include "tdefs.h"
#include <iterator>
#include <locale>
#include <sstream>
#include <string>
#include <wchar.h>

#define ADD_FACET(loc, pfac) STD locale(loc, pfac)
#define HAS_FACET(loc, Fac)  STD has_facet<Fac>(loc)
#define USE_FACET(loc, Fac)  STD use_facet<Fac>(loc)

STD locale loc;

void test_codecvt() { // test codecvt<char, char, mbstate_t>
    typedef STD codecvt<char, char, CSTD mbstate_t> Myfac;
    const Myfac* pf       = &USE_FACET(loc, Myfac);
    STD locale loc_byname = ADD_FACET(loc, (new STD codecvt_byname<char, char, CSTD mbstate_t>("C")));

    STD locale loc_byname_str = ADD_FACET(loc, (new STD codecvt_byname<char, char, CSTD mbstate_t>(STD string("C"))));

    const STD codecvt_base* pbase = pf;
    STD locale::id* pid           = &pf->id;
    Myfac::intern_type* pin       = (char*) nullptr;
    Myfac::extern_type* pex       = (char*) nullptr;
    Myfac::state_type* pst        = (CSTD mbstate_t*) nullptr;

    STD codecvt_base::result res_tab[] = {
        STD codecvt_base::error, STD codecvt_base::noconv, STD codecvt_base::ok, STD codecvt_base::partial};

    pbase = pbase; // to quiet diagnostics;
    pid   = pid;
    pin   = pin;
    pex   = pex;
    pst   = pst;

    int i, j, ok;

    for (ok = 1, i = sizeof(res_tab) / sizeof(res_tab[0]); 0 < i;) {
        for (j = --i; 0 < j;) { // test an (i, j) pair
            int testno = i * 0x100 + j;
            ok         = CHECK0(ok, testno, res_tab[i] != res_tab[--j]);
        }
    }
    CHECK_MSG("codecvt_base::result values are distinct", ok);

    const char ibuf[] = "abc";
    const char* inext;
    char obuf[10];
    char* onext;
    static CSTD mbstate_t st0; // initial state
    CSTD mbstate_t st;
    STD codecvt_base::result ans;

    CHECK(-1 <= pf->encoding());
    CHECK_INT(pf->max_length(), 1);

    st = st0;
    CHECK_INT(pf->length(st, ibuf, ibuf + 3, 1), 1);
    st = st0;
    CHECK_INT(pf->length(st, ibuf, ibuf + 3, 10), 3);

    st    = st0;
    inext = nullptr;
    onext = nullptr;
    ans   = pf->out(st, ibuf, ibuf + 3, inext, obuf, obuf + 10, onext);
    if (ans == STD codecvt_base::noconv) { // no conversion, acceptable
        CHECK_INT(ans, STD codecvt_base::noconv);
        CHECK(pf->always_noconv());
        CHECK_PTR(inext, ibuf);
        CHECK_PTR(onext, obuf);
    } else { // converted, should succeed
        CHECK_INT(ans, STD codecvt_base::ok);
        CHECK(!pf->always_noconv());
        CHECK_PTR(inext, ibuf + 3);
        CHECK_PTR(onext, obuf + 3);
        CHECK_MEM(obuf, "abc", 3);
    }

    onext = nullptr;
    ans   = pf->unshift(st, obuf, obuf + 10, onext);
    CHECK(ans == STD codecvt_base::noconv || ans == STD codecvt_base::ok);
    CHECK_PTR(onext, obuf);

    st    = st0;
    inext = nullptr;
    onext = nullptr;
    ans   = pf->in(st, ibuf, ibuf + 3, inext, obuf, obuf + 10, onext);
    if (ans == STD codecvt_base::noconv) { // no conversion, acceptable
        CHECK_INT(ans, STD codecvt_base::noconv);
        CHECK_PTR(inext, ibuf);
        CHECK_PTR(onext, obuf);
    } else { // converted, should succeed
        CHECK_INT(ans, STD codecvt_base::ok);
        CHECK_PTR(inext, ibuf + 3);
        CHECK_PTR(onext, obuf + 3);
        CHECK_MEM(obuf, "abc", 3);
    }
}

struct Myxctype : public STD ctype<char> { // get protected members
    typedef STD ctype<char> Myfac;

    Myxctype(const mask* tab_arg) : Myfac(tab_arg) { // construct with specified table
    }

    const mask* get_table() const { // get table
        return table();
    }

    const mask* get_classic_table() const { // get classic table
        return classic_table();
    }
};

void test_ctype() { // test ctype<char>
    typedef STD ctype<char> Myfac;
    const Myfac* pf       = &USE_FACET(loc, Myfac);
    STD locale loc_byname = ADD_FACET(loc, (new STD ctype_byname<char>("C")));

    STD locale loc_byname_str = ADD_FACET(loc, (new STD ctype_byname<char>(STD string("C"))));

    const STD ctype_base* pbase = pf;
    STD locale::id* pid         = &pf->id;
    Myfac::char_type* pc        = (char*) nullptr;
    STD ctype_base::mask mbuf[10];
    char r2d2[]   = {'R', '2', 'd', '2', '\0'};
    char r2d2_b[] = "R2d2";
    const char* pend_b;
    const char* pend;

    pid   = pid; // to quiet diagnostics
    pc    = pc;
    pbase = pbase;

    CHECK(pf->is(STD ctype_base::alnum, 'a'));
    pend = pf->is(r2d2, r2d2 + 4, mbuf);
    CHECK_PTR(pend, r2d2 + 4);
    CHECK_INT(mbuf[1] & STD ctype_base::digit, STD ctype_base::digit);
    pend = pf->scan_is(STD ctype_base::lower, r2d2, r2d2 + 4);
    CHECK_PTR(pend, r2d2 + 2);
    pend = pf->scan_not(STD ctype_base::alpha, r2d2, r2d2 + 4);
    CHECK_PTR(pend, r2d2 + 1);

    CHECK_INT(pf->toupper('a'), 'A');
    pend = pf->toupper(r2d2, r2d2 + 4);
    CHECK_PTR(pend, r2d2 + 4);
    CHECK_INT(r2d2[2], 'D');
    CHECK_INT(pf->tolower('A'), 'a');
    pend = pf->tolower(r2d2, r2d2 + 4);
    CHECK_PTR(pend, r2d2 + 4);
    CHECK_INT(r2d2[2], 'd');

    CHECK_INT(pf->widen('a'), 'a');
    pend_b = pf->widen(r2d2_b, r2d2_b + 4, r2d2);
    CHECK_PTR(pend_b, r2d2_b + 4);
    CHECK_INT(r2d2[0], 'R');
    CHECK_INT(pf->narrow('a', '\0'), 'a');
    r2d2_b[0] = '.';
    pend      = pf->narrow(r2d2, r2d2 + 4, 'x', r2d2_b);
    CHECK_PTR(pend, r2d2 + 4);
    CHECK_INT(r2d2_b[0], 'R');

    CHECK(STD isalnum('a', loc));
    CHECK(STD isalpha('a', loc));
    CHECK(!STD isblank('a', loc));
    CHECK(!STD iscntrl('a', loc));
    CHECK(!STD isdigit('a', loc));
    CHECK(STD isgraph('a', loc));
    CHECK(STD islower('a', loc));
    CHECK(STD isprint('a', loc));
    CHECK(!STD ispunct('a', loc));
    CHECK(!STD isspace('a', loc));
    CHECK(!STD isupper('a', loc));
    CHECK(STD isxdigit('a', loc));

    { // test ctype<char> specifics
        STD ctype_base::mask masks[] = {STD ctype_base::alnum, STD ctype_base::alpha,
            // STD ctype_base::blank,
            STD ctype_base::cntrl, STD ctype_base::digit, STD ctype_base::graph, STD ctype_base::lower,
            STD ctype_base::print, STD ctype_base::punct, STD ctype_base::space, STD ctype_base::upper,
            STD ctype_base::xdigit};

        int i, j, ok;

        for (ok = 1, i = sizeof(masks) / sizeof(masks[0]); 0 < i;) {
            for (j = --i; 0 < j;) { // test an (i, j) pair
                int testno = i * 0x100 + j;
                ok         = CHECK0(ok, testno, masks[i] != masks[--j]);
            }
        }
        CHECK_MSG("ctype_base::mask values are distinct", ok);

        Myxctype* pf3        = new Myxctype(masks);
        STD locale loc_masks = ADD_FACET(loc, pf3);

        CHECK(256 <= pf->table_size);
        CHECK_PTR(pf3->get_table(), masks);
        CHECK_INT(pf3->get_classic_table()[static_cast<int>('3')] & STD ctype_base::digit, STD ctype_base::digit);
    }
}

struct Myxnpunct : public STD numpunct<char> { // specify numeric punctuation
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

    virtual STD string do_truename() const { // return name for true
        return "yes";
    }

    virtual STD string do_falsename() const { // return name for false
        return "no";
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
};

void test_num_get() { // test num_get<char, istreambuf_iterator<char> >
    typedef STD istreambuf_iterator<char> Myiter;
    typedef STD num_get<char, Myiter> Myfac;
    const Myfac* pf      = &USE_FACET(loc, Myfac);
    STD locale::id* pid  = &pf->id;
    Myfac::char_type* pc = (char*) nullptr;
    Myfac::iter_type* pi = (Myiter*) nullptr;

    pid = pid; // to quiet diagnostics
    pc  = pc;
    pi  = pi;

    STD istringstream istr(" 1 no -123 4;56 789 012 3_ 0;45 6_78e2 0 ");
    STD locale loc_mypunct   = ADD_FACET(loc, new Myxnpunct);
    STD ios_base::iostate st = STD ios_base::goodbit;
    Myiter iit0, iit(istr);

    istr.imbue(loc_mypunct);

    bool bo = false;
    iit     = pf->get(++iit, iit0, istr, st, bo);
    CHECK_INT(*iit, ' ');
    CHECK(bo);
    istr.setf(STD ios_base::boolalpha);
    iit = pf->get(++iit, iit0, istr, st, bo);
    CHECK_INT(*iit, ' ');
    CHECK(!bo);

    long lo = 0;
    iit     = pf->get(++iit, iit0, istr, st, lo);
    CHECK_INT(*iit, ' ');
    CHECK_INT(lo, -123);

    unsigned short ush = 0;
    iit                = pf->get(++iit, iit0, istr, st, ush);
    CHECK_INT(*iit, ' ');
    CHECK_INT(ush, 456);

    unsigned int ui = 0;
    iit             = pf->get(++iit, iit0, istr, st, ui);
    CHECK_INT(*iit, ' ');
    CHECK_INT(ui, 789);

    unsigned long ul = 0;
    iit              = pf->get(++iit, iit0, istr, st, ul);
    CHECK_INT(*iit, ' ');
    CHECK_INT(ul, 12);

    float fl = 0.0F;
    iit      = pf->get(++iit, iit0, istr, st, fl);
    CHECK_INT(*iit, ' ');
    CHECK(fl == 3.0f);

    double db = 0.0;
    iit       = pf->get(++iit, iit0, istr, st, db);
    CHECK_INT(*iit, ' ');
    CHECK(db == 45.0);

    long double ld = 0.0L;
    iit            = pf->get(++iit, iit0, istr, st, ld);
    CHECK_INT(*iit, ' ');
    CHECK(ld == 678.0L);

    void* pv = nullptr;
    iit      = pf->get(++iit, iit0, istr, st, pv);
    CHECK_INT(*iit, ' ');
    CHECK_PTR(pv, nullptr);

    { // check for widen calls
        STD istringstream istr2(" @3Ab @45bA ");
        STD locale loc_myctype = ADD_FACET(loc, new Myxctype2);

        st  = STD ios_base::goodbit;
        iit = Myiter(istr2);

        istr2.imbue(loc_myctype);

        lo  = 0;
        iit = pf->get(++iit, iit0, istr2, st, lo);
        CHECK_INT(*iit, ' ');
        CHECK_INT(lo, -301);

        fl  = 0.0F;
        iit = pf->get(++iit, iit0, istr2, st, fl);
        CHECK_INT(*iit, ' ');
        CHECK(fl == -4510.0f);
    }
}

void test_num_put() { // test num_put<char, ostreambuf_iterator<char> >
    typedef STD ostreambuf_iterator<char> Myiter;
    typedef STD num_put<char, Myiter> Myfac;
    const Myfac* pf      = &USE_FACET(loc, Myfac);
    STD locale::id* pid  = &pf->id;
    Myfac::char_type* pc = (char*) nullptr;
    Myfac::iter_type* pi = (Myiter*) nullptr;

    pid = pid; // to quiet diagnostics
    pc  = pc;
    pi  = pi;

    STD ostringstream ostr;
    STD locale loc_mypunct = ADD_FACET(loc, new Myxnpunct);
    Myiter oit(ostr);

    ostr.imbue(loc_mypunct);

    oit = pf->put(oit, ostr, '*', false);
    ostr.setf(STD ios_base::boolalpha);
    oit = pf->put(oit, ostr, '*', true);
    CHECK_STR(ostr.str().c_str(), "0yes");

    ostr.str("");
    oit = Myiter(ostr);
    ostr.width(8);
    ostr.setf(STD ios_base::internal);
    oit = pf->put(oit, ostr, '*', -1234L);
    CHECK_STR(ostr.str().c_str(), "-**12;34");

    ostr.str("");
    oit = Myiter(ostr);
    ostr.width(8);
    ostr.setf(STD ios_base::left, STD ios_base::adjustfield);
    ostr.setf(STD ios_base::hex, STD ios_base::basefield);
    oit = pf->put(oit, ostr, '*', 0xbeefLU);
    CHECK_STR(ostr.str().c_str(), "be;ef***");

    ostr.str("");
    oit = Myiter(ostr);
    ostr.setf(STD ios_base::left, STD ios_base::adjustfield);
    oit = pf->put(oit, ostr, '*', 1234.5);
    CHECK_STR(ostr.str().c_str(), "12;34_5");

    ostr.str("");
    oit = Myiter(ostr);
    ostr.setf(STD ios_base::left, STD ios_base::adjustfield);
    ostr.setf(STD ios_base::scientific, STD ios_base::floatfield);
    oit = pf->put(oit, ostr, '*', -6789.0L);

    CHECK_STR(ostr.str().c_str(), "-6_789000e+03");

    ostr.str("");
    oit = Myiter(ostr);
    oit = pf->put(oit, ostr, '*', (void*) &oit);
    CHECK(0 < ostr.str().size());

    { // check for widen calls
        STD ostringstream ostr2;
        STD locale loc_myctype = ADD_FACET(loc, new Myxctype2);

        ostr2.imbue(loc_myctype);

        ostr2.str("");
        oit = Myiter(ostr2);
        oit = pf->put(oit, ostr2, '*', -301L);
        CHECK_STR(ostr2.str().c_str(), "@3Ab");

        ostr2.str("");
        oit = Myiter(ostr2);
        oit = pf->put(oit, ostr2, '*', -4501.0F);
        CHECK_STR(ostr2.str().c_str(), "@45Ab");
    }
}

void test_numpunct() { // test numpunct<char>
    typedef STD numpunct<char> Myfac;
    STD locale loc_mypunct = ADD_FACET(loc, new Myxnpunct);
    const Myfac* pf        = &USE_FACET(loc_mypunct, Myfac);
    STD locale loc_byname  = ADD_FACET(loc, (new STD numpunct_byname<char>("C")));

    STD locale loc_byname_str = ADD_FACET(loc, (new STD numpunct_byname<char>(STD string("C"))));

    STD locale::id* pid      = &pf->id;
    Myfac::char_type* pc     = (char*) nullptr;
    Myfac::string_type* pstr = (STD string*) nullptr;

    pid  = pid; // to quiet diagnostics
    pc   = pc;
    pstr = pstr;

    CHECK_INT(pf->decimal_point(), '_');
    CHECK_INT(pf->thousands_sep(), ';');
    CHECK_STR(pf->grouping().c_str(), "\2");
    CHECK_STR(pf->truename().c_str(), "yes");
    CHECK_STR(pf->falsename().c_str(), "no");
}

void test_main() { // test basic workings of locale definitions
    STD locale::category cats[] = {STD locale::none, STD locale::collate, STD locale::ctype, STD locale::monetary,
        STD locale::numeric, STD locale::time, STD locale::messages, STD locale::all};
    STD locale* ploc            = (STD locale*) nullptr;
    STD locale::facet* pfac     = (STD locale::facet*) nullptr;
    STD locale::id* pid         = (STD locale::id*) nullptr;
    STD locale::category* pcat  = (int*) nullptr;

    ploc = ploc; // to quiet diagnostics
    pfac = pfac;
    pid  = pid;
    pcat = pcat;

    int i, j, ok;

    for (ok = 1, i = sizeof(cats) / sizeof(cats[0]); 0 < i;) {
        for (j = --i; 0 < j;) { // test an (i, j) pair
            int testno = i * 0x100 + j;
            ok         = CHECK0(ok, testno, cats[i] != cats[--j]);
        }
    }
    CHECK_MSG("locale::category values are distinct", ok);

    STD locale::category allcats = 0;
    for (i = 1; i < 7; ++i) {
        allcats |= cats[i];
    }
    CHECK_INT(cats[7] & allcats, allcats);

    STD locale cloc("C");
    STD locale partial_cloc(loc, "C", STD locale::numeric);
    STD locale partial_loc = ADD_FACET(loc, new STD ctype<char>(nullptr));
    STD locale partial_loc2(loc, partial_loc, STD locale::time);

    STD locale cloc_str(STD string("C"));
    STD locale partial_cloc_str(loc, STD string("C"), STD locale::numeric);

    partial_loc2 = loc.combine<STD ctype<wchar_t>>(partial_cloc);
    typedef STD numpunct<char> Mynpunct;

    CHECK(loc == STD locale::classic());
    STD locale::global(partial_loc);
    CHECK(loc != STD locale());
    STD locale::global(loc);
    CHECK(HAS_FACET(loc, Mynpunct));
    CHECK_INT(USE_FACET(loc, Mynpunct).decimal_point(), '.');

    CHECK_STR(loc.name().c_str(), "C");
    CHECK_STR(partial_loc.name().c_str(), "*");

    test_codecvt();
    test_ctype();
    test_num_get();
    test_num_put();
    test_numpunct();
}
