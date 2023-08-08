// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test "cvt/wbuffer"
#define TEST_NAME "<wbuffer>"

#include "tdefs.h"
#include <cvt/utf8>
#include <cvt/wbuffer>
#include <iostream>
#include <sstream>
#include <string>

typedef stdext::cvt::codecvt_utf8<wchar_t> Mycvt;
typedef stdext::cvt::wbuffer_convert<Mycvt> Mybuffer;

void test_read() { // test wbuffer_convert reads
    STD stringbuf strbuf("Hello world");
    Mybuffer cvtbuf(&strbuf);
    STD wistream source(&cvtbuf);
    STD wstring wans;

    STD getline(source, wans);
    CHECK_WSTR(wans.c_str(), L"Hello world");
    CHECK_PTR(cvtbuf.rdbuf(), &strbuf);
    cvtbuf.rdbuf(STD cin.rdbuf());
    CHECK_PTR(cvtbuf.rdbuf(), STD cin.rdbuf());
}

void test_write() { // test wbuffer_convert writes
    STD stringbuf strbuf;
    Mybuffer cvtbuf(&strbuf);
    STD wostream sink(&cvtbuf);
    STD string ans;

    sink << L"Hello world";
    ans = strbuf.str();
    CHECK_STR(ans.c_str(), "Hello world");
    CHECK_PTR(cvtbuf.rdbuf(), &strbuf);
    cvtbuf.rdbuf(STD cout.rdbuf());
    CHECK_PTR(cvtbuf.rdbuf(), STD cout.rdbuf());
}

void test_main() { // test wbuffer_convert
    STD stringbuf strbuf;
    static CSTD mbstate_t state;

    Mybuffer cvtbuf0;
    Mybuffer cvtbuf1(&strbuf);
    Mybuffer cvtbuf2(&strbuf, new Mycvt);
    Mybuffer cvtbuf3(&strbuf, new Mycvt, state);
    state = cvtbuf0.state();

    test_read();
    test_write();
}
