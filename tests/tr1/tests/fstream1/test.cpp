// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <fstream>, part 1
#define TEST_NAME "<fstream>, part 1"

#include "tdefs.h"
#include <assert.h>
#include <fstream>
#include <string>

#undef tmpnam
#define tmpnam(x) _tempnam(".", "")

void test_main() { // test basic workings of char fstream definitions
    const char* tn = CSTD tmpnam(0);

    assert(tn != nullptr);

    STD string tn_str(tn);

    // test output file opening
    STD ofstream ofs(tn);
    CHECK(ofs.is_open());
    ofs.close();

    STD ofstream ofs_str(tn_str);
    CHECK(ofs_str.is_open());
    ofs_str.close();
    ofs_str.open(STD string(tn));
    ofs_str.close();

    CHECK(!ofs.is_open());
    CHECK(!ofs.fail());
    ofs.close();
    CHECK(ofs.fail());
    CHECK(ofs.rdbuf()->close() == nullptr);

    {
        typedef STD filebuf Mycont;
        Mycont v6;
        CHECK(v6.open(tn, STD ios::in | STD ios::out) == &v6);
        Mycont v7(STD move(v6));
        CHECK(!v6.is_open());
        CHECK(v7.is_open());

        Mycont v8;
        v8 = STD move(v7);
        CHECK(!v7.is_open());
        CHECK(v8.is_open());
    }

    // test output file operations
    ofs.clear();
    ofs.open(tn, STD ios::out | STD ios::trunc);
    CHECK(ofs.is_open());
    CHECK(ofs.rdbuf()->is_open());
    ofs << "this is a test" << STD endl;
    ofs.close();
    CHECK(!ofs.is_open());
    CHECK(ofs.good());
    CHECK(ofs.rdbuf()->open(tn, STD ios::app | STD ios::out) != nullptr);
    ofs << "this is only a test" << STD endl;
    ofs.close();
    CHECK(!ofs.is_open());
    CHECK(ofs.good());

    {
        typedef STD ofstream Mycont;
        Mycont v6;
        v6.open(tn, STD ios::in | STD ios::out);
        CHECK(v6.is_open());
        Mycont v7(STD move(v6));
        CHECK(!v6.is_open());
        CHECK(v7.is_open());

        Mycont v8;
        v8 = STD move(v7);
        CHECK(!v7.is_open());
        CHECK(v8.is_open());
    }

    { // test ofstream typedefs
        STD ofstream::char_type* pc   = (char*) nullptr;
        STD ofstream::int_type* pi    = (int*) nullptr;
        STD ofstream::pos_type* pp    = (STD streampos*) nullptr;
        STD ofstream::off_type* po    = (STD streamoff*) nullptr;
        STD ofstream::traits_type* pt = (STD char_traits<char>*) nullptr;

        pc = pc; // to quiet diagnostics
        pi = pi;
        pp = pp;
        po = po;
        pt = pt;
    }

    // test input file opening
    STD ifstream ifs(tn);
    CHECK(ifs.is_open());
    ifs.close();

    STD ifstream ifs_str(tn_str);
    CHECK(ifs_str.is_open());
    ifs_str.close();
    ifs_str.open(STD string(tn));
    ifs_str.close();

    CHECK(!ifs.is_open());
    CHECK(!ifs.fail());
    ifs.close();
    CHECK(ifs.fail());
    CHECK(ifs.rdbuf()->close() == nullptr);

    // test input file operations
    char buf[50];

    ifs.clear();
    ifs.open(tn, STD ios::in);
    CHECK(ifs.is_open());
    CHECK(ifs.rdbuf()->is_open());
    ifs.getline(buf, sizeof(buf));
    CHECK_STR(buf, "this is a test");

    STD streampos p1 = ifs.rdbuf()->pubseekoff(0, STD ios::cur);
    ifs.getline(buf, sizeof(buf));
    CHECK_STR(buf, "this is only a test");
    CHECK(ifs.rdbuf()->pubseekpos(p1) == p1);
    ifs.getline(buf, sizeof(buf));
    CHECK_STR(buf, "this is only a test");
    ifs.rdbuf()->pubseekoff(0, STD ios::beg);
    ifs.getline(buf, sizeof(buf));
    CHECK_STR(buf, "this is a test");
    ifs.close();
    CHECK(!ifs.is_open());
    CHECK(ifs.good());

    {
        typedef STD ifstream Mycont;
        Mycont v6;
        v6.open(tn, STD ios::in | STD ios::out);
        CHECK(v6.is_open());
        Mycont v7(STD move(v6));
        CHECK(!v6.is_open());
        CHECK(v7.is_open());

        Mycont v8;
        v8 = STD move(v7);
        CHECK(!v7.is_open());
        CHECK(v8.is_open());
    }

    { // test istream typedefs
        STD ifstream::char_type* pc   = (char*) nullptr;
        STD ifstream::int_type* pi    = (int*) nullptr;
        STD ifstream::pos_type* pp    = (STD streampos*) nullptr;
        STD ifstream::off_type* po    = (STD streamoff*) nullptr;
        STD ifstream::traits_type* pt = (STD char_traits<char>*) nullptr;

        pc = pc; // to quiet diagnostics
        pi = pi;
        pp = pp;
        po = po;
        pt = pt;
    }

    // test combined file operations
    STD filebuf fb;

    fb.open(tn_str, STD ios::in | STD ios::out);
    fb.close();

    STD istream nifs(&fb);
    STD ostream nofs(nifs.rdbuf());
    STD streambuf* psb = &fb;
    STD istream* pis   = &nifs;
    STD ostream* pos   = &nofs;

    psb = psb; // to quiet diagnostics
    pis = pis;
    pos = pos;

    CHECK(!fb.is_open());
    CHECK(fb.open(tn, STD ios::in | STD ios::out) == &fb);
    CHECK(fb.is_open());
    CHECK(nifs.good());
    CHECK(nofs.good());
    nifs.rdbuf()->pubseekoff(0, STD ios::end);
    nofs << "this is still just a test" << STD endl;
    nifs.rdbuf()->pubseekoff(0, STD ios::beg);
    nifs.getline(buf, sizeof(buf));
    CHECK_STR(buf, "this is a test");
    nifs.getline(buf, sizeof(buf));
    CHECK_STR(buf, "this is only a test");
    nifs.getline(buf, sizeof(buf));
    CHECK_STR(buf, "this is still just a test");
    CHECK(fb.close() == &fb);

    STD ofstream nnofs(tn, STD ios::in | STD ios::out | STD ios::ate);
    CHECK(nnofs.is_open());
    nnofs << "one last test" << STD endl;
    nnofs.close();

    {
        typedef STD fstream Mycont;
        Mycont v6;
        v6.open(tn, STD ios::in | STD ios::out);
        CHECK(v6.is_open());
        Mycont v7(STD move(v6));
        CHECK(!v6.is_open());
        CHECK(v7.is_open());

        Mycont v8;
        v8 = STD move(v7);
        CHECK(!v7.is_open());
        CHECK(v8.is_open());
    }

    { // test filebuf typedefs
        STD filebuf::char_type* pc   = (char*) nullptr;
        STD filebuf::int_type* pi    = (int*) nullptr;
        STD filebuf::pos_type* pp    = (STD streampos*) nullptr;
        STD filebuf::off_type* po    = (STD streamoff*) nullptr;
        STD filebuf::traits_type* pt = (STD char_traits<char>*) nullptr;

        pc = pc; // to quiet diagnostics
        pi = pi;
        pp = pp;
        po = po;
        pt = pt;
    }

    { // test fstream
        STD fstream nfs(tn);
        CHECK(nfs.is_open());
        nfs.close();

        STD fstream nfs_str(tn_str);
        CHECK(nfs_str.is_open());
        nfs_str.close();
        nfs_str.open(STD string(tn));
        nfs_str.close();

        pis = &nfs;
        pos = &nfs;

        nfs.open(tn);
        CHECK(nfs.is_open());
        CHECK(nfs.good());
        nfs.rdbuf()->pubseekoff(0, STD ios::end);
        nfs << "one more last test" << STD endl;
        nfs.rdbuf()->pubseekoff(0, STD ios::beg);
        nfs.getline(buf, sizeof(buf));
        CHECK_STR(buf, "this is a test");
        nfs.getline(buf, sizeof(buf));
        CHECK_STR(buf, "this is only a test");
        nfs.getline(buf, sizeof(buf));
        CHECK_STR(buf, "this is still just a test");
        nfs.getline(buf, sizeof(buf));
        CHECK_STR(buf, "one last test");
        nfs.getline(buf, sizeof(buf));
        CHECK_STR(buf, "one more last test");

        STD basic_filebuf<char>* pfb1                        = &fb;
        STD basic_filebuf<char, STD char_traits<char>>* pfb2 = pfb1;
        pfb2                                                 = pfb2;

        STD basic_fstream<char>* pf1                        = &nfs;
        STD basic_fstream<char, STD char_traits<char>>* pf2 = pf1;
        pf2                                                 = pf2;

        STD basic_ifstream<char>* pif1                        = &ifs;
        STD basic_ifstream<char, STD char_traits<char>>* pif2 = pif1;
        pif2                                                  = pif2;

        STD basic_ofstream<char>* pof1                        = &ofs;
        STD basic_ofstream<char, STD char_traits<char>>* pof2 = pof1;
        pof2                                                  = pof2;
    }

    { // test fstream typedefs
        STD fstream::char_type* pc   = (char*) nullptr;
        STD fstream::int_type* pi    = (int*) nullptr;
        STD fstream::pos_type* pp    = (STD streampos*) nullptr;
        STD fstream::off_type* po    = (STD streamoff*) nullptr;
        STD fstream::traits_type* pt = (STD char_traits<char>*) nullptr;

        pc = pc; // to quiet diagnostics
        pi = pi;
        pp = pp;
        po = po;
        pt = pt;
    }

    CSTD remove(tn);
}
