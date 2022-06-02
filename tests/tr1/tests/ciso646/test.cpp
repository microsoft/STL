// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <ciso646>
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING
#define TEST_NAME "<ciso646>"

#include "tdefs.h"
#include <ciso646>

void test_cpp() { // test C++ header
    int i = 3 bitand 1;
    int j = 4 bitor 8;

    CHECK(i == 1 and j == 12);
    CHECK(i == 0 or not(j == 0));
    i or_eq j;
    CHECK(i == 13);
    j and_eq compl 4;
    CHECK(j == 8);
    i xor_eq j xor 9;
    CHECK(i == 12);
}

void test_main() { // test basic workings of ciso646 definitions
    test_cpp();
}
