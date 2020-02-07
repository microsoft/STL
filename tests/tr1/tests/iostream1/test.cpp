// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <iostream>
#define TEST_NAME "<iostream>"

#include "tdefs.h"
#include <iostream>

void test_main() { // test basic workings of iostream definitions
    CHECK(STD cin.good());
    CHECK_INT(STD cin.exceptions(), STD ios::goodbit);
    CHECK_INT(STD cin.flags(), STD ios::skipws | STD ios::dec);
    CHECK_INT(STD cin.precision(), 6);
    CHECK_INT(STD cin.width(), 0);
    CHECK_INT(STD cin.fill(), ' ');

    CHECK(STD cout.good());
    CHECK_INT(STD cout.exceptions(), STD ios::goodbit);
    CHECK_INT(STD cout.flags(), STD ios::skipws | STD ios::dec);
    CHECK_INT(STD cout.precision(), 6);
    CHECK_INT(STD cout.width(), 0);
    CHECK_INT(STD cout.fill(), ' ');

    CHECK(STD cerr.good());
    CHECK_INT(STD cerr.exceptions(), STD ios::goodbit);
    CHECK_INT(STD cerr.flags(), STD ios::skipws | STD ios::dec | STD ios::unitbuf);
    CHECK_INT(STD cerr.precision(), 6);
    CHECK_INT(STD cerr.width(), 0);
    CHECK_INT(STD cerr.fill(), ' ');

    CHECK(STD clog.good());
    CHECK_INT(STD clog.exceptions(), STD ios::goodbit);
    CHECK_INT(STD clog.flags(), STD ios::skipws | STD ios::dec);
    CHECK_INT(STD clog.precision(), 6);
    CHECK_INT(STD clog.width(), 0);
    CHECK_INT(STD clog.fill(), ' ');

    if (!terse) { // not terse, try to write on output streams
        STD cout << "Can write on streams:" << STD endl;
        STD cout << "\tcout" << STD endl;
        STD cerr << "\tcerr" << STD endl;
        STD clog << "\tclog" << STD endl;

        CHECK(STD cout.good());
        CHECK(STD cerr.good());
        CHECK(STD clog.good());
    }

    CHECK(STD wcin.good());
    CHECK_INT(STD wcin.exceptions(), STD ios::goodbit);
    CHECK_INT(STD wcin.flags(), STD ios::skipws | STD ios::dec);
    CHECK_INT(STD wcin.precision(), 6);
    CHECK_INT(STD wcin.width(), 0);
    CHECK_INT(STD wcin.fill(), ' ');

    CHECK(STD wcout.good());
    CHECK_INT(STD wcout.exceptions(), STD ios::goodbit);
    CHECK_INT(STD wcout.flags(), STD ios::skipws | STD ios::dec);
    CHECK_INT(STD wcout.precision(), 6);
    CHECK_INT(STD wcout.width(), 0);
    CHECK_INT(STD wcout.fill(), ' ');

    CHECK(STD wcerr.good());
    CHECK_INT(STD wcerr.exceptions(), STD ios::goodbit);
    CHECK_INT(STD wcerr.flags(), STD ios::skipws | STD ios::dec | STD ios::unitbuf);
    CHECK_INT(STD wcerr.precision(), 6);
    CHECK_INT(STD wcerr.width(), 0);
    CHECK_INT(STD wcerr.fill(), ' ');

    CHECK(STD wclog.good());
    CHECK_INT(STD wclog.exceptions(), STD ios::goodbit);
    CHECK_INT(STD wclog.flags(), STD ios::skipws | STD ios::dec);
    CHECK_INT(STD wclog.precision(), 6);
    CHECK_INT(STD wclog.width(), 0);
    CHECK_INT(STD wclog.fill(), ' ');
}
