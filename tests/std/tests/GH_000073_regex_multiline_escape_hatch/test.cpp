// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _REGEX_LEGACY_MULTILINE_MODE 1

#include <cstddef>
#include <cstdio>
#include <regex>
#include <string>

#include <test_regex_support.hpp>

using namespace std;
using namespace std::regex_constants;

regex_fixture g_regexTester;

void test_VSO_225160_match_bol_flag() {
    // Old tests for caret anchor in default multiline mode
    for (syntax_option_type syntax : {syntax_option_type{}, ECMAScript, basic, grep, extended, egrep, awk}) {
        const test_regex emptyAnchor(&g_regexTester, R"(^)", syntax);
        emptyAnchor.should_search_match("", "");
        emptyAnchor.should_search_fail("", match_not_bol);
        emptyAnchor.should_search_match("\n", "");
        emptyAnchor.should_search_match("\n", "", match_not_bol);

        const test_regex beginCd(&g_regexTester, R"(^cd)", syntax);
        beginCd.should_search_match("ab\ncdefg", "cd");
        beginCd.should_search_match("ab\ncdefg", "cd", match_not_bol);

        beginCd.should_search_match("cdefg", "cd");
        beginCd.should_search_fail("cdefg", match_not_bol);
        beginCd.should_search_match("\ncdefg", "cd");
        beginCd.should_search_match("\ncdefg", "cd", match_not_bol);

        beginCd.should_search_fail("ab\nxcdefg");
        beginCd.should_search_fail("ab\nxcdefg", match_not_bol);
    }
}

void test_VSO_225160_match_eol_flag() {
    // Old tests for dollar anchor in default multiline mode
    for (syntax_option_type syntax : {syntax_option_type{}, ECMAScript, basic, grep, extended, egrep, awk}) {
        const test_regex emptyAnchor(&g_regexTester, R"($)", syntax);
        emptyAnchor.should_search_match("", "");
        emptyAnchor.should_search_fail("", match_not_eol);
        emptyAnchor.should_search_match("\n", "");
        emptyAnchor.should_search_match("\n", "", match_not_eol);

        const test_regex cdEnd(&g_regexTester, R"(cd$)", syntax);
        cdEnd.should_search_match("abcd\nefg", "cd");
        cdEnd.should_search_match("abcd\nefg", "cd", match_not_eol);

        cdEnd.should_search_match("abcd", "cd");
        cdEnd.should_search_fail("abcd", match_not_eol);
        cdEnd.should_search_match("abcd\n", "cd");
        cdEnd.should_search_match("abcd\n", "cd", match_not_eol);

        cdEnd.should_search_fail("abcdx\nefg");
        cdEnd.should_search_fail("abcdx\nefg", match_not_eol);
    }
}

void test_gh_73() {
    for (syntax_option_type syntax : {syntax_option_type{}, ECMAScript, basic, grep, extended, egrep, awk}) {
        {
            test_regex a_anchored_on_both_sides(&g_regexTester, "^a$", syntax);
            a_anchored_on_both_sides.should_search_match("a", "a");
            a_anchored_on_both_sides.should_search_match("b\na", "a");
            a_anchored_on_both_sides.should_search_match("a\nb", "a");
            a_anchored_on_both_sides.should_search_fail("a\nb", match_not_bol);
            a_anchored_on_both_sides.should_search_fail("b\na", match_not_eol);
        }

        {
            test_regex a_anchored_front(&g_regexTester, "^a", syntax);
            a_anchored_front.should_search_match("a", "a");
            a_anchored_front.should_search_match("a\n", "a");
            a_anchored_front.should_search_match("a\nb", "a");
            a_anchored_front.should_search_match("b\na", "a");
            a_anchored_front.should_search_match("\na", "a");
            a_anchored_front.should_search_fail("a", match_not_bol);
            a_anchored_front.should_search_match("\na", "a", match_not_bol);
            a_anchored_front.should_search_match("b\na", "a", match_not_bol);
        }

        {
            test_regex a_anchored_back(&g_regexTester, "a$", syntax);
            a_anchored_back.should_search_match("a", "a");
            a_anchored_back.should_search_match("\na", "a");
            a_anchored_back.should_search_match("b\na", "a");
            a_anchored_back.should_search_match("a\nb", "a");
            a_anchored_back.should_search_match("a\n", "a");
            a_anchored_back.should_search_fail("a", match_not_eol);
            a_anchored_back.should_search_match("a\n", "a", match_not_eol);
            a_anchored_back.should_search_match("a\nb", "a", match_not_eol);
        }
    }
}

int main() {
    test_VSO_225160_match_bol_flag();
    test_VSO_225160_match_eol_flag();
    test_gh_73();

    return g_regexTester.result();
}
