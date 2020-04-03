// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <sstream>, part 1
#define TEST_NAME "<sstream>, part 1"

#include "tdefs.h"
#include <sstream>
#include <string>

void t1() { // test stringbuf
    STD string s0("s0"), s1("s1"), s2("s2"), s3("s3");
    STD stringbuf sb0, sb1(STD ios::in), sb2(STD ios::out), sb3(STD ios::in | STD ios::out);
    STD stringbuf sb10(s0), sb11(s1, STD ios::in), sb12(s2, STD ios::out), sb13(s3, STD ios::in | STD ios::out);
    STD ostream outs(&sb0);

    STD stringbuf::char_type* pc   = (char*) nullptr;
    STD stringbuf::int_type* pi    = (int*) nullptr;
    STD stringbuf::pos_type* pp    = (STD streampos*) nullptr;
    STD stringbuf::off_type* po    = (STD streamoff*) nullptr;
    STD stringbuf::traits_type* pt = (STD char_traits<char>*) nullptr;

    pc = pc; // to quiet diagnostics
    pi = pi;
    pp = pp;
    po = po;
    pt = pt;

    outs << "dynamic stringbuf 0";
    CHECK_STR(sb0.str().c_str(), "dynamic stringbuf 0");
    sb0.str(s0);
    CHECK_STR(sb0.str().c_str(), "s0");
    outs.rdbuf(&sb2);
    outs << "dynamic stringbuf 2";
    CHECK_STR(sb2.str().c_str(), "dynamic stringbuf 2");
    outs.rdbuf(&sb10);
    outs << "x";
    CHECK_STR(sb10.str().c_str(), "x0");
    outs.rdbuf(&sb11);
    outs << "x";
    CHECK(!outs.good());
    CHECK_STR(sb11.str().c_str(), "s1");
    outs.rdbuf(&sb12);
    outs << "x";
    CHECK_STR(sb12.str().c_str(), "x2");
    CHECK_INT((STD streamoff) sb12.pubseekoff(2, STD ios::beg, STD ios::out), 2);
    CHECK_STR(sb12.str().c_str(), "x2");

    {
        typedef STD stringbuf Mycont;
        Mycont v6("xx");
        Mycont v7(STD move(v6));
        CHECK_STR(v6.str().c_str(), "");
        CHECK_STR(v7.str().c_str(), "xx");

        Mycont v8;
        v8 = STD move(v7);
        CHECK_STR(v7.str().c_str(), "");
        CHECK_STR(v8.str().c_str(), "xx");
    }
}

void t2() { // test istringstream
    STD string s0("s0"), s1("s1"), s2("s2"), s3("s3");
    STD istringstream is0, is1(STD ios::in), is2(STD ios::out), is3(STD ios::in | STD ios::out);
    STD istringstream is10(s0), is11(s1, STD ios::in), is12(s2, STD ios::out), is13(s3, STD ios::in | STD ios::out);

    STD istringstream::char_type* pc   = (char*) nullptr;
    STD istringstream::int_type* pi    = (int*) nullptr;
    STD istringstream::pos_type* pp    = (STD streampos*) nullptr;
    STD istringstream::off_type* po    = (STD streamoff*) nullptr;
    STD istringstream::traits_type* pt = (STD char_traits<char>*) nullptr;

    pc = pc; // to quiet diagnostics
    pi = pi;
    pp = pp;
    po = po;
    pt = pt;

    CHECK_STR(is10.rdbuf()->str().c_str(), "s0");
    CHECK_STR(is11.str().c_str(), "s1");
    is0.str("abc");
    CHECK_STR(is0.str().c_str(), "abc");
    is0 >> s0;
    CHECK_STR(s0.c_str(), "abc");

    {
        typedef STD istringstream Mycont;
        Mycont v6("xx");
        Mycont v7(STD move(v6));
        CHECK_STR(v6.str().c_str(), "");
        CHECK_STR(v7.str().c_str(), "xx");

        Mycont v8;
        v8 = STD move(v7);
        CHECK_STR(v7.str().c_str(), "");
        CHECK_STR(v8.str().c_str(), "xx");
    }
}

