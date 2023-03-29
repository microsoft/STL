// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstdio>
#include <cstdlib>
#include <regex>
#include <string>

#include <test_regex_support.hpp>

using namespace std;
using namespace std::regex_constants;

regex_fixture g_regexTester;

void test_dev10_449367_case_insensitivity_should_work() {
    g_regexTester.should_match("a", "a");
    g_regexTester.should_not_match("A", "a");
    g_regexTester.should_not_match("a", "A");
    g_regexTester.should_match("A", "A");

    g_regexTester.should_match("a", "[a]");
    g_regexTester.should_not_match("A", "[a]");
    g_regexTester.should_not_match("a", "[A]");
    g_regexTester.should_match("A", "[A]");

    g_regexTester.should_match("a", "[a-z]");
    g_regexTester.should_not_match("A", "[a-z]");
    g_regexTester.should_not_match("a", "[A-Z]");
    g_regexTester.should_match("A", "[A-Z]");

    g_regexTester.should_match("meow", "[a-z]+");
    g_regexTester.should_not_match("MeOw", "[a-z]+");
    g_regexTester.should_not_match("MEOW", "[a-z]+");
    g_regexTester.should_not_match("meow", "[A-Z]+");
    g_regexTester.should_not_match("MeOw", "[A-Z]+");
    g_regexTester.should_match("MEOW", "[A-Z]+");

    g_regexTester.should_match("a", "a", ECMAScript | icase);
    g_regexTester.should_match("A", "a", ECMAScript | icase);
    g_regexTester.should_match("a", "A", ECMAScript | icase);
    g_regexTester.should_match("A", "A", ECMAScript | icase);

    g_regexTester.should_match("a", "[a]", ECMAScript | icase);
    g_regexTester.should_match("A", "[a]", ECMAScript | icase);
    g_regexTester.should_match("a", "[A]", ECMAScript | icase);
    g_regexTester.should_match("A", "[A]", ECMAScript | icase);

    g_regexTester.should_match("a", "[a-z]", ECMAScript | icase);
    g_regexTester.should_match("A", "[a-z]", ECMAScript | icase);
    g_regexTester.should_match("a", "[A-Z]", ECMAScript | icase);
    g_regexTester.should_match("A", "[A-Z]", ECMAScript | icase);

    g_regexTester.should_match("meow", "[a-z]+", ECMAScript | icase);
    g_regexTester.should_match("MeOw", "[a-z]+", ECMAScript | icase);
    g_regexTester.should_match("MEOW", "[a-z]+", ECMAScript | icase);
    g_regexTester.should_match("meow", "[A-Z]+", ECMAScript | icase);
    g_regexTester.should_match("MeOw", "[A-Z]+", ECMAScript | icase);
    g_regexTester.should_match("MEOW", "[A-Z]+", ECMAScript | icase);
}

void test_dev11_462743_regex_collate_should_not_disable_regex_icase() {
    g_regexTester.should_not_match("kitty", "KITTY");
    g_regexTester.should_match("kitty", "KITTY", ECMAScript | icase);
    g_regexTester.should_match("kitty", "KITTY", ECMAScript | icase | regex::collate);

    g_regexTester.should_not_match("KITTY", "kitty", ECMAScript);
    g_regexTester.should_match("KITTY", "kitty", ECMAScript | icase);
    g_regexTester.should_match("KITTY", "kitty", ECMAScript | icase | regex::collate);
}

void test_dev10_448906_regex_infinite_loops_incorrect_results() {
    // ECMA-262 15.10.2.5 "Term":
    // Step 1 of the RepeatMatcher's closure d states that, once the minimum number
    // of repetitions has been satisfied, any more expansions of Atom that match
    // the empty string are not considered for further repetitions.
    g_regexTester.should_capture("xyyxxxyyyy", "(x|y*)+", "yyyy");
    g_regexTester.should_capture("xyyxxxyyyy", "(x|y+)+", "yyyy");
    g_regexTester.should_capture("xyyxxxyyyy", "(x|y*?)+", "y");
    g_regexTester.should_capture("xyyxxxyyyy", "(x|y+?)+", "y");
}

