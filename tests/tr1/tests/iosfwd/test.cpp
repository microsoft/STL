// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <iosfwd>
#define TEST_NAME "<iosfwd>"

#include "tdefs.h"
#include <cwchar>
#include <iosfwd>

#define STDx STD

void test_main() { // test basic workings of iosfwd declarations
    STD ios* p1       = (STD ios*) nullptr;
    STD streambuf* p2 = (STD streambuf*) nullptr;
    STD istream* p3   = (STD istream*) nullptr;
    STD ostream* p4   = (STD ostream*) nullptr;

    STD stringbuf* p6     = (STD stringbuf*) nullptr;
    STD istringstream* p7 = (STD istringstream*) nullptr;
    STD ostringstream* p8 = (STD ostringstream*) nullptr;

    STD filebuf* p10  = (STD filebuf*) nullptr;
    STD ifstream* p11 = (STD ifstream*) nullptr;
    STD ofstream* p12 = (STD ofstream*) nullptr;

    STD streampos* p14 = (STD streampos*) nullptr;

    p1  = p1; // to quiet diagnostics
    p2  = p2;
    p3  = p3;
    p4  = p4;
    p6  = p6;
    p7  = p7;
    p8  = p8;
    p10 = p10;
    p11 = p11;
    p12 = p12;
    p14 = p14;

    STD fpos<STDx mbstate_t>* p15 = (STD fpos<STDx mbstate_t>*) nullptr;

    p15 = p15;

    STD basic_ios<char>* tp1       = p1;
    STD basic_streambuf<char>* tp2 = p2;
    STD basic_istream<char>* tp3   = p3;
    STD basic_ostream<char>* tp4   = p4;

    STD basic_stringbuf<char>* tp6     = p6;
    STD basic_istringstream<char>* tp7 = p7;
    STD basic_ostringstream<char>* tp8 = p8;

    STD basic_filebuf<char>* tp10  = p10;
    STD basic_ifstream<char>* tp11 = p11;
    STD basic_ofstream<char>* tp12 = p12;

    tp1  = tp1; // to quiet diagnostics
    tp2  = tp2;
    tp3  = tp3;
    tp4  = tp4;
    tp6  = tp6;
    tp7  = tp7;
    tp8  = tp8;
    tp10 = tp10;
    tp11 = tp11;
    tp12 = tp12;

    STD wios* wp1       = (STD wios*) nullptr;
    STD wstreambuf* wp2 = (STD wstreambuf*) nullptr;
    STD wistream* wp3   = (STD wistream*) nullptr;
    STD wostream* wp4   = (STD wostream*) nullptr;

    STD wstringbuf* wp6     = (STD wstringbuf*) nullptr;
    STD wistringstream* wp7 = (STD wistringstream*) nullptr;
    STD wostringstream* wp8 = (STD wostringstream*) nullptr;

    STD wfilebuf* wp10  = (STD wfilebuf*) nullptr;
    STD wifstream* wp11 = (STD wifstream*) nullptr;
    STD wofstream* wp12 = (STD wofstream*) nullptr;

    STD wstreampos* wp14 = (STD wstreampos*) nullptr;

    STD basic_ios<wchar_t>* twp1       = wp1;
    STD basic_streambuf<wchar_t>* twp2 = wp2;
    STD basic_istream<wchar_t>* twp3   = wp3;
    STD basic_ostream<wchar_t>* twp4   = wp4;

    STD basic_stringbuf<wchar_t>* twp6     = wp6;
    STD basic_istringstream<wchar_t>* twp7 = wp7;
    STD basic_ostringstream<wchar_t>* twp8 = wp8;

    STD basic_filebuf<wchar_t>* twp10  = wp10;
    STD basic_ifstream<wchar_t>* twp11 = wp11;
    STD basic_ofstream<wchar_t>* twp12 = wp12;

    STD char_traits<char>* tp20         = (STD char_traits<char>*) nullptr;
    STD char_traits<wchar_t>* tp21      = (STD char_traits<wchar_t>*) nullptr;
    STD char_traits<int>* tp22          = (STD char_traits<int>*) nullptr;
    STD allocator<float>* tp23          = (STD allocator<float>*) nullptr;
    STD istreambuf_iterator<char>* tp24 = (STD istreambuf_iterator<char>*) nullptr;
    STD ostreambuf_iterator<char>* tp25 = (STD ostreambuf_iterator<char>*) nullptr;

    wp14 = wp14; // to quiet diagnostics

    twp1  = twp1;
    twp2  = twp2;
    twp3  = twp3;
    twp4  = twp4;
    twp6  = twp6;
    twp7  = twp7;
    twp8  = twp8;
    twp10 = twp10;
    twp11 = twp11;
    twp12 = twp12;

    tp20 = tp20;
    tp21 = tp21;
    tp22 = tp22;
    tp23 = tp23;
    tp24 = tp24;
    tp25 = tp25;

    STD wiostream* wp5     = (STD wiostream*) nullptr;
    STD wstringstream* wp9 = (STD wstringstream*) nullptr;
    STD wfstream* wp13     = (STD wfstream*) nullptr;

    STD basic_iostream<wchar_t>* twp5     = wp5;
    STD basic_stringstream<wchar_t>* twp9 = wp9;
    STD basic_fstream<wchar_t>* twp13     = wp13;

    twp5  = twp5; // to quiet diagnostics
    twp9  = twp9;
    twp13 = twp13;

    CHECK_MSG("<iosfwd> compiles", 1);
}
