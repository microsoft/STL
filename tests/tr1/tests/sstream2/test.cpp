// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <sstream>, part 2
#define TEST_NAME "<sstream>, part 2"

#include "tdefs.h"
#include <sstream>
#include <string>
#include <wchar.h>

void t1() { // test wstringbuf
    STD wstring s0(L"s0"), s1(L"s1"), s2(L"s2"), s3(L"s3");
    STD wstringbuf sb0, sb1(STD ios::in), sb2(STD ios::out), sb3(STD ios::in | STD ios::out);
    STD wstringbuf sb10(s0), sb11(s1, STD ios::in), sb12(s2, STD ios::out), sb13(s3, STD ios::in | STD ios::out);
    STD wostream outs(&sb0);

    STD wstringbuf::char_type* pc   = (wchar_t*) nullptr;
    STD wstringbuf::int_type* pi    = (CSTD wint_t*) nullptr;
    STD wstringbuf::pos_type* pp    = (STD wstreampos*) nullptr;
    STD wstringbuf::off_type* po    = (STD streamoff*) nullptr;
    STD wstringbuf::traits_type* pt = (STD char_traits<wchar_t>*) nullptr;

    pc = pc; // to quiet diagnostics
    pi = pi;
    pp = pp;
    po = po;
    pt = pt;

    outs << L"dynamic stringbuf 0";
    CHECK_WSTR(sb0.str().c_str(), L"dynamic stringbuf 0");
    sb0.str(s0);
    CHECK_WSTR(sb0.str().c_str(), L"s0");
    outs.rdbuf(&sb2);
    outs << L"dynamic stringbuf 2";
    CHECK_WSTR(sb2.str().c_str(), L"dynamic stringbuf 2");
    outs.rdbuf(&sb10);
    outs << L"x";
    CHECK_WSTR(sb10.str().c_str(), L"x0");
    outs.rdbuf(&sb11);
    outs << L"x";
    CHECK(!outs.good());
    CHECK_WSTR(sb11.str().c_str(), L"s1");
    outs.rdbuf(&sb12);
    outs << L"x";
    CHECK_WSTR(sb12.str().c_str(), L"x2");
    CHECK_INT((STD streamoff) sb12.pubseekoff(2, STD ios::beg, STD ios::out), 2);
    CHECK_WSTR(sb12.str().c_str(), L"x2");

    {
        typedef STD wstringbuf Mycont;
        Mycont v6(L"xx");
        Mycont v7(STD move(v6));
        CHECK_WSTR(v6.str().c_str(), L"");
        CHECK_WSTR(v7.str().c_str(), L"xx");

        Mycont v8;
        v8 = STD move(v7);
        CHECK_WSTR(v7.str().c_str(), L"");
        CHECK_WSTR(v8.str().c_str(), L"xx");
    }
}

void t2() { // test wistringstream
    STD wstring s0(L"s0"), s1(L"s1"), s2(L"s2"), s3(L"s3");
    STD wistringstream is0, is1(STD ios::in), is2(STD ios::out), is3(STD ios::in | STD ios::out);
    STD wistringstream is10(s0), is11(s1, STD ios::in), is12(s2, STD ios::out), is13(s3, STD ios::in | STD ios::out);

    STD wistringstream::char_type* pc   = (wchar_t*) nullptr;
    STD wistringstream::int_type* pi    = (CSTD wint_t*) nullptr;
    STD wistringstream::pos_type* pp    = (STD wstreampos*) nullptr;
    STD wistringstream::off_type* po    = (STD streamoff*) nullptr;
    STD wistringstream::traits_type* pt = (STD char_traits<wchar_t>*) nullptr;

    pc = pc; // to quiet diagnostics
    pi = pi;
    pp = pp;
    po = po;
    pt = pt;

    CHECK_WSTR(is10.rdbuf()->str().c_str(), L"s0");
    CHECK_WSTR(is11.str().c_str(), L"s1");
    is0.str(L"abc");
    CHECK_WSTR(is0.str().c_str(), L"abc");
    is0 >> s0;
    CHECK_WSTR(s0.c_str(), L"abc");

    {
        typedef STD wistringstream Mycont;
        Mycont v6(L"xx");
        Mycont v7(STD move(v6));
        CHECK_WSTR(v6.str().c_str(), L"");
        CHECK_WSTR(v7.str().c_str(), L"xx");

        Mycont v8;
        v8 = STD move(v7);
        CHECK_WSTR(v7.str().c_str(), L"");
        CHECK_WSTR(v8.str().c_str(), L"xx");
    }
}