void test_dev10_505773_regex_should_accept_empty_alternations() {
    g_regexTester.should_not_match("a", "");
    g_regexTester.should_not_match("b", "");
    g_regexTester.should_not_match("ab", "");

    g_regexTester.should_match("", "a|");
    g_regexTester.should_match("a", "a|");
    g_regexTester.should_not_match("b", "a|");
    g_regexTester.should_not_match("ab", "a|");

    g_regexTester.should_match("", "|b");
    g_regexTester.should_not_match("a", "|b");
    g_regexTester.should_match("b", "|b");
    g_regexTester.should_not_match("ab", "|b");

    g_regexTester.should_not_match("", "a|b");
    g_regexTester.should_match("a", "a|b");
    g_regexTester.should_match("b", "a|b");
    g_regexTester.should_not_match("ab", "a|b");

    g_regexTester.should_match("", "|");
    g_regexTester.should_not_match("a", "|");
    g_regexTester.should_not_match("b", "|");
    g_regexTester.should_not_match("ab", "|");
}

void test_dev11_821930_literal_dollar_replacement_characters() {
    // Also test DevDiv-821930 "<regex>: match_results formatting: _Format_default increment error [libs-conformance]"

    // N3797 28.5.2 [re.matchflag]:
    // Element: format_default
    // Effect(s) if set: When a regular expression match is to be replaced by a new string,
    // the new string shall be constructed using the rules used by the ECMAScript replace
    // function in ECMA-262, part 15.5.4.11 String.prototype.replace. In addition, during
    // search and replace operations all non-overlapping occurrences of the regular expression
    // shall be located and replaced, and sections of the input that did not match the
    // expression shall be copied unchanged to the output string.

    // ECMA-262 15.5.4.11:
    // A $ in newstring that does not match any of the forms below is left as is.
    // Characters  Replacement text
    // $$          $
    // $&          The matched substring.
    // $`?         The portion of string that precedes the matched substring.
    // $'?         The portion of string that follows the matched substring.
    // $n          The nth capture, where n is a single digit 1-9 and $n is not followed by a decimal digit.
    //             If n<=m and the nth capture is undefined, use the empty string instead.
    //             If n>m, the result is implementation-defined.
    // $nn         The nnth capture, where nn is a two-digit decimal number 01-99.
    //             If nn<=m and the nnth capture is undefined, use the empty string instead.
    //             If nn>m, the result is implementation-defined.

    // N3797 28.5.2 [re.matchflag]:
    // Element: format_sed
    // Effect(s) if set: When a regular expression match is to be replaced by a new string,
    // the new string shall be constructed using the rules used by the sed utility in POSIX.

    // POSIX.1-2008:
    // The replacement string shall be scanned from beginning to end. An <ampersand> ( '&' ) appearing
    // in the replacement shall be replaced by the string matching the BRE. The special meaning of '&'
    // in this context can be suppressed by preceding it by a <backslash>. The characters "\n", where
    // n is a digit, shall be replaced by the text matched by the corresponding back-reference expression.
    // If the corresponding back-reference expression does not match, then the characters "\n" shall be
    // replaced by the empty string. The special meaning of "\n" where n is a digit in this context, can
    // be suppressed by preceding it by a <backslash>. For each other <backslash> encountered, the
    // following character shall lose its special meaning (if any). The meaning of a <backslash>
    // immediately followed by any character other than '&', <backslash>, a digit, or the delimiter
    // character used for this command, is unspecified.

    const string s("abcdefghijklmnopqrstuvwxyz");
    const string r("d(e.)(g.)(i.)(k)(.)(m)(.)(o)(.)(q.)(s.)(u.)w");

    g_regexTester.should_replace_to(s, r,
        "<$$><$$1><$$$$><$&><$`><$'><$1><$2><$8><$9>($01)($02)($08)($09)<$10><$11><$12><$114>[$0][$00][$007][$X]$",
        format_default | format_no_copy,
        "<$><$1><$$><defghijklmnopqrstuvw><abc><xyz><ef><gh><o><p>(ef)(gh)(o)(p)<qr><st><uv><st4>[$0][$00][$007][$X]$");

    g_regexTester.should_replace_to(s, r, R"(<\\><\\1><\\\\><&><\&><\1><\2><\8><\9><\12>(\0)(\03)(\X)\)",
        format_sed | format_no_copy,
        R"(<\><\1><\\><defghijklmnopqrstuvw><&><ef><gh><o><p><ef2>(defghijklmnopqrstuvw)(defghijklmnopqrstuvw3)(X))");

    const string s2("aabbccddee");
    const string r2("(b.)(c.)(d.)");

    g_regexTester.should_replace_to(s2, r2, R"(<$1><$2><$3><$4>)", format_default | format_no_copy, "<bb><cc><dd><>");
    g_regexTester.should_replace_to(s2, r2, R"(<\1><\2><\3><\4>)", format_sed | format_no_copy, "<bb><cc><dd><>");
}

