// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <clocale>
#define TEST_NAME "<clocale>"

#include "tdefs.h"
#include <clocale>

#define STDx STD

void test_cpp() { // test C++ header
    static const int cats[] = {
#ifndef LC_MESSAGES
        0,
#else // LC_MESSAGES
        LC_MESSAGES,
#endif // LC_MESSAGES
        LC_ALL, LC_COLLATE, LC_CTYPE, LC_MONETARY, LC_NUMERIC, LC_TIME};
    STDx lconv* p = (STDx lconv*) nullptr;

    CHECK(cats[1] == LC_ALL); // to quiet diagnostics
    CHECK(STDx setlocale(LC_ALL, "") != nullptr);
    CHECK(STDx localeconv() != p);
}

void test_main() { // test basic workings of clocale definitions
    test_cpp();
}
