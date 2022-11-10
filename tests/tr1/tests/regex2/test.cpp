// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <regex> header, part 2
#define TEST_NAME "<regex>, part 2"

#include "tdefs.h"
#include <regex>
#include <sstream>
#include <string.h>

#define ECMA     0x01
#define BASIC    0x02
#define EXTENDED 0x04
#define AWK      0x08
#define GREP     0x10
#define EGREP    0x20

#define BG       (BASIC | GREP)
#define EEA      (EXTENDED | EGREP | AWK)
#define ALL      (ECMA | BASIC | EXTENDED | AWK | GREP | EGREP)
#define NOT_ECMA (BASIC | EXTENDED | AWK | GREP | EGREP)
#define NOT_BG   (ECMA | EXTENDED | AWK | EGREP)
#define NOT_AWK  (ECMA | BASIC | EXTENDED | GREP | EGREP)

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

static int lang(STD regex_constants::syntax_option_type opt) { // determine dialect
    if ((opt & 0xff) == STD regex_constants::ECMAScript) {
        return ECMA;
    } else if ((opt & 0xff) == STD regex_constants::basic) {
        return BASIC;
    } else if ((opt & 0xff) == STD regex_constants::extended) {
        return EXTENDED;
    } else if ((opt & 0xff) == STD regex_constants::awk) {
        return AWK;
    } else if ((opt & 0xff) == STD regex_constants::grep) {
        return GREP;
    } else if ((opt & 0xff) == STD regex_constants::egrep) {
        return EGREP;
    } else {
        return 0;
    }
}

struct regex_test { // test parameters and results
    unsigned int line;
    const CHR* re;
    const CHR* txt;
    const char* res;
    unsigned int valid;
};

