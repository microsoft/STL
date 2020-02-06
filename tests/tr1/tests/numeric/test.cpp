// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <numeric>
#define TEST_NAME "<numeric>"

#include "tdefs.h"
#include <functional>
#include <numeric>

void test_main() { // test basic workings of numeric definitions
    STD multiplies<int> multipliesf;
    STD plus<int> plusf;
    int buf[]   = {1, 2, 3, 4, 5, 6};
    int dest[6] = {0};
    int *first = buf, *last = buf + 6;
    int val = 0;

    CHECK_INT(STD accumulate(first, last, val), 21);
    CHECK_INT(STD accumulate(first, last, val, plusf), 21);
    CHECK_INT(STD inner_product(first, last, first, val), 91);
    CHECK_INT(STD inner_product(first, last, first, val, plusf, multipliesf), 91);

    STD partial_sum(first, last, dest);
    CHECK_INT(dest[0], 1);
    CHECK_INT(dest[1], 3);
    CHECK_INT(dest[2], 6);
    CHECK_INT(dest[3], 10);
    CHECK_INT(dest[4], 15);
    CHECK_INT(dest[5], 21);

    STD adjacent_difference(first, last, dest);
    CHECK_INT(dest[0], 1);
    CHECK_INT(dest[1], 1);
    CHECK_INT(dest[2], 1);
    CHECK_INT(dest[3], 1);
    CHECK_INT(dest[4], 1);
    CHECK_INT(dest[5], 1);

    STD partial_sum(first, last, dest, plusf);
    CHECK_INT(dest[0], 1);
    CHECK_INT(dest[1], 3);
    CHECK_INT(dest[2], 6);
    CHECK_INT(dest[3], 10);
    CHECK_INT(dest[4], 15);
    CHECK_INT(dest[5], 21);

    STD adjacent_difference(first, last, dest, plusf);
    CHECK_INT(dest[0], 1);
    CHECK_INT(dest[1], 3);
    CHECK_INT(dest[2], 5);
    CHECK_INT(dest[3], 7);
    CHECK_INT(dest[4], 9);
    CHECK_INT(dest[5], 11);

    STD iota(dest, dest + 6, 'a');
    CHECK_INT(dest[0], 'a');
    CHECK_INT(dest[1], 'b');
    CHECK_INT(dest[2], 'c');
    CHECK_INT(dest[3], 'd');
    CHECK_INT(dest[4], 'e');
    CHECK_INT(dest[5], 'f');
}
