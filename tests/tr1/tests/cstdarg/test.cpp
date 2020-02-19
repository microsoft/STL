// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <cstdarg>
#define TEST_NAME "<cstdarg>"

#include "tdefs.h"
#include <cstdarg>
#include <string.h>

#pragma warning(disable : 4793) // function compiled as native

void test_cpp(int first, ...) { // test C++ header
    CSTD va_list ap, ap2;

    va_start(ap, first);

    va_copy(ap2, ap); // C99 only

    CHECK_MEM(&ap2, &ap, sizeof(CSTD va_list));
    CHECK_INT(first, 1);
    CHECK_INT(va_arg(ap, int), 2);
    va_end(ap);
}

void test_main() { // test basic workings of cstdarg definitions
    test_cpp(1, 2);
}
