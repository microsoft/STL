// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <cstddef>
#define TEST_NAME "<cstddef>"

#include "tdefs.h"
#include <cstddef>
#include <limits.h>

#define STDx STD

// type definitions
typedef struct {
    char f1;
    struct {
        float flt;
    } f2;
    int f3;
} Str;

void test_cpp() { // test C++ header
    static const STDx size_t offs[] = {offsetof(Str, f1), offsetof(Str, f2), offsetof(Str, f3)};
    STDx ptrdiff_t pdiff            = &offs[2] - &offs[0];
    wchar_t wc                      = L'Z';
    Str x                           = {1, {2}, 3};
    char* ps                        = (char*) &x;

    CHECK_INT(sizeof(STDx size_t), sizeof(sizeof(char)));
    CHECK_INT(pdiff, &offs[2] - &offs[0]);
    CHECK_INT(wc, L'Z');
    CHECK(offs[0] < offs[1]);
    CHECK_INT((long) *(float*) (ps + offs[1]), 2);
}

void test_main() { // test basic workings of cstddef definitions
    test_cpp();
}
