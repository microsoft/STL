// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test "cvt/wbuffer"
#define TEST_NAME "<wstring>"

#include "tdefs.h"
#include <cvt/utf8_utf16>
#include <cvt/wstring>
#include <iostream>
#include <string>

typedef stdext::cvt::codecvt_utf8_utf16<wchar_t> Mycvt;
typedef stdext::cvt::wstring_convert<Mycvt> Mystrcvt;

void test_from_bytes() { // test conversions from bytes to wide
    Mystrcvt strcvt;
    STD string str("hello");
    const char* ptr = &*str.begin();

    CHECK_INT(strcvt.converted(), 0);
    CHECK_WSTR(strcvt.from_bytes('x').c_str(), L"x");
    CHECK_INT(strcvt.converted(), 1);
    CHECK_WSTR(strcvt.from_bytes("abc").c_str(), L"abc");
    CHECK_INT(strcvt.converted(), 3);
    CHECK_WSTR(strcvt.from_bytes(str).c_str(), L"hello");
    CHECK_INT(strcvt.converted(), 5);
    CHECK_WSTR(strcvt.from_bytes(ptr + 1, ptr + 4).c_str(), L"ell");
    CHECK_INT(strcvt.converted(), 3);

    // check error handling
    const char* mesg = "from_bytes didn't throw";
    char bad         = (char) '\x80'; // not first byte

#if NO_EXCEPTIONS
#else // NO_EXCEPTIONS
    try { // convert a bad byte
        (void) strcvt.from_bytes(bad);
    } catch (STD range_error) { // expected behavior
        mesg = "ok";
    } catch (...) { // wrong exception
        mesg = "from_bytes threw wrong exception";
    }
    CHECK_STR(mesg, "ok");
#endif // NO_EXCEPTIONS

    Mystrcvt strcvt2("err", L"ERR");
    char bad2[] = {
        // embedded bad byte
        'a',
        'a',
        'a',
        'a',
        'a',
        'a',
        'a',
        'a',
        (char) '\x80', // not first byte
        'b',
        0,
    };

    CHECK_WSTR(strcvt2.from_bytes(bad2).c_str(), L"ERR");
}

void test_to_bytes() { // test wstring_convert from wide to bytes
    Mystrcvt strcvt;
    STD wstring wstr(L"hello");
    const wchar_t* wptr = &*wstr.begin();

    Mystrcvt::byte_string* pbs = (STD string*) 0;
    Mystrcvt::wide_string* pws = (STD wstring*) 0;
    Mystrcvt::state_type* pst  = (CSTD mbstate_t*) 0;
    Mystrcvt::int_type* pit    = (STD char_traits<wchar_t>::int_type*) 0;

    pbs = pbs; // to quiet diagnostics
    pws = pws;
    pst = pst;
    pit = pit;

    CHECK_INT(strcvt.converted(), 0);
    CHECK_STR(strcvt.to_bytes(L'x').c_str(), "x");
    CHECK_INT(strcvt.converted(), 1);
    CHECK_STR(strcvt.to_bytes(L"abc").c_str(), "abc");
    CHECK_INT(strcvt.converted(), 3);
    CHECK_STR(strcvt.to_bytes(wstr).c_str(), "hello");
    CHECK_INT(strcvt.converted(), 5);
    CHECK_STR(strcvt.to_bytes(wptr + 1, wptr + 4).c_str(), "ell");
    CHECK_INT(strcvt.converted(), 3);

    // check error handling
    const char* mesg = "to_bytes didn't throw";
    wchar_t bad[]    = {
           // bad two-word sequence
        (wchar_t) 0xd800, // first of two words
        (wchar_t) 0xd800, // not second word
        0,
    };

#if NO_EXCEPTIONS
#else // NO_EXCEPTIONS
    try { // convert a bad wchar_t sequence
        (void) strcvt.to_bytes(bad);
    } catch (STD range_error) { // expected behavior
        mesg = "ok";
    } catch (...) { // wrong exception
        mesg = "to_bytes threw wrong exception";
    }
    CHECK_STR(mesg, "ok");
#endif // NO_EXCEPTIONS

    Mystrcvt strcvt2("ERR");
    wchar_t bad2[] = {
        // embedded bad two-word sequence
        L'a',
        L'a',
        L'a',
        L'a',
        L'a',
        L'a',
        L'a',
        L'a',
        (wchar_t) 0xd800, // first of two words
        (wchar_t) 0xd800, // not second word
        L'b',
        0,
    };

    CHECK_STR(strcvt2.to_bytes(bad2).c_str(), "ERR");
}

void test_main() { // test wstring_convert
    static CSTD mbstate_t state;

    Mystrcvt strcvt0;
    Mystrcvt strcvt1(new Mycvt);
    Mystrcvt strcvt2(new Mycvt, state);
    state = strcvt0.state();

    test_from_bytes();
    test_to_bytes();
}
