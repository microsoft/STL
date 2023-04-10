// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <codecvt>
#define TEST_NAME "<codecvt>"

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#undef _ENFORCE_FACET_SPECIALIZATIONS
#define _ENFORCE_FACET_SPECIALIZATIONS 0

#include "tdefs.h"
#include <codecvt>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>

template <class Cvt>
void test_write(Cvt*, int maxval = 0x10ffff) { // test codecvt writes
    typedef typename Cvt::intern_type char_type;
    STD stringbuf ostrbuf;
    STD wbuffer_convert<Cvt, char_type> ocvtbuf(&ostrbuf);
    STD basic_ostream<char_type> sink(&ocvtbuf);

    int ch;
    for (ch = 1; ch <= maxval; ch <<= 1) {
        sink.put((char_type) ch);
    }

    STD stringbuf istrbuf(ostrbuf.str());
    STD wbuffer_convert<Cvt, char_type> icvtbuf(&istrbuf);
    STD basic_istream<char_type> source(&icvtbuf);
    for (ch = 1; ch <= maxval; ch <<= 1) {
        CHECK_INT(source.get(), ch);
    }
}

void test_main() { // test wbuffer_convert
    test_write((STD codecvt_utf8<unsigned int>*) nullptr);
    test_write((STD codecvt_utf16<unsigned int>*) nullptr);
    test_write((STD codecvt_utf8_utf16<unsigned short>*) nullptr, 0xffff);
}
