// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <istream>, part 2
#define TEST_NAME "<istream>, part 2"

#include "tdefs.h"
#include <istream>
#include <sstream>
#include <wchar.h>

// class Boolx
class Boolx { // wrap an int
public:
    Boolx(int v) : val(v) { // construct from value
    }

    int value() const { // get value
        return val;
    }

    int value(int v) { // set value
        return val = v;
    }

private:
    int val;
};

STD wistream& operator>>(STD wistream& istr, Boolx& b) { // extract a Boolx
    STD ios_base::iostate state = STD ios_base::goodbit;
    const STD wistream::sentry ok(istr, false);

    if (ok) { // state okay, extract Y or N
        const CSTD wint_t c = istr.rdbuf()->sbumpc();

        if (c == L'Y')
            b.value(1);
        else if (c == L'N')
            b.value(0);
        else { // report failure
            istr.rdbuf()->sputbackc(c);
            state = STD ios::failbit;
        }
    }
    istr.setstate(state);
    return istr;
}

void test_main() { // test basic workings of istream definitions
    static const wchar_t input[] = L"s1"
                                   L"   a"
                                   L" 1 23 -456 +7890   0012 00034 0"
                                   L" -12 34"
                                   L" 12 +3.4567e+0004-8900e-2"
                                   L" 0x8.000p-2"
                                   L" N  Y"
                                   L" false true"
                                   L" 0 1"
                                   L"line of text\n"
                                   L"rest of stream";
    STD wstringbuf strinput(input);
    STD wistream ins(&strinput);

    STD wistream::char_type* pc   = (wchar_t*) nullptr;
    STD wistream::int_type* pi    = (CSTD wint_t*) nullptr;
    STD wistream::pos_type* pp    = (STD wstreampos*) nullptr;
    STD wistream::off_type* po    = (STD streamoff*) nullptr;
    STD wistream::traits_type* pt = (STD char_traits<wchar_t>*) nullptr;

    pc = pc; // to quiet diagnostics
    pi = pi;
    pp = pp;
    po = po;
    pt = pt;

    CHECK(ins.good());
    CHECK((ins.flags() & STD ios_base::showbase) == 0);
    ins >> STD showbase;
    CHECK((ins.flags() & STD ios_base::showbase) != 0);

    {
        typedef STD wistringstream Mycont;
        STD wstringbuf sbuf;
        STD wostream ostr(&sbuf);

        Mycont v6;
        v6.tie(&ostr);
        Mycont v7(STD move(v6));
        CHECK_PTR(v6.tie(), nullptr);
        CHECK_PTR(v7.tie(), &ostr);

        Mycont v8;
        v8 = STD move(v7);
        CHECK_PTR(v7.tie(), nullptr);
        CHECK_PTR(v8.tie(), &ostr);

        v7.swap(v8);
        CHECK_PTR(v7.tie(), &ostr);
        CHECK_PTR(v8.tie(), nullptr);

        STD swap(v7, v8);
        CHECK_PTR(v7.tie(), nullptr);
        CHECK_PTR(v8.tie(), &ostr);

        wchar_t ch;
        sbuf.str(L"x");
        STD wistream(&sbuf) >> ch;
        CHECK_INT(ch, 'x');

        wchar_t cha[10] = {0};
        sbuf.str(L"x");
        STD wistream(&sbuf) >> cha;
        CHECK_WSTR(&cha[0], L"x");
    }

    // test string extractor
    wchar_t s1[10];

    ins >> s1;
    CHECK_WSTR(s1, L"s1");

    // test character extractor
    wchar_t ch;

    ins >> STD noskipws >> STD ws >> ch >> STD skipws;
    CHECK_INT(ch, L'a');

    // test integer extractors
    int i;
    long lo;
    short sh;
    unsigned int ui;
    unsigned long ulo;
    unsigned short ush;
    void* p;

    ins >> sh >> STD hex >> ush >> STD oct >> i >> STD dec >> ui;
    CHECK_INT(sh, 1);
    CHECK_INT(ush, 0x23);
    CHECK_INT(i, -0456);
    CHECK_INT(ui, 7890);

    ins >> lo >> ulo >> p;
    CHECK_INT(lo, 12);
    CHECK_INT(ulo, 34);
    CHECK_PTR(p, nullptr);

    // test long long extractors
    long long llo;
    unsigned long long ullo;

    ins >> llo >> ullo;
    CHECK(llo == -12);
    CHECK(ullo == 34);

    // test floating-point extractors
    double db;
    float fl;
    long double ldb;

    ins >> fl >> db >> ldb;
    CHECK(fl == 12.0);
    CHECK(db == 34567.0);
    CHECK(ldb == -89.0);

    ins >> STD hexfloat >> db;
    CHECK(db == 2.0);

    // test Boolx extractor
    Boolx no(0), yes(1);

    CHECK_INT(no.value(), 0);
    CHECK_INT(yes.value(), 1);
    ins >> yes >> no;
    CHECK_INT(no.value(), 1);
    CHECK_INT(yes.value(), 0);

    // test bool extractors
    bool b0 = 1, b1 = 0;
    ins >> STD boolalpha >> b0 >> b1;
    CHECK(!b0);
    CHECK(b1);
    ins >> STD noboolalpha >> b1 >> b0;
    CHECK(b0);
    CHECK(!b1);

    // test positioning functions
    wchar_t buf[20];
    STD wstreampos pos = ins.tellg();

    CHECK(pos != (STD wstreampos)(-1));
    CHECK_PTR(&ins.ignore(), &ins);
    CHECK_INT(ins.peek(), L'i');
    CHECK_PTR(&ins.putback(L'l'), &ins);
    CHECK_PTR(&ins.read(buf, 4), &ins);
    CHECK_WMEM(buf, L"line", 3);
    CHECK_PTR(&ins.ignore(20, L'f'), &ins);
    CHECK_INT(ins.gcount(), 3);
    CHECK_INT(ins.readsome(buf, 5), 5);
    CHECK_WMEM(buf, L" text", 5);
    CHECK_PTR(&ins.unget(), &ins);
    CHECK_INT(ins.peek(), L't');
    CHECK_PTR(&ins.ignore(100), &ins);
    CHECK_INT(ins.get(), WEOF);
    CHECK(!ins.good());
    ins.clear();

    CHECK_PTR(&ins.seekg(pos), &ins);
    CHECK_PTR(&ins.getline(buf, sizeof(buf) / sizeof(wchar_t), L' '), &ins);
    CHECK_WSTR(buf, L"line");
    CHECK_INT(ins.gcount(), 5);
    CHECK_PTR(&ins.seekg(0, STD ios_base::cur), &ins);
    CHECK_PTR(&ins.getline(buf, sizeof(buf) / sizeof(wchar_t)), &ins);
    CHECK_WSTR(buf, L"of text");

    // test streambuf extractor and get functions
    wchar_t sbuf[20], ubuf[20];
    STD wstringbuf strbuf;

    ins >> (STD wstreambuf*) &strbuf;
    ins.rdbuf(&strbuf);
    ins.get(buf, sizeof(buf) / sizeof(wchar_t), L'o');
    CHECK_WSTR(buf, L"rest ");
    ins.get(sbuf, sizeof(sbuf) / sizeof(wchar_t), L's');
    CHECK_WSTR(sbuf, L"of ");
    ins.get(ubuf, sizeof(ubuf) / sizeof(wchar_t));
    CHECK_WSTR(ubuf, L"stream");
    CHECK(ins.sync() == 0);

    // test template equivalence
    STD basic_istream<wchar_t, STD char_traits<wchar_t>>* pbi   = (STD wistream*) nullptr;
    STD basic_iostream<wchar_t, STD char_traits<wchar_t>>* pbio = (STD wiostream*) nullptr;

    pbi  = pbi; // to quiet diagnostics
    pbio = pbio;

    // test wiostream
    STD wstringbuf strsb;
    STD wiostream iostr(&strsb);
    STD wistream* pistr = &iostr;
    STD wostream* postr = &iostr;

    pistr = pistr; // to quiet diagnostics
    postr = postr;

    iostr << L"this is a test\n";
    iostr.getline(buf, sizeof(buf) / sizeof(wchar_t));
    CHECK_WSTR(buf, L"this is a test");

    {
        typedef STD wstringstream Mycont;
        STD wostream ostr(&strsb);

        Mycont v6;
        v6.tie(&ostr);
        Mycont v7(STD move(v6));
        CHECK_PTR(v6.tie(), nullptr);
        CHECK_PTR(v7.tie(), &ostr);

        Mycont v8;
        v8 = STD move(v7);
        CHECK_PTR(v7.tie(), nullptr);
        CHECK_PTR(v8.tie(), &ostr);

        v7.swap(v8);
        CHECK_PTR(v7.tie(), &ostr);
        CHECK_PTR(v8.tie(), nullptr);

        STD swap(v7, v8);
        CHECK_PTR(v7.tie(), nullptr);
        CHECK_PTR(v8.tie(), &ostr);
    }
}