void t3() { // test wostringstream
    STD wstring s0(L"s0"), s1(L"s1"), s2(L"s2"), s3(L"s3");
    STD wostringstream os0, os1(STD ios::in), os2(STD ios::out), os3(STD ios::in | STD ios::out);
    STD wostringstream os10(s0), os11(s1, STD ios::in), os12(s2, STD ios::out), os13(s3, STD ios::in | STD ios::out);

    STD wostringstream::char_type* pc   = (wchar_t*) nullptr;
    STD wostringstream::int_type* pi    = (CSTD wint_t*) nullptr;
    STD wostringstream::pos_type* pp    = (STD wstreampos*) nullptr;
    STD wostringstream::off_type* po    = (STD streamoff*) nullptr;
    STD wostringstream::traits_type* pt = (STD char_traits<wchar_t>*) nullptr;

    pc = pc; // to quiet diagnostics
    pi = pi;
    pp = pp;
    po = po;
    pt = pt;

    CHECK_WSTR(os10.rdbuf()->str().c_str(), L"s0");
    CHECK_WSTR(os13.str().c_str(), L"s3");
    os0.str(L"abc");
    CHECK_WSTR(os0.str().c_str(), L"abc");
    CHECK_INT((STD streamoff) os0.rdbuf()->pubseekoff(2, STD ios::beg, STD ios::out), 2);
    CHECK_WSTR(os0.str().c_str(), L"abc");
    os0 << L"Cde";
    CHECK_WSTR(os0.str().c_str(), L"abCde");

    {
        typedef STD wostringstream Mycont;
        Mycont v6(L"xx");
        Mycont v7(STD move(v6));
        CHECK_WSTR(v6.str().c_str(), L"");
        CHECK_WSTR(v7.str().c_str(), L"xx");

        Mycont v8;
        v8 = STD move(v7);
        CHECK_WSTR(v7.str().c_str(), L"");
        CHECK_WSTR(v8.str().c_str(), L"xx");
    }
}

void t4() { // test stringstream and template equivalents
    STD wstring s0(L"s0"), s1(L"s1"), s2(L"s2"), s3(L"s3");
    STD wstringstream ss0, ss1(STD ios::in), ss2(STD ios::out), ss3(STD ios::in | STD ios::out);
    STD wstringstream ss10(s0), ss11(s1, STD ios::in), ss12(s2, STD ios::out), ss13(s3, STD ios::in | STD ios::out);

    STD wstringstream::char_type* pc   = (wchar_t*) nullptr;
    STD wstringstream::int_type* pi    = (CSTD wint_t*) nullptr;
    STD wstringstream::pos_type* pp    = (STD wstreampos*) nullptr;
    STD wstringstream::off_type* po    = (STD streamoff*) nullptr;
    STD wstringstream::traits_type* pt = (STD char_traits<wchar_t>*) nullptr;

    pc = pc; // to quiet diagnostics
    pi = pi;
    pp = pp;
    po = po;
    pt = pt;

    CHECK_WSTR(ss10.rdbuf()->str().c_str(), L"s0");
    CHECK_WSTR(ss11.str().c_str(), L"s1");
    ss0.str(L"abc");
    CHECK_WSTR(ss0.str().c_str(), L"abc");
    ss0 >> s0;
    CHECK_WSTR(s0.c_str(), L"abc");
    ss0.clear();

    CHECK_WSTR(ss10.rdbuf()->str().c_str(), L"s0");
    CHECK_WSTR(ss13.str().c_str(), L"s3");
    ss0.str(L"abc");
    CHECK_WSTR(ss0.str().c_str(), L"abc");
    CHECK_INT((STD streamoff) ss0.rdbuf()->pubseekoff(2, STD ios::beg), 2);
    CHECK_WSTR(ss0.str().c_str(), L"abc");
    ss0 << L"Cde";
    CHECK_WSTR(ss0.str().c_str(), L"abCde");

    {
        typedef STD wstringstream Mycont;
        Mycont v6(L"xx");
        Mycont v7(STD move(v6));
        CHECK_WSTR(v6.str().c_str(), L"");
        CHECK_WSTR(v7.str().c_str(), L"xx");

        Mycont v8;
        v8 = STD move(v7);
        CHECK_WSTR(v7.str().c_str(), L"");
        CHECK_WSTR(v8.str().c_str(), L"xx");
    }

    { // test template equivalents
        STD basic_stringbuf<wchar_t>* psb1                           = (STD wstringbuf*) nullptr;
        STD basic_stringbuf<wchar_t, STD char_traits<wchar_t>>* psb2 = psb1;
        psb2                                                         = psb2;

        STD basic_istringstream<wchar_t>* pis1                           = (STD wistringstream*) nullptr;
        STD basic_istringstream<wchar_t, STD char_traits<wchar_t>>* pis2 = pis1;
        pis2                                                             = pis2;

        STD basic_ostringstream<wchar_t>* pos1                           = (STD wostringstream*) nullptr;
        STD basic_ostringstream<wchar_t, STD char_traits<wchar_t>>* pos2 = pos1;
        pos2                                                             = pos2;

        STD basic_stringstream<wchar_t>* ps1                           = (STD wstringstream*) nullptr;
        STD basic_stringstream<wchar_t, STD char_traits<wchar_t>>* ps2 = ps1;
        ps2                                                            = ps2;
    }
}

void test_main() { // test basic workings of stringstream definitions
    t1();
    t2();
    t3();
    t4();
}
