// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "tdefs.h"
#include <cvt/sjis_0208>
#include <cvt/wstring>
#include <iostream>
#include <string>

#define MYFILE  "sjis_0208"
#define MYNAME  stdext::cvt::codecvt_sjis_0208<wchar_t>
#define MYMAKE  mymake
#define MYWCHAR wchar_t

using namespace std;

using Myistream  = basic_istream<MYWCHAR>;
using Myostream  = basic_ostream<MYWCHAR>;
using Mystring   = basic_string<MYWCHAR>;
using Mysconvert = stdext::cvt::wstring_convert<MYNAME, MYWCHAR>;

Mystring mymake() {
    Mystring mystring;
    Mysconvert myconv("");

    // All ASCII and modified ASCII characters single byte characters
    for (unsigned long ch = 0; ch <= 0x7f; ++ch) {
        string buf = myconv.to_bytes((MYWCHAR) ch);
        if (0 < buf.size() && ch != static_cast<unsigned long>(WEOF)) {
            mystring.insert(mystring.end(), (MYWCHAR) ch);
        }
    }

    // All single byte katakana
    for (unsigned long ch = 0xa1; ch <= 0xe0; ++ch) {
        string buf = myconv.to_bytes((MYWCHAR) ch);
        if (0 < buf.size() && ch != static_cast<unsigned long>(WEOF)) {
            mystring.insert(mystring.end(), (MYWCHAR) ch);
        }
    }

    // First set of two byte characters with odd first bytes
    for (unsigned long ch = 0x8100; ch <= 0xa000; ch += 0x0200) {
        for (unsigned long offset = 0x40; offset < 0x7f; ++offset) {
            string buf = myconv.to_bytes((MYWCHAR)(ch + offset));
            if (0 < buf.size() && (ch + offset) != static_cast<unsigned long>(WEOF)) {
                mystring.insert(mystring.end(), (MYWCHAR)(ch + offset));
            }
        }

        for (unsigned long offset = 0x8A; offset < 0x9f; ++offset) {
            string buf = myconv.to_bytes((MYWCHAR)(ch + offset));
            if (0 < buf.size() && (ch + offset) != static_cast<unsigned long>(WEOF)) {
                mystring.insert(mystring.end(), (MYWCHAR)(ch + offset));
            }
        }
    }

    // First set of two byte characters with even first bytes
    for (unsigned long ch = 0x8200; ch <= 0xa000; ch += 0x0200) {
        for (unsigned long offset = 0x9f; offset < 0xfd; ++offset) {
            string buf = myconv.to_bytes((MYWCHAR)(ch + offset));
            if (0 < buf.size() && (ch + offset) != static_cast<unsigned long>(WEOF)) {
                mystring.insert(mystring.end(), (MYWCHAR)(ch + offset));
            }
        }
    }

    // Second set of two byte characters with odd first bytes
    for (unsigned long ch = 0xe100; ch <= 0xf000; ch += 0x0200) {
        for (unsigned long offset = 0x40; offset < 0x7f; ++offset) {
            string buf = myconv.to_bytes((MYWCHAR)(ch + offset));
            if (0 < buf.size() && (ch + offset) != static_cast<unsigned long>(WEOF)) {
                mystring.insert(mystring.end(), (MYWCHAR)(ch + offset));
            }
        }

        for (unsigned long offset = 0x8A; offset < 0x9f; ++offset) {
            string buf = myconv.to_bytes((MYWCHAR)(ch + offset));
            if (0 < buf.size() && (ch + offset) != static_cast<unsigned long>(WEOF)) {
                mystring.insert(mystring.end(), (MYWCHAR)(ch + offset));
            }
        }
    }

    // Second set of two byte characters with even first bytes
    for (unsigned long ch = 0xe000; ch <= 0xf000; ch += 0x0200) {
        for (unsigned long offset = 0x9f; offset < 0xfd; ++offset) {
            string buf = myconv.to_bytes((MYWCHAR)(ch + offset));
            if (0 < buf.size() && (ch + offset) != static_cast<unsigned long>(WEOF)) {
                mystring.insert(mystring.end(), (MYWCHAR)(ch + offset));
            }
        }
    }

    return mystring;
}

#include <cvt_xtest.h>