void test_dev10_514062_regex_should_throw_regex_error_rather_than_crash_for_bogus_character_classes() {
    g_regexTester.should_throw("[[::]]", error_ctype);
    g_regexTester.should_throw("[[..]]", error_collate);
    g_regexTester.should_throw("[[==]]", error_collate);

    g_regexTester.should_throw("[[:::]]", error_ctype);
    g_regexTester.should_throw("[[.:.]]", error_collate);
    g_regexTester.should_throw("[[=:=]]", error_collate);

    g_regexTester.should_throw("[[:.:]]", error_ctype);
    g_regexTester.should_throw("[[...]]", error_collate);
    g_regexTester.should_throw("[[=.=]]", error_collate);

    g_regexTester.should_throw("[[:=:]]", error_ctype);
    g_regexTester.should_throw("[[.=.]]", error_collate);
    g_regexTester.should_throw("[[===]]", error_collate);

    g_regexTester.should_throw("[[:a:b:]]", error_ctype);
    g_regexTester.should_throw("[[.a:b.]]", error_collate);
    g_regexTester.should_throw("[[=a:b=]]", error_collate);

    g_regexTester.should_throw("[[:a.b:]]", error_ctype);
    g_regexTester.should_throw("[[.a.b.]]", error_collate);
    g_regexTester.should_throw("[[=a.b=]]", error_collate);

    g_regexTester.should_throw("[[:a=b:]]", error_ctype);
    g_regexTester.should_throw("[[.a=b.]]", error_collate);
    g_regexTester.should_throw("[[=a=b=]]", error_collate);
}

void test_dev10_767618_nested_quantifiers_should_match() {
    g_regexTester.should_match(
        "some random sub.srt:ANSI:regexp", "^\"?([^\":]+?)\"?(?::\"?([^\":]+?)\"?(?::\"?([^\":]+?)\"?)?)?$");

    g_regexTester.should_match("kittens", "(.+?)?");

    g_regexTester.should_match("aaaaaa", "(a+)?aaa");
}

void test_dev11_291918_more_nested_quantifiers_should_match() {
    const string r1 = "^([\\w|-]*)(((\\.)([\\w|-]+))*)(\\.)?(devices|ra)(-(.+))?(\\.)(live)(-int)?(\\.)(com/)(.*)";
    g_regexTester.should_match("server.ra-df.live.com/", r1);
    g_regexTester.should_match("abcz", "(a.+)?z");

    const string r2 = "^npt=(?:(?:(?:(?:(\\d+(?:\\.\\d+)?)|((?:\\d+):(?:[0-5]\\d):(?:[0-5]\\d(?:\\."
                      "\\d+)?))))-(?:(?:(\\d+(?:\\.\\d+)?)|((?:\\d+):(?:[0-5]\\d):(?:[0-5]\\d(?:\\.\\d+)?))))?)|"
                      "(-(?:(?:(?:\\d+(?:\\.\\d+)?)|(?:(?:\\d+):(?:[0-5]\\d):(?:[0-5]\\d(?:\\.\\d+)?))))))";
    g_regexTester.should_match("npt=30:23:15-40:28:10", r2);
    g_regexTester.should_match("npt=40-40:28:10", r2);

    const string r3 = "(\\d+(\\.\\d+)?|\\d+:[0-5]\\d:[0-5]\\d(\\.\\d+)?)-(\\d+(\\.\\d+)?|\\d+:[0-5]\\"
                      "d:[0-5]\\d(\\.\\d+)?)?|-(\\d+(\\.\\d+)?|\\d+:[0-5]\\d:[0-5]\\d(\\.\\d+)?)";
    g_regexTester.should_match("30:23:15-40:28:10", r3);
    g_regexTester.should_match("40-40:28:10", r3);

    g_regexTester.should_match("xxy", "(x+|x+y)?");
}

