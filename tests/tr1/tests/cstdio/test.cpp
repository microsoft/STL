// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <cstdio>
#define TEST_NAMEX "<cstdio>"

#include "tdefs.h"
#include "temp_file_name.h"
#include <assert.h>
#include <cstdio>
#include <errno.h>
#include <stdarg.h>
#include <string.h>

#undef clearerr // tested in stdio2.c
#undef feof
#undef ferror
#undef getc
#undef getchar
#undef putc
#undef putchar

#define STDx STD

#include <stddef.h>
#include <stdint.h>

#pragma warning(disable : 4793) // function compiled as native

static int vfs(const char* fmt, ...) { // test vfscanf
    int ans;
    va_list ap;

    va_start(ap, fmt);
    ans = STDx vfscanf(stdin, fmt, ap);
    va_end(ap);
    return ans;
}

static int vs(const char* fmt, ...) { // test vscanf
    int ans;
    va_list ap;

    va_start(ap, fmt);
    ans = STDx vscanf(fmt, ap);
    va_end(ap);
    return ans;
}

static void vfp(const char* fmt, ...) { // test vfprintf
    va_list ap;

    va_start(ap, fmt);
    STDx vfprintf(stdout, fmt, ap);
    va_end(ap);
}

static void vp(const char* fmt, ...) { // test vprintf
    va_list ap;

    va_start(ap, fmt);
    STDx vprintf(fmt, ap);
    va_end(ap);
}

static void vsp(char* s, const char* fmt, ...) { // test vsprintf
    va_list ap;

    va_start(ap, fmt);
    STDx vsprintf(s, fmt, ap);
    va_end(ap);
}

void test_cpp() { // test C++ header
    char buf[32], ch;
    int in;
    long lo;
    short sh;

    CHECK_INT(STDx sprintf(buf, "%2c|%-4d|%.4o|%#lX", 'a', -4, 8, 12L), 16);
    CHECK_STR(buf, " a|-4  |0010|0XC");
    CHECK_INT(STDx sscanf(buf, " %c|%hd |%i|%lx", &ch, &sh, &in, &lo), 4);
    CHECK_INT(ch, 'a');
    CHECK_INT(sh, -4);
    CHECK_INT(in, 8);
    CHECK_INT(lo, 12);

    { // test file I/O
        const char* tn;
        STDx FILE* pf;
        STDx fpos_t fp1;
        int in1;
        long off;

        const auto temp_name = temp_file_name();
        tn                   = temp_name.c_str();
        assert((pf = STDx fopen(tn, "w+")) != nullptr);

        STDx setbuf(pf, (char*) nullptr);
        CHECK_INT(STDx fprintf(pf, "123\n"), 4);
        CHECK((off = STDx ftell(pf)) != -1);
        CHECK_INT(STDx fprintf(pf, "456\n"), 4);
        CHECK(STDx fgetpos(pf, &fp1) == 0);
        CHECK_INT(STDx fprintf(pf, "789\n"), 4);
        STDx rewind(pf);
        CHECK_INT(STDx fscanf(pf, "%i", &in1), 1);
        CHECK_INT(in1, 123);
        CHECK(STDx fsetpos(pf, &fp1) == 0);
        CHECK_INT(STDx fscanf(pf, "%i", &in1), 1);
        CHECK_INT(in1, 789);
        CHECK(STDx fseek(pf, off, SEEK_SET) == 0);
        CHECK_INT(STDx fscanf(pf, "%i", &in1), 1);
        CHECK_INT(in1, 456);
        assert(STDx fclose(pf) == 0 && STDx freopen(tn, "r", stdin) == stdin);
        CHECK(STDx setvbuf(stdin, buf, _IOLBF, sizeof(buf)) == 0);
        CHECK_INT(STDx scanf("%i", &in1), 1);
        CHECK_INT(in1, 123);

        CHECK_INT(vs("%i", &in1), 1); // test C99 functions
        CHECK_INT(in1, 456);
        CHECK_INT(vfs("%i", &in1), 1);
        CHECK_INT(in1, 789);

        CHECK(STDx fclose(stdin) == 0);
        STDx remove(tn);
    }

    { // test character I/O
        char tname[L_tmpnam];
        char tn[100];
        STDx FILE* pf;

        const auto temp_name1 = temp_file_name();
        CHECK(temp_name1.size() < sizeof(tname));
        CSTD strcpy_s(tname, sizeof(tname), temp_name1.c_str());

        const auto temp_name2 = temp_file_name();
        CHECK(temp_name2.size() < sizeof(tn));
        CSTD strcpy_s(tn, sizeof(tn), temp_name2.c_str());

        CHECK(CSTD strcmp(tn, tname) != 0);
        assert((pf = STDx fopen(tname, "w")) != nullptr);

        CHECK(STDx feof(pf) == 0);
        CHECK(STDx ferror(pf) == 0);
        CHECK_INT(STDx fgetc(pf), EOF);
        CHECK(STDx feof(pf) == 0);
        CHECK(STDx ferror(pf) != 0);
        STDx clearerr(pf);
        CHECK(STDx ferror(pf) == 0);

        CHECK_INT(STDx fputc('a', pf), 'a');
        CHECK_INT(STDx putc('b', pf), 'b');
        CHECK(0 <= STDx fputs("cde\n", pf));
        CHECK(0 <= STDx fputs("fghij\n", pf));
        CHECK(STDx fflush(pf) == 0);
        CHECK_INT(STDx fwrite("klmnopq\n", 2, 4, pf), 4);
        CHECK(STDx fclose(pf) == 0);

        assert(STDx freopen(tname, "r", stdin) == stdin);
        CHECK_INT(STDx fgetc(stdin), 'a');
        CHECK_INT(STDx getc(stdin), 'b');
        CHECK_INT(STDx getchar(), 'c');
        CHECK_PTR(STDx fgets(buf, sizeof(buf), stdin), buf);
        CHECK_STR(buf, "de\n");
        CHECK_INT(STDx ungetc('x', stdin), 'x');

        CHECK_PTR(STDx fgets(buf, sizeof(buf), stdin), buf);
        CHECK_STR(buf, "xfghij\n");

        CHECK_INT(STDx fread(buf, 2, 4, stdin), 4);
        buf[8] = '\0';
        CHECK_STR(buf, "klmnopq\n");
        CHECK_INT(STDx getchar(), EOF);
        CHECK(STDx feof(stdin) != 0);
        CHECK(STDx fclose(stdin) == 0);

        STDx remove(tn);
        CHECK(STDx rename(tname, tn) == 0);
        CHECK(STDx fopen(tname, "r") == nullptr);
        CHECK((pf = STDx fopen(tn, "r")) != nullptr && STDx fclose(pf) == 0);
        CHECK(STDx remove(tn) == 0);
        CHECK(STDx fopen(tn, "r") == nullptr);
        assert((pf = STDx tmpfile()) != nullptr);
        CHECK_INT(STDx fputc('x', pf), 'x');
        errno = EDOM;
        if (!terse) {
            STDx perror("Domain error reported as");
        }
    }
}

void test_main() { // test basic workings of cstdio definitions
    char buf[32];

    test_cpp();

    if (!terse) { // print with different v* functions
        vsp(buf, "FIN%c%s", 'I', "SHED");
        vfp("%s testing %s", buf, "<cstdio>");
        vp("\n");
    }
}
