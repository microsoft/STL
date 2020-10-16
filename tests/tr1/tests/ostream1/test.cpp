// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <ostream>, part 1
#define TEST_NAME "<ostream>, part 1"

#include "tdefs.h"
#include <ostream>
#include <sstream>
#include <string>

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

STD ostream& operator<<(STD ostream& ostr, const Boolx& b) { // insert a Boolx
    STD ios_base::iostate state = STD ios_base::goodbit;
    const STD ostream::sentry ok(ostr);

    if (ok) {
        if (b.value() != 0)
            ostr.rdbuf()->sputc('Y');
        else
            ostr.rdbuf()->sputc('N');
    }
    ostr.setstate(state);
    return ostr;
}

void test_main() { // test basic workings of ostream definitions
    STD ostringstream outs;

    STD ostream::char_type* pc   = (char*) nullptr;
    STD ostream::int_type* pi    = (int*) nullptr;
    STD ostream::pos_type* pp    = (STD streampos*) nullptr;
    STD ostream::off_type* po    = (STD streamoff*) nullptr;
    STD ostream::traits_type* pt = (STD char_traits<char>*) nullptr;

    pc = pc; // to quiet diagnostics
    pi = pi;
    pp = pp;
    po = po;
    pt = pt;

    CHECK(outs.good());
    CHECK_INT((outs.flags() & STD ios::showbase), 0);
    outs << STD noskipws;
    CHECK_INT(outs.flags() & (STD ios::dec | STD ios::skipws), STD ios::dec);

    {
        typedef STD ostringstream Mycont;
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

        sbuf.str("");
        STD ostream(&sbuf) << (char) 'x';
        CHECK_STR(sbuf.str().c_str(), "x");

        sbuf.str("");
        STD ostream(&sbuf) << (signed char) 'x';
        CHECK_STR(sbuf.str().c_str(), "x");

        sbuf.str("");
        STD ostream(&sbuf) << (unsigned char) 'x';
        CHECK_STR(sbuf.str().c_str(), "x");

        sbuf.str("");
        STD ostream(&sbuf) << (const char*) "x";
        CHECK_STR(sbuf.str().c_str(), "x");

        sbuf.str("");
        STD ostream(&sbuf) << (const signed char*) "x";
        CHECK_STR(sbuf.str().c_str(), "x");

        sbuf.str("");
        STD ostream(&sbuf) << (const unsigned char*) "x";
        CHECK_STR(sbuf.str().c_str(), "x");
    }

    // test character inserters
    outs.str("");
    outs << "s1" << '+' << (const signed char*) "s2" << (signed char) '+' << (const unsigned char*) "s3"
         << (unsigned char) '\n';
    CHECK_STR(outs.str().c_str(), "s1+s2+s3\n");
    outs.str("");
    outs.fill('*');
    outs.width(4);
    outs << "s2";
    CHECK_STR(outs.str().c_str(), "**s2");

    outs.str("");
    outs.fill('@');
    outs.width(3);
    outs << STD left << "s3";
    CHECK_STR(outs.str().c_str(), "s3@");
    outs.str("");
    outs.fill('~');
    outs.width(3);
    outs << STD right << "s4";
    CHECK_STR(outs.str().c_str(), "~s4");
    outs.str("");
    outs.fill('?');
    outs.width(4);
    outs << STD internal << "s5";
    outs << STD endl << STD flush;
    CHECK_STR(outs.str().c_str(), "??s5\n");

    // test integer inserters
    outs.str("");
    outs << (short) 1;
    CHECK_STR(outs.str().c_str(), "1");
    outs.str("");
    outs << (unsigned short) 23;
    CHECK_STR(outs.str().c_str(), "23");
    outs.str("");
    outs << (int) -456;
    CHECK_STR(outs.str().c_str(), "-456");
    outs.str("");
    outs << STD showpos << (int) 7890;
    CHECK_STR(outs.str().c_str(), "+7890");
    outs.str("");
    outs << (unsigned int) 3 << STD noshowpos;
    CHECK_STR(outs.str().c_str(), "3");

    outs.str("");
    outs << STD oct << (long) 10;
    CHECK_STR(outs.str().c_str(), "12");
    outs.str("");
    outs.fill('?'), outs.width(5);
    outs << STD showbase << (long) 8;
    CHECK_STR(outs.str().c_str(), "??010");
    outs.str("");
    outs.width(6);
    outs << STD hex << (unsigned long) 0x3e;
    CHECK_STR(outs.str().c_str(), "0x??3e");
    outs.str("");
    outs << STD uppercase << (unsigned long) 0xab;
    CHECK_STR(outs.str().c_str(), "0XAB");
    outs.str("");
    outs << (unsigned long) 0;
    CHECK_STR(outs.str().c_str(), "0");
    outs.str("");
    outs << (void*) nullptr;
    CHECK(outs.str().size() != 0);

    // test long long inserters
    outs.str("");
    outs << STD dec << (long long) (-12);
    CHECK_STR(outs.str().c_str(), "-12");
    outs.str("");
    outs << (unsigned long long) 34;
    CHECK_STR(outs.str().c_str(), "34");

    // test floating-point inserters
    outs.str("");
    outs << STD showpos << (float) 12 << STD noshowpos;
    CHECK_STR(outs.str().c_str(), "+12");
    outs.str("");
    outs << STD showpoint << (double) 14 << STD noshowpoint;
    CHECK_STR(outs.str().c_str(), "14.0000");
    outs.str("");
    outs.precision(3);
    outs.width(11);
    outs.fill('?');
    outs << (long double) -168e-7;

    CHECK_STR(outs.str().c_str(), "-??1.68E-05");

    outs.str("");
    outs << STD fixed << 1.0;
    CHECK_STR(outs.str().c_str(), "1.000");
    outs.str("");
    outs << STD scientific << 2.0;

    CHECK_STR(outs.str().c_str(), "2.000E+00");

    {
        outs.str("");
        outs.unsetf(STD ios_base::uppercase);
        outs << STD hexfloat << 2.0;
        STD string ans  = outs.str();
        const char* buf = ans.c_str();
        CHECK_STR(buf, "0x1.0000000000000p+1");
    }

    outs.precision(0);

    outs.str("");
    outs << STD defaultfloat << 1.5;
    CHECK_STR(outs.str().c_str(), "2");

    outs.str("");
    outs << STD fixed << 1.0;
    CHECK_STR(outs.str().c_str(), "1");

    outs.str("");
    outs << STD scientific << 2.0;
    CHECK_STR(outs.str().c_str(), "2e+00");

    outs.str("");
    outs << STD hexfloat << 2.0;
    CHECK_STR(outs.str().c_str(), "0x1.0000000000000p+1");

    outs.precision(-1);

    outs.str("");
    outs << STD defaultfloat << 1.5;
    CHECK_STR(outs.str().c_str(), "1.5");

    outs.str("");
    outs << STD fixed << 1.0;
    CHECK_STR(outs.str().c_str(), "1.000000");

    outs.str("");
    outs << STD scientific << 2.0;
    CHECK_STR(outs.str().c_str(), "2.000000e+00");

    outs.str("");
    outs << STD hexfloat << 2.0;
    CHECK_STR(outs.str().c_str(), "0x1.0000000000000p+1");

    outs.precision(-49);
    outs.str("");
    outs << STD fixed << 1.0;
    CHECK_STR(outs.str().c_str(), "1.000000");

    outs.precision(3);

    outs.str("");
    outs << STD defaultfloat << 1.5L;
    CHECK_STR(outs.str().c_str(), "1.5");

    outs.str("");
    outs << STD fixed << 1.0L;
    CHECK_STR(outs.str().c_str(), "1.000");

    outs.str("");
    outs << STD scientific << 2.0L;
    CHECK_STR(outs.str().c_str(), "2.000e+00");

    outs.str("");
    outs << STD hexfloat << 2.0L;
    CHECK_STR(outs.str().c_str(), "0x1.0000000000000p+1");

    outs.precision(0);

    outs.str("");
    outs << STD defaultfloat << 1.5L;
    CHECK_STR(outs.str().c_str(), "2");

    outs.str("");
    outs << STD fixed << 1.0L;
    CHECK_STR(outs.str().c_str(), "1");

    outs.str("");
    outs << STD scientific << 2.0L;
    CHECK_STR(outs.str().c_str(), "2e+00");

    outs.str("");
    outs << STD hexfloat << 2.0L;
    CHECK_STR(outs.str().c_str(), "0x1.0000000000000p+1");

    outs.precision(-1);

    outs.str("");
    outs << STD defaultfloat << 1.5L;
    CHECK_STR(outs.str().c_str(), "1.5");

    outs.str("");
    outs << STD fixed << 1.0L;
    CHECK_STR(outs.str().c_str(), "1.000000");

    outs.str("");
    outs << STD scientific << 2.0L;
    CHECK_STR(outs.str().c_str(), "2.000000e+00");

    outs.str("");
    outs << STD hexfloat << 2.0L;
    CHECK_STR(outs.str().c_str(), "0x1.0000000000000p+1");

    outs.precision(-49);
    outs.str("");
    outs << STD fixed << 1.0L;
    CHECK_STR(outs.str().c_str(), "1.000000");

    // test Boolx inserter
    const Boolx no(0), yes(1);
    outs.str("");
    outs << ' ' << no << ' ' << yes;
    CHECK_STR(outs.str().c_str(), " N Y");

    // test bool inserter
    outs.str("");
    outs << STD boolalpha << false << ' ' << true;
    CHECK_STR(outs.str().c_str(), "false true");
    outs.str("");
    outs << STD noboolalpha << STD dec;
    outs << false << true;
    CHECK_STR(outs.str().c_str(), "01");

    // test streambuf inserter, positioning, and puts
    STD istringstream istr("rest of stream\n");

    outs.str("");
    outs << "some stuff ";
    STD streampos pos = outs.tellp();
    CHECK(pos != (STD streampos)(-1));
    outs << istr.rdbuf();
    CHECK_STR(outs.str().c_str(), "some stuff rest of stream\n");
    CHECK_PTR(&outs.seekp(pos), &outs);
    outs.put('b');
    CHECK_PTR(&outs.seekp(0, STD ios_base::cur), &outs);
    CHECK_STR(outs.str().c_str(), "some stuff best of stream\n");
    outs.str("");
    outs.flush().write("s1", 2);
    CHECK_STR(outs.str().c_str(), "s1");

    // test template equivalence
    STD basic_ostream<char, STD char_traits<char>>* pbo = (STD ostream*) nullptr;

    pbo = pbo; // to quiet diagnostics
}