void test_VSO_167760_nested_quantifiers_should_not_infinite_loop() {
    g_regexTester.should_match("12345678901;phonecontext=IndiaDialPlan", R"(1?([2-9]\d\d[2-9]\d{6})(\s*\S*)*)");
}

void test_DDB_153116_replacements() {
    g_regexTester.should_replace_to("abc def def ghi", "^", "X", format_default, "Xabc def def ghi");
    g_regexTester.should_replace_to("abc def def ghi", "$", "X", format_default, "abc def def ghiX");
    g_regexTester.should_replace_to("abc def def ghi", "\\b", "X", format_default, "XabcX XdefX XdefX XghiX");
    g_regexTester.should_replace_to("abc def def ghi", "\\B", "X", format_default, "aXbXc dXeXf dXeXf gXhXi");
    g_regexTester.should_replace_to("abc def def ghi", "(?=ef)", "X", format_default, "abc dXef dXef ghi");
    g_regexTester.should_replace_to("abc def def ghi", "(?!ef)", "X", format_default, "XaXbXcX XdeXfX XdeXfX XgXhXiX");
}

void test_DDB_169529_replacements() {
    g_regexTester.should_replace_to("if (b) { ++v[i]; }", "[\\.\\*\\+\\?\\|\\^\\$\\(\\)\\[\\]\\{\\}\\\\]", "\\$&",
        format_default, "if \\(b\\) \\{ \\+\\+v\\[i\\]; \\}");
    g_regexTester.should_replace_to("AAAAAAAAAA", "A", "x", format_default, "xxxxxxxxxx");
    g_regexTester.should_replace_to("ABRACADABRA", "..A", "***", format_default, "A***C******");
}

void test_dev11_900202_regex_should_accept_backreferences() {
    const string pattern(R"(([a-z]+)([0-9]+)<\2><\1>)");
    g_regexTester.should_match("abc123<123><abc>", pattern);
    g_regexTester.should_not_match("abc123<456><xyz>", pattern);
}

void test_dev11_900202_regex_should_throw_for_self_referencing_backreferences() {
    g_regexTester.should_throw(R"(aaa(bbb)\2ccc)", error_backref);
    g_regexTester.should_throw(R"(ddd(eee\1)fff)", error_backref);
}

void test_dev10_897466_regex_should_support_more_than_31_capture_groups() {
    g_regexTester.should_replace_to(
        // subject
        "1x4x9x16x25x36x49x64x81x100x121x144x169x196x225x256x"
        "289x324x361x400x441x484x529x576x625x676x729x784x841x"
        "900x961x1024x1089x1156x1225x1296x1369x1444x1521x1600x"
        "1681x1764x1849x1936x2025x2116x2209x2304x2401x2500x2601x"
        "2704x2809x2916x3025x3136x3249x3364x3481x3600x3721x3844x"
        "3969x4096x4225x4356x4489x4624x4761x4900x5041x5184x5329x"
        "5476x5625x5776x5929x6084x6241x6400x6561x6724x6889x7056x"
        "7225x7396x7569x7744x7921x8100x8281x8464x8649x8836x9025x"
        "9216x9409x9604x9801x",
        // pattern
        "(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x"
        "(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x"
        "(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x"
        "(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x"
        "(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x"
        "(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x"
        "(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x"
        "(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x"
        "(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x"
        "(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x(\\d+)x",
        // format
        "$99,$98,$97,$96,$95,$94,$93,$92,$91,$90,"
        "$89,$88,$87,$86,$85,$84,$83,$82,$81,$80,"
        "$79,$78,$77,$76,$75,$74,$73,$72,$71,$70,"
        "$69,$68,$67,$66,$65,$64,$63,$62,$61,$60,"
        "$59,$58,$57,$56,$55,$54,$53,$52,$51,$50,"
        "$49,$48,$47,$46,$45,$44,$43,$42,$41,$40,"
        "$39,$38,$37,$36,$35,$34,$33,$32,$31,$30,"
        "$29,$28,$27,$26,$25,$24,$23,$22,$21,$20,"
        "$19,$18,$17,$16,$15,$14,$13,$12,$11,$10,"
        "$9,$8,$7,$6,$5,$4,$3,$2,$1,",
        // flags
        format_default,
        // expected
        "9801,9604,9409,9216,9025,8836,8649,8464,8281,8100,7921,"
        "7744,7569,7396,7225,7056,6889,6724,6561,6400,6241,6084,"
        "5929,5776,5625,5476,5329,5184,5041,4900,4761,4624,4489,"
        "4356,4225,4096,3969,3844,3721,3600,3481,3364,3249,3136,"
        "3025,2916,2809,2704,2601,2500,2401,2304,2209,2116,2025,"
        "1936,1849,1764,1681,1600,1521,1444,1369,1296,1225,1156,"
        "1089,1024,961,900,841,784,729,676,625,576,529,484,441,"
        "400,361,324,289,256,225,196,169,144,121,100,81,64,49,"
        "36,25,16,9,4,1,");
}

