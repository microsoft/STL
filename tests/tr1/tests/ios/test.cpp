// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <ios>
#define TEST_NAME "<ios>"

#include "tdefs.h"
#include <exception>
#include <ios>
#include <iostream>
#include <limits.h>
#include <string.h>

// static data
static const STD ios_base::fmtflags ffl[] = {STD ios_base::dec, STD ios_base::fixed, STD ios_base::hex,
    STD ios_base::internal, STD ios_base::left, STD ios_base::oct, STD ios_base::right, STD ios_base::scientific,
    STD ios_base::showbase, STD ios_base::showpoint, STD ios_base::showpos, STD ios_base::skipws, STD ios_base::unitbuf,
    STD ios_base::uppercase, STD ios_base::boolalpha, STD ios_base::adjustfield, STD ios_base::basefield,
    STD ios_base::floatfield};

static const STD ios_base::iostate ifl[] = {
    STD ios_base::badbit, STD ios_base::eofbit, STD ios_base::failbit, STD ios_base::goodbit};

static const STD ios_base::openmode ofl[] = {STD ios_base::app, STD ios_base::ate, STD ios_base::binary,
    STD ios_base::in, STD ios_base::out, STD ios_base::trunc};

static const STD ios_base::seekdir sfl[] = {STD ios_base::beg, STD ios_base::cur, STD ios_base::end};

static STD ios_base::Init init_object;

struct myios : public STD ios { // makes protected functions public
    myios(STD streambuf* sb = nullptr) : STD ios(sb) { // initialize
    }

    void move(myios&& other) { // move from other
        STD ios::move(STD move(other));
    }

    void move(myios& other) { // move from other
        STD ios::move(other);
    }

    void swap(myios& other) { // swap with other
        STD ios::swap(other);
    }

    void set_rdbuf(STD streambuf* sb) { // set stream buffer pointer
        STD ios::set_rdbuf(sb);
    }
};

