// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test a codecvt facet
#pragma once
#ifndef _CVT_XTEST_
#define _CVT_XTEST_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#include <cstdlib>
#include <cvt/wbuffer>
#include <cvt/wstring>
#include <fstream>
#include <iostream>
#include <string>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

#ifndef MYNAME
#error do not include this header directly
#endif // MYNAME


using namespace std;

// define macros if freestanding
#ifndef SUMMARIZE
#define CHECK(x)
#define CHECK_INT(x, y)
#define CHECK_MSG(msg, ok)

int leave_chk(const char* fname) { // display message and quit
    cout << "#PASSED: " << fname << endl;
    return 0;
}
#endif // SUMMARIZE

#ifndef NCHARS
#define NCHARS 0x100
#endif // NCHARS

#ifndef MYWCHAR
#define MYWCHAR wchar_t
#define Mywchar MYWCHAR
#endif // MYWCHAR

#ifndef MYWC_MAX
#define MYWCMAX 0xffff
#endif // MYWC_MAX

using Myistream  = basic_istream<MYWCHAR>;
using Myostream  = basic_ostream<MYWCHAR>;
using Mystring   = basic_string<MYWCHAR>;
using Mysconvert = stdext::cvt::wstring_convert<MYNAME, MYWCHAR>;

using Mybfilebuf = basic_filebuf<char>;

bool test_write(const char* fname, const Mystring& mystring) { // write sequences of wide chars to file
    Mybfilebuf mybbuf;

    mybbuf.open(fname, ios_base::binary | ios_base::out);
    if (!mybbuf.is_open()) { // open failed, give up
#ifndef TERSE
        cout << "can't write to " << fname << endl;
#endif // TERSE

        CHECK_MSG("can't open file for writing", 0);
        return false;
    }

    stdext::cvt::wbuffer_convert<MYNAME, MYWCHAR> mybuf(&mybbuf);
    Myostream mystr(&mybuf);

    for (size_t idx = 0; idx < mystring.size(); ++idx) {
        if (mystr.write(&mystring[idx], 1)) {
            CHECK(1);
        } else { // write failed, quit
#ifndef TERSE
            cout << hex << "write failed for " << static_cast<unsigned long>(mystring[idx]) << endl;
#endif // TERSE

            CHECK_INT(static_cast<int>(idx), -1);
            return false;
        }
    }

    return true;
}

bool test_read(const char* fname, const Mystring& mystring) { // read sequences of wide chars from file
    Mybfilebuf mybbuf;

    mybbuf.open(fname, ios_base::binary | ios_base::in);
    if (!mybbuf.is_open()) { // open failed, give up
#ifndef TERSE
        cout << "can't read from " << fname << endl;
#endif // TERSE

        CHECK_MSG("can't open file for reading", 0);
        return false;
    }

    stdext::cvt::wbuffer_convert<MYNAME, MYWCHAR> mybuf(&mybbuf);
    Myistream mystr(&mybuf);

    MYWCHAR ch;
    for (size_t idx = 0; idx < mystring.size(); ++idx) { // read a wide char and test for expected value
        ch = (MYWCHAR)(-1);
        if (mystr.read(&ch, 1) && ch == mystring[idx]) {
            CHECK(1);
        } else { // read failed, quit
#ifndef TERSE
            cout << hex << "read failed for " << static_cast<unsigned long>(mystring[idx]) << ", got "
                 << static_cast<unsigned long>(ch) << endl;
#endif // TERSE

            CHECK_INT(static_cast<int>(idx), -1);
            return false;
        }
    }
    if (mystr.read(&ch, 1)) { // read too much, complain
        CHECK_MSG("end of file not reached", 0);
        return false;
    }
    return true;
}

void test_other() { // test other codecvt member functions
    MYNAME* p = new MYNAME;

    CHECK(!p->always_noconv());
    CHECK(0 < p->max_length());
    CHECK(-1 <= p->encoding() && p->encoding() <= p->max_length());

    const char arr[] = "ab";
    static mbstate_t state0;
    mbstate_t state = state0;
    int len         = p->length(state, &arr[0], &arr[2], 2 * p->max_length());
    CHECK(0 <= len && len <= 2 * p->max_length());

    delete p;
}

int main() { // write a file and read it back
    const char* fname = "testfile.txt";
    Mystring mystring;

#if defined(MYMAKE)
    mystring = MYMAKE();

#else // defined(MYMAKE)
    Mysconvert myconv(""); // don't throw on bad codes

    for (unsigned long ch = 0; ch <= MYWC_MAX && mystring.size() < NCHARS; ++ch) { // add a wide character if valid
        string buf = myconv.to_bytes((MYWCHAR) ch);
        if (0 < buf.size() && ch != static_cast<unsigned long>(WEOF)) {
            mystring.insert(mystring.end(), (MYWCHAR) ch);
        }
    }
#endif // defined(MYMAKE)

    if (test_write(fname, mystring)) {
        test_read(fname, mystring);
    }

    remove(fname);

    test_other();

    cout << "FINISHED testing \"cvt/" << MYFILE << "\"" << endl;
    return leave_chk(MYFILE ".cpp");
}
#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // _CVT_XTEST_
