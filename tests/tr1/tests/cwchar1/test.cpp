// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <cwchar>, part 1
#define TEST_NAMEX "<cwchar>, part 1"

#include "tdefs.h"
#include <assert.h>
#include <cwchar>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#pragma warning(disable : 4793) // function compiled as native

#define NO_TMPNAM_TESTS 1
#undef tmpnam
#define tmpnam(x) _tempnam(".", "")

#undef clearerr // tested in stdio2.c
#undef feof
#undef ferror
#undef getc
#undef getchar
#undef putc
#undef putchar

#define STDx STD

#define BUF_SIZE 32

static int vfs(const wchar_t* fmt, ...) { // test vfwscanf
    int ans;
    va_list ap;

    va_start(ap, fmt);
    ans = STDx vfwscanf(stdin, fmt, ap);
    va_end(ap);
    return ans;
}

static int vs(const wchar_t* fmt, ...) { // test vwscanf
    int ans;
    va_list ap;

    va_start(ap, fmt);
    ans = STDx vwscanf(fmt, ap);
    va_end(ap);
    return ans;
}

static int vfp(CSTD FILE* pf, const wchar_t* fmt, ...) { // test vfwprintf
    int ans;
    CSTD va_list ap;

    va_start(ap, fmt);
    ans = STDx vfwprintf(pf, fmt, ap);
    va_end(ap);
    return ans;
}

static int vp(const wchar_t* fmt, ...) { // test vwprintf
    int ans;
    CSTD va_list ap;

    va_start(ap, fmt);
    printf(""); // in case simple call sets narrow/wide mode
    ans = STDx vwprintf(fmt, ap);
    va_end(ap);
    return ans;
}

static void vsp(wchar_t* s, CSTD size_t n, const wchar_t* fmt, ...) { // test vswprintf
    va_list ap;

    va_start(ap, fmt);
    STDx vswprintf(s, n, fmt, ap);
    va_end(ap);
}

