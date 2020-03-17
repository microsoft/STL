// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <cerrno>
#define TEST_NAME "<cerrno>"

#include "tdefs.h"
#include <cerrno>

void test_cpp() { // test C++ header
#undef OK
#ifndef errno
#define OK 0
#else // errno
#define OK 1
#endif // errno
    CHECK_MSG("errno is a macro", OK);
}

void test_main() { // test basic workings of cerrno definitions
    CHECK_INT(errno, 0);

#if 0 < ERANGE
    errno = ERANGE;
    CHECK_INT(errno, ERANGE);
#else // 0 < ERANGE
    CHECK_MSG("ERANGE is defined", 0);
#endif // 0 < ERANGE

#if 0 < EDOM
    errno = EDOM;
    CHECK_INT(errno, EDOM);
#else // 0 < EDOM
    CHECK_MSG("EDOM is defined", 0);
#endif // 0 < EDOM

#if 0 < EILSEQ
    errno = EILSEQ;
    CHECK_INT(errno, EILSEQ);
#else // 0 < EILSEQ
    CHECK_MSG("EILSEQ is defined", 0);
#endif // 0 < EILSEQ

    test_cpp();
}
