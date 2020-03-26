// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "tdefs.h"
#include <cvt/utf8_utf16>

#define NCHARS   0x10000
#define MYWC_MAX 0x10ffff
#define MYFILE   "utf8_utf16"
#define MYNAME   stdext::cvt::codecvt_utf8_utf16<Mywchar>
#define MYMAKE   make_string

#include <string>

_STD wstring make_string() { // make a valid UTF-16 sequence
    _STD wstring mystring;
    unsigned long ch = 0;

    for (; ch < 0xd800; ++ch) {
        mystring.push_back(static_cast<wchar_t>(ch));
    }
    for (; ch < 0xdc00; ++ch) { // add a valid pair
        mystring.push_back(static_cast<wchar_t>(ch));
        mystring.push_back(static_cast<wchar_t>(ch + 0x400));
    }
    for (; ch < 0xffff; ++ch) {
        mystring.push_back(static_cast<wchar_t>(ch));
    }
    if (ch != static_cast<unsigned long>(WEOF)) {
        mystring.push_back(static_cast<wchar_t>(ch));
    }
    return mystring;
}

#include <cvt_xtest.h>
