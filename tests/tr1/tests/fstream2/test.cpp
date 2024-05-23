// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <fstream>, part 2
#define TEST_NAME "<fstream>, part 2"

#include "tdefs.h"
#include <assert.h>
#include <fstream>
#include <wchar.h>

#include <temp_file_name.hpp>

void test_main() { // test basic workings of wide fstream definitions
    const auto temp_name = temp_file_name();
    const char* tn       = temp_name.c_str();

    // test output file opening
    STD wofstream ofs(tn);
    CHECK(ofs.is_open());
    ofs.close();

    // test input file opening
    STD wifstream ifs(tn);
    CHECK(ifs.is_open());
    ifs.close();

    // test closed file closing
    CHECK(!ifs.is_open());
    CHECK(!ifs.fail());
    ifs.close();
    CHECK(ifs.fail());
    CHECK(ifs.rdbuf()->close() == nullptr);
    CHECK(!ofs.is_open());
    CHECK(!ofs.fail());
    ofs.close();
    CHECK(ofs.fail());
    CHECK(ofs.rdbuf()->close() == nullptr);

    {
        typedef STD wfilebuf Mycont;
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
    ofs.clear(), ofs.open(tn, STD ios::out | STD ios::trunc);
    CHECK(ofs.is_open());
    CHECK(ofs.rdbuf()->is_open());
    ofs << L"this is a test" << STD endl;
    ofs.close();
    CHECK(!ofs.is_open());
    CHECK(ofs.good());
    CHECK(ofs.rdbuf()->open(tn, STD ios::app | STD ios::out) != nullptr);
    ofs << L"this is only a test" << STD endl;
    ofs.close();
    CHECK(!ofs.is_open());
    CHECK(ofs.good());

    {
        typedef STD wofstream Mycont;
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

    { // test wofstream typedefs
        STD wofstream::char_type* pc   = (wchar_t*) nullptr;
        STD wofstream::int_type* pi    = (CSTD wint_t*) nullptr;
        STD wofstream::pos_type* pp    = (STD wstreampos*) nullptr;
        STD wofstream::off_type* po    = (STD streamoff*) nullptr;
        STD wofstream::traits_type* pt = (STD char_traits<wchar_t>*) nullptr;

        pc = pc; // to quiet diagnostics
        pi = pi;
        pp = pp;
        po = po;
        pt = pt;
    }

    // test input file operations
    wchar_t buf[50];

    ifs.clear(), ifs.open(tn, STD ios::in);
    CHECK(ifs.is_open());
    CHECK(ifs.rdbuf()->is_open());
    ifs.getline(buf, sizeof(buf) / sizeof(buf[0]));
    CHECK_WSTR(buf, L"this is a test");

    STD streampos p1 = ifs.rdbuf()->pubseekoff(0, STD ios::cur);
    ifs.getline(buf, sizeof(buf) / sizeof(buf[0]));
    CHECK_WSTR(buf, L"this is only a test");
    CHECK(ifs.rdbuf()->pubseekpos(p1) == p1);
    ifs.getline(buf, sizeof(buf) / sizeof(buf[0]));
    CHECK_WSTR(buf, L"this is only a test");
    ifs.rdbuf()->pubseekoff(0, STD ios::beg);
    ifs.getline(buf, sizeof(buf) / sizeof(buf[0]));
    CHECK_WSTR(buf, L"this is a test");
    ifs.close();
    CHECK(!ifs.is_open());
    CHECK(ifs.good());

    {
        typedef STD wifstream Mycont;
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
        STD wifstream::char_type* pc   = (wchar_t*) nullptr;
        STD wifstream::int_type* pi    = (CSTD wint_t*) nullptr;
        STD wifstream::pos_type* pp    = (STD wstreampos*) nullptr;
        STD wifstream::off_type* po    = (STD streamoff*) nullptr;
        STD wifstream::traits_type* pt = (STD char_traits<wchar_t>*) nullptr;

        pc = pc; // to quiet diagnostics
        pi = pi;
        pp = pp;
        po = po;
        pt = pt;
    }

    // test combined file operations
    STD wfilebuf fb;
    STD wistream nifs(&fb);
    STD wostream nofs(nifs.rdbuf());
    STD wstreambuf* psb = &fb;
    STD wistream* pis   = &nifs;
    STD wostream* pos   = &nofs;

    psb = psb; // to quiet diagnostics
    pis = pis;
    pos = pos;

    CHECK(!fb.is_open());
    CHECK(fb.open(tn, STD ios::in | STD ios::out) == &fb);
    CHECK(fb.is_open());
    CHECK(nifs.good());
    CHECK(nofs.good());
    nifs.rdbuf()->pubseekoff(0, STD ios::end);
    nofs << L"this is still just a test" << STD endl;
    nifs.rdbuf()->pubseekoff(0, STD ios::beg);
    nifs.getline(buf, sizeof(buf) / sizeof(buf[0]));
    CHECK_WSTR(buf, L"this is a test");
    nifs.getline(buf, sizeof(buf) / sizeof(buf[0]));
    CHECK_WSTR(buf, L"this is only a test");
    nifs.getline(buf, sizeof(buf) / sizeof(buf[0]));
    CHECK_WSTR(buf, L"this is still just a test");
    CHECK(fb.close() == &fb);

    STD wofstream nnofs(tn, STD ios::in | STD ios::out | STD ios::ate);
    CHECK(nnofs.is_open());
    nnofs << L"one last test" << STD endl;
    nnofs.close();

    {
        typedef STD wfstream Mycont;
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

    { // test wfilebuf typedefs
        STD wfilebuf::char_type* pc   = (wchar_t*) nullptr;
        STD wfilebuf::int_type* pi    = (CSTD wint_t*) nullptr;
        STD wfilebuf::pos_type* pp    = (STD wstreampos*) nullptr;
        STD wfilebuf::off_type* po    = (STD streamoff*) nullptr;
        STD wfilebuf::traits_type* pt = (STD char_traits<wchar_t>*) nullptr;

        pc = pc; // to quiet diagnostics
        pi = pi;
        pp = pp;
        po = po;
        pt = pt;
    }

    { // test fstream and template equivalents
        STD wfstream nfs(tn);

        pis = &nfs;
        pos = &nfs;

        CHECK(nfs.is_open());
        CHECK(nfs.good());
        nfs.rdbuf()->pubseekoff(0, STD ios::end);
        nfs << L"one more last test" << STD endl;
        nfs.rdbuf()->pubseekoff(0, STD ios::beg);
        nfs.getline(buf, sizeof(buf) / sizeof(buf[0]));
        CHECK_WSTR(buf, L"this is a test");
        nfs.getline(buf, sizeof(buf) / sizeof(buf[0]));
        CHECK_WSTR(buf, L"this is only a test");
        nfs.getline(buf, sizeof(buf) / sizeof(buf[0]));
        CHECK_WSTR(buf, L"this is still just a test");
        nfs.getline(buf, sizeof(buf) / sizeof(buf[0]));
        CHECK_WSTR(buf, L"one last test");
        nfs.getline(buf, sizeof(buf) / sizeof(buf[0]));
        CHECK_WSTR(buf, L"one more last test");

        STD basic_filebuf<wchar_t>* pfb1                           = &fb;
        STD basic_filebuf<wchar_t, STD char_traits<wchar_t>>* pfb2 = pfb1;
        pfb2                                                       = pfb2;

        STD basic_fstream<wchar_t>* pf1                           = &nfs;
        STD basic_fstream<wchar_t, STD char_traits<wchar_t>>* pf2 = pf1;
        pf2                                                       = pf2;

        STD basic_ifstream<wchar_t>* pif1                           = &ifs;
        STD basic_ifstream<wchar_t, STD char_traits<wchar_t>>* pif2 = pif1;
        pif2                                                        = pif2;

        STD basic_ofstream<wchar_t>* pof1                           = &ofs;
        STD basic_ofstream<wchar_t, STD char_traits<wchar_t>>* pof2 = pof1;
        pof2                                                        = pof2;
    }

    { // test wfstream typedefs
        STD wfstream::char_type* pc   = (wchar_t*) nullptr;
        STD wfstream::int_type* pi    = (CSTD wint_t*) nullptr;
        STD wfstream::pos_type* pp    = (STD wstreampos*) nullptr;
        STD wfstream::off_type* po    = (STD streamoff*) nullptr;
        STD wfstream::traits_type* pt = (STD char_traits<wchar_t>*) nullptr;

        pc = pc; // to quiet diagnostics
        pi = pi;
        pp = pp;
        po = po;
        pt = pt;
    }

    CSTD remove(tn);
}
