// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <cctype>
#define TEST_NAME "<cctype>"

#include "tdefs.h"
#include <cctype>

#define STDx STD

void test_cpp() { // test C++ header
#undef OK
#ifdef isalpha
#define OK 0
#undef isalpha
#else // isalpha
#define OK 1
#endif // isalpha
    CHECK_MSG("isalpha is not a macro", OK);
    CHECK(STDx isalpha((int) 'a') != 0);

#undef OK
#ifdef isblank
#define OK 0
#undef isblank
#else // isblank
#define OK 1
#endif // isblank
    CHECK_MSG("isblank is not a macro", OK);
    CHECK(STDx isblank((int) 'a') == 0);

#undef OK
#ifdef isalnum
#define OK 0
#undef isalnum
#else // isalnum
#define OK 1
#endif // isalnum
    CHECK_MSG("isalnum is not a macro", OK);
    CHECK(STDx isalnum((int) 'a') != 0);

#undef OK
#ifdef iscntrl
#define OK 0
#undef iscntrl
#else // iscntrl
#define OK 1
#endif // iscntrl
    CHECK_MSG("iscntrl is not a macro", OK);
    CHECK(STDx iscntrl((int) 'a') == 0);

#undef OK
#ifdef isdigit
#define OK 0
#undef isdigit
#else // isdigit
#define OK 1
#endif // isdigit
    CHECK_MSG("isdigit is not a macro", OK);
    CHECK(STDx isdigit((int) 'a') == 0);

#undef OK
#ifdef isgraph
#define OK 0
#undef isgraph
#else // isgraph
#define OK 1
#endif // isgraph
    CHECK_MSG("isgraph is not a macro", OK);
    CHECK(STDx isgraph((int) 'a') != 0);

#undef OK
#ifdef islower
#define OK 0
#undef islower
#else // islower
#define OK 1
#endif // islower
    CHECK_MSG("islower is not a macro", OK);
    CHECK(STDx islower((int) 'a') != 0);

#undef OK
#ifdef isprint
#define OK 0
#undef isprint
#else // isprint
#define OK 1
#endif // isprint
    CHECK_MSG("isprint is not a macro", OK);
    CHECK(STDx isprint((int) 'a') != 0);

#undef OK
#ifdef ispunct
#define OK 0
#undef ispunct
#else // ispunct
#define OK 1
#endif // ispunct
    CHECK_MSG("ispunct is not a macro", OK);
    CHECK(STDx ispunct((int) 'a') == 0);

#undef OK
#ifdef isspace
#define OK 0
#undef isspace
#else // isspace
#define OK 1
#endif // isspace
    CHECK_MSG("isspace is not a macro", OK);
    CHECK(STDx isspace((int) 'a') == 0);

#undef OK
#ifdef isupper
#define OK 0
#undef isupper
#else // isupper
#define OK 1
#endif // isupper
    CHECK_MSG("isupper is not a macro", OK);
    CHECK(STDx isupper((int) 'a') == 0);

#undef OK
#ifdef isxdigit
#define OK 0
#undef isxdigit
#else // isxdigit
#define OK 1
#endif // isxdigit
    CHECK_MSG("isxdigit is not a macro", OK);
    CHECK(STDx isxdigit((int) 'a') != 0);

    CHECK_INT(STDx tolower((int) 'A'), 'a');
    CHECK_INT(STDx toupper((int) 'a'), 'A');
}

void test_main() { // test basic workings of cctype definitions
    test_cpp();
}