void test_cpp() { // test C++ header
    typedef STDx wint_t wint_t;

    wchar_t buf[BUF_SIZE], ch;
    int in;
    long lo;
    short sh;

    CHECK_INT(STDx swprintf(buf, BUF_SIZE, L"%2c|%-4d|%.4o|%#lX", L'a', -4, 8, 12L), 16);
    CHECK_WSTR(buf, L" a|-4  |0010|0XC");
    CHECK_INT(STDx swscanf(buf, L" %lc|%hd |%i|%lx", &ch, &sh, &in, &lo), 4);
    CHECK_INT(ch, L'a');
    CHECK_INT(sh, -4);
    CHECK_INT(in, 8);
    CHECK_INT(lo, 12);

    { // test file I/O
        const char* tn;
        CSTD FILE* pf;
        CSTD fpos_t fp1;
        int in1;
        long off;

        assert((tn = CSTD tmpnam((char*) nullptr)) != nullptr);
        assert((pf = CSTD fopen(tn, "w+")) != nullptr);
        CHECK_INT(STDx fwide(pf, 0), 0);
        CHECK_INT(STDx fwprintf(pf, L"123\n"), 4);

        CHECK((off = CSTD ftell(pf)) != -1);
        CHECK_INT(vfp(pf, L"%ls\n", L"456"), 4);
        CHECK_INT(vp(L""), 0);
        CHECK(CSTD fgetpos(pf, &fp1) == 0);
        CHECK_INT(STDx fwprintf(pf, L"789\n"), 4);
        CSTD rewind(pf);
        CHECK_INT(STDx fwscanf(pf, L"%i", &in1), 1);
        CHECK_INT(in1, 123);
        CHECK(CSTD fsetpos(pf, &fp1) == 0);
        CHECK_INT(STDx fwscanf(pf, L"%i", &in1), 1);
        CHECK_INT(in1, 789);
        CHECK(CSTD fseek(pf, off, SEEK_SET) == 0);
        CHECK_INT(STDx fwscanf(pf, L"%i", &in1), 1);
        CHECK_INT(in1, 456);
        assert(CSTD fclose(pf) == 0 && CSTD freopen(tn, "r", stdin) == stdin);

        CHECK_INT(STDx wscanf(L"%i", &in1), 1);
        CHECK_INT(in1, 123);

        CHECK_INT(vs(L"%i", &in1), 1);
        CHECK_INT(in1, 456);
        CHECK_INT(vfs(L"%i", &in1), 1);
        CHECK_INT(in1, 789);

        CHECK(CSTD fclose(stdin) == 0);
        CSTD remove(tn);
    }

    { // test character I/O
        CSTD FILE* pf;
        wchar_t wmacs[]     = {WCHAR_MAX, WCHAR_MIN};
        STDx wint_t wintval = WEOF;

        CHECK(wmacs[1] < wmacs[0]);

#if NO_TMPNAM_TESTS
        char *tname, *tn;
        assert((tn = CSTD tmpnam((char*) nullptr)) != nullptr);
        tname = (char*) CSTD malloc(CSTD strlen(tn) + 1);
        CSTD strcpy(tname, tn);

#else // NO_TMPNAM_TESTS
        char tname[L_tmpnam], *tn;
        CHECK_PTR(CSTD tmpnam(tname), tname);
        assert(CSTD strlen(tname) < L_tmpnam);
#endif // NO_TMPNAM_TESTS

        assert((tn = CSTD tmpnam((char*) nullptr)) != nullptr);
        CHECK(CSTD strcmp(tn, tname) != 0);
        assert((pf = CSTD fopen(tname, "w")) != nullptr);
        CHECK_INT(STDx fgetwc(pf), wintval);

        CHECK(CSTD feof(pf) == 0);
        CHECK(CSTD ferror(pf) != 0);
        CSTD clearerr(pf);
        CHECK(CSTD ferror(pf) == 0);
        CHECK_INT(STDx fputwc(L'a', pf), L'a');
        CHECK_INT(STDx putwc(L'b', pf), L'b');
        CHECK(0 <= STDx fputws(L"cde\n", pf));
        CHECK(0 <= STDx fputws(L"fghij\n", pf));
        CHECK(CSTD fflush(pf) == 0);
        CHECK(CSTD fclose(pf) == 0);

        assert(CSTD freopen(tname, "r", stdin) == stdin);
        CHECK_INT(STDx fgetwc(stdin), L'a');
        CHECK_INT(STDx getwc(stdin), L'b');
        CHECK_INT(STDx getwchar(), L'c');
        CHECK_PTR(STDx fgetws(buf, BUF_SIZE, stdin), buf);
        CHECK_WSTR(buf, L"de\n");
        wintval = STDx ungetwc(L'x', stdin);
        CHECK_INT(wintval, L'x');
        CHECK_PTR(STDx fgetws(buf, BUF_SIZE, stdin), buf);
        CHECK_WSTR(buf, L"xfghij\n");
        CHECK_INT(STDx getwchar(), WEOF);
        CHECK(CSTD feof(stdin) != 0);
        CHECK(CSTD fclose(stdin) == 0);

        CSTD remove(tn);
        CHECK(CSTD rename(tname, tn) == 0);
        CHECK(CSTD fopen(tname, "r") == nullptr);
        CHECK((pf = CSTD fopen(tn, "r")) != nullptr && CSTD fclose(pf) == 0);
        CHECK(CSTD remove(tn) == 0);
        CHECK(CSTD fopen(tn, "r") == nullptr);
        assert((pf = CSTD tmpfile()) != nullptr);
        CHECK_INT(STDx fputwc(L'x', pf), L'x');
    }
}

void test_main() { // test basic workings of cwchar definitions
    wchar_t buf[BUF_SIZE];

    test_cpp();

    if (!terse) { // print with different v* functions
        vsp(buf, BUF_SIZE, L"FIN%c%ls", 'I', L"SHED");
        CSTD printf("%ls testing <cwchar>, part 1\n", buf);
    }
}
