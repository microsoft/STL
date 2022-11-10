// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <istream>, part 1
#define TEST_NAME "<istream>, part 1"

#include "tdefs.h"
#include <istream>
#include <sstream>

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

STD istream& operator>>(STD istream& istr, Boolx& b) { // extract a Boolx
    STD ios_base::iostate state = STD ios_base::goodbit;
    const STD istream::sentry ok(istr, false);

    if (ok) { // state okay, extract Y or N
        const int c = istr.rdbuf()->sbumpc();

        if (c == 'Y') {
            b.value(1);
        } else if (c == 'N') {
            b.value(0);
        } else { // report failure
            istr.rdbuf()->sputbackc((char) c);
            state = STD ios::failbit;
        }
    }
    istr.setstate(state);
    return istr;
}

void test_main() { // test basic workings of istream definitions
    static const char input[] = "s1 s2 s3"
                                "   abc"
                                " 1 23 -456 +7890   0012 00034 0"
                                " -12 34"
                                " 12 +3.4567e+0004-8900e-2"
                                " 0x8.000p-2"
                                " N  Y"
                                " false true"
                                " 0 1"
                                "line of text\n"
                                "rest of stream";
    STD stringbuf strinput(input);
    STD istream ins(&strinput);

    STD istream::char_type* pc   = (char*) nullptr;
    STD istream::int_type* pi    = (int*) nullptr;
    STD istream::pos_type* pp    = (STD streampos*) nullptr;
    STD istream::off_type* po    = (STD streamoff*) nullptr;
    STD istream::traits_type* pt = (STD char_traits<char>*) nullptr;

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
        typedef STD istringstream Mycont;
        STD stringbuf sbuf;
        STD ostream ostr(&sbuf);

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

        char ch;
        sbuf.str("x");
        STD istream(&sbuf) >> ch;
        CHECK_INT(ch, 'x');

        signed char sch;
        sbuf.str("x");
        STD istream(&sbuf) >> sch;
        CHECK_INT(sch, 'x');

        unsigned char uch;
        sbuf.str("x");
        STD istream(&sbuf) >> uch;
        CHECK_INT(uch, 'x');

        char cha[10] = {0};
        sbuf.str("x");
        STD istream(&sbuf) >> cha;
        CHECK_STR(&cha[0], "x");

        signed char scha[10] = {0};
        sbuf.str("x");
        STD istream(&sbuf) >> scha;
        CHECK_STR((char*) &scha[0], "x");

        unsigned char ucha[10] = {0};
        sbuf.str("x");
        STD istream(&sbuf) >> ucha;
        CHECK_STR((char*) &ucha[0], "x");
    }

    // test string extractors
    char s1[10];
    signed char s2[10];
    unsigned char s3[10];

    ins >> s1 >> s2 >> s3;
    CHECK_STR(s1, "s1");
    CHECK_STR((char*) s2, "s2");
    CHECK_STR((char*) s3, "s3");

    // test character extractors
    char ch, sch, uch;

    ins >> STD noskipws >> STD ws >> ch >> sch >> uch >> STD skipws;
    CHECK_INT(ch, 'a');
    CHECK_INT(sch, 'b');
    CHECK_INT(uch, 'c');

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
    char buf[20];
    STD streampos pos = ins.tellg();

    CHECK(pos != (STD streampos)(-1));
    CHECK_PTR(&ins.ignore(), &ins);
    CHECK_INT(ins.peek(), 'i');
    CHECK_PTR(&ins.putback('l'), &ins);
    CHECK_PTR(&ins.read(buf, 4), &ins);
    CHECK_MEM(buf, "line", 3);
    CHECK_PTR(&ins.ignore(20, 'f'), &ins);
    CHECK_INT(ins.gcount(), 3);
    CHECK_INT(ins.readsome(buf, 5), 5);
    CHECK_MEM(buf, " text", 5);
    CHECK_PTR(&ins.unget(), &ins);
    CHECK_INT(ins.peek(), 't');
    CHECK_PTR(&ins.ignore(100), &ins);
    CHECK_INT(ins.get(), EOF);
    CHECK(!ins.good());
    ins.clear();

    CHECK_PTR(&ins.seekg(pos), &ins);
    CHECK_PTR(&ins.getline(buf, sizeof(buf), ' '), &ins);
    CHECK_STR(buf, "line");
    CHECK_INT(ins.gcount(), 5);
    CHECK_PTR(&ins.seekg(0, STD ios_base::cur), &ins);
    CHECK_PTR(&ins.getline(buf, sizeof(buf)), &ins);
    CHECK_STR(buf, "of text");

    // test streambuf extractor and get functions
    char sbuf[20], ubuf[20];
    STD stringbuf strbuf;

    ins >> (STD streambuf*) &strbuf;
    ins.rdbuf(&strbuf);
    ins.get(buf, sizeof(buf), 'o');
    CHECK_STR(buf, "rest ");
    ins.get(sbuf, sizeof(sbuf), 's');
    CHECK_STR(sbuf, "of ");
    ins.get(ubuf, sizeof(ubuf));
    CHECK_STR(ubuf, "stream");
    CHECK(ins.sync() == 0);

    // test template equivalence
    STD basic_istream<char, STD char_traits<char>>* pbi   = (STD istream*) nullptr;
    STD basic_iostream<char, STD char_traits<char>>* pbio = (STD iostream*) nullptr;

    pbi  = pbi; // to quiet diagnostics
    pbio = pbio;

    // test iostream
    STD stringbuf strsb;
    STD iostream iostr(&strsb);
    STD istream* pistr = &iostr;
    STD ostream* postr = &iostr;

    pistr = pistr; // to quiet diagnostics
    postr = postr;

    iostr << "this is a test\n";
    iostr.getline(buf, sizeof(buf));
    CHECK_STR(buf, "this is a test");

    {
        typedef STD stringstream Mycont;
        STD ostream ostr(&strsb);

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
