// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <ctime>
#define TEST_NAME "<ctime>"

#include "tdefs.h"
#include <ctime>

#define STDx STD

void test_cpp() { // test C++ header
    char buf[32];
    STDx clock_t tc = STDx clock();
    STDx tm ts1;
    STDx time_t tt1 = 85;
    STDx time_t tt2 = 170;
    const char* dstr;

    tt1 = STDx time(&tt2);
    CHECK(tt1 == tt2);

    ts1.tm_sec   = 15;
    ts1.tm_min   = 55;
    ts1.tm_hour  = 6;
    ts1.tm_mday  = 2;
    ts1.tm_mon   = 11;
    ts1.tm_year  = 79;
    ts1.tm_isdst = -1;
    tt1          = STDx mktime(&ts1);
    CHECK_INT(ts1.tm_wday, 0);
    CHECK_INT(ts1.tm_yday, 335);

    STDx strftime(buf, sizeof(buf), "%a|%A|%b|%B", &ts1);
    CHECK_STR(buf, "Sun|Sunday|Dec|December");
    STDx strftime(buf, sizeof(buf), "%d|%H|%I|%j|%m|%M|%p", &ts1);
    CHECK_STR(buf, "02|06|06|336|12|55|AM");
    STDx strftime(buf, sizeof(buf), "%S|%U|%w|%W|%y|%Y|%%", &ts1);
    CHECK_STR(buf, "15|48|0|48|79|1979|%");

    ++ts1.tm_sec;
    tt2 = STDx mktime(&ts1);
    CHECK(STDx difftime(tt1, tt2) < 0.0);

    dstr = "Sun Dec  2 06:55:15 1979\n";

    CHECK_STR(STDx asctime(STDx localtime(&tt1)), dstr);
    CHECK_INT(STDx strftime(buf, sizeof(buf), "%S", STDx gmtime(&tt2)), 2);
    CHECK_STR(buf, "16");
    CHECK(tc <= STDx clock());
}

void test_main() { // test basic workings of ctime definitions
    test_cpp();
}