void test_regex_should_throw_for_lookbehind() {
    g_regexTester.should_throw(R"((?<=abc))", error_syntax);
    g_regexTester.should_throw(R"((?<!abc))", error_syntax);
}

void test_regex_simple_loop_detection_enters_alternations_and_assertions() {
    // Starting with test_VSO_167760_nested_quantifiers_should_not_infinite_loop above,
    // which depends on _Simple_loop detection.

    // If simple loop detection didn't enter both if branches, this would infinite loop:
    g_regexTester.should_match("12345678923;phonecontext=IndiaDialPlan",
        // This branch doesn't match due to the x in the middle
        R"(1?([2-9]\d\d[2-9]\d{6})x(\s*\S*)*)"
        "|"
        // This branch will match
        R"(1?([2-9]\d\d[2-9]\d{6})(\s*\S*)*)");

    // If simple loop detection doesn't enter assertions, the following would break
    // for the same reason:
    g_regexTester.should_match("12345678923;phonecontext=IndiaDialPlan",
        R"(1(?=1?([2-9]\d\d[2-9]\d{6})(\s*\S*)*)2345678923;phonecontext=IndiaDialPlan)");
    g_regexTester.should_not_match("12345678923;phonecontext=IndiaDialPlan",
        R"(1(?!1?([2-9]\d\d[2-9]\d{6})(\s*\S*)*)2345678923;phonecontext=IndiaDialPlan)");
}

void test_VSO_159685_trailing_escape_should_throw_error_escape() {
    g_regexTester.should_throw(R"(\)", error_escape);
    g_regexTester.should_throw(R"([\)", error_escape);
}

void test_dev11_658169_regex_universe_character_classes_should_match_correctly() {
    g_regexTester.should_not_match("X", "[]");
    g_regexTester.should_not_match("#", "[]");
    g_regexTester.should_not_match("\n", "[]");

    g_regexTester.should_match("X", "[^]");
    g_regexTester.should_match("#", "[^]");
    g_regexTester.should_match("\n", "[^]");

    g_regexTester.should_match("X", R"([\w\W])");
    g_regexTester.should_match("#", R"([\w\W])");
    g_regexTester.should_match("\n", R"([\w\W])");
}

void test_VSO_101318_ECMAScript_identity_escapes_should_be_lax() {
    // Test the ambiguous cases that we now want to accept
    g_regexTester.should_match("z", "\\z");
    g_regexTester.should_match("\xB5", "\\\xB5"); // \xB5 is the micro symbol in Latin-1
    // ES6 explicitly says "but not c" so this should fail
    g_regexTester.should_throw("\\c", error_escape);
    // Make sure w s and d still do what they're supposed to
    g_regexTester.should_match("1", "\\d");
    g_regexTester.should_match("a", "\\D");
    g_regexTester.should_match(" ", "\\s");
    g_regexTester.should_match("a", "\\S");
    g_regexTester.should_match("a", "\\w");
    g_regexTester.should_match(" ", "\\W");
    // Make sure the SyntaxCharacter s are identity escapes
    g_regexTester.should_match("^", "\\^");
    g_regexTester.should_match("$", "\\$");
    g_regexTester.should_match(R"(\)", R"(\\)");
    g_regexTester.should_match(".", "\\.");
    g_regexTester.should_match("*", "\\*");
    g_regexTester.should_match("+", "\\+");
    g_regexTester.should_match("?", "\\?");
    g_regexTester.should_match("(", "\\(");
    g_regexTester.should_match(")", "\\)");
    g_regexTester.should_match("[", "\\[");
    g_regexTester.should_match("]", "\\]");
    g_regexTester.should_match("{", "\\{");
    g_regexTester.should_match("}", "\\}");
    g_regexTester.should_match("|", "\\|");
    // / is explicitly called out as being allowed for Unicode IdentityEscape
    g_regexTester.should_match("/", "\\/");
    // Make sure "limited file escapes" still operate
    g_regexTester.should_match("\f", R"(\f)");
    g_regexTester.should_match("\n", R"(\n)");
    g_regexTester.should_match("\r", R"(\r)");
    g_regexTester.should_match("\t", R"(\t)");
    g_regexTester.should_match("\v", R"(\v)");
    // Make sure Unicode and hex escapes still are special
    g_regexTester.should_throw("\\u", error_escape);
    g_regexTester.should_throw("\\x", error_escape);
}

