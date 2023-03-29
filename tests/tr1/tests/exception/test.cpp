// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <exception>
#define TEST_NAME "<exception>"

#define _HAS_DEPRECATED_UNCAUGHT_EXCEPTION 1
#define _SILENCE_CXX17_UNCAUGHT_EXCEPTION_DEPRECATION_WARNING

#include "tdefs.h"
#include <exception>
#include <setjmp.h>

#pragma warning(disable : 4611) // interaction between '_setjmp' and C++ object destruction is non-portable
#pragma warning(disable : 4793) // function compiled as native

#if NO_EXCEPTIONS
void test_main() { // (don't) test exceptions
}
#else // NO_EXCEPTIONS

#define STDx STD

static CSTD jmp_buf jbuf;

void jmpback() { // longjmp back to caller
    CSTD longjmp(jbuf, 1);
}

void try_terminate() { // test terminate machinery
    STDx terminate_handler save_hand = STDx set_terminate(&jmpback);

    CHECK(STDx get_terminate() == &jmpback);

    if (setjmp(jbuf) == 0) { // try calling terminate
        int zero = 0;

        if (zero == 0) {
            STDx terminate();
        }
        CHECK_MSG("terminate() doesn't return", 0);
    }
    CHECK(STDx set_terminate(save_hand) == &jmpback);
}

#pragma warning(push)
#pragma warning(disable : 4702) // unreachable code
void try_unexpected() { // test unexpected machinery
#if _HAS_UNEXPECTED
    STDx unexpected_handler save_hand = STDx set_unexpected(&jmpback);

    CHECK(STDx get_unexpected() == &jmpback);

    if (setjmp(jbuf) == 0) { // try calling unexpected
        STDx unexpected();
        CHECK_MSG("unexpected() doesn't return", 0);
    }
    CHECK(STDx set_unexpected(save_hand) == &jmpback);
#endif // _HAS_UNEXPECTED
}
#pragma warning(pop)

void test_main() { // test basic workings of exception definitions
    STD bad_exception bex1, bex2(bex1);
    STD exception ex1, ex2(bex1);

    ex1  = ex2;
    bex1 = bex2;
    CHECK(ex1.what() != nullptr);

    try_terminate();
    try_unexpected();
    CHECK(!STD uncaught_exception());
    CHECK(STD uncaught_exceptions() == 0);
}
#endif // NO_EXCEPTIONS
