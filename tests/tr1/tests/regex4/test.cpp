// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <regex> header, part 4
#define TEST_NAME "<regex> interesting inputs, part 4"

#include "tdefs.h"
#include <regex>
#include <sstream>
#include <string.h>

#ifdef WIDE
#define T(x)                  L##x
#define xlen(x)               wcslen(x)
#define CHECKSTR(left, right) CHECK_WSTR(left, right)
typedef wchar_t CHR;
typedef STD wstring STDString;

#else // defined WIDE
#define T(x)                  x
#define xlen(x)               CSTD strlen(x)
#define CHECKSTR(left, right) CHECK_STR(left, right)
typedef char CHR;
typedef STD string STDString;
#endif // defined WIDE

typedef STD match_results<const CHR*> MyMr;
typedef STD basic_regex<CHR> MyRgx;

struct regex_testBool { // test parameters and results
    const CHR* strRegex;
    const CHR* strText;
    int len; // hard code the length
    int expectedOutput;
};

struct regex_testStr { // test parameters and results
    const CHR* strRegex;
    const CHR* strText;
    int len; // hard code the length
    const CHR* expectedOutput;
};

static const regex_testBool search_tests[] = {
    // tests
    {T(""), T(""), 0, 1},
    {T("\\0"), T("\0"), 1, 1},
    {T("\\0\\0\\0\\0\\0"), T("\0\0\0\0\0"), 5, 1},
    {T("\\0This string has a leading null!"), T("\0This string has a leading null!"), 32, 1},
    {T("This string has a trailing null!\\0"), T("This string has a trailing null!\0"), 33, 1},
    {T("There is a null to the right!\\0There is a null to the left!"),
        T("There is a null to the right!\0There is a null to the left!"), 58, 1},
};

static const regex_testBool match_tests[] = {
    // tests
    {T(""), T(""), 0, 1},
    {T("\\0"), T("\0"), 1, 1},
    {T("\\0\\0\\0\\0\\0"), T("\0\0\0\0\0"), 5, 1},
    {T("\\0This string has a leading null!"), T("\0This string has a leading null!"), 32, 1},
    {T("This string has a trailing null!\\0"), T("This string has a trailing null!\0"), 33, 1},
    {T("There is a null to the right!\\0There is a null to the left!"),
        T("There is a null to the right!\0There is a null to the left!"), 58, 1},
};

static const regex_testStr replace_tests[] = {
    // tests
    {T(""), T(""), 0, T("x")},
    {T("\\0"), T("\0"), 1, T("x")},
    {T("\\0\\0\\0\\0\\0"), T("\0\0\0\0\0"), 5, T("x")},
    {T("\\0This string has a leading null!"), T("\0This string has a leading null!"), 32, T("x")},
    {T("This string has a trailing null!\\0"), T("This string has a trailing null!\0"), 33, T("x")},
    {T("There is a null to the right!\\0There is a null to the left!"),
        T("There is a null to the right!\0There is a null to the left!"), 58, T("x")},
};

static void test_regex_search() {
    for (size_t i = 0; i < sizeof(search_tests) / sizeof(search_tests[0]); ++i) { // do one test
#if NO_EXCEPTIONS
        MyRgx re(search_tests[i].strRegex);
        MyMr match;
        CHECK_INT(STD regex_search((const CHR*) search_tests[i].strText, search_tests[i].strText + search_tests[i].len,
                      match, re, STD regex_constants::match_default),
            search_tests[i].expectedOutput);

#else // NO_EXCEPTIONS
        try { // do one test
            MyRgx re(search_tests[i].strRegex);
            MyMr match;
            CHECK_INT(STD regex_search((const CHR*) search_tests[i].strText,
                          search_tests[i].strText + search_tests[i].len, match, re, STD regex_constants::match_default),
                search_tests[i].expectedOutput);
        } catch (const STD regex_error&) { // catch invalid expression
            CHECK_INT(search_tests[i].expectedOutput, 0);
        }
#endif // NO_EXCEPTIONS
    }
}

static void test_regex_match() {
    for (size_t i = 0; i < sizeof(match_tests) / sizeof(match_tests[0]); ++i) { // do one test
#if NO_EXCEPTIONS
        MyRgx re(match_tests[i].strRegex);
        MyMr match;
        CHECK_INT(STD regex_match((const CHR*) match_tests[i].strText, match_tests[i].strText + match_tests[i].len,
                      match, re, STD regex_constants::match_default),
            match_tests[i].expectedOutput);

#else // NO_EXCEPTIONS
        try { // do one test
            MyRgx re(match_tests[i].strRegex);
            MyMr match;
            CHECK_INT(STD regex_match((const CHR*) match_tests[i].strText, match_tests[i].strText + match_tests[i].len,
                          match, re, STD regex_constants::match_default),
                match_tests[i].expectedOutput);
        } catch (const STD regex_error&) { // catch invalid expression
            CHECK_INT(match_tests[i].expectedOutput, 0);
        }
#endif // NO_EXCEPTIONS
    }
}

template <class T>
struct Myalloc {
    typedef T value_type;

    Myalloc() {}

    template <class U>
    Myalloc(const Myalloc<U>&) {}

    T* allocate(size_t n) {
        return STD allocator<T>{}.allocate(n);
    }

    void deallocate(T* p, size_t n) {
        STD allocator<T>{}.deallocate(p, n);
    }

    template <class U>
    bool operator==(const Myalloc<U>&) const {
        return true;
    }

    template <class U>
    bool operator!=(const Myalloc<U>&) const {
        return false;
    }
};

static void test_regex_replace() { // test function regex_replace
    CHR out[100];
    STD basic_string<char, STD char_traits<char>, Myalloc<char>> fmt(T("x"));

    for (size_t i = 0; i < sizeof(replace_tests) / sizeof(replace_tests[0]); ++i) { // do one test
#if NO_EXCEPTIONS
        MyRgx re(replace_tests[i].strRegex);
        CHR* res = STD regex_replace(out, (const CHR*) replace_tests[i].strText,
            replace_tests[i].strText + replace_tests[i].len, re, fmt, STD regex_constants::match_default);
        *res     = '\0';
        CHECKSTR(out, replace_tests[i].expectedOutput);

#else // NO_EXCEPTIONS
        try { // do one test
            MyRgx re(replace_tests[i].strRegex);
            CHR* res = STD regex_replace(out, (const CHR*) replace_tests[i].strText,
                replace_tests[i].strText + replace_tests[i].len, re, fmt, STD regex_constants::match_default);
            *res     = '\0';
            CHECKSTR(out, replace_tests[i].expectedOutput);
        } catch (const STD regex_error&) { // catch invalid expression
            CHECKSTR(replace_tests[i].expectedOutput, T("0"));
        }
#endif // NO_EXCEPTIONS
    }
}

void test_main() { // test header <regex>
    test_regex_search();
    test_regex_match();
    test_regex_replace();
}
