// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <cwchar>, part 2
#define TEST_NAME "<cwchar>, part 2"

#include "tdefs.h"
#include <cwchar>
#include <limits.h>
#include <time.h>

#define STDx STD

#define BUF_SIZE 32

#include <math.h>

extern "C" {
const wchar_t* (*pmemchr2)(const wchar_t*, wchar_t, CSTD size_t) = &STDx wmemchr;
const wchar_t* (*pwcspbrk2)(const wchar_t*, const wchar_t*)      = &STDx wcspbrk;
const wchar_t* (*pwcschr2)(const wchar_t*, wchar_t)              = &STDx wcschr;
const wchar_t* (*pwcsrchr2)(const wchar_t*, wchar_t)             = &STDx wcsrchr;
const wchar_t* (*pwcsstr2)(const wchar_t*, const wchar_t*)       = &STDx wcsstr;
} // extern "C"

void test_cpp() { // test C++ header
    { // test wide stdlib functions
        char str[10 < MB_LEN_MAX ? MB_LEN_MAX : 10];
        const char* pc;
        STDx mbstate_t mbst;
        wchar_t *s1, wc, wcs[10];
        const wchar_t* pwc;
        static const char abc[] = "abc";
        static STDx mbstate_t mbst0;

        CHECK(1 <= MB_CUR_MAX && MB_CUR_MAX <= MB_LEN_MAX);
        CHECK(STDx wcstod(L"28G", &s1) == 28.0 && s1 != nullptr && *s1 == L'G');
        CHECK(STDx wcstol(L"-a0", &s1, 11) == -110 && s1 != nullptr && *s1 == L'\0');
        CHECK(STDx wcstoul(L"0xFFg", &s1, 16) == 255 && s1 != nullptr && *s1 == L'g');
        CHECK(STDx mbsinit((const STDx mbstate_t*) nullptr) != 0 && STDx mbsinit(&mbst0) != 0);

        mbst = mbst0;
        CHECK(STDx mbsinit(&mbst) != 0);
        CHECK_INT(STDx mbrlen(abc + 2, 9, &mbst), 1);
        CHECK_INT(STDx mbrlen(abc + 3, 9, &mbst), 0);

        CHECK(STDx mbsinit(&mbst) != 0);
        CHECK_INT(STDx mbrtowc(&wc, abc + 2, 9, &mbst), 1);
        CHECK_INT(wc, L'c');
        CHECK_INT(STDx mbrtowc(&wc, abc + 3, 9, &mbst), 0);
        CHECK_INT(wc, L'\0');

        CHECK(STDx mbsinit(&mbst) != 0);
        pc = &abc[0];
        CHECK_INT(STDx mbsrtowcs(wcs, &pc, 10, &mbst), 3);
        CHECK(pc == nullptr);

        CHECK(STDx mbsinit(&mbst) != 0);
        CHECK_INT(STDx wcrtomb(str, wcs[0], &mbst), 1);
        CHECK_INT(*str, 'a');
        CHECK_INT(STDx wcrtomb(str, wcs[1], &mbst), 1);
        CHECK_INT(*str, 'b');

        CHECK(STDx mbsinit(&mbst) != 0);
        pwc = (const wchar_t*) wcs;
        CHECK_INT(STDx wcsrtombs(str, &pwc, 10, &mbst), 3);
        CHECK(pwc == nullptr);
        CHECK(STDx mbsinit(&mbst) != 0);
        CHECK_STR(str, &abc[0]);

        CHECK_INT(STDx btowc('a'), L'a');
        CHECK_INT(STDx wctob(L'a'), 'a');

        { // test C99 stuff
            CHECK(STDx wcstof(L"28G", &s1) == 28.0 && s1 != nullptr && *s1 == L'G');
            CHECK(isnan(STDx wcstof(L"NaN", &s1)) && s1 != nullptr && *s1 == L'\0');
            CHECK(STDx wcstod(L"Infx", &s1) == INFINITY && s1 != nullptr && *s1 == L'x');
            CHECK(STDx wcstold(L"28G", &s1) == 28.0 && s1 != nullptr && *s1 == L'G');
            CHECK(STDx wcstoll(L"-a0", &s1, 11) == -110 && s1 != nullptr && *s1 == L'\0');
            CHECK(STDx wcstoull(L"0xFFg", &s1, 16) == 255 && s1 != nullptr && *s1 == L'g');
        }
    }

    { // test wide string functions
        wchar_t s[20] = {0};
        CSTD size_t n;
        static const wchar_t abcde[] = {L"abcde"};
        static const wchar_t abcdx[] = {L"abcdx"};

        CHECK_PTR(STDx wmemchr(&abcde[0], L'c', 5), &abcde[2]);
        CHECK(STDx wmemcmp(&abcde[0], &abcdx[0], 4) == 0);

        // the following tests are interrelated
        CHECK_PTR(STDx wmemcpy(s, &abcde[0], 6), s);
        CHECK_WSTR(s, L"abcde");
        CHECK_PTR(STDx wmemmove(s + 1, s, 3), s + 1);
        CHECK_WSTR(s, L"aabce");
        CHECK_PTR(STDx wmemmove(s, s + 2, 3), s);
        CHECK_WSTR(s, L"bcece");
        CHECK_PTR(STDx wmemset(s, L'*', 10), s);
        CHECK_WSTR(s, L"**********");
        CHECK_PTR(STDx wmemset(s + 2, L'%', 0), s + 2);
        CHECK_WSTR(s, L"**********");

        CHECK_PTR(STDx wcscat(STDx wmemcpy(s, &abcde[0], 6), L"fg"), s);
        CHECK_PTR(STDx wcschr(&abcde[0], L'c'), &abcde[2]);
        CHECK(STDx wcscmp(&abcde[0], L"abcde") == 0);
        CHECK(STDx wcscoll(&abcde[0], L"abcde") == 0);
        CHECK_PTR(STDx wcscpy(s, &abcde[0]), s);
        CHECK_WSTR(s, &abcde[0]);
        CHECK_INT(STDx wcscspn(&abcde[0], L"xdy"), 3);
        CHECK_INT(STDx wcslen(&abcde[0]), 5);
        CHECK_PTR(STDx wcsncat(STDx wcscpy(s, &abcde[0]), L"fg", 1), s);
        CHECK_WSTR(s, L"abcdef");
        CHECK(STDx wcsncmp(&abcde[0], &abcdx[0], 4) == 0);
        CHECK_PTR(STDx wcsncpy(s, &abcde[0], 7), s);
        CHECK_WMEM(s, L"abcde\0", 7);
        CHECK_PTR(STDx wcspbrk(&abcde[0], L"xdy"), &abcde[3]);
        CHECK_PTR(STDx wcsrchr(&abcde[0], L'c'), &abcde[2]);
        CHECK_WSTR(STDx wcsrchr(L"ababa", L'b'), L"ba");
        CHECK_INT(STDx wcsspn(&abcde[0], L"abce"), 3);
        CHECK_PTR(STDx wcsstr(&abcde[0], L"cd"), &abcde[2]);

        wchar_t* temp;
        CHECK_PTR(STDx wcstok(STDx wcscpy(s, &abcde[0]), L"ac", &temp), &s[1]);

        n = STDx wcsxfrm((wchar_t*) nullptr, &abcde[0], 0);
        if (n < sizeof(s) / sizeof(wchar_t) - 1) { // buffer big enough, check results
            CHECK_INT(STDx wcsxfrm(s, &abcde[0], n + 1), n);
            CHECK_INT(STDx wcslen(s), n);
        }

        // test overloads
        wchar_t* (*pmemchr1)(wchar_t*, wchar_t, CSTD size_t) = &STDx wmemchr;
        CHECK_PTR((*pmemchr1)((wchar_t*) &abcde[0], L'c', 5), &abcde[2]);
        // const wchar_t *(*pmemchr2)(const wchar_t *, wchar_t, CSTD size_t) = &STDx wmemchr;
        CHECK_PTR((*pmemchr2)(&abcde[0], L'c', 5), &abcde[2]);

        wchar_t* (*pwcschr1)(wchar_t*, wchar_t) = &STDx wcschr;
        CHECK_PTR((*pwcschr1)((wchar_t*) &abcde[0], L'c'), &abcde[2]);
        // const wchar_t *(*pwcschr2)(const wchar_t *, wchar_t) = &STDx wcschr;
        CHECK_PTR((*pwcschr2)(&abcde[0], L'c'), &abcde[2]);

        wchar_t* (*pwcspbrk1)(wchar_t*, const wchar_t*) = &STDx wcspbrk;
        CHECK_PTR((*pwcspbrk1)((wchar_t*) &abcde[0], L"xdy"), &abcde[3]);
        // const wchar_t *(*pwcspbrk2)(const wchar_t *, const wchar_t *) = &STDx wcspbrk;
        CHECK_PTR((*pwcspbrk2)(&abcde[0], L"xdy"), &abcde[3]);

        wchar_t* (*pwcsrchr1)(wchar_t*, wchar_t) = &STDx wcsrchr;
        CHECK_PTR((*pwcsrchr1)((wchar_t*) &abcde[0], L'c'), &abcde[2]);
        // const wchar_t *(*pwcsrchr2)(const wchar_t *, wchar_t) = &STDx wcsrchr;
        CHECK_PTR((*pwcsrchr2)(&abcde[0], L'c'), &abcde[2]);

        wchar_t* (*pwcsstr1)(wchar_t*, const wchar_t*) = &STDx wcsstr;
        CHECK_PTR((*pwcsstr1)((wchar_t*) &abcde[0], L"cd"), &abcde[2]);
        // const wchar_t *(*pwcsstr2)(const wchar_t *, const wchar_t *) = &STDx wcsstr;
        CHECK_PTR((*pwcsstr2)(&abcde[0], L"cd"), &abcde[2]);
    }

    { // test wide time functions
        CSTD tm ts1;
        CSTD time_t tt1 = 85;
        CSTD time_t tt2 = 170;
        wchar_t buf[BUF_SIZE];

        tt1 = CSTD time(&tt2);
        CHECK(tt1 == tt2);

        ts1.tm_sec   = 15;
        ts1.tm_min   = 55;
        ts1.tm_hour  = 6;
        ts1.tm_mday  = 2;
        ts1.tm_mon   = 11;
        ts1.tm_year  = 79;
        ts1.tm_isdst = -1;
        tt1          = mktime(&ts1);
        CHECK_INT(ts1.tm_wday, 0);
        CHECK_INT(ts1.tm_yday, 335);

        STDx wcsftime(buf, BUF_SIZE, L"%a|%A|%b|%B", &ts1);
        CHECK_WSTR(buf, L"Sun|Sunday|Dec|December");
        STDx wcsftime(buf, BUF_SIZE, L"%d|%H|%I|%j|%m|%M|%p", &ts1);
        CHECK_WSTR(buf, L"02|06|06|336|12|55|AM");
        STDx wcsftime(buf, BUF_SIZE, L"%S|%U|%w|%W|%y|%Y|%%", &ts1);
        CHECK_WSTR(buf, L"15|48|0|48|79|1979|%");

        ++ts1.tm_sec;
        tt2 = CSTD mktime(&ts1);
        CHECK(CSTD difftime(tt1, tt2) < 0.0);
        CHECK_INT(STDx wcsftime(buf, BUF_SIZE, L"%S", CSTD gmtime(&tt2)), 2);
        CHECK_WSTR(buf, L"16");
        time(&tt1);
        CHECK(0 < STDx wcsftime(buf, BUF_SIZE, L"%c", CSTD localtime(&tt1)));
    }
}

void test_main() { // test basic workings of cwchar definitions
    test_cpp();
}
