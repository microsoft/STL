// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <iomanip>
#define TEST_NAME "<iomanip>"

#include "tdefs.h"
#include <iomanip>
#include <iostream>
#include <sstream>

struct Smanip { // store function pointer and argument value
    Smanip(void (*left)(STD ios_base&, char), char val)
        : pfun(left), manarg(val) { // construct from function pointer and argument value
    }

    void (*pfun)(STD ios_base&, char); // the function pointer
    char manarg; // the argument value
};

STD istream& operator>>(
    STD istream& istr, const Smanip& manip) { // extract by calling function with input stream and argument
    (*manip.pfun)(istr, manip.manarg);
    return istr;
}

STD ostream& operator<<(
    STD ostream& ostr, const Smanip& manip) { // insert by calling function with output stream and argument
    (*manip.pfun)(ostr, manip.manarg);
    return ostr;
}

static void fltfun(STD ios_base& iostr, char code) { // set fixed/scientific and uppercase flags
    STD ios_base::fmtflags fl;

    switch (code) {
    case 'f':
        fl = STD ios_base::fixed;
        break;
    case 'e':
        fl = STD ios_base::scientific;
        break;
    case 'E':
        fl = STD ios_base::scientific | STD ios_base::uppercase;
        break;
    case 'G':
        fl = STD ios_base::uppercase;
        break;
    default:
        fl = (STD ios_base::fmtflags) 0;
        break;
    }

    iostr.setf(fl, STD ios_base::floatfield | STD ios_base::uppercase);
}

Smanip fpfmt(char code) { // inserter to set floating-point flags
    return Smanip(&fltfun, code);
}

static void intfun(STD ios_base& iostr, char code) { // set dec/hex/oct and uppercase flags
    STD ios_base::fmtflags fl;

    switch (code) {
    case 'd':
        fl = STD ios_base::dec;
        break;
    case 'o':
        fl = STD ios_base::oct;
        break;
    case 'x':
        fl = STD ios_base::hex;
        break;
    case 'X':
        fl = STD ios_base::hex | STD ios_base::uppercase;
        break;
    default:
        fl = (STD ios_base::fmtflags) 0;
        break;
    }

    iostr.setf(fl, STD ios_base::basefield | STD ios_base::uppercase);
}

Smanip intfmt(char code) { // extractor/inserter to set integer flags
    return Smanip(&intfun, code);
}

void test_quoted(const char* in, const char* escaped, char delim = '"', char escape = '\\') { // test quoted
    STD stringstream sbuf;
    sbuf << STD quoted(in, delim, escape);
    CHECK(sbuf.good());
    CHECK_STR(sbuf.str().c_str(), escaped);

    STD string str;
    sbuf >> STD quoted(str, delim, escape);
    CHECK(sbuf.good());
    CHECK_STR(str.c_str(), in);
}

void test_main() { // test basic workings of iomanip definitions
    STD cin.flags((STD ios_base::fmtflags) 0);
    CHECK_INT(STD cin.flags(), (STD ios_base::fmtflags) 0);
    STD cin >> STD setiosflags(STD ios_base::left | STD ios_base::showpoint);
    CHECK_INT(STD cin.flags(), (STD ios_base::left | STD ios_base::showpoint));
    STD cin >> STD resetiosflags(STD ios_base::showpoint);
    CHECK_INT(STD cin.flags(), STD ios_base::left);

    STD cin >> STD setbase(8);
    CHECK_INT(STD cin.flags(), STD ios_base::left | STD ios_base::oct);
    STD cin >> STD setbase(10);
    CHECK_INT(STD cin.flags(), STD ios_base::left | STD ios_base::dec);
    STD cin >> STD setbase(16);
    CHECK_INT(STD cin.flags(), STD ios_base::left | STD ios_base::hex);
    STD cin >> STD setbase(-23);
    CHECK_INT(STD cin.flags(), STD ios_base::left);

    CHECK_INT(STD wcout.fill(), L' ');
    STD wcout << STD setfill(L'*');
    CHECK_INT(STD wcout.fill(), L'*');

    CHECK_INT(STD cout.fill(), ' ');
    STD cout << STD setfill('*');
    CHECK_INT(STD cout.fill(), '*');
    CHECK_INT(STD cout.precision(), 6);
    STD cout << STD setprecision(17);
    CHECK_INT(STD cout.precision(), 17);
    CHECK_INT(STD cout.width(), 0);
    STD cout << STD setw(-60);
    CHECK_INT(STD cout.width(), -60);

    // test user-defined instantiations
    STD cout << STD resetiosflags((STD ios_base::fmtflags) ~0) << fpfmt('E');
    CHECK_INT(STD cout.flags(), STD ios_base::scientific | STD ios_base::uppercase);
    STD cout << intfmt('x');
    CHECK_INT(STD cout.flags(), STD ios_base::hex | STD ios_base::scientific);
    STD cin >> STD resetiosflags((STD ios_base::fmtflags) ~0) >> intfmt('X');
    CHECK_INT(STD cin.flags(), STD ios_base::hex | STD ios_base::uppercase);

    { // test money_get/put
        STD stringstream sbuf;
        sbuf << STD put_money(STD string("123"));
        CHECK_STR(sbuf.str().c_str(), "123");

        STD string ans;
        sbuf >> STD get_money(ans);
        CHECK_STR(ans.c_str(), "123");
    }

    { // test time_get/put
        STD stringstream sbuf;
        CSTD tm tbuf{};
        tbuf.tm_mon = 2;
        sbuf << STD put_time(&tbuf, "%B");
        CHECK_STR(sbuf.str().c_str(), "March");

        tbuf.tm_mon = 0;
        sbuf >> STD get_time(&tbuf, "%B");
        CHECK_INT(tbuf.tm_mon, 2);
    }

    { // test quoted
        test_quoted("abc", "\"abc\"");
        test_quoted("ab\"c", "\"ab\\\"c\"");
        test_quoted("a\"b\"c", "\"a\\\"b\\\"c\"");

        test_quoted("abc", "|abc|", '|');
        test_quoted("ab|c", "|ab@|c|", '|', '@');
    }
}
