// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <cstring>
#define TEST_NAME "<cstring>"

#include "tdefs.h"
#include <cstring>
#include <errno.h>

#define STDx STD

extern "C" {
const void* (*pmemchr2)(const void*, int, STDx size_t) = &STDx memchr;
const char* (*pstrchr2)(const char*, int)              = &STDx strchr;
const char* (*pstrpbrk2)(const char*, const char*)     = &STDx strpbrk;
const char* (*pstrrchr2)(const char*, int)             = &STDx strrchr;
const char* (*pstrstr2)(const char*, const char*)      = &STDx strstr;
}

void test_cpp() { // test C++ header
    char s[20] = {0};
    STDx size_t n;
    STDx size_t zero          = 0;
    static const char abcde[] = "abcde";
    static const char abcdx[] = "abcdx";

    CHECK_PTR(STDx memchr(&abcde[0], 'c', 5), &abcde[2]);
    CHECK(STDx memcmp(&abcde[0], &abcdx[0], 4) == 0);

    // the following tests are interrelated
    CHECK_PTR(STDx memcpy(s, &abcde[0], 6), s);
    CHECK_STR(s, "abcde");
    CHECK_PTR(STDx memmove(s + 1, s, 3), s + 1);
    CHECK_STR(s, "aabce");
    CHECK_PTR(STDx memmove(s, s + 2, 3), s);
    CHECK_STR(s, "bcece");

    CHECK_PTR(STDx memset(s, '*', 10), s);

    CHECK_STR(s, "**********");
    CHECK_PTR(STDx memset(s + 2, '%', zero), s + 2);
    CHECK_STR(s, "**********");

    CHECK_PTR(STDx strcat((char*) STDx memcpy(s, &abcde[0], 6), "fg"), s);
    CHECK_PTR(STDx strchr(&abcde[0], 'c'), &abcde[2]);
    CHECK(STDx strcmp(&abcde[0], "abcde") == 0);
    CHECK(STDx strcoll(&abcde[0], "abcde") == 0);
    CHECK_PTR(STDx strcpy(s, &abcde[0]), s);
    CHECK_STR(s, &abcde[0]);
    CHECK_INT(STDx strcspn(&abcde[0], "xdy"), 3);
    CHECK(STDx strerror(EDOM) != nullptr);
    CHECK_INT(STDx strlen(&abcde[0]), 5);
    CHECK_PTR(STDx strncat(STDx strcpy(s, &abcde[0]), "fg", 1), s);
    CHECK_STR(s, "abcdef");
    CHECK(STDx strncmp(&abcde[0], &abcdx[0], 4) == 0);
    CHECK_PTR(STDx strncpy(s, &abcde[0], 7), s);
    CHECK_MEM(s, "abcde\0", 7);
    CHECK_PTR(STDx strpbrk(&abcde[0], "xdy"), &abcde[3]);
    CHECK_PTR(STDx strrchr(&abcde[0], 'c'), &abcde[2]);
    CHECK_STR(STDx strrchr("ababa", 'b'), "ba");
    CHECK_INT(STDx strspn(&abcde[0], "abce"), 3);
    CHECK_PTR(STDx strstr(&abcde[0], "cd"), &abcde[2]);
    CHECK_PTR(STDx strtok(STDx strcpy(s, &abcde[0]), "ac"), &s[1]);
    n = STDx strxfrm((char*) nullptr, &abcde[0], 0);
    if (n < sizeof(s) - 1) { // buffer big enough, check results
        CHECK_INT(STDx strxfrm(s, &abcde[0], n + 1), n);
        CHECK_INT(STDx strlen(s), n);
    }

    // test overloads
    void* (*pmemchr1)(void*, int, STDx size_t) = &STDx memchr;
    CHECK_PTR((*pmemchr1)((void*) &abcde[0], 'c', 5), &abcde[2]);
    // const void *(*pmemchr2)(const void *, int, STDx size_t) = &STDx memchr;
    CHECK_PTR((*pmemchr2)(&abcde[0], 'c', 5), &abcde[2]);

    char* (*pstrchr1)(char*, int) = &STDx strchr;
    CHECK_PTR((*pstrchr1)((char*) &abcde[0], 'c'), &abcde[2]);
    // const char *(*pstrchr2)(const char *, int) = &STDx strchr;
    CHECK_PTR((*pstrchr2)(&abcde[0], 'c'), &abcde[2]);

    char* (*pstrpbrk1)(char*, const char*) = &STDx strpbrk;
    CHECK_PTR((*pstrpbrk1)((char*) &abcde[0], "xdy"), &abcde[3]);
    // const char *(*pstrpbrk2)(const char *, const char *) = &STDx strpbrk;
    CHECK_PTR((*pstrpbrk2)(&abcde[0], "xdy"), &abcde[3]);

    char* (*pstrrchr1)(char*, int) = &STDx strrchr;
    CHECK_PTR((*pstrrchr1)((char*) &abcde[0], 'c'), &abcde[2]);
    // const char *(*pstrrchr2)(const char *, int) = &STDx strrchr;
    CHECK_PTR((*pstrrchr2)(&abcde[0], 'c'), &abcde[2]);

    char* (*pstrstr1)(char*, const char*) = &STDx strstr;
    CHECK_PTR((*pstrstr1)((char*) &abcde[0], "cd"), &abcde[2]);
    // const char *(*pstrstr2)(const char *, const char *) = &STDx strstr;
    CHECK_PTR((*pstrstr2)(&abcde[0], "cd"), &abcde[2]);
}

void test_main() { // test basic workings of cstring definitions
    test_cpp();
}
