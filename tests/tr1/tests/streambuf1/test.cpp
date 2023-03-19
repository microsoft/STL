// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <streambuf>, part 1
#define TEST_NAME "<streambuf>, part 1"

#include "tdefs.h"
#include <sstream>
#include <streambuf>

class Mybuf : public STD streambuf {
public:
    Mybuf() { // default constructor
    }

    typedef STD streambuf Mybase;

    Mybuf(const Mybuf& right) : Mybase(right) { // construct by copying
    }

    Mybuf& operator=(const Mybuf& right) { // assign
        Mybase::operator=(right);
        return *this;
    }

    void swap(Mybuf& right) { // swap
        Mybase::swap(right);
    }

    void setp(char* p) { // set pptr
        Mybase::setp(p, p);
    }

    char* getp() const { // get pptr
        return Mybase::pptr();
    }

protected:
    virtual STD streambuf* setbuf(char*, STD streamsize) { // fake setbuf
        return nullptr;
    }

    virtual int overflow(int = EOF) { // fake overflow
        return 1;
    }

    virtual int pbackfail(int = EOF) { // fake pbackfail
        return 2;
    }

    virtual STD streamsize showmanyc() { // fake showmanyc
        return 3;
    }

    virtual int underflow() { // fake underflow
        return 4;
    }

    virtual int uflow() { // fake uflow
        return 5;
    }

    virtual STD streamsize xsgetn(char*, STD streamsize) { // fake xsgetn
        return 6;
    }

    virtual STD streamsize xsputn(const char*, STD streamsize) { // fake xsputn
        return 7;
    }

    virtual STD streampos seekoff(STD streamoff, STD ios::seekdir,
        STD ios::openmode = STD ios::in | STD ios::out) { // fake seekoff
        return 8;
    }

    virtual STD streampos seekpos(STD streampos,
        STD ios::openmode = STD ios::in | STD ios::out) { // fake seekpos
        return 9;
    }

    virtual int sync() { // fake sync
        return 10;
    }
};

void try_mybuf() { // test that virtuals all get called
    Mybuf mine;
    STD streambuf* p = (STD streambuf*) &mine;

    CHECK_PTR(p->pubsetbuf(nullptr, 0), nullptr);
    CHECK_INT(p->sputc(0), 1);
    CHECK_INT(p->sungetc(), 2);
    CHECK_INT(p->in_avail(), 3);
    CHECK_INT(p->sgetc(), 4);
    CHECK_INT(p->sbumpc(), 5);
    CHECK_INT(p->sgetn(nullptr, 0), 6);
    CHECK_INT(p->sputn(nullptr, 0), 7);
    CHECK_INT((STD streamoff) p->pubseekoff(0, (STD ios::seekdir) 0), 8);
    CHECK_INT((STD streamoff) p->pubseekpos(0), 9);
    CHECK_INT(p->pubsync(), 10);
}

void test_main() { // test basic workings of streambuf definitions
    STD streamoff soff = 3;
    STD streampos s1(0), s2(10);
    STD streamsize ssize           = -1;
    STD streambuf::char_type* pc   = (char*) nullptr;
    STD streambuf::int_type* pi    = (int*) nullptr;
    STD streambuf::pos_type* pp    = (STD streampos*) nullptr;
    STD streambuf::off_type* po    = (STD streamoff*) nullptr;
    STD streambuf::traits_type* pt = (STD char_traits<char>*) nullptr;

    pc    = pc; // to quiet diagnostics
    pi    = pi;
    pp    = pp;
    po    = po;
    pt    = pt;
    ssize = ssize; // to quiet diagnostics

    CHECK_INT((STD streamoff) s1, 0);
    CHECK_INT((STD streamoff) s2, 10);
    CHECK_INT(s2 - s1, 10);
    CHECK(s1 + (STD streamoff) 4 == s2 - (STD streamoff) 6);
    CHECK(!((s1 += soff) != (s2 -= 7)));
    CHECK_INT((STD streamoff) s1, soff);
    CHECK_INT((STD streamoff) s2, soff);

    // test streambuf read/write members
    char buf[10];
    STD ostringstream os(STD ios_base::in | STD ios_base::out);
    STD streambuf* p = os.rdbuf();

    CHECK(p != nullptr);
    CHECK_INT(p->sputc('a'), 'a');
    s1 = p->pubseekoff(0, STD ios::cur, STD ios::out);
    CHECK(s1 != s2);
    s2 = p->pubseekpos(s1, STD ios::out);
    CHECK(s1 == s2);
    CHECK_INT(p->sgetc(), 'a');
    CHECK_INT(p->sbumpc(), 'a');
    CHECK_INT(p->sputn("xyz", 3), 3);
    CHECK_INT(p->sgetn(buf, 10), 3);
    CHECK_INT(buf[0], 'x');
    CHECK_INT(p->sputbackc('w'), 'w');
    CHECK_INT(p->sungetc(), 'y');
    CHECK_INT(p->in_avail(), 2);
    CHECK_INT(p->snextc(), 'w');
    CHECK(p->pubseekpos(s1, STD ios::in) == s2);
    CHECK_INT(p->sputc('!'), '!');
    CHECK_INT(p->sgetc(), 'x');
    CHECK((STD streamoff) p->pubseekoff(0, STD ios::end, STD ios::in) != -1);
    CHECK_INT(p->sungetc(), '!');

    STD basic_streambuf<char>* psb1                        = (STD streambuf*) nullptr;
    STD basic_streambuf<char, STD char_traits<char>>* psb2 = psb1;

    psb2 = psb2; // to quiet diagnostics

    s1 = p->pubseekoff(0, STD ios::cur, STD ios::out);
    s2 = p->pubseekpos(s1, STD ios::out);
    CHECK(s1 == s2);
    p->pubsetbuf(buf, sizeof(buf));
    p->pubsync();

    {
        char ch = 'x';
        Mybuf x1;
        x1.setp(&ch);

        Mybuf x2(x1);
        CHECK_PTR(x2.getp(), &ch);

        x1.setp(nullptr);
        x1 = x2;
        CHECK_PTR(x1.getp(), &ch);

        x2.setp(nullptr);
        x1.swap(x2);
        CHECK_PTR(x2.getp(), &ch);
    }

    try_mybuf();
}
