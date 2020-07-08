// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <cwctype>
#define TEST_NAME "<cwctype>"

#include "tdefs.h"
#include <cwctype>

#define STDx STD

void test_cpp() { // test C++ header
    typedef STDx wint_t wint_t;

    wchar_t wc;
    STDx wctrans_t tr = STDx wctrans("toupper");
    STDx wctype_t ty  = STDx wctype("alnum");
    wint_t weof       = WEOF;

    CHECK(weof != 0);
    CHECK(tr != 0);
    CHECK(ty != 0);
    CHECK(STDx iswctype(L'3', STDx wctype("digit")) != 0);
    wc = STDx towctrans(L'B', STDx wctrans("tolower"));
    CHECK_INT(STDx towlower(L'B'), wc);

    CHECK(STDx iswalpha(L'a') != 0);

    CHECK(STDx iswblank(L'a') == 0);

    CHECK(STDx iswalnum(L'a') != 0);
    CHECK(STDx iswcntrl(L'a') == 0);
    CHECK(STDx iswdigit(L'a') == 0);
    CHECK(STDx iswgraph(L'a') != 0);
    CHECK(STDx iswlower(L'a') != 0);
    CHECK(STDx iswprint(L'a') != 0);
    CHECK(STDx iswpunct(L'a') == 0);
    CHECK(STDx iswspace(L'a') == 0);
    CHECK(STDx iswupper(L'a') == 0);
    CHECK(STDx iswxdigit(L'a') != 0);
    CHECK_INT(STDx towlower(L'a'), L'a');
    CHECK_INT(STDx towupper(L'a'), L'A');
}

void test_main() { // test basic workings of cwctype definitions
    test_cpp();
}