void test_VSO_208146_regex_should_special_case_optional_as_non_quantifier() {
    // This tests that we can engage the "simple loop" optimization for a quantifier inside of a ?
    // (before this change, the following match would have triggered a stack overflow)
    // This example came from a customer
    g_regexTester.should_match(
        R"(runLevel@Win32([],"levels/WorkflowTests/TestLevel_Main","TestLevel_Main",True,True,False,False,False,False,)"
        R"(True,False,True,False,False,False,False,False,False,False,True,False,True,False,False,False,False,False,-1,)"
        R"(1052520,"7155349B","Globals/Sound/AudioSystem","Sound/VoiceOverSystem","","","","",ExecutionMode_GameView,P)"
        R"(layAsMode_ClientAndServer,RunLevelConfigType_AutoAssign,Hardware_Autodetect,596,1742,[],""," ",False,False,)"
        R"("",False,00000000-0000-0000-0000-000000000000,"","","",3,1,"Sequential",30,True,1,1,"png","Single",False,Tr)"
        R"(ue,False,False,0,0,1,False,True,False,False,False,CllNone,0,0,0,0,False,False,False,0,"",1,False))",
        R"((([^:]*):)?(.*?)(@(.*?))?(\(.*\))$)");
}

void test_case_VSO_208146_regex_smoke_test_rewritten_zero_or_one_quantifier(const string& quantifier) {
    const string ecmaPattern = "before(x)" + quantifier + "?(x)" + quantifier;
    const regex ecma(ecmaPattern, ECMAScript);
    cmatch results;
    g_regexTester.verify(regex_match("before", results, ecma));
    g_regexTester.verify(results[0] == "before");
    g_regexTester.verify(!results[1].matched);
    g_regexTester.verify(!results[2].matched);
    g_regexTester.verify(results.size() == 3);
    g_regexTester.verify(regex_match("beforex", results, ecma));
    g_regexTester.verify(results[0] == "beforex");
    g_regexTester.verify(!results[1].matched);
    g_regexTester.verify(results[2] == "x");
    g_regexTester.verify(results.size() == 3);
    g_regexTester.verify(regex_match("beforexx", results, ecma));
    g_regexTester.verify(results[0] == "beforexx");
    g_regexTester.verify(results[1] == "x");
    g_regexTester.verify(results[2] == "x");
    g_regexTester.verify(results.size() == 3);

    const string erePattern = "b" + quantifier + "(ba)*";
    const regex ere(erePattern, extended);
    g_regexTester.verify(regex_search("", results, ere));
    g_regexTester.verify(results[0].matched);
    g_regexTester.verify(results[0] == "");
    g_regexTester.verify(!results[1].matched);
    g_regexTester.verify(results.size() == 2);
    g_regexTester.verify(regex_search("b", results, ere));
    g_regexTester.verify(results[0] == "b");
    g_regexTester.verify(!results[1].matched);
    g_regexTester.verify(results.size() == 2);
    // Note that in the following ? must give up its match in order for the overall match to be longest
    // (ECMA would match only "b")
    g_regexTester.verify(regex_search("baba", results, ere));
    g_regexTester.verify(results[0] == "baba");
    g_regexTester.verify(results[1] == "ba");
    g_regexTester.verify(results.size() == 2);
}

void test_VSO_208146_regex_smoke_test_rewritten_op_question() {
    test_case_VSO_208146_regex_smoke_test_rewritten_zero_or_one_quantifier("?");
}

