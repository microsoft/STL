// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <strstream>
#define TEST_NAME "<strstream>"

#define _SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING

#include "tdefs.h"
#include <limits.h>
#include <stdlib.h>
#include <strstream>

#define HUGE 2

char buf[]        = "12345678";
static int allocs = 0;
static int frees  = 0;

void* salloc(CSTD size_t n) { // allocate storage for string
    ++allocs;
    return CSTD malloc(n);
}

void sfree(void* p) { // free storage for string
    ++frees;
    CSTD free(p);
}

void t1() { // test dynamic strstreambufs
    STD strstreambuf sb0, sb1(1), sb2(&salloc, &sfree);
    STD ostream outs(&sb0);

    outs << "dynamic strstreambuf 0" << STD ends;
    CHECK_STR(sb0.str(), "dynamic strstreambuf 0");
    sb0.freeze();
    outs.rdbuf(&sb1);
    outs << "dynamic strstreambuf 1" << STD ends;
    CHECK_STR(sb1.str(), "dynamic strstreambuf 1");
    sb1.freeze(1);
    outs.rdbuf(&sb2);
    outs << "allocating strstreambuf" << STD ends;
    CHECK_STR(sb2.str(), "allocating strstreambuf");
    sb2.freeze(0);
}

void t2() { // test read-only strstreambufs
    STD strstreambuf sb1((const char*) buf, 5);
    STD strstreambuf sb2((unsigned char*) buf, 0);
    STD strstreambuf sb3((signed char*) buf, HUGE);
    STD strstreambuf sb4((const char*) buf, 5);
    STD strstreambuf sb5((const unsigned char*) buf, 0);
    STD strstreambuf sb6((const signed char*) buf, HUGE);

    CHECK_STR(sb1.str(), buf);
    CHECK_INT(sb1.pcount(), 0);
    CHECK_INT((STD streamoff) sb1.pubseekoff(0, STD ios::end, STD ios::in), 5);
    CHECK_INT((STD streamoff) sb1.pubseekoff(1, STD ios::cur, STD ios::out), -1); // force failure for DR453
    CHECK_STR(sb2.str(), buf);
    CHECK_INT(sb2.pcount(), 0);
    CHECK_INT((STD streamoff) sb2.pubseekoff(0, STD ios::end, STD ios::in), 8);
    CHECK_INT((STD streamoff) sb2.pubseekoff(0, STD ios::cur), -1);
    CHECK_STR(sb3.str(), buf);
    CHECK_INT(sb3.pcount(), 0);
    CHECK_STR(sb4.str(), buf);
    CHECK_INT(sb4.pcount(), 0);
    CHECK_INT((STD streamoff) sb4.pubseekoff(0, STD ios::end, STD ios::in), 5);
    CHECK_STR(sb5.str(), buf);
    CHECK_INT(sb5.pcount(), 0);
    CHECK_STR(sb6.str(), buf);
    CHECK_INT(sb6.pcount(), 0);

    {
        typedef STD strstreambuf Mycont;
        Mycont v6("xx", HUGE);
        Mycont v7(STD move(v6));
        CHECK_PTR(v6.str(), nullptr);
        CHECK_STR(v7.str(), "xx");

        Mycont v8;
        v8 = STD move(v7);
        CHECK_PTR(v7.str(), nullptr);
        CHECK_STR(v8.str(), "xx");

        v7.swap(v8);
        CHECK_STR(v7.str(), "xx");
        CHECK_PTR(v8.str(), nullptr);
    }

    {
        typedef STD istrstream Mycont;
        Mycont v6((const char*) "xx", HUGE);
        Mycont v7(STD move(v6));
        CHECK_PTR(v6.str(), nullptr);
        CHECK_STR(v7.str(), "xx");

        Mycont v8((const char*) "yy");
        v8 = STD move(v7);
        CHECK_STR(v8.str(), "xx");

        v7.swap(v8);
        CHECK_STR(v7.str(), "xx");
    }

    {
        typedef STD ostrstream Mycont;
        Mycont v6((char*) "xx", HUGE);
        Mycont v7(STD move(v6));
        CHECK_PTR(v6.str(), nullptr);
        CHECK_STR(v7.str(), "xx");

        Mycont v8;
        v8 = STD move(v7);
        CHECK_PTR(v7.str(), nullptr);
        CHECK_STR(v8.str(), "xx");

        v7.swap(v8);
        CHECK_STR(v7.str(), "xx");
        CHECK_PTR(v8.str(), nullptr);
    }

    {
        typedef STD strstream Mycont;
        Mycont v6((char*) "xx", HUGE);
        Mycont v7(STD move(v6));
        CHECK_PTR(v6.str(), nullptr);
        CHECK_STR(v7.str(), "xx");

        Mycont v8;
        v8 = STD move(v7);
        CHECK_PTR(v7.str(), nullptr);
        CHECK_STR(v8.str(), "xx");

        v7.swap(v8);
        CHECK_STR(v7.str(), "xx");
        CHECK_PTR(v8.str(), nullptr);
    }
}

void t3() { // test read-write strstreambufs
    STD strstreambuf sb1(buf, 5, buf);
    STD strstreambuf sb2((unsigned char*) buf, 0, (unsigned char*) buf + 1);
    STD strstreambuf sb3((signed char*) buf, HUGE, (signed char*) buf);
    STD ostream outs(&sb1);

    outs << 'A';
    CHECK_STR(sb1.str(), "A2345678");
    CHECK_INT(sb1.pcount(), 1);
    outs << 'B';
    CHECK_STR(sb1.str(), "AB345678");
    CHECK_INT(sb1.pcount(), 2);
    CHECK_INT((STD streamoff) sb1.pubseekoff(6, STD ios::beg, STD ios::out), -1);
    CHECK_STR(sb2.str(), "AB345678");
    CHECK_INT(sb2.pcount(), 0);
    outs.rdbuf(&sb2), sb2.freeze(0), outs << 'C';
    CHECK_STR(sb2.str(), "AC345678");
    CHECK_INT((STD streamoff) sb2.pubseekoff(6, STD ios::beg, STD ios::out), 6);
    CHECK_INT(sb2.pcount(), 6);
    CHECK_STR(sb3.str(), "AC345678");
    CHECK_INT(sb3.pcount(), 0);
}

void test_main() { // test basic workings of strstream definitions
    STD istrstream is1("s1"), is2((const char*) "s2x", 2);
    STD istrstream is3((char*) "s3"), is4((char*) "s4", 2);
    CHECK_STR(is1.rdbuf()->str(), "s1");
    CHECK_STR(is3.str(), "s3");

    char buf0[] = "12345";
    STD ostrstream os1, os2(buf0, 4);
    CHECK_STR(os2.rdbuf()->str(), "12345");

    char buf1[] = "d\0fgh";
    STD ostrstream os3(buf1, sizeof(buf1), STD ios::app);
    os3 << 'e';
    CHECK_STR(os3.str(), "defgh");
    CHECK_INT(os3.pcount(), 1);
    os3.freeze(0), os3 << 'F';
    CHECK_STR(os3.str(), "deFgh");
    CHECK_INT(os3.pcount(), 2);

    t1();
    CHECK(0 < allocs && 0 < frees);
    t2();
    t3();
}