static const regex_test tests[] = {
    // tests

    // a few simple patterns
    {__LINE__, T(""), T(""), "1 0 0", ALL},
    {__LINE__, T(""), T("a"), "1 0 0", ALL},
    {__LINE__, T("a"), T(""), "0", ALL},
    {__LINE__, T("a"), T("a"), "1 0 1", ALL},
    {__LINE__, T("a"), T("ab"), "1 0 1", ALL},
    {__LINE__, T("a"), T("bab"), "1 1 2", ALL},
    {__LINE__, T("1"), T("1"), "1 0 1", ALL},
    {__LINE__, T("-"), T("-"), "1 0 1", ALL},
    {__LINE__, T(","), T(","), "1 0 1", ALL},
    {__LINE__, T("="), T("="), "1 0 1", ALL},
    {__LINE__, T(":"), T(":"), "1 0 1", ALL},
    {__LINE__, T("!"), T("!"), "1 0 1", ALL},

    // disjunction
    {__LINE__, T("a|b"), T("a|b"), "1 0 3", BG},
    {__LINE__, T("a|b"), T("a"), "1 0 1", NOT_BG},
    {__LINE__, T("a|b"), T("b"), "1 0 1", NOT_BG},

    {__LINE__, T("|"), T("|"), "1 0 1", BG},
    {__LINE__, T("|"), T(""), "1 0 0", NOT_BG},

    {__LINE__, T("a|"), T("a|"), "1 0 2", BG},
    {__LINE__, T("a|"), T(""), "1 0 0", NOT_BG},

    {__LINE__, T("|a"), T("|a"), "1 0 2", BG},
    {__LINE__, T("|a"), T(""), "1 0 0", NOT_BG},

    {__LINE__, T("||"), T("||"), "1 0 2", BG},
    {__LINE__, T("||"), T(""), "1 0 0", NOT_BG},

    {__LINE__, T("a||"), T(""), "1 0 0", NOT_BG},
    {__LINE__, T("a||"), T("a||"), "1 0 3", BG},

    {__LINE__, T("a|b|c"), T("a"), "1 0 1", NOT_BG},
    {__LINE__, T("a|b|c"), T("b"), "1 0 1", NOT_BG},
    {__LINE__, T("a|b|c"), T("c"), "1 0 1", NOT_BG},
    {__LINE__, T("a|(b|c)"), T("a"), "2 0 1 -1 -1", NOT_BG},
    {__LINE__, T("a|(b|c)"), T("b"), "2 0 1 0 1", NOT_BG},
    {__LINE__, T("a|(b|c)"), T("c"), "2 0 1 0 1", NOT_BG},
    {__LINE__, T("(a|b)|c"), T("a"), "2 0 1 0 1", NOT_BG},
    {__LINE__, T("(a|b)|c"), T("b"), "2 0 1 0 1", NOT_BG},
    {__LINE__, T("(a|b)|c"), T("c"), "2 0 1 -1 -1", NOT_BG},

    {__LINE__, T("a\nb"), T("a"), "1 0 1", GREP | EGREP},
    {__LINE__, T("a\nb"), T("b"), "1 0 1", GREP | EGREP},
    {__LINE__, T("a\nb"), T("a\nb"), "1 0 3", BASIC | ECMA | EXTENDED | AWK},

    // assertions
    {__LINE__, T("^"), T(""), "1 0 0", ALL},
    {__LINE__, T("$"), T(""), "1 0 0", ALL},

    {__LINE__, T("^a"), T("a"), "1 0 1", ALL},
    {__LINE__, T("^a"), T("^a"), "0", ALL},
    {__LINE__, T("^a"), T("ba"), "0", ALL},

    {__LINE__, T("a^"), T("a"), "0", ALL},

    {__LINE__, T("$a"), T("a"), "0", ALL},
    {__LINE__, T("$a"), T("$a"), "0", NOT_BG},
    {__LINE__, T("$a"), T("$a"), "1 0 2", BG},

    {__LINE__, T("a$"), T("ba"), "1 1 2", ALL},
    {__LINE__, T("a$"), T("ab"), "0", ALL},

    {__LINE__, T("^a$"), T("b\na"), "1 2 3", ALL},

    {__LINE__, T("\\b"), T("a"), "1 0 0", ECMA},
    {__LINE__, T("\\b"), T(""), "-1", BASIC | GREP | EXTENDED | EGREP},
    {__LINE__, T("\\b"), T("\b"), "1 0 1", AWK},

    {__LINE__, T("a\\b"), T(" a "), "1 1 2", ECMA},
    {__LINE__, T("\\ba"), T(" a "), "1 1 2", ECMA},
    {__LINE__, T("\\b"), T(" "), "0", ECMA},
    {__LINE__, T("a\\b"), T("ab"), "0", ECMA},
    {__LINE__, T("\\B"), T("a"), "0", ECMA},
    {__LINE__, T("\\B"), T(""), "-1", NOT_ECMA},
    {__LINE__, T("a\\B"), T(" a "), "0", ECMA},
    {__LINE__, T("\\Ba"), T(" a "), "0", ECMA},
    {__LINE__, T("\\B"), T(" "), "1 0 0", ECMA},
    {__LINE__, T("a\\B"), T("ab"), "1 0 1", ECMA},

    // initial quantifier
    {__LINE__, T("*"), T("*"), "1 0 1", BG},
    {__LINE__, T("*"), T("a"), "0", BG},
    {__LINE__, T("*a"), T("*a"), "1 0 2", BG},
    {__LINE__, T("*"), T(""), "-1", NOT_BG},

    {__LINE__, T("\\(*\\)"), T("*"), "2 0 1 0 1", BG},
    {__LINE__, T("\\(*\\)"), T("a"), "0", BG},
    {__LINE__, T("(*)"), T(""), "-1", NOT_BG},

    {__LINE__, T("^*"), T("*"), "1 0 1", BG},
    {__LINE__, T("^*"), T("^*"), "0", BG},
    {__LINE__, T("^*"), T(""), "-1", NOT_BG},

    {__LINE__, T("+"), T("+"), "1 0 1", BG},
    {__LINE__, T("+"), T("a"), "0", BG},
    {__LINE__, T("+"), T(""), "-1", NOT_BG},

    {__LINE__, T("?"), T("?"), "1 0 1", BG},
    {__LINE__, T("?"), T("a"), "0", BG},
    {__LINE__, T("?"), T(""), "-1", NOT_BG},

    {__LINE__, T("{10}"), T("{10}"), "1 0 4", BG},
    {__LINE__, T("{10}a"), T("{10}a"), "1 0 5", BG},
    {__LINE__, T("{10}"), T(""), "-1", NOT_BG},

    {__LINE__, T("{10,}"), T("{10,}"), "1 0 5", BG},
    {__LINE__, T("{10,}a"), T("{10,}a"), "1 0 6", BG},
    {__LINE__, T("{10,}"), T(""), "-1", NOT_BG},

    {__LINE__, T("{10,12}"), T("{10,12}"), "1 0 7", BG},
    {__LINE__, T("{10,12}a"), T("{10,12}a"), "1 0 8", BG},
    {__LINE__, T("{10,12}"), T("aaaaaaaaaa"), "0", BG},
    {__LINE__, T("{10,12}"), T(""), "-1", NOT_BG},

    {__LINE__, T("*?"), T("*?"), "1 0 2", BG},
    {__LINE__, T("*?a"), T("*?a"), "1 0 3", BG},
    {__LINE__, T("*?"), T("**"), "0", BG},
    {__LINE__, T("*?"), T(""), "-1", NOT_BG},

    {__LINE__, T("(*a)"), T("a)"), "1 0 2", BG},
    {__LINE__, T("(*a)"), T("((a)"), "1 0 4", BG},
    {__LINE__, T("(*?a)"), T("?a)"), "1 0 3", BG},
    {__LINE__, T("(*?)"), T("?)"), "1 0 2", BG},
    {__LINE__, T("(*?)"), T("((?)"), "1 0 4", BG},
    {__LINE__, T("(*?a)"), T("((?a)"), "1 0 5", BG},
    {__LINE__, T("(*)"), T(""), "-1", NOT_BG},

    {__LINE__, T("+?"), T("+?"), "1 0 2", BG},
    {__LINE__, T("+?a"), T("+?a"), "1 0 3", BG},
    {__LINE__, T("+a"), T("+a"), "1 0 2", BG},
    {__LINE__, T("+?"), T(""), "-1", NOT_BG},

    {__LINE__, T("??"), T("??"), "1 0 2", BG},
    {__LINE__, T("?a"), T("?a"), "1 0 2", BG},
    {__LINE__, T("??a"), T("??a"), "1 0 3", BG},
    {__LINE__, T("??"), T(""), "-1", NOT_BG},

    {__LINE__, T("{10}?"), T("{10}?"), "1 0 5", BG},
    {__LINE__, T("{10}?a"), T("{10}?a"), "1 0 6", BG},
    {__LINE__, T("{10}?"), T(""), "-1", NOT_BG},

    {__LINE__, T("{10,}?"), T("{10,}?"), "1 0 6", BG},
    {__LINE__, T("{10,}?a"), T("{10,}?a"), "1 0 7", BG},
    {__LINE__, T("{10,}?"), T(""), "-1", NOT_BG},

    {__LINE__, T("{10,12}?"), T("{10,12}?"), "1 0 8", BG},
    {__LINE__, T("{10,12}?a"), T("{10,12}?a"), "1 0 9", BG},
    {__LINE__, T("{10,12}?"), T(""), "-1", NOT_BG},

    // term quantifier
    {__LINE__, T("b*"), T(""), "1 0 0", ALL},
    {__LINE__, T("b*"), T("b"), "1 0 1", ALL},
    {__LINE__, T("b*"), T("bbbb"), "1 0 4", ALL},

    {__LINE__, T("b+"), T(""), "0", ALL},
    {__LINE__, T("b+"), T("b+"), "1 0 2", BG},
    {__LINE__, T("b+"), T("b"), "1 0 1", NOT_BG},
    {__LINE__, T("b+"), T("bbbb"), "1 0 4", NOT_BG},

    {__LINE__, T("b?"), T("b?"), "1 0 2", BG},
    {__LINE__, T("b?"), T(""), "1 0 0", NOT_BG},
    {__LINE__, T("b?"), T("b"), "1 0 1", NOT_BG},
    {__LINE__, T("b?"), T("bb"), "1 0 1", NOT_BG},

    {__LINE__, T("b{10}"), T("b{10}"), "1 0 5", BG},
    {__LINE__, T("b\\{10\\}"), T("bbbbbbbbb"), "0", BG},
    {__LINE__, T("b{10}"), T("bbbbbbbbb"), "0", NOT_BG},
    {__LINE__, T("b\\{10\\}"), T("bbbbbbbbbb"), "1 0 10", BG},
    {__LINE__, T("b{10}"), T("bbbbbbbbbb"), "1 0 10", NOT_BG},
    {__LINE__, T("b\\{10\\}"), T("bbbbbbbbbbb"), "1 0 10", BG},
    {__LINE__, T("b{10}"), T("bbbbbbbbbbb"), "1 0 10", NOT_BG},

    {__LINE__, T("b{10,}"), T("b{10,}"), "1 0 6", BG},
    {__LINE__, T("b\\{10,\\}"), T("bbbbbbbbb"), "0", BG},
    {__LINE__, T("b{10,}"), T("bbbbbbbbb"), "0", NOT_BG},
    {__LINE__, T("b\\{10,\\}"), T("bbbbbbbbbb"), "1 0 10", BG},
    {__LINE__, T("b{10,}"), T("bbbbbbbbbb"), "1 0 10", NOT_BG},
    {__LINE__, T("b\\{10,\\}"), T("bbbbbbbbbbb"), "1 0 11", BG},
    {__LINE__, T("b{10,}"), T("bbbbbbbbbbb"), "1 0 11", NOT_BG},

    {__LINE__, T("b{10,12}"), T("b{10,12}"), "1 0 8", BG},
    {__LINE__, T("b\\{10,12\\}"), T("bbbbbbbbb"), "0", BG},
    {__LINE__, T("b{10,12}"), T("bbbbbbbbb"), "0", NOT_BG},
    {__LINE__, T("b\\{10,12\\}"), T("bbbbbbbbbb"), "1 0 10", BG},
    {__LINE__, T("b{10,12}"), T("bbbbbbbbbb"), "1 0 10", NOT_BG},
    {__LINE__, T("b\\{10,12\\}"), T("bbbbbbbbbbb"), "1 0 11", BG},
    {__LINE__, T("b{10,12}"), T("bbbbbbbbbbb"), "1 0 11", NOT_BG},
    {__LINE__, T("b\\{10,12\\}"), T("bbbbbbbbbbbb"), "1 0 12", BG},
    {__LINE__, T("b{10,12}"), T("bbbbbbbbbbbb"), "1 0 12", NOT_BG},
    {__LINE__, T("b\\{10,12\\}"), T("bbbbbbbbbbbbb"), "1 0 12", BG},
    {__LINE__, T("b{10,12}"), T("bbbbbbbbbbbbb"), "1 0 12", NOT_BG},

    {__LINE__, T("b*?"), T(""), "1 0 0", ECMA},
    {__LINE__, T("b*?a"), T("a"), "1 0 1", ECMA},
    {__LINE__, T("b*?"), T("b"), "1 0 0", ECMA},
    {__LINE__, T("b*?a"), T("ba"), "1 0 2", ECMA},
    {__LINE__, T("b*?"), T("bb"), "1 0 0", ECMA},
    {__LINE__, T("b*?a"), T("bba"), "1 0 3", ECMA},
    {__LINE__, T("b*?"), T("?"), "1 0 1", BG},
    {__LINE__, T("b*?"), T(""), "-1", EEA},

    {__LINE__, T("b+?"), T(""), "0", ECMA},
    {__LINE__, T("b+?a"), T("a"), "0", ECMA},
    {__LINE__, T("b+?"), T("b"), "1 0 1", ECMA},
    {__LINE__, T("b+?a"), T("ba"), "1 0 2", ECMA},
    {__LINE__, T("b+?"), T("bb"), "1 0 1", ECMA},
    {__LINE__, T("b+?a"), T("bba"), "1 0 3", ECMA},
    {__LINE__, T("b+?"), T("b+?"), "1 0 3", BG},
    {__LINE__, T("b+?"), T(""), "-1", EEA},

    {__LINE__, T("b??"), T(""), "1 0 0", ECMA},
    {__LINE__, T("b??a"), T("a"), "1 0 1", ECMA},
    {__LINE__, T("b??"), T("b"), "1 0 0", ECMA},
    {__LINE__, T("b??a"), T("ba"), "1 0 2", ECMA},
    {__LINE__, T("b??"), T("bb"), "1 0 0", ECMA},
    {__LINE__, T("b??a"), T("bba"), "1 1 3", ECMA},
    {__LINE__, T("b??"), T("b??"), "1 0 3", BG},
    {__LINE__, T("b??"), T(""), "-1", EEA},

    {__LINE__, T("b{10}?"), T("bbbbbbbbb"), "0", ECMA},
    {__LINE__, T("b{10}?a"), T("bbbbbbbbba"), "0", ECMA},
    {__LINE__, T("b{10}?"), T("bbbbbbbbbb"), "1 0 10", ECMA},
    {__LINE__, T("b{10}?a"), T("bbbbbbbbbba"), "1 0 11", ECMA},
    {__LINE__, T("b{10}?"), T("bbbbbbbbbbb"), "1 0 10", ECMA},
    {__LINE__, T("b{10}?a"), T("bbbbbbbbbbba"), "1 1 12", ECMA},
    {__LINE__, T("b{10}?"), T("b{10}?"), "1 0 6", BG},
    {__LINE__, T("b\\{10\\}?"), T("bbbbbbbbbb?"), "1 0 11", BG},
    {__LINE__, T("b{10}?"), T(""), "-1", EEA},

    {__LINE__, T("b{10,}?"), T("bbbbbbbbb"), "0", ECMA},
    {__LINE__, T("b{10,}?a"), T("bbbbbbbbba"), "0", ECMA},
    {__LINE__, T("b{10,}?"), T("bbbbbbbbbb"), "1 0 10", ECMA},
    {__LINE__, T("b{10,}?a"), T("bbbbbbbbbba"), "1 0 11", ECMA},
    {__LINE__, T("b{10,}?"), T("bbbbbbbbbbb"), "1 0 10", ECMA},
    {__LINE__, T("b{10,}?a"), T("bbbbbbbbbbba"), "1 0 12", ECMA},
    {__LINE__, T("b{10,}?"), T("b{10,}?"), "1 0 7", BG},
    {__LINE__, T("b\\{10,\\}?"), T("bbbbbbbbbbb?"), "1 0 12", BG},
    {__LINE__, T("b{10,}?"), T(""), "-1", EEA},

    {__LINE__, T("b{10,12}?"), T("bbbbbbbbb"), "0", ECMA},
    {__LINE__, T("b{10,12}?a"), T("bbbbbbbbba"), "0", ECMA},
    {__LINE__, T("b{10,12}?"), T("bbbbbbbbbb"), "1 0 10", ECMA},
    {__LINE__, T("b{10,12}?a"), T("bbbbbbbbbba"), "1 0 11", ECMA},
    {__LINE__, T("b{10,12}?"), T("bbbbbbbbbbb"), "1 0 10", ECMA},
    {__LINE__, T("b{10,12}?a"), T("bbbbbbbbbbba"), "1 0 12", ECMA},
    {__LINE__, T("b{10,12}?a"), T("bbbbbbbbbbbba"), "1 0 13", ECMA},
    {__LINE__, T("b{10,12}?a"), T("bbbbbbbbbbbbba"), "1 1 14", ECMA},
    {__LINE__, T("b{10,12}?"), T("b{10,12}?"), "1 0 9", BG},
    {__LINE__, T("b\\{10,12\\}?"), T("bbbbbbbbbbb?"), "1 0 12", BG},
    {__LINE__, T("b{10,12}?"), T(""), "-1", EEA},

    {__LINE__, T("b*a"), T("a"), "1 0 1", ALL},
    {__LINE__, T("b*a"), T("ba"), "1 0 2", ALL},
    {__LINE__, T("b*a"), T("bbbba"), "1 0 5", ALL},

    {__LINE__, T("b+a"), T("b+a"), "1 0 3", BG},
    {__LINE__, T("b+a"), T("a"), "0", NOT_BG},
    {__LINE__, T("b+a"), T("ba"), "1 0 2", NOT_BG},
    {__LINE__, T("b+a"), T("bbbba"), "1 0 5", NOT_BG},

    {__LINE__, T("b?a"), T("b?a"), "1 0 3", BG},
    {__LINE__, T("b?a"), T("a"), "1 0 1", NOT_BG},
    {__LINE__, T("b?a"), T("ba"), "1 0 2", NOT_BG},
    {__LINE__, T("b?a"), T("bba"), "1 1 3", NOT_BG},

    {__LINE__, T("b{10}a"), T("b{10}a"), "1 0 6", BG},
    {__LINE__, T("b\\{10\\}a"), T("bbbbbbbbba"), "0", BG},
    {__LINE__, T("b{10}a"), T("bbbbbbbbba"), "0", NOT_BG},
    {__LINE__, T("b\\{10\\}a"), T("bbbbbbbbbba"), "1 0 11", BG},
    {__LINE__, T("b{10}a"), T("bbbbbbbbbba"), "1 0 11", NOT_BG},
    {__LINE__, T("b\\{10\\}a"), T("bbbbbbbbbbba"), "1 1 12", BG},
    {__LINE__, T("b{10}a"), T("bbbbbbbbbbba"), "1 1 12", NOT_BG},

    {__LINE__, T("b{10,}a"), T("b{10,}a"), "1 0 7", BG},
    {__LINE__, T("b\\{10,\\}a"), T("bbbbbbbbba"), "0", BG},
    {__LINE__, T("b{10,}a"), T("bbbbbbbbba"), "0", NOT_BG},
    {__LINE__, T("b\\{10,\\}a"), T("bbbbbbbbbba"), "1 0 11", BG},
    {__LINE__, T("b{10,}a"), T("bbbbbbbbbba"), "1 0 11", NOT_BG},
    {__LINE__, T("b\\{10,\\}a"), T("bbbbbbbbbbba"), "1 0 12", BG},
    {__LINE__, T("b{10,}a"), T("bbbbbbbbbbba"), "1 0 12", NOT_BG},

    {__LINE__, T("b\\{10,12\\}a"), T("bbbbbbbbba"), "0", BG},
    {__LINE__, T("b{10,12}a"), T("bbbbbbbbba"), "0", NOT_BG},
    {__LINE__, T("b\\{10,12\\}a"), T("bbbbbbbbbba"), "1 0 11", BG},
    {__LINE__, T("b{10,12}a"), T("bbbbbbbbbba"), "1 0 11", NOT_BG},
    {__LINE__, T("b\\{10,12\\}a"), T("bbbbbbbbbbba"), "1 0 12", BG},
    {__LINE__, T("b{10,12}a"), T("bbbbbbbbbbba"), "1 0 12", NOT_BG},
    {__LINE__, T("b\\{10,12\\}a"), T("bbbbbbbbbbbba"), "1 0 13", BG},
    {__LINE__, T("b{10,12}a"), T("bbbbbbbbbbbba"), "1 0 13", NOT_BG},
    {__LINE__, T("b\\{10,12\\}a"), T("bbbbbbbbbbbbba"), "1 1 14", BG},
    {__LINE__, T("b{10,12}a"), T("bbbbbbbbbbbbba"), "1 1 14", NOT_BG},

    {__LINE__, T("ab*"), T("abb"), "1 0 3", ALL},
    {__LINE__, T("ab{0,0}"), T("ab"), "1 0 1", NOT_BG},

    // atom escape
    // identity escape
    {__LINE__, T("\\(\\)\\^\\.\\*\\+\\?\\[\\]\\|\\\\\\-") T("\\{\\}\\,\\:\\=\\!\\\n\\\r\\\b"),
        T("()^.*+?[]|\\-{},:=!\n\r\b"), "1 0 21", ECMA},
    {__LINE__, T("\\.\\[\\\\\\*\\^\\$"), T(".[\\*^$"), "1 0 6", NOT_ECMA},
    {__LINE__, T("\\(\\)\\+\\?\\{"), T("()+?{"), "1 0 5", EEA},
    {__LINE__, T("\\\"\\/"), T("\"/"), "1 0 2", AWK},
    // decimal escape
    //! T("\\0"), T("\0"), "1 0 1", ECMA,
    {__LINE__, T("\\01"), T("\x01"), "1 0 1", AWK},
    {__LINE__, T("(x)\\1"), T("xx"), "2 0 2 0 1", ECMA},
    {__LINE__, T("(x)\\1"), T(""), "-1", BASIC | GREP | EXTENDED | EGREP},
    {__LINE__, T("\\(x\\)\\1"), T("xx"), "2 0 2 0 1", BG},
    {__LINE__, T("(x)\\1"), T("x\x01"), "2 0 2 0 1", AWK},
    {__LINE__, T("(x)\\2"), T("x\x02"), "2 0 2 0 1", AWK},
    {__LINE__, T("(x\\1)"), T("x\x01"), "2 0 2 0 2", AWK},

    // character escape
    {__LINE__, T("\\a"), T("\a"), "1 0 1", AWK},
    {__LINE__, T("\\b"), T("\b"), "1 0 1", AWK},
    {__LINE__, T("\\f"), T("\f"), "1 0 1", ECMA | AWK},
    {__LINE__, T("\\n"), T("\n"), "1 0 1", ECMA | AWK},
    {__LINE__, T("\\r"), T("\r"), "1 0 1", ECMA | AWK},
    {__LINE__, T("\\t"), T("\t"), "1 0 1", ECMA | AWK},
    {__LINE__, T("\\v"), T("\v"), "1 0 1", ECMA | AWK},

    {__LINE__, T("\\ca"), T("\1"), "1 0 1", ECMA},

    {__LINE__, T("\\x1b"), T("\x1b"), "1 0 1", ECMA},

#if WIDE
    {__LINE__, T("\\u12cd"), T("\u12cd"), "1 0 1", ECMA},
    {__LINE__, T("\\u12cd"), T(""), "-1", NOT_ECMA},
#else
    {__LINE__, T("\\u12cd"), T(""), "-1", ALL},
#endif

    {__LINE__, T("\\07"), T("\07"), "1 0 1", AWK},
    {__LINE__, T("\\177"), T("\177"), "1 0 1", AWK},
    {__LINE__, T("\\177"), T(""), "-1", NOT_AWK},
    {__LINE__, T("\\1777"), T("\177") T("7"), "1 0 2", AWK},
    {__LINE__, T("\\39"), T("\3") T("9"), "1 0 2", AWK},
    {__LINE__, T("\\39"), T(""), "-1", NOT_AWK},

    // character class escape
    {__LINE__, T("\\d"), T("a"), "0", ECMA},
    {__LINE__, T("\\d"), T("1"), "1 0 1", ECMA},
    {__LINE__, T("\\d"), T(""), "-1", NOT_ECMA},
    {__LINE__, T("\\D"), T("a"), "1 0 1", ECMA},
    {__LINE__, T("\\D"), T("1"), "0", ECMA},
    {__LINE__, T("\\D"), T(""), "-1", NOT_ECMA},
    {__LINE__, T("\\s"), T("a"), "0", ECMA},
    {__LINE__, T("\\s"), T(" "), "1 0 1", ECMA},
    {__LINE__, T("\\s"), T(""), "-1", NOT_ECMA},
    {__LINE__, T("\\S"), T("a"), "1 0 1", ECMA},
    {__LINE__, T("\\S"), T(" "), "0", ECMA},
    {__LINE__, T("\\S"), T(""), "-1", NOT_ECMA},
    {__LINE__, T("\\w"), T("a"), "1 0 1", ECMA},
    {__LINE__, T("\\w"), T(" "), "0", ECMA},
    {__LINE__, T("\\w"), T(""), "-1", NOT_ECMA},
    {__LINE__, T("\\W"), T("a"), "0", ECMA},
    {__LINE__, T("\\W"), T(" "), "1 0 1", ECMA},
    {__LINE__, T("\\W"), T(""), "-1", NOT_ECMA},

    // character classes
    {__LINE__, T("[]"), T(""), "0", ECMA},
    {__LINE__, T("[]"), T(""), "-1", NOT_ECMA},
    {__LINE__, T("[]]"), T("]"), "-1", ECMA},
    {__LINE__, T("[]]"), T("]"), "1 0 1", NOT_ECMA},
    {__LINE__, T("[\\]]"), T("]"), "1 0 1", ECMA},
    {__LINE__, T("[^]"), T("^"), "1 0 1", ECMA},
    {__LINE__, T("[^]]"), T("^"), "1 0 1", NOT_ECMA},
    {__LINE__, T("[^]]"), T("]"), "0", NOT_ECMA},
    {__LINE__, T("[^]]"), T("a"), "1 0 1", NOT_ECMA},
    {__LINE__, T("[abc]"), T("a"), "1 0 1", ALL},
    {__LINE__, T("[abc]"), T("b"), "1 0 1", ALL},
    {__LINE__, T("[abc]"), T("c"), "1 0 1", ALL},
    {__LINE__, T("[abc]"), T("d"), "0", ALL},
    {__LINE__, T("[a-z]"), T("a"), "1 0 1", ALL},
    {__LINE__, T("[a-z]"), T("b"), "1 0 1", ALL},
    {__LINE__, T("[a-z]"), T("c"), "1 0 1", ALL},
    {__LINE__, T("[a-z]"), T("d"), "1 0 1", ALL},
    {__LINE__, T("[*+?]"), T("*"), "1 0 1", ALL},
    {__LINE__, T("[*+?]"), T("+"), "1 0 1", ALL},
    {__LINE__, T("[*+?]"), T("?"), "1 0 1", ALL},

    // numeric escape
    //! T("[\\0]"), ALL,
    {__LINE__, T("[\\1]"), T("\\"), "1 0 1", BASIC | EXTENDED | GREP | EGREP},
    {__LINE__, T("[\\1]"), T("1"), "1 0 1", BASIC | EXTENDED | GREP | EGREP},
    // character escape
    {__LINE__, T("[\\a]"), T("\a"), "1 0 1", AWK},
    {__LINE__, T("[\\a]"), T("\\"), "1 0 1", BASIC | GREP | EXTENDED | EGREP},
    {__LINE__, T("[\\a]"), T("a"), "1 0 1", ECMA | BASIC | GREP | EXTENDED | EGREP},

    {__LINE__, T("[\\b]"), T("\b"), "1 0 1", AWK},
    {__LINE__, T("[\\b]"), T(""), "0", ECMA},
    {__LINE__, T("[\\b]"), T("\\"), "1 0 1", BASIC | GREP | EXTENDED | EGREP},
    {__LINE__, T("[\\b]"), T("b"), "1 0 1", BASIC | GREP | EXTENDED | EGREP},

    {__LINE__, T("[\\f]"), T("\f"), "1 0 1", ECMA | AWK},
    {__LINE__, T("[\\f]"), T("\\"), "1 0 1", BASIC | EXTENDED | GREP | EGREP},
    {__LINE__, T("[\\f]"), T("f"), "1 0 1", BASIC | EXTENDED | GREP | EGREP},

    {__LINE__, T("[\\n]"), T("\n"), "1 0 1", ECMA | AWK},
    {__LINE__, T("[\\n]"), T("\\"), "1 0 1", BASIC | EXTENDED | GREP | EGREP},
    {__LINE__, T("[\\n]"), T("n"), "1 0 1", BASIC | EXTENDED | GREP | EGREP},

    {__LINE__, T("[\\r]"), T("\r"), "1 0 1", ECMA | AWK},
    {__LINE__, T("[\\r]"), T("\\"), "1 0 1", BASIC | EXTENDED | GREP | EGREP},
    {__LINE__, T("[\\r]"), T("r"), "1 0 1", BASIC | EXTENDED | GREP | EGREP},

    {__LINE__, T("[\\t]"), T("\t"), "1 0 1", ECMA | AWK},
    {__LINE__, T("[\\t]"), T("\\"), "1 0 1", BASIC | EXTENDED | GREP | EGREP},
    {__LINE__, T("[\\t]"), T("t"), "1 0 1", BASIC | EXTENDED | GREP | EGREP},

    {__LINE__, T("[\\v]"), T("\v"), "1 0 1", ECMA | AWK},
    {__LINE__, T("[\\v]"), T("\\"), "1 0 1", BASIC | EXTENDED | GREP | EGREP},
    {__LINE__, T("[\\v]"), T("v"), "1 0 1", BASIC | EXTENDED | GREP | EGREP},

    {__LINE__, T("[\\ca]"), T("\x01"), "1 0 1", ECMA},
    {__LINE__, T("[\\c3]"), T("\\"), "1 0 1", BASIC | EXTENDED | GREP | EGREP},
    {__LINE__, T("[\\c3]"), T("c"), "1 0 1", NOT_ECMA},
    {__LINE__, T("[\\c3]"), T("3"), "1 0 1", NOT_ECMA},

    {__LINE__, T("[\\x1b]"), T("\x1b"), "1 0 1", ECMA},
    {__LINE__, T("[\\x1b]"), T("\\"), "1 0 1", BASIC | EXTENDED | GREP | EGREP},
    {__LINE__, T("[\\x1b]"), T("x"), "1 0 1", NOT_ECMA},
    {__LINE__, T("[\\x1b]"), T("1"), "1 0 1", NOT_ECMA},
    {__LINE__, T("[\\x1b]"), T("b"), "1 0 1", NOT_ECMA},
#if WIDE
    {__LINE__, T("[\\u12cd]"), T("\u12cd"), "1 0 1", ECMA},
#else
    {__LINE__, T("[\\u12cd]"), T(""), "-1", ECMA},
#endif
    {__LINE__, T("[\\u12cd]"), T("\\"), "1 0 1", NOT_ECMA},
    {__LINE__, T("[\\u12cd]"), T("u"), "1 0 1", NOT_ECMA},
    {__LINE__, T("[\\u12cd]"), T("1"), "1 0 1", NOT_ECMA},
    {__LINE__, T("[\\u12cd]"), T("2"), "1 0 1", NOT_ECMA},
    {__LINE__, T("[\\u12cd]"), T("c"), "1 0 1", NOT_ECMA},
    {__LINE__, T("[\\u12cd]"), T("d"), "1 0 1", NOT_ECMA},

    {__LINE__, T("[\\\\]"), T("\\"), "1 0 1", ALL},
    {__LINE__, T("[\\^]"), T("^"), "1 0 1", ALL},
    {__LINE__, T("[\\(]"), T("("), "1 0 1", ALL},
    {__LINE__, T("[\\)]"), T(")"), "1 0 1", ALL},
    {__LINE__, T("[\\{]"), T("{"), "1 0 1", ALL},
    {__LINE__, T("[\\}]"), T("}"), "1 0 1", ALL},
    {__LINE__, T("[\\[]"), T("["), "1 0 1", ALL},
    {__LINE__, T("[\\]]"), T("]"), "1 0 1", ECMA},
    {__LINE__, T("[\\+]"), T("+"), "1 0 1", ALL},
    {__LINE__, T("[\\*]"), T("*"), "1 0 1", ALL},
    {__LINE__, T("[\\?]"), T("?"), "1 0 1", ALL},

    // character class escape
    {__LINE__, T("[\\d]"), T("1"), "0", NOT_ECMA},
    {__LINE__, T("[\\D]"), T("a"), "0", NOT_ECMA},
    {__LINE__, T("[\\s]"), T(" "), "0", NOT_ECMA},
    {__LINE__, T("[\\S]"), T("a"), "0", NOT_ECMA},
    {__LINE__, T("[\\w]"), T("a"), "0", NOT_ECMA},
    {__LINE__, T("[\\W]"), T(" "), "0", NOT_ECMA},

    // named character classes
    {__LINE__, T("[[:alnum:]]"), T("b"), "1 0 1", ALL},
    {__LINE__, T("[[:alnum:]]"), T("1"), "1 0 1", ALL},
    {__LINE__, T("[[:alnum:]]"), T(" "), "0", ALL},
    {__LINE__, T("[[:alpha:]]"), T("b"), "1 0 1", ALL},
    {__LINE__, T("[[:alpha:]]"), T("1"), "0", ALL},
    {__LINE__, T("[[:alpha:]]"), T(" "), "0", ALL},
    {__LINE__, T("[[:blank:]]"), T("b"), "0", ALL},
    {__LINE__, T("[[:blank:]]"), T("1"), "0", ALL},
    {__LINE__, T("[[:blank:]]"), T("\t"), "1 0 1", ALL},
    {__LINE__, T("[[:cntrl:]]"), T("b"), "0", ALL},
    {__LINE__, T("[[:cntrl:]]"), T("1"), "0", ALL},
    {__LINE__, T("[[:cntrl:]]"), T("\a"), "1 0 1", ALL},
    {__LINE__, T("[[:digit:]]"), T("b"), "0", ALL},
    {__LINE__, T("[[:digit:]]"), T("1"), "1 0 1", ALL},
    {__LINE__, T("[[:digit:]]"), T(" "), "0", ALL},
    {__LINE__, T("[[:graph:]]"), T("b"), "1 0 1", ALL},
    {__LINE__, T("[[:graph:]]"), T("1"), "1 0 1", ALL},
    {__LINE__, T("[[:graph:]]"), T(" "), "0", ALL},
    {__LINE__, T("[[:graph:]]"), T("!"), "1 0 1", ALL},
    {__LINE__, T("[[:lower:]]"), T("A"), "0", ALL},
    {__LINE__, T("[[:lower:]]"), T("a"), "1 0 1", ALL},
    {__LINE__, T("[[:lower:]]"), T("0"), "0", ALL},
    {__LINE__, T("[[:print:]]"), T("b"), "1 0 1", ALL},
    {__LINE__, T("[[:print:]]"), T("1"), "1 0 1", ALL},
    {__LINE__, T("[[:print:]]"), T(" "), "1 0 1", ALL},
    {__LINE__, T("[[:print:]]"), T("!"), "1 0 1", ALL},
    {__LINE__, T("[[:print:]]"), T("\a"), "0", ALL},
    {__LINE__, T("[[:punct:]]"), T("b"), "0", ALL},
    {__LINE__, T("[[:punct:]]"), T("1"), "0", ALL},
    {__LINE__, T("[[:punct:]]"), T(" "), "0", ALL},
    {__LINE__, T("[[:punct:]]"), T("!"), "1 0 1", ALL},
    {__LINE__, T("[[:space:]]"), T("b"), "0", ALL},
    {__LINE__, T("[[:space:]]"), T("1"), "0", ALL},
    {__LINE__, T("[[:space:]]"), T(" "), "1 0 1", ALL},
    {__LINE__, T("[[:space:]]"), T("\a"), "0", ALL},
    {__LINE__, T("[[:upper:]]"), T("A"), "1 0 1", ALL},
    {__LINE__, T("[[:upper:]]"), T("a"), "0", ALL},
    {__LINE__, T("[[:upper:]]"), T("0"), "0", ALL},
    {__LINE__, T("[[:xdigit:]]"), T("b"), "1 0 1", ALL},
    {__LINE__, T("[[:xdigit:]]"), T("g"), "0", ALL},
    {__LINE__, T("[[:xdigit:]]"), T("1"), "1 0 1", ALL},
    {__LINE__, T("[[:xdigit:]]"), T(" "), "0", ALL},
    {__LINE__, T("[[.ch.]]"), T("ch"), "1 0 2", ALL},
    {__LINE__, T("[[=x=]]"), T("X"), "1 0 1", ALL},
    {__LINE__, T("[[=x=]]"), T("x"), "1 0 1", ALL},

    // character class ranges
    {__LINE__, T("[-]"), T("-"), "1 0 1", ALL},
    {__LINE__, T("[a-f]"), T("a"), "1 0 1", ALL},
    {__LINE__, T("[a-f]"), T("f"), "1 0 1", ALL},
    {__LINE__, T("[a-f]"), T("g"), "0", ALL},
    {__LINE__, T("[a-a]"), T("a"), "1 0 1", ALL},
    {__LINE__, T("[a-a]"), T("b"), "0", ALL},
    {__LINE__, T("[a-a]"), T("-"), "0", ALL},
    {__LINE__, T("[-a-f]"), T("-"), "1 0 1", ALL},
    {__LINE__, T("[-a-f]"), T("a"), "1 0 1", ALL},
    {__LINE__, T("[-a-f]"), T("f"), "1 0 1", ALL},
    {__LINE__, T("[-a-f]"), T("g"), "0", ALL},
    {__LINE__, T("[a-f-]"), T("-"), "1 0 1", ALL},
    {__LINE__, T("[a-f-]"), T("a"), "1 0 1", ALL},
    {__LINE__, T("[a-f-]"), T("f"), "1 0 1", ALL},
    {__LINE__, T("[a-f-]"), T("g"), "0", ALL},
    {__LINE__, T("[a-f-g-i]"), T("a"), "1 0 1", ALL},
    {__LINE__, T("[a-f-g-i]"), T("f"), "1 0 1", ALL},
    {__LINE__, T("[a-f-g-i]"), T("g"), "1 0 1", ALL},
    {__LINE__, T("[a-f-g-i]"), T("i"), "1 0 1", ALL},
    {__LINE__, T("[a-f-g-i]"), T("-"), "1 0 1", ALL},
    {__LINE__, T("[a-f-g-i]"), T("j"), "0", ALL},

    // negation
    {__LINE__, T("[^]"), T("^"), "1 0 1", ECMA},
    {__LINE__, T("[^]"), T("a"), "1 0 1", ECMA},
    {__LINE__, T("[^^]"), T("^"), "0", ALL},
    {__LINE__, T("[^^]"), T("a"), "1 0 1", ALL},
    {__LINE__, T("[^a-f]"), T("a"), "0", ALL},
    {__LINE__, T("[^a-f]"), T("f"), "0", ALL},
    {__LINE__, T("[^a-f]"), T("g"), "1 0 1", ALL},
    {__LINE__, T("[^^-a-f]"), T("^"), "0", ALL},
    {__LINE__, T("[^^-a-f]"), T("-"), "0", ALL},
    {__LINE__, T("[^^-a-f]"), T("a"), "0", ALL},
    {__LINE__, T("[^^-a-f]"), T("f"), "0", ALL},
    {__LINE__, T("[^^-a-f]"), T("g"), "1 0 1", ALL},

    // capture groups
    {__LINE__, T("(a)"), T("aaa"), "2 0 1 0 1", NOT_BG},
    {__LINE__, T("()"), T(""), "2 0 0 0 0", ECMA},
    {__LINE__, T("()"), T("()"), "1 0 2", BG},
    {__LINE__, T("("), T("("), "1 0 1", BG},
    {__LINE__, T(")"), T(")"), "1 0 1", NOT_ECMA},
    {__LINE__, T("((((((((((((((((((((a))))))))))))))))))))\\18"), T("aa"),
        "21 0 2 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 "
        "0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1",
        ECMA},
    {__LINE__, T("((((((((((((((((((((a))))))))))))))))))))"), T("a"),
        "21 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 "
        "0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1",
        NOT_BG},

    // non-capture groups
    {__LINE__, T("(?:abc)"), T("abc"), "1 0 3", ECMA},
    {__LINE__, T("(?:abc)|(?:def)"), T("abc"), "1 0 3", ECMA},
    {__LINE__, T("(?:abc)|(?:def)"), T("def"), "1 0 3", ECMA},
    {__LINE__, T("(?:(abc))"), T("abc"), "2 0 3 0 3", ECMA},

    // positive assert
    {__LINE__, T("(?=a(bc))."), T("abcd"), "2 0 1 1 3", ECMA},

    // negative assert
    {__LINE__, T("(?!abc)."), T("abcd"), "1 1 2", ECMA},

    // maximum munch
    {__LINE__, T("(week|wee)(knights|night)"), T("weeknights"), "3 0 9 0 4 4 9", ECMA},
    {__LINE__, T("(week|wee)(knights|night)"), T("weeknights"), "3 0 10 0 3 3 10", EEA},
    {__LINE__, T("(wee|week)(knights|night)"), T("weeknights"), "3 0 10 0 3 3 10", EEA},

    {__LINE__, T("a|ab"), T("abc"), "1 0 1", ECMA},
    {__LINE__, T("a|ab"), T("abc"), "1 0 2", EEA},

    {__LINE__, T("(a|ab)b*"), T("ab"), "2 0 2 0 1", ECMA},
    {__LINE__, T("(a|ab)b*"), T("ab"), "2 0 2 0 2", EEA},

    {__LINE__, T("(a+|.*a)(b|bc)c*d"), T("abcd"), "3 0 4 0 1 1 2", ECMA},
    {__LINE__, T("(a+|.*a)(b|bc)c*d"), T("abcd"), "3 0 4 0 1 1 3", EEA},

    // ECMAScript examples
    {__LINE__, T("((a)|(ab))((c)|(bc))"), T("abc"), "7 0 3 0 1 0 1 -1 -1 1 3 -1 -1 1 3", ECMA},
    {__LINE__, T("((a)|(ab))((c)|(bc))"), T("abc"), "7 0 3 0 2 -1 -1 0 2 2 3 2 3 -1 -1", EEA},
    {__LINE__, T("a[a-z]{2,4}"), T("abcdefghi"), "1 0 5", NOT_BG},
    {__LINE__, T("a[a-z]\\{2,4\\}"), T("abcdefghi"), "1 0 5", BASIC | GREP},
    {__LINE__, T("a[a-z]{2,4}?"), T("abcdefghi"), "1 0 3", ECMA},
    {__LINE__, T("(aa|aabaac|ba|b|c)*"), T("aabaac"), "2 0 4 2 4", ECMA},
    {__LINE__, T("(aa|aabaac|ba|b|c)*"), T("aabaac"), "2 0 6 5 6", EEA},
    {__LINE__, T("(z)((a+)?(b+)?(c))*"), T("zaacbbbcac"), "6 0 10 0 1 8 10 8 9 -1 -1 9 10", ECMA},
    {__LINE__, T("(a*)b\\1+"), T("baaaac"), "2 0 1 0 0", ECMA},
    {__LINE__, T("(?=(a+))"), T("baaabac"), "2 1 1 1 4", ECMA},
    {__LINE__, T("(?=(a+))a*b\\1"), T("baaabac"), "2 3 6 3 4", ECMA},
    {__LINE__, T("(.*?)a(?!(a+)b\\2c)\\2(.*)"), T("baaabaac"), "4 0 8 0 2 -1 -1 3 8", ECMA},

    // other
    {__LINE__, T("a(((b)))c"), T("abc"), "4 0 3 1 2 1 2 1 2", NOT_BG},
    {__LINE__, T("a(b|(c))d"), T("abd"), "3 0 3 1 2 -1 -1", NOT_BG},
    {__LINE__, T("a(b|(c))d"), T("acd"), "3 0 3 1 2 1 2", NOT_BG},
    {__LINE__, T("a(b*|c)d"), T("abbd"), "2 0 4 1 3", NOT_BG},
    {__LINE__, T("a[ab]{20}"), T("aaaaabaaaabaaaabaaaab"), "1 0 21", NOT_BG},
    {__LINE__, T("a[ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab]") T("[ab][ab][ab][ab][ab][ab][ab][ab]"),
        T("aaaaabaaaabaaaabaaaab"), "1 0 21", ALL},
    {__LINE__,
        T("a[ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab]")
            T("[ab][ab][ab][ab][ab][ab][ab][ab](week|wee)(knights|night)"),
        T("aaaaabaaaabaaaabaaaabweeknights"), "3 0 30 21 25 25 30", ECMA},
    {__LINE__,
        T("a[ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab][ab]")
            T("[ab][ab][ab][ab][ab][ab][ab][ab](week|wee)(knights|night)"),
        T("aaaaabaaaabaaaabaaaabweeknights"), "3 0 31 21 24 24 31", EXTENDED | EGREP | AWK},
    {__LINE__, T("1234567890123456789012345678901234567890123456789") T("012345678901234567890"),
        T("a1234567890123456789012345678901234567890123456789") T("012345678901234567890b"), "1 1 71", ALL},
    {__LINE__, T("[ab][cd][ef][gh][ij][kl][mn]"), T("xacegikmoq"), "1 1 8", ALL},
    {__LINE__, T("[ab][cd][ef][gh][ij][kl][mn][op]"), T("xacegikmoq"), "1 1 9", ALL},
    {__LINE__, T("[ab][cd][ef][gh][ij][kl][mn][op][qr]"), T("xacegikmoqy"), "1 1 10", ALL},
    {__LINE__, T("[ab][cd][ef][gh][ij][kl][mn][op][q]"), T("xacegikmoqy"), "1 1 10", ALL},

    {__LINE__, T("(a)(b)(c)(d)(e)(f)(g)(h)"), T("zabcdefghi"), "9 1 9 1 2 2 3 3 4 4 5 5 6 6 7 7 8 8 9", NOT_BG},
    {__LINE__, T("(a)(b)(c)(d)(e)(f)(g)(h)(i)"), T("zabcdefghij"), "10 1 10 1 2 2 3 3 4 4 5 5 6 6 7 7 8 8 9 9 10",
        NOT_BG},
    {__LINE__, T("(a)(b)(c)(d)(e)(f)(g)(h)(i)(j)"), T("zabcdefghijk"),
        "11 1 11 1 2 2 3 3 4 4 5 5 6 6 7 7 8 8 9 9 10 10 11", NOT_BG},
    {__LINE__, T("(a)(b)(c)(d)(e)(f)(g)(h)(i)(j)(k)"), T("zabcdefghijkl"),
        "12 1 12 1 2 2 3 3 4 4 5 5 6 6 7 7 8 8 9 9 10 10 11 11 12", NOT_BG},
    {__LINE__, T("(a)d|(b)c"), T("abc"), "3 1 3 -1 -1 1 2", NOT_BG},
    {__LINE__, T("_+((www)|(ftp)|(mailto)):_*"), T("_wwwnocolon _mailto:"), "5 12 20 13 19 -1 -1 -1 -1 13 19", NOT_BG},
    {__LINE__, T("a(b)?c\\1d"), T("acd"), "2 0 3 -1 -1", ECMA},
    {__LINE__, T("a(b?c)+d"), T("accd"), "2 0 4 2 3", NOT_BG},
    {__LINE__, T(".*"), T("abc"), "1 0 3", NOT_BG},
    {__LINE__, T("a(b|(c))d"), T("abd"), "3 0 3 1 2 -1 -1", NOT_BG},
    {__LINE__, T("a(b|(c))d"), T("acd"), "3 0 3 1 2 1 2", NOT_BG},
    {__LINE__, T("a(b*|c|e)d"), T("abbd"), "2 0 4 1 3", NOT_BG},
    {__LINE__, T("a(b*|c|e)d"), T("acd"), "2 0 3 1 2", NOT_BG},
    {__LINE__, T("a(b*|c|e)d"), T("ad"), "2 0 2 1 1", NOT_BG},
    {__LINE__, T("a(b?)c"), T("abc"), "2 0 3 1 2", NOT_BG},
    {__LINE__, T("a(b?)c"), T("ac"), "2 0 2 1 1", NOT_BG},
    {__LINE__, T("a(b+)c"), T("abc"), "2 0 3 1 2", NOT_BG},
    {__LINE__, T("a(b+)c"), T("abbbc"), "2 0 5 1 4", NOT_BG},
    {__LINE__, T("a(b*)c"), T("ac"), "2 0 2 1 1", NOT_BG},
    {__LINE__, T("(a|ab)(bc([de]+)f|cde)"), T("abcdef"), "4 0 6 0 1 1 6 3 5", NOT_BG},
    {__LINE__, T("a([bc]?)c"), T("abc"), "2 0 3 1 2", NOT_BG},
    {__LINE__, T("a([bc]?)c"), T("ac"), "2 0 2 1 1", NOT_BG},
    {__LINE__, T("a([bc]+)c"), T("abc"), "2 0 3 1 2", NOT_BG},
    {__LINE__, T("a([bc]+)c"), T("abcc"), "2 0 4 1 3", NOT_BG},
    {__LINE__, T("a([bc]+)bc"), T("abcbc"), "2 0 5 1 3", NOT_BG},
    {__LINE__, T("a(bb+|b)b"), T("abb"), "2 0 3 1 2", NOT_BG},
    {__LINE__, T("a(bbb+|bb+|b)b"), T("abb"), "2 0 3 1 2", NOT_BG},
    {__LINE__, T("a(bbb+|bb+|b)b"), T("abbb"), "2 0 4 1 3", NOT_BG},
    {__LINE__, T("a(bbb+|bb+|b)bb"), T("abbb"), "2 0 4 1 2", NOT_BG},
    {__LINE__, T("(.*).*"), T("abcdef"), "2 0 6 0 6", NOT_BG},
    {__LINE__, T("(a*)*"), T("bc"), "2 0 0 0 0", NOT_BG},
    {__LINE__, T("a(b|c)*d"), T("ad"), "2 0 2 -1 -1", NOT_BG},
    {__LINE__, T("a(b|c)*d"), T("abcd"), "2 0 4 2 3", NOT_BG},
    {__LINE__, T("a(b|c)+d"), T("abd"), "2 0 3 1 2", NOT_BG},
    {__LINE__, T("a(b|c)+d"), T("abcd"), "2 0 4 2 3", NOT_BG},
    {__LINE__, T("a(b|c?)+d"), T("ad"), "2 0 2 1 1", NOT_BG},
    {__LINE__, T("a(b|c?)+d"), T("abcd"), "2 0 4 2 3", ECMA},
    {__LINE__, T("a(b|c){0,0}d"), T("ad"), "2 0 2 -1 -1", NOT_BG},
    {__LINE__, T("a(b|c){0,1}d"), T("ad"), "2 0 2 -1 -1", NOT_BG},
    {__LINE__, T("a(b|c){0,1}d"), T("abd"), "2 0 3 1 2", NOT_BG},
    {__LINE__, T("a(b|c){0,2}d"), T("ad"), "2 0 2 -1 -1", NOT_BG},
    {__LINE__, T("a(b|c){0,2}d"), T("abcd"), "2 0 4 2 3", NOT_BG},
    {__LINE__, T("a(b|c){0,}d"), T("ad"), "2 0 2 -1 -1", NOT_BG},
    {__LINE__, T("a(b|c){0,}d"), T("abcd"), "2 0 4 2 3", NOT_BG},
    {__LINE__, T("a(b|c){1,1}d"), T("abd"), "2 0 3 1 2", NOT_BG},
    {__LINE__, T("a(b|c){1,2}d"), T("abd"), "2 0 3 1 2", NOT_BG},
    {__LINE__, T("a(b|c){1,2}d"), T("abcd"), "2 0 4 2 3", NOT_BG},
    {__LINE__, T("a(b|c){1,}d"), T("abd"), "2 0 3 1 2", NOT_BG},
    {__LINE__, T("a(b|c){1,}d"), T("abcd"), "2 0 4 2 3", NOT_BG},
    {__LINE__, T("a(b|c){2,2}d"), T("acbd"), "2 0 4 2 3", NOT_BG},
    {__LINE__, T("a(b|c){2,2}d"), T("abcd"), "2 0 4 2 3", NOT_BG},
    {__LINE__, T("a(b|c){2,4}d"), T("abcd"), "2 0 4 2 3", NOT_BG},
    {__LINE__, T("a(b|c){2,4}d"), T("abcbd"), "2 0 5 3 4", NOT_BG},
    {__LINE__, T("a(b|c){2,4}d"), T("abcbcd"), "2 0 6 4 5", NOT_BG},
    {__LINE__, T("a(b|c){2,}d"), T("abcd"), "2 0 4 2 3", NOT_BG},
    {__LINE__, T("a(b|c){2,}d"), T("abcbd"), "2 0 5 3 4", NOT_BG},
    {__LINE__, T("a(b+|((c)*))+d"), T("abd"), "4 0 3 1 2 -1 -1 -1 -1", ECMA},
    {__LINE__, T("a(b+|((c)*))+d"), T("abcd"), "4 0 4 2 3 2 3 2 3", ECMA},
    {__LINE__, T("/\\*([^*]|\\*+[^*/])*\\*+/"), T("/* here is a block comment */"), "2 0 29 26 27", NOT_BG},
    {__LINE__, T("/\\*([^*]|\\*+[^*/])*\\*+/"), T("/**/"), "2 0 4 -1 -1", NOT_BG},
    {__LINE__, T("/\\*([^*]|\\*+[^*/])*\\*+/"), T("/***/"), "2 0 5 -1 -1", NOT_BG},
    {__LINE__, T("/\\*([^*]|\\*+[^*/])*\\*+/"), T("/****/"), "2 0 6 -1 -1", NOT_BG},
    {__LINE__, T("/\\*([^*]|\\*+[^*/])*\\*+/"), T("/*****/"), "2 0 7 -1 -1", NOT_BG},
    {__LINE__, T("/\\*([^*]|\\*+[^*/])*\\*+/"), T("/*****/*/"), "2 0 7 -1 -1", NOT_BG},

    {__LINE__, T("((0x[[:xdigit:]]+)|([[:digit:]]+))u?((int(8|16|32|64))|L)?"), T("0xFF"),
        "7 0 4 0 4 0 4 -1 -1 -1 -1 -1 -1 -1 -1", NOT_BG},
    {__LINE__, T("((0x[[:xdigit:]]+)|([[:digit:]]+))u?((int(8|16|32|64))|L)?"), T("35"),
        "7 0 2 0 2 -1 -1 0 2 -1 -1 -1 -1 -1 -1", NOT_BG},
    {__LINE__, T("((0x[[:xdigit:]]+)|([[:digit:]]+))u?((int(8|16|32|64))|L)?"), T("0xFFu"),
        "7 0 5 0 4 0 4 -1 -1 -1 -1 -1 -1 -1 -1", NOT_BG},
    {__LINE__, T("((0x[[:xdigit:]]+)|([[:digit:]]+))u?((int(8|16|32|64))|L)?"), T("0xFFL"),
        "7 0 5 0 4 0 4 -1 -1 4 5 -1 -1 -1 -1", NOT_BG},
    {__LINE__, T("((0x[[:xdigit:]]+)|([[:digit:]]+))u?((int(8|16|32|64))|L)?"), T("0xFFFFFFFFFFFFFFFFuint64"),
        "7 0 24 0 18 0 18 -1 -1 19 24 19 24 22 24", NOT_BG},
    {__LINE__, T("(\\.[[:alnum:]]+){2}"), T("w.a.b"), "2 1 5 3 5", NOT_BG},

    {__LINE__, T("^[[:blank:]]*#([^\\n]*\\\\[[:space:]]+)*[^\\n]*"), T("#define some_symbol"), "2 0 19 -1 -1",
        ECMA | AWK},
    {__LINE__, T("^[[:blank:]]*#([^\\n]*\\\\[[:space:]]+)*[^\\n]*"), T("#define some_symbol(x) #x"), "2 0 25 -1 -1",
        ECMA | AWK},
    {__LINE__, T("^[[:blank:]]*#([^\\n]*\\\\[[:space:]]+)*[^\\n]*"),
        T("#define some_symbol(x) \\  \r\n  cat();\\\r\n   printf(#x);"), "2 0 53 30 42", ECMA | AWK},
};