void t3() { // test ostringstream
    STD string s0("s0"), s1("s1"), s2("s2"), s3("s3");
    STD ostringstream os0, os1(STD ios::in), os2(STD ios::out), os3(STD ios::in | STD ios::out);
    STD ostringstream os10(s0), os11(s1, STD ios::in), os12(s2, STD ios::out), os13(s3, STD ios::in | STD ios::out);

    STD ostringstream::char_type* pc   = (char*) nullptr;
    STD ostringstream::int_type* pi    = (int*) nullptr;
    STD ostringstream::pos_type* pp    = (STD streampos*) nullptr;
    STD ostringstream::off_type* po    = (STD streamoff*) nullptr;
    STD ostringstream::traits_type* pt = (STD char_traits<char>*) nullptr;

    pc = pc; // to quiet diagnostics
    pi = pi;
    pp = pp;
    po = po;
    pt = pt;

    CHECK_STR(os10.rdbuf()->str().c_str(), "s0");
    CHECK_STR(os13.str().c_str(), "s3");
    os0.str("abc");
    CHECK_STR(os0.str().c_str(), "abc");
    CHECK_INT((STD streamoff) os0.rdbuf()->pubseekoff(2, STD ios::beg, STD ios::out), 2);
    CHECK_STR(os0.str().c_str(), "abc");
    os0 << "Cde";
    CHECK_STR(os0.str().c_str(), "abCde");

    {
        typedef STD ostringstream Mycont;
        Mycont v6("xx");
        Mycont v7(STD move(v6));
        CHECK_STR(v6.str().c_str(), "");
        CHECK_STR(v7.str().c_str(), "xx");

        Mycont v8;
        v8 = STD move(v7);
        CHECK_STR(v7.str().c_str(), "");
        CHECK_STR(v8.str().c_str(), "xx");
    }
}

void t4() { // test stringstream and template equivalents
    STD string s0("s0"), s1("s1"), s2("s2"), s3("s3");
    STD stringstream ss0, ss1(STD ios::in), ss2(STD ios::out), ss3(STD ios::in | STD ios::out);
    STD stringstream ss10(s0), ss11(s1, STD ios::in), ss12(s2, STD ios::out), ss13(s3, STD ios::in | STD ios::out);

    STD stringstream::char_type* pc   = (char*) nullptr;
    STD stringstream::int_type* pi    = (int*) nullptr;
    STD stringstream::pos_type* pp    = (STD streampos*) nullptr;
    STD stringstream::off_type* po    = (STD streamoff*) nullptr;
    STD stringstream::traits_type* pt = (STD char_traits<char>*) nullptr;

    pc = pc; // to quiet diagnostics
    pi = pi;
    pp = pp;
    po = po;
    pt = pt;

    CHECK_STR(ss10.rdbuf()->str().c_str(), "s0");
    CHECK_STR(ss11.str().c_str(), "s1");
    ss0.str("abc");
    CHECK_STR(ss0.str().c_str(), "abc");
    ss0 >> s0;
    CHECK_STR(s0.c_str(), "abc");
    ss0.clear();

    CHECK_STR(ss10.rdbuf()->str().c_str(), "s0");
    CHECK_STR(ss13.str().c_str(), "s3");
    ss0.str("abc");
    CHECK_STR(ss0.str().c_str(), "abc");
    CHECK_INT((STD streamoff) ss0.rdbuf()->pubseekoff(2, STD ios::beg), 2);
    CHECK_STR(ss0.str().c_str(), "abc");
    ss0 << "Cde";
    CHECK_STR(ss0.str().c_str(), "abCde");

    {
        typedef STD stringstream Mycont;
        Mycont v6("xx");
        Mycont v7(STD move(v6));
        CHECK_STR(v6.str().c_str(), "");
        CHECK_STR(v7.str().c_str(), "xx");

        Mycont v8;
        v8 = STD move(v7);
        CHECK_STR(v7.str().c_str(), "");
        CHECK_STR(v8.str().c_str(), "xx");
    }

    { // test template equivalents
        STD basic_stringbuf<char>* psb1                        = (STD stringbuf*) nullptr;
        STD basic_stringbuf<char, STD char_traits<char>>* psb2 = psb1;
        psb2                                                   = psb2;

        STD basic_istringstream<char>* pis1                        = (STD istringstream*) nullptr;
        STD basic_istringstream<char, STD char_traits<char>>* pis2 = pis1;
        pis2                                                       = pis2;

        STD basic_ostringstream<char>* pos1                        = (STD ostringstream*) nullptr;
        STD basic_ostringstream<char, STD char_traits<char>>* pos2 = pos1;
        pos2                                                       = pos2;

        STD basic_stringstream<char>* ps1                        = (STD stringstream*) nullptr;
        STD basic_stringstream<char, STD char_traits<char>>* ps2 = ps1;
        ps2                                                      = ps2;
    }
}

void test_main() { // test basic workings of stringstream definitions
    t1();
    t2();
    t3();
    t4();
}