void test_main() { // test basic workings of ios definitions
    STD ios* pi = &STD cin;
    STD ios* po = &STD cout;
    STD ios x(pi->rdbuf());
    STD streamoff* pso = (STD streamoff*) nullptr;
    STD streampos* psp = (STD streampos*) nullptr;
    STD ios_base::failure fail_ex("message");
    STD exception* pex = &fail_ex;
    STD ios_base* pb   = (STD ios_base*) nullptr;

    setvbuf(stdout, nullptr, _IONBF, 0);

    STD wios* pwi = &STD wcin;
    STD wios* pwo = &STD wcout;
    STD wios wx(pwi->rdbuf());

    pb = pwi;
    pb = pwo;
    pb = pb; // to quiet diagnostics

    pb  = pi;
    pb  = pb;
    pb  = po;
    pso = pso; // to quiet diagnostics
    psp = psp;

    CHECK(ffl[0] != ffl[1]);
    CHECK(ifl[0] != ifl[1]);
    CHECK(ofl[0] != ofl[1]);
    CHECK(sfl[0] != sfl[1]);
    CHECK(CSTD memcmp(pex->what(), "message", 7) == 0);

    // test fmtflags groups
    CHECK_INT(STD ios_base::left | STD ios_base::right | STD ios_base::internal, STD ios_base::adjustfield);
    CHECK_INT(STD ios_base::dec | STD ios_base::oct | STD ios_base::hex, STD ios_base::basefield);
    CHECK_INT(STD ios_base::scientific | STD ios_base::fixed, STD ios_base::floatfield);

    // test assignment and control functions
    CHECK(x && true);
    CHECK(!x == 0);

    CHECK_PTR(x.tie(&STD cerr), nullptr);
    CHECK_PTR(x.tie(), &STD cerr);

    CHECK_PTR(x.rdbuf(po->rdbuf()), pi->rdbuf());
    CHECK_PTR(x.rdbuf(), po->rdbuf());

    x.clear();
    CHECK(x.good());
    CHECK_INT(x.rdstate(), STD ios_base::goodbit);
    x.clear(STD ios_base::badbit);
    CHECK(x.bad());
    CHECK_INT(x.rdstate(), STD ios_base::badbit);
    x.setstate(STD ios_base::failbit);
    CHECK(x.fail());
    CHECK(!x.eof());
    x.setstate(STD ios_base::eofbit);
    CHECK_INT(x.rdstate(), STD ios_base::badbit | STD ios_base::eofbit | STD ios_base::failbit);

    x.clear(STD ios_base::eofbit);
    pi->clear(STD ios_base::failbit);
    CHECK_INT(x.copyfmt(*pi).rdstate(), STD ios_base::eofbit);
    x.exceptions(STD ios_base::badbit);
    CHECK_INT(x.exceptions(), STD ios_base::badbit);

    {
        myios x1(pi->rdbuf());
        myios x2(po->rdbuf());
        x2.setstate(STD ios_base::eofbit);
        CHECK_INT(x1.rdstate(), 0);
        CHECK_INT(x2.rdstate(), STD ios_base::eofbit);

        x1.move(STD move(x2));
        CHECK_INT(x1.rdstate(), STD ios_base::eofbit);
        CHECK_INT(x2.rdstate(), 0);
        CHECK_PTR(x1.rdbuf(), nullptr);
        CHECK_PTR(x2.rdbuf(), po->rdbuf());

        x1.move(x2);
        CHECK_INT(x1.rdstate(), 0);
        CHECK_INT(x2.rdstate(), STD ios_base::eofbit);
        CHECK_PTR(x1.rdbuf(), nullptr);
        CHECK_PTR(x2.rdbuf(), po->rdbuf());

        x1.move(STD move(x2));
        CHECK_INT(x1.rdstate(), STD ios_base::eofbit);
        CHECK_INT(x2.rdstate(), 0);
        CHECK_PTR(x1.rdbuf(), nullptr);
        CHECK_PTR(x2.rdbuf(), po->rdbuf());

        x1.set_rdbuf(nullptr);
        CHECK_INT(x1.rdstate(), STD ios_base::eofbit);
        CHECK_PTR(x1.rdbuf(), nullptr);
        x1.set_rdbuf(pi->rdbuf());
        CHECK_INT(x1.rdstate(), STD ios_base::eofbit);
        CHECK_PTR(x1.rdbuf(), pi->rdbuf());

        x1.swap(x2);
        CHECK_INT(x1.rdstate(), 0);
        CHECK_INT(x2.rdstate(), STD ios_base::eofbit);
        CHECK_PTR(x1.rdbuf(), pi->rdbuf());
        CHECK_PTR(x2.rdbuf(), po->rdbuf());
    }

    // test format control functions
    CHECK_INT(x.flags(STD ios_base::oct), STD ios_base::skipws | STD ios_base::dec);
    CHECK_INT(x.flags(), STD ios_base::oct);
    CHECK_INT(x.setf(STD ios_base::showbase), STD ios_base::oct);
    CHECK_INT(x.setf(STD ios_base::scientific), STD ios_base::oct | STD ios_base::showbase);
    CHECK_INT(x.setf((STD ios_base::fmtflags) ~0, STD ios_base::unitbuf),
        STD ios_base::oct | STD ios_base::scientific | STD ios_base::showbase);
    x.unsetf(STD ios_base::oct | STD ios_base::showbase);
    CHECK_INT(x.flags(), STD ios_base::scientific | STD ios_base::unitbuf);
    x.setf(STD ios_base::boolalpha);
    CHECK_INT(x.flags(), STD ios_base::boolalpha | STD ios_base::scientific | STD ios_base::unitbuf);
    x.unsetf(STD ios_base::boolalpha);
    CHECK_INT(x.flags(), STD ios_base::scientific | STD ios_base::unitbuf);

    x.clear(STD ios_base::floatfield);
    x.setf(STD ios_base::scientific | STD ios_base::fixed);
    CHECK_INT(x.flags(), STD ios_base::scientific | STD ios_base::fixed | STD ios_base::unitbuf);

    CHECK_INT(x.precision(INT_MIN), 6);
    CHECK_INT(x.precision(), INT_MIN);
    CHECK_INT(x.width(INT_MAX), 0);
    CHECK_INT(x.width(), INT_MAX);
    CHECK_INT(x.fill('y'), ' ');
    CHECK_INT(x.fill(), 'y');

    // test additional storage
    int i = pi->xalloc();
    int j = pi->xalloc();

    CHECK_INT(i, j - 1);
    CHECK_INT(pi->iword(i), 0);
    CHECK_PTR(pi->pword(j), nullptr);
    pi->iword(i)            = 3;
    pi->pword(j)            = (void*) &STD cout;
    x.copyfmt(*pi).iword(j) = 4;
    CHECK_INT(pi->iword(i), 3);
    CHECK_PTR(pi->pword(i), nullptr);
    CHECK_INT(pi->iword(j), 0);
    CHECK_PTR(pi->pword(j), (void*) &STD cout);
    CHECK_INT(x.iword(i), 3);
    CHECK_PTR(x.pword(i), nullptr);
    CHECK_INT(x.iword(j), 4);
    CHECK_PTR(x.pword(j), (void*) &STD cout);

    // test manipulators
    pi->unsetf((STD ios_base::fmtflags) ~0);
    STD dec(STD fixed(STD internal(STD showbase(STD showpoint(*pi)))));
    CHECK_INT(pi->flags(), STD ios_base::dec | STD ios_base::fixed | STD ios_base::internal | STD ios_base::showbase
                               | STD ios_base::showpoint);
    po->unsetf((STD ios_base::fmtflags) ~0);
    STD hex(STD left(STD scientific(STD showpos(STD skipws(STD boolalpha(*po))))));
    CHECK_INT(po->flags(), STD ios_base::hex | STD ios_base::left | STD ios_base::scientific | STD ios_base::showpos
                               | STD ios_base::skipws | STD ios_base::boolalpha);
    STD noshowbase(STD noshowpoint(STD noshowpos(STD noskipws(STD noboolalpha(*po)))));
    STD uppercase(STD oct(STD right(*po)));
    CHECK_INT(
        po->flags(), STD ios_base::oct | STD ios_base::right | STD ios_base::scientific | STD ios_base::uppercase);
    STD scientific(STD nouppercase(*po));
    CHECK_INT(po->flags(), STD ios_base::oct | STD ios_base::right | STD ios_base::scientific);

    STD hexfloat(*po);
    CHECK_INT(po->flags(), STD ios_base::oct | STD ios_base::right | STD ios_base::scientific | STD ios_base::fixed);

    STD defaultfloat(*po);
    CHECK_INT(po->flags(), STD ios_base::oct | STD ios_base::right);

    STD hexfloat(*po);
    CHECK_INT(po->flags(), STD ios_base::oct | STD ios_base::right | STD ios_base::scientific | STD ios_base::fixed);

    STD error_code ec1 = STD make_error_code(STD io_errc::stream);
    CHECK(ec1.value() == (int) STD io_errc::stream);
    CHECK(ec1.category() == STD iostream_category());

    STD error_condition ec2 = STD make_error_condition(STD io_errc::stream);
    CHECK(ec2.value() == (int) STD io_errc::stream);
    CHECK(ec2.category() == STD iostream_category());

    STD ios_base::failure fail_ex2((const char*) "message", ec1);
    STD system_error* pse = &fail_ex2;
    pse                   = pse; // to quiet diagnostics

    // test template equivalence
    STD basic_ios<char, STD char_traits<char>>* pbi        = (STD ios*) nullptr;
    STD basic_ios<wchar_t, STD char_traits<wchar_t>>* pwbi = (STD wios*) nullptr;

    pbi  = pbi; // to quiet diagnostics
    pwbi = pwbi;
}
