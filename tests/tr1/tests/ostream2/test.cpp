// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <ostream>, part 2
#define TEST_NAME "<ostream>, part 2"

#include "tdefs.h"
#include <ostream>
#include <sstream>
#include <string>
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

STD wostream& operator<<(STD wostream& ostr, const Boolx& b) { // insert a Boolx
    STD ios_base::iostate state = STD ios_base::goodbit;
    const STD wostream::sentry ok(ostr);

    if (ok) {
        if (b.value() != 0) {
            ostr.rdbuf()->sputc(L'Y');
        } else {
            ostr.rdbuf()->sputc(L'N');
        }
    }
    ostr.setstate(state);
    return ostr;
}

void test_main() { // test basic workings of ostream definitions
    STD wostringstream outs;

    STD wostream::char_type* pc   = (wchar_t*) nullptr;
    STD wostream::int_type* pi    = (wint_t*) nullptr;
    STD wostream::pos_type* pp    = (STD streampos*) nullptr;
    STD wostream::off_type* po    = (STD streamoff*) nullptr;
    STD wostream::traits_type* pt = (STD char_traits<wchar_t>*) nullptr;

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
        typedef STD wostringstream Mycont;
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

        sbuf.str(L"");
        STD wostream(&sbuf) << (wchar_t) L'x';
        CHECK_WSTR(sbuf.str().c_str(), L"x");

        sbuf.str(L"");
        STD wostream(&sbuf) << (char) 'x';
        CHECK_WSTR(sbuf.str().c_str(), L"x");

        sbuf.str(L"");
        STD wostream(&sbuf) << (const wchar_t*) L"x";
        CHECK_WSTR(sbuf.str().c_str(), L"x");

        sbuf.str(L"");
        STD wostream(&sbuf) << (const char*) "x";
        CHECK_WSTR(sbuf.str().c_str(), L"x");
    }

    // test character inserters
    outs.str(L"");
    outs << L"s1" << L'+' << "s2" << '\n';
    CHECK_WSTR(outs.str().c_str(), L"s1+s2\n");

    outs.str(L"");
    outs.fill(L'*');
    outs.width(4);
    outs << L"s2";
    CHECK_WSTR(outs.str().c_str(), L"**s2");

    outs.str(L"");
    outs.fill(L'@');
    outs.width(3);
    outs << STD left << L"s3";
    CHECK_WSTR(outs.str().c_str(), L"s3@");
    outs.str(L"");
    outs.fill(L'~');
    outs.width(3);
    outs << STD right << L"s4";
    CHECK_WSTR(outs.str().c_str(), L"~s4");
    outs.str(L"");
    outs.fill(L'?');
    outs.width(4);
    outs << STD internal << L"s5";
    outs << STD endl << STD flush;
    CHECK_WSTR(outs.str().c_str(), L"??s5\n");

    // test integer inserters
    outs.str(L"");
    outs << (short) 1;
    CHECK_WSTR(outs.str().c_str(), L"1");

    outs.str(L"");
    outs << (unsigned short) 23;
    CHECK_WSTR(outs.str().c_str(), L"23");

    outs.str(L"");
    outs << (int) -456;
    CHECK_WSTR(outs.str().c_str(), L"-456");
    outs.str(L"");
    outs << STD showpos << (int) 7890;
    CHECK_WSTR(outs.str().c_str(), L"+7890");
    outs.str(L"");
    outs << (unsigned int) 3 << STD noshowpos;
    CHECK_WSTR(outs.str().c_str(), L"3");

    outs.str(L"");
    outs << STD oct << (long) 10;
    CHECK_WSTR(outs.str().c_str(), L"12");
    outs.str(L"");
    outs.fill(L'?');
    outs.width(5);
    outs << STD showbase << (long) 8;
    CHECK_WSTR(outs.str().c_str(), L"??010");
    outs.str(L"");
    outs.width(6);
    outs << STD hex << (unsigned long) 0x3e;
    CHECK_WSTR(outs.str().c_str(), L"0x??3e");
    outs.str(L"");
    outs << STD uppercase << (unsigned long) 0xab;
    CHECK_WSTR(outs.str().c_str(), L"0XAB");
    outs.str(L"");
    outs << (unsigned long) 0;
    CHECK_WSTR(outs.str().c_str(), L"0");
    outs.str(L"");
    outs << (void*) nullptr;
    CHECK(outs.str().size() != 0);

    // test long long inserters
    outs.str(L"");
    outs << STD dec << (long long) (-12);
    CHECK_WSTR(outs.str().c_str(), L"-12");
    outs.str(L"");
    outs << (unsigned long long) 34;
    CHECK_WSTR(outs.str().c_str(), L"34");

    // test floating-point inserters
    outs.str(L"");
    outs << STD showpos << (float) 12 << STD noshowpos;
    CHECK_WSTR(outs.str().c_str(), L"+12");
    outs.str(L"");
    outs << STD showpoint << (double) 14 << STD noshowpoint;
    CHECK_WSTR(outs.str().c_str(), L"14.0000");
    outs.str(L"");
    outs.precision(3);
    outs.width(11);
    outs.fill(L'?');
    outs << (long double) -168e-7;

    CHECK_WSTR(outs.str().c_str(), L"-??1.68E-05");

    outs.str(L"");
    outs << STD fixed << 1.0;
    CHECK_WSTR(outs.str().c_str(), L"1.000");
    outs.str(L"");
    outs << STD scientific << 2.0;

    CHECK_WSTR(outs.str().c_str(), L"2.000E+00");

    outs.str(L"");
    outs.unsetf(STD ios_base::uppercase);
    outs << STD hexfloat << 2.0;
    STD wstring ans    = outs.str();
    const wchar_t* buf = ans.c_str();
    CHECK_WSTR(buf, L"0x1.0000000000000p+1");

    outs.precision(0);

    outs.str(L"");
    outs << STD defaultfloat << 1.5;
    CHECK_WSTR(outs.str().c_str(), L"2");

    outs.str(L"");
    outs << STD fixed << 1.0;
    CHECK_WSTR(outs.str().c_str(), L"1");

    outs.str(L"");
    outs << STD scientific << 2.0;
    CHECK_WSTR(outs.str().c_str(), L"2e+00");

    outs.str(L"");
    outs << STD hexfloat << 2.0;
    CHECK_WSTR(outs.str().c_str(), L"0x1.0000000000000p+1");

    outs.precision(-1);

    outs.str(L"");
    outs << STD defaultfloat << 1.5;
    CHECK_WSTR(outs.str().c_str(), L"1.5");

    outs.str(L"");
    outs << STD fixed << 1.0;
    CHECK_WSTR(outs.str().c_str(), L"1.000000");

    outs.str(L"");
    outs << STD scientific << 2.0;
    CHECK_WSTR(outs.str().c_str(), L"2.000000e+00");

    outs.str(L"");
    outs << STD hexfloat << 2.0;
    CHECK_WSTR(outs.str().c_str(), L"0x1.0000000000000p+1");

    outs.precision(-49);
    outs.str(L"");
    outs << STD fixed << 1.0;
    CHECK_WSTR(outs.str().c_str(), L"1.000000");

    outs.precision(3);

    outs.str(L"");
    outs << STD defaultfloat << 1.5L;
    CHECK_WSTR(outs.str().c_str(), L"1.5");

    outs.str(L"");
    outs << STD fixed << 1.0L;
    CHECK_WSTR(outs.str().c_str(), L"1.000");

    outs.str(L"");
    outs << STD scientific << 2.0L;
    CHECK_WSTR(outs.str().c_str(), L"2.000e+00");

    outs.str(L"");
    outs << STD hexfloat << 2.0L;
    CHECK_WSTR(outs.str().c_str(), L"0x1.0000000000000p+1");

    outs.precision(0);

    outs.str(L"");
    outs << STD defaultfloat << 1.5L;
    CHECK_WSTR(outs.str().c_str(), L"2");

    outs.str(L"");
    outs << STD fixed << 1.0L;
    CHECK_WSTR(outs.str().c_str(), L"1");

    outs.str(L"");
    outs << STD scientific << 2.0L;
    CHECK_WSTR(outs.str().c_str(), L"2e+00");

    outs.str(L"");
    outs << STD hexfloat << 2.0L;
    CHECK_WSTR(outs.str().c_str(), L"0x1.0000000000000p+1");

    outs.precision(-1);

    outs.str(L"");
    outs << STD defaultfloat << 1.5L;
    CHECK_WSTR(outs.str().c_str(), L"1.5");

    outs.str(L"");
    outs << STD fixed << 1.0L;
    CHECK_WSTR(outs.str().c_str(), L"1.000000");

    outs.str(L"");
    outs << STD scientific << 2.0L;
    CHECK_WSTR(outs.str().c_str(), L"2.000000e+00");

    outs.str(L"");
    outs << STD hexfloat << 2.0L;
    CHECK_WSTR(outs.str().c_str(), L"0x1.0000000000000p+1");

    outs.precision(-49);
    outs.str(L"");
    outs << STD fixed << 1.0L;
    CHECK_WSTR(outs.str().c_str(), L"1.000000");

    // test Boolx inserter
    const Boolx no(0), yes(1);
    outs.str(L"");
    outs << L" " << no << L" " << yes;
    CHECK_WSTR(outs.str().c_str(), L" N Y");

    // test bool inserter
    outs.str(L"");
    outs << STD boolalpha << false << L" " << true;
    CHECK_WSTR(outs.str().c_str(), L"false true");
    outs.str(L"");
    outs << STD noboolalpha << STD dec;
    outs << false << true;
    CHECK_WSTR(outs.str().c_str(), L"01");

    // test streambuf inserter, positioning, and puts
    STD wistringstream istr(L"rest of stream\n");

    outs.str(L"");
    outs << L"some stuff ";
    STD wstreampos pos = outs.tellp();
    CHECK(pos != (STD wstreampos)(-1));
    outs << istr.rdbuf();
    CHECK_WSTR(outs.str().c_str(), L"some stuff rest of stream\n");
    CHECK_PTR(&outs.seekp(pos), &outs);
    outs.put(L'b');
    CHECK_PTR(&outs.seekp(0, STD ios_base::cur), &outs);
    CHECK_WSTR(outs.str().c_str(), L"some stuff best of stream\n");
    outs.str(L"");
    outs.flush().write(L"s1", 2);
    CHECK_WSTR(outs.str().c_str(), L"s1");

    // test template equivalence
    STD basic_ostream<wchar_t, STD char_traits<wchar_t>>* pbo = (STD wostream*) nullptr;

    pbo = pbo; // to quiet diagnostics
}
