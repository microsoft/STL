// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <csetjmp>
#define TEST_NAME "<csetjmp>"

#include "tdefs.h"
#include <csetjmp>

#pragma warning(disable : 4611) // interaction between '_setjmp' and C++ object destruction is non-portable
#pragma warning(disable : 4793) // function compiled as native

#define STDx STD

void test_cpp() { // test C++ header
    STDx jmp_buf jbuf;
    volatile int ctr = 0;

    switch (setjmp(jbuf)) { // jump among cases
    case 0:
        ctr = ctr + 1;
        CHECK_INT(ctr, 1);
        STDx longjmp(jbuf, 5);

    case 5:
        ctr = ctr + 1;
        CHECK_INT(ctr, 2);
        break;

    default:
        CHECK_MSG("longjmp returns properly", 0);
    }

#undef OK
#ifndef setjmp
#define OK 0
#else // setjmp
#define OK 1
#endif // setjmp
    CHECK_MSG("setjmp is a macro", OK);
}

void test_main() { // test basic workings of csetjmp definitions
    test_cpp();
}