void test_VSO_208146_regex_smoke_test_rewritten_explicit_quantifier() {
    test_case_VSO_208146_regex_smoke_test_rewritten_zero_or_one_quantifier("{0,1}");
}

void test_VSO_225160_match_bol_flag() {
    // Note that this tests that we are consistent about the ECMAScript "multiline" setting being
    // true, but the standard currently appears to mandate that that is false. We don't want to
    // break existing customers, but we should at least be consistently multiline.
    // See also: LWG-2343, LWG-2503
    const test_regex emptyAnchor(&g_regexTester, R"(^)");
    emptyAnchor.should_search_match("", "");
    emptyAnchor.should_search_fail("", match_not_bol);
    emptyAnchor.should_search_match("\n", "");
    emptyAnchor.should_search_match("\n", "", match_not_bol);

    const test_regex beginCd(&g_regexTester, R"(^cd)");
    beginCd.should_search_match("ab\ncdefg", "cd");
    beginCd.should_search_match("ab\ncdefg", "cd", match_not_bol);

    beginCd.should_search_match("cdefg", "cd");
    beginCd.should_search_fail("cdefg", match_not_bol);
    beginCd.should_search_match("\ncdefg", "cd");
    beginCd.should_search_match("\ncdefg", "cd", match_not_bol);

    beginCd.should_search_fail("ab\nxcdefg");
    beginCd.should_search_fail("ab\nxcdefg", match_not_bol);
}

void test_VSO_225160_match_eol_flag() {
    // Ditto multiline comment
    const test_regex emptyAnchor(&g_regexTester, R"($)");
    emptyAnchor.should_search_match("", "");
    emptyAnchor.should_search_fail("", match_not_eol);
    emptyAnchor.should_search_match("\n", "");
    emptyAnchor.should_search_match("\n", "", match_not_eol);

    const test_regex cdEnd(&g_regexTester, R"(cd$)");
    cdEnd.should_search_match("abcd\nefg", "cd");
    cdEnd.should_search_match("abcd\nefg", "cd", match_not_eol);

    cdEnd.should_search_match("abcd", "cd");
    cdEnd.should_search_fail("abcd", match_not_eol);
    cdEnd.should_search_match("abcd\n", "cd");
    cdEnd.should_search_match("abcd\n", "cd", match_not_eol);

    cdEnd.should_search_fail("abcdx\nefg");
    cdEnd.should_search_fail("abcdx\nefg", match_not_eol);
}

void test_VSO_226914_word_boundaries() {
    const test_regex emptyAnchor(&g_regexTester, R"(\b)");
    emptyAnchor.should_search_match("", "");
    emptyAnchor.should_search_fail("", match_not_bow);
    emptyAnchor.should_search_fail("", match_not_eow);
    emptyAnchor.should_search_fail("", match_not_bow | match_not_eow);

    const test_regex wordCd(&g_regexTester, R"(\bcd)");
    wordCd.should_search_match("cd", "cd");
    wordCd.should_search_fail("cd", match_not_bow);
    wordCd.should_search_match("cd", "cd", match_not_eow);
    wordCd.should_search_fail("cd", match_not_bow | match_not_eow);
    wordCd.should_search_match("#cd", "cd");
    wordCd.should_search_match("#cd", "cd", match_not_bow);
    wordCd.should_search_match("#cd", "cd", match_not_eow);
    wordCd.should_search_match("#cd", "cd", match_not_bow | match_not_eow);

    const test_regex cdWord(&g_regexTester, R"(cd\b)");
    cdWord.should_search_match("cd", "cd");
    cdWord.should_search_match("cd", "cd", match_not_bow);
    cdWord.should_search_fail("cd", match_not_eow);
    cdWord.should_search_fail("cd", match_not_bow | match_not_eow);
    cdWord.should_search_match("cd#", "cd");
    cdWord.should_search_match("cd#", "cd", match_not_bow);
    cdWord.should_search_match("cd#", "cd", match_not_eow);
    cdWord.should_search_match("cd#", "cd", match_not_bow | match_not_eow);

    const test_regex anyWordA(&g_regexTester, R"(.\ba)");
    anyWordA.should_search_match("#a", "#a");
    anyWordA.should_search_match("#a", "#a", match_not_bow);
    anyWordA.should_search_match("#a", "#a", match_not_eow);
    anyWordA.should_search_match("#a", "#a", match_not_bow | match_not_eow);

    anyWordA.should_search_fail("aa");
    anyWordA.should_search_fail("aa", match_not_bow);
    anyWordA.should_search_fail("aa", match_not_eow);
    anyWordA.should_search_fail("aa", match_not_bow | match_not_eow);

    const test_regex aWordAny(&g_regexTester, R"(a\b.)");
    aWordAny.should_search_match("a#", "a#");
    aWordAny.should_search_match("a#", "a#", match_not_bow);
    aWordAny.should_search_match("a#", "a#", match_not_eow);
    aWordAny.should_search_match("a#", "a#", match_not_bow | match_not_eow);

    aWordAny.should_search_fail("aa");
    aWordAny.should_search_fail("aa", match_not_bow);
    aWordAny.should_search_fail("aa", match_not_eow);
    aWordAny.should_search_fail("aa", match_not_bow | match_not_eow);
}