static STD string check_matches(
    STD match_results<const CHR*>& match) { // generate string representing matched capture groups
    STD ostringstream mtch;
    mtch << match.size();
    for (CSTD size_t i = 0; i < match.size(); ++i) { // append i-th capture group
        if (match[i].matched) {
            mtch << ' ' << match.position(i) << ' ' << (match.position(i) + match.length(i));
        } else {
            mtch << " -1 -1";
        }
    }
    return mtch.str();
}

static void check_match(unsigned int line, const CHR* re, const CHR* txt, const char* mtch,
    const char* res) { // compare actual result with expected result
    const CHR* p;
    STD string str;

    for (p = re; *p != 0; ++p) {
        str.append(1, (char) *p);
    }
    str.append(" |");
    for (p = txt; *p != 0; ++p) {
        str.append(1, (char) *p);
    }
    str.append("|");

    check_str(str.c_str(), __FILE__, line, mtch, res);
}

static void test_opt(STD regex_constants::syntax_option_type opts) { // do all valid tests for option set opts
    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i) {
        if (tests[i].valid & lang(opts)) { // do one test
#if NO_EXCEPTIONS
            STD basic_regex<CHR> re(tests[i].re, opts);
            STD match_results<const CHR*> match;
            STD regex_search((const CHR*) tests[i].txt, tests[i].txt + xlen(tests[i].txt), match, re,
                STD regex_constants::match_default);
            check_match(tests[i].line, tests[i].re, tests[i].txt, check_matches(match).c_str(), tests[i].res);

#else // NO_EXCEPTIONS
            try { // do one test
                STD basic_regex<CHR> re(tests[i].re, opts);
                STD match_results<const CHR*> match;
                STD regex_search((const CHR*) tests[i].txt, tests[i].txt + xlen(tests[i].txt), match, re,
                    STD regex_constants::match_default);
                check_match(tests[i].line, tests[i].re, tests[i].txt, check_matches(match).c_str(), tests[i].res);
            } catch (const STD regex_error&) { // catch invalid expression
                check_match(tests[i].line, tests[i].re, tests[i].txt, "-1", tests[i].res);
            }
#endif // NO_EXCEPTIONS
        }
    }
}

void test_main() { // test all six regular expression languages
    if (!terse) {
        puts("Testing ECMAScript");
    }
    test_opt(STD regex_constants::ECMAScript);

    if (!terse) {
        puts("Testing UNIX BRE");
    }
    test_opt(STD regex_constants::basic);

    if (!terse) {
        puts("Testing grep");
    }
    test_opt(STD regex_constants::grep);

    if (!terse) {
        puts("Testing UNIX ERE");
    }
    test_opt(STD regex_constants::extended);

    if (!terse) {
        puts("Testing egrep");
    }
    test_opt(STD regex_constants::egrep);

    if (!terse) {
        puts("Testing awk");
    }
    test_opt(STD regex_constants::awk);
}