void test_gh_993() {
    // GH-993 regex::icase is not handled correctly for some input.
    {
        const wstring subject = L" Copyright";
        const test_wregex case_regex(&g_regexTester, LR"([a-z][a-z])", ECMAScript);
        const test_wregex icase_regex(&g_regexTester, LR"([a-z][a-z])", ECMAScript | icase);

        case_regex.should_search_match(subject, L"op");
        icase_regex.should_search_match(subject, L"Co");
    }

    {
        const wstring subject = L"blahZblah";
        const test_wregex Z_case_regex(&g_regexTester, LR"([Z])", ECMAScript);
        const test_wregex Z_icase_regex(&g_regexTester, LR"([Z])", ECMAScript | icase);
        const test_wregex z_case_regex(&g_regexTester, LR"([z])", ECMAScript);
        const test_wregex z_icase_regex(&g_regexTester, LR"([z])", ECMAScript | icase);

        Z_case_regex.should_search_match(subject, L"Z");
        Z_icase_regex.should_search_match(subject, L"Z");
        z_icase_regex.should_search_match(subject, L"Z");

        z_case_regex.should_search_fail(subject);
        z_case_regex.should_search_fail(subject, match_not_bow);
        z_case_regex.should_search_fail(subject, match_not_eow);
        z_case_regex.should_search_fail(subject, match_not_bow | match_not_eow);

        const wstring lowercase_subject = L"hungry_zombies";
        Z_case_regex.should_search_fail(lowercase_subject);
        Z_icase_regex.should_search_match(lowercase_subject, L"z");
        z_case_regex.should_search_match(lowercase_subject, L"z");
        z_icase_regex.should_search_match(lowercase_subject, L"z");
    }
}

int main() {
    test_dev10_449367_case_insensitivity_should_work();
    test_dev11_462743_regex_collate_should_not_disable_regex_icase();
    test_dev10_448906_regex_infinite_loops_incorrect_results();
    test_dev10_505773_regex_should_accept_empty_alternations();
    test_dev11_821930_literal_dollar_replacement_characters();
    test_dev10_514062_regex_should_throw_regex_error_rather_than_crash_for_bogus_character_classes();
    test_dev10_767618_nested_quantifiers_should_match();
    test_dev11_291918_more_nested_quantifiers_should_match();
    test_VSO_167760_nested_quantifiers_should_not_infinite_loop();
    test_DDB_153116_replacements();
    test_DDB_169529_replacements();
    test_dev11_900202_regex_should_accept_backreferences();
    test_dev11_900202_regex_should_throw_for_self_referencing_backreferences();
    test_dev10_897466_regex_should_support_more_than_31_capture_groups();
    test_regex_should_throw_for_lookbehind();
    test_regex_simple_loop_detection_enters_alternations_and_assertions();
    test_VSO_159685_trailing_escape_should_throw_error_escape();
    test_dev11_658169_regex_universe_character_classes_should_match_correctly();
    test_VSO_101318_ECMAScript_identity_escapes_should_be_lax();
    test_VSO_208146_regex_should_special_case_optional_as_non_quantifier();
    test_VSO_208146_regex_smoke_test_rewritten_op_question();
    test_VSO_208146_regex_smoke_test_rewritten_explicit_quantifier();
    test_VSO_225160_match_bol_flag();
    test_VSO_225160_match_eol_flag();
    test_VSO_226914_word_boundaries();
    test_gh_993();

    return g_regexTester.result();
}
