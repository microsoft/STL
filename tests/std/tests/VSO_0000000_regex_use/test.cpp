// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
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
    g_regexTester.should_replace_to("abc  def def  ghi", "\\b", "X", format_default, "XabcX  XdefX XdefX  XghiX");
    g_regexTester.should_replace_to("abc  def def  ghi", "\\B", "X", format_default, "aXbXc X dXeXf dXeXf X gXhXi");
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
    g_regexTester.should_throw(R"((?<=abc))", error_badrepeat);
    g_regexTester.should_throw(R"((?<!abc))", error_badrepeat);
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
    // After implementation of LWG-2503/GH-73: These tests make sure that
    // we consistently implement the "multiline" option for ECMAScript
    // (whether the ECMAScript flag is included or not)
    for (syntax_option_type syntax : {multiline, ECMAScript | multiline}) {
        for (string line_terminator : {"\n", "\r"}) {
            const test_regex emptyAnchor(&g_regexTester, R"(^)", syntax);
            emptyAnchor.should_search_match("", "");
            emptyAnchor.should_search_fail("", match_not_bol);
            emptyAnchor.should_search_match(line_terminator, "");
            emptyAnchor.should_search_match(line_terminator, "", match_not_bol);

            const test_regex beginCd(&g_regexTester, R"(^cd)", syntax);
            beginCd.should_search_match("ab" + line_terminator + "cdefg", "cd");
            beginCd.should_search_match("ab" + line_terminator + "cdefg", "cd", match_not_bol);

            beginCd.should_search_match("cdefg", "cd");
            beginCd.should_search_fail("cdefg", match_not_bol);
            beginCd.should_search_match(line_terminator + "cdefg", "cd");
            beginCd.should_search_match(line_terminator + "cdefg", "cd", match_not_bol);

            beginCd.should_search_fail("ab" + line_terminator + "xcdefg");
            beginCd.should_search_fail("ab" + line_terminator + "xcdefg", match_not_bol);
        }

        for (wstring line_terminator :
            {L"\n", L"\r", L"\u2028", L"\u2029"}) { // U+2028 LINE SEPARATOR, U+2029 PARAGRAPH SEPARATOR
            const test_wregex emptyAnchor(&g_regexTester, LR"(^)", syntax);
            emptyAnchor.should_search_match(L"", L"");
            emptyAnchor.should_search_fail(L"", match_not_bol);
            emptyAnchor.should_search_match(line_terminator, L"");
            emptyAnchor.should_search_match(line_terminator, L"", match_not_bol);

            const test_wregex beginCd(&g_regexTester, LR"(^cd)", syntax);
            beginCd.should_search_match(L"ab" + line_terminator + L"cdefg", L"cd");
            beginCd.should_search_match(L"ab" + line_terminator + L"cdefg", L"cd", match_not_bol);

            beginCd.should_search_match(L"cdefg", L"cd");
            beginCd.should_search_fail(L"cdefg", match_not_bol);
            beginCd.should_search_match(line_terminator + L"cdefg", L"cd");
            beginCd.should_search_match(line_terminator + L"cdefg", L"cd", match_not_bol);

            beginCd.should_search_fail(L"ab" + line_terminator + L"xcdefg");
            beginCd.should_search_fail(L"ab" + line_terminator + L"xcdefg", match_not_bol);
        }
    }
}

void test_VSO_225160_match_eol_flag() {
    // Ditto multiline comment
    for (syntax_option_type syntax : {multiline, ECMAScript | multiline}) {
        for (string line_terminator : {"\n", "\r"}) {
            const test_regex emptyAnchor(&g_regexTester, R"($)", syntax);
            emptyAnchor.should_search_match("", "");
            emptyAnchor.should_search_fail("", match_not_eol);
            emptyAnchor.should_search_match(line_terminator, "");
            emptyAnchor.should_search_match(line_terminator, "", match_not_eol);

            const test_regex cdEnd(&g_regexTester, R"(cd$)", syntax);
            cdEnd.should_search_match("abcd" + line_terminator + "efg", "cd");
            cdEnd.should_search_match("abcd" + line_terminator + "efg", "cd", match_not_eol);

            cdEnd.should_search_match("abcd", "cd");
            cdEnd.should_search_fail("abcd", match_not_eol);
            cdEnd.should_search_match("abcd" + line_terminator, "cd");
            cdEnd.should_search_match("abcd" + line_terminator, "cd", match_not_eol);

            cdEnd.should_search_fail("abcdx" + line_terminator + "efg");
            cdEnd.should_search_fail("abcdx" + line_terminator + "efg", match_not_eol);
        }

        for (wstring line_terminator :
            {L"\n", L"\r", L"\u2028", L"\u2029"}) { // U+2028 LINE SEPARATOR, U+2029 PARAGRAPH SEPARATOR
            const test_wregex emptyAnchor(&g_regexTester, LR"($)", syntax);
            emptyAnchor.should_search_match(L"", L"");
            emptyAnchor.should_search_fail(L"", match_not_eol);
            emptyAnchor.should_search_match(line_terminator, L"");
            emptyAnchor.should_search_match(line_terminator, L"", match_not_eol);

            const test_wregex cdEnd(&g_regexTester, LR"(cd$)", syntax);
            cdEnd.should_search_match(L"abcd" + line_terminator + L"efg", L"cd");
            cdEnd.should_search_match(L"abcd" + line_terminator + L"efg", L"cd", match_not_eol);

            cdEnd.should_search_match(L"abcd", L"cd");
            cdEnd.should_search_fail(L"abcd", match_not_eol);
            cdEnd.should_search_match(L"abcd" + line_terminator, L"cd");
            cdEnd.should_search_match(L"abcd" + line_terminator, L"cd", match_not_eol);

            cdEnd.should_search_fail(L"abcdx" + line_terminator + L"efg");
            cdEnd.should_search_fail(L"abcdx" + line_terminator + L"efg", match_not_eol);
        }
    }
}

void test_VSO_226914_word_boundaries() {
    const test_regex emptyAnchor(&g_regexTester, R"(\b)");
    emptyAnchor.should_search_fail("");
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

void test_construction_from_nullptr_and_zero() {
    {
        regex re(nullptr, 0);
        assert(re.mark_count() == 0);
    }
    {
        wregex re(nullptr, 0);
        assert(re.mark_count() == 0);
    }
}

void test_gh_73() {
    // GH-73: LWG-2503 multiline option should be added to syntax_option_type
    for (syntax_option_type grammar : {basic, grep, extended, egrep, awk}) {
        for (syntax_option_type multiline_mode : {syntax_option_type{}, multiline}) {
            {
                test_regex a_anchored_on_both_sides(&g_regexTester, "^a$", grammar | multiline_mode);
                a_anchored_on_both_sides.should_search_match("a", "a");
                a_anchored_on_both_sides.should_search_fail("b\na");
                a_anchored_on_both_sides.should_search_fail("a\nb");
            }

            {
                test_regex a_anchored_front(&g_regexTester, "^a", grammar | multiline_mode);
                a_anchored_front.should_search_match("a", "a");
                a_anchored_front.should_search_match("a\n", "a");
                a_anchored_front.should_search_match("a\nb", "a");
                a_anchored_front.should_search_fail("b\na");
                a_anchored_front.should_search_fail("\na");
            }

            {
                test_regex a_anchored_back(&g_regexTester, "a$", grammar | multiline_mode);
                a_anchored_back.should_search_match("a", "a");
                a_anchored_back.should_search_match("\na", "a");
                a_anchored_back.should_search_match("b\na", "a");
                a_anchored_back.should_search_fail("a\nb");
                a_anchored_back.should_search_fail("a\n");
            }
        }
    }

    for (syntax_option_type grammar : {syntax_option_type{}, ECMAScript}) {
        {
            test_regex a_anchored_on_both_sides(&g_regexTester, "^a$", grammar);
            a_anchored_on_both_sides.should_search_match("a", "a");
            a_anchored_on_both_sides.should_search_fail("b\na");
            a_anchored_on_both_sides.should_search_fail("a\nb");
        }

        {
            test_regex a_anchored_front(&g_regexTester, "^a", grammar);
            a_anchored_front.should_search_match("a", "a");
            a_anchored_front.should_search_match("a\n", "a");
            a_anchored_front.should_search_match("a\nb", "a");
            a_anchored_front.should_search_fail("b\na");
            a_anchored_front.should_search_fail("\na");
        }

        {
            test_regex a_anchored_back(&g_regexTester, "a$", grammar);
            a_anchored_back.should_search_match("a", "a");
            a_anchored_back.should_search_match("\na", "a");
            a_anchored_back.should_search_match("b\na", "a");
            a_anchored_back.should_search_fail("a\nb");
            a_anchored_back.should_search_fail("a\n");
        }
    }

    for (syntax_option_type syntax : {multiline, ECMAScript | multiline}) {
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

void test_gh_731() {
    // GH-731: <regex>: Incorrect behavior for capture groups
    // GH-996: regex_search behaves incorrectly when the regex contains R"(\[)"

    // Several bugs were fixed in ECMAScript (depth-first) and POSIX (leftmost-longest) matching rules.
    {
        const test_regex ecma_regex(&g_regexTester, R"((A+)\s*(B+)?\s*B*)", ECMAScript);
        ecma_regex.should_search_match_capture_groups("AAA BBB", "AAA BBB", match_default, {{0, 3}, {4, 7}});
    }
    for (syntax_option_type option : {extended, egrep, awk}) {
        const test_regex posix_regex(&g_regexTester, R"((A+)[[:space:]]*(B+)?[[:space:]]*B*)", option);
        posix_regex.should_search_match_capture_groups("AAA BBB", "AAA BBB", match_default, {{0, 3}, {4, 7}});
    }

    {
        const test_regex ecma_regex(&g_regexTester, ".*(cat|concatenate)", ECMAScript);
        ecma_regex.should_search_match_capture_groups("WXconcatenateYZ", "WXconcat", match_default, {{5, 8}});
    }
    for (syntax_option_type option : {extended, egrep, awk}) {
        const test_regex posix_regex(&g_regexTester, ".*(cat|concatenate)", option);
        posix_regex.should_search_match_capture_groups("WXconcatenateYZ", "WXconcatenate", match_default, {{2, 13}});
    }

    {
        const test_regex ecma_regex(&g_regexTester, "(aa|aabaac|ba|b|c)*", ECMAScript);
        ecma_regex.should_search_match_capture_groups("aabaac", "aaba", match_default, {{2, 4}});
    }
    for (syntax_option_type option : {extended, egrep, awk}) {
        const test_regex posix_regex(&g_regexTester, "(aa|aabaac|ba|b|c)*", option);
        posix_regex.should_search_match_capture_groups("aabaac", "aabaac", match_default, {{0, 6}});
    }

    {
        const test_regex ecma_regex(&g_regexTester, ".*(a|bacc|baccc)", ECMAScript);
        ecma_regex.should_search_match_capture_groups("ddbacccd", "ddba", match_default, {{3, 4}});
    }
    {
        const test_regex ecma_regex(&g_regexTester, ".*?(a|bacc|baccc)", ECMAScript);
        ecma_regex.should_search_match_capture_groups("ddbacccd", "ddbacc", match_default, {{2, 6}});
    }
    for (syntax_option_type option : {extended, egrep, awk}) {
        const test_regex posix_regex(&g_regexTester, ".*(a|bacc|baccc)", option);
        posix_regex.should_search_match_capture_groups("ddbacccd", "ddbaccc", match_default, {{2, 7}});
    }

    {
        const test_regex ecma_regex(&g_regexTester, R"(^[[:blank:]]*#([^\n]*\\[[:space:]]+)*[^\n]*)", ECMAScript);
        ecma_regex.should_search_match_capture_groups("#define some_symbol(x) \\  \r\n  cat();\\\r\n   printf(#x);",
            "#define some_symbol(x) \\  \r\n  cat();\\\r\n   printf(#x);", match_default, {{30, 42}});
    }
    {
        const test_regex awk_regex(&g_regexTester, R"(^[[:blank:]]*#([^\n]*\\[[:space:]]+)*[^\n]*)", awk);
        awk_regex.should_search_match_capture_groups("#define some_symbol(x) \\  \r\n  cat();\\\r\n   printf(#x);",
            "#define some_symbol(x) \\  \r\n  cat();\\\r\n   printf(#x);", match_default, {{28, 42}});
    }
    {
        const test_regex extended_regex(&g_regexTester, "^[[:blank:]]*#([^\n]*\\\\[[:space:]]+)*[^\n]*", extended);
        extended_regex.should_search_match_capture_groups("#define some_symbol(x) \\  \r\n  cat();\\\r\n   printf(#x);",
            "#define some_symbol(x) \\  \r\n  cat();\\\r\n   printf(#x);", match_default, {{28, 42}});
    }

    {
        const test_regex ecma_regex(&g_regexTester, "(ab*)*(ce|bbceef)", ECMAScript);
        ecma_regex.should_search_match_capture_groups("aababbbceef", "aababbbce", match_default, {{3, 7}, {7, 9}});
    }
    for (syntax_option_type option : {extended, egrep, awk}) {
        const test_regex posix_regex(&g_regexTester, "(ab*)*(ce|bbceef)", option);
        posix_regex.should_search_match_capture_groups("aababbbceef", "aababbbceef", match_default, {{3, 5}, {5, 11}});
    }

    {
        // GH-996 test case
        const test_regex ecma_regex(&g_regexTester, R"( *((<<)|(\[)|(.+)))", ECMAScript);
        ecma_regex.should_search_match_capture_groups(
            " [<</Category/Export>>]>>", " [", match_default, {{1, 2}, {-1, -1}, {1, 2}, {-1, -1}});
    }
}

void test_gh_992() {
    // GH-992 <regex> mishandles locale-based character classes outside of the char range
    {
        const test_wregex neg_w_regex(&g_regexTester, LR"(Y[\W]*Z)");
        neg_w_regex.should_search_match(L"xxxY      Zxxx", L"Y      Z");
        neg_w_regex.should_search_match(L"xxxY  \x2009   Zxxx", L"Y  \x2009   Z"); // U+2009 THIN SPACE
        neg_w_regex.should_search_fail(L"xxxY  \x0078   Zxxx"); // U+0078 LATIN SMALL LETTER X
        neg_w_regex.should_search_fail(L"xxxY  \x03C7   Zxxx"); // U+03C7 GREEK SMALL LETTER CHI
        neg_w_regex.should_search_fail(L"xxxY  3   Zxxx");
        neg_w_regex.should_search_fail(L"xxxY  \x0662   Zxxx"); // U+0662 ARABIC-INDIC DIGIT TWO
    }
    {
        const test_wregex neg_s_regex(&g_regexTester, LR"(Y[\S]*Z)");
        neg_s_regex.should_search_match(L"xxxYxx\x0078xxxZxxx", L"Yxx\x0078xxxZ"); // U+0078 LATIN SMALL LETTER X
        neg_s_regex.should_search_match(L"xxxYxx\x03C7xxxZxxx", L"Yxx\x03C7xxxZ"); // U+03C7 GREEK SMALL LETTER CHI
        neg_s_regex.should_search_match(L"xxxYxx3xxxZxxx", L"Yxx3xxxZ");
        neg_s_regex.should_search_match(L"xxxYxx\x0662xxxZxxx", L"Yxx\x0662xxxZ"); // U+0662 ARABIC-INDIC DIGIT TWO
        neg_s_regex.should_search_fail(L"xxxYxx xxxZxxx");
        neg_s_regex.should_search_fail(L"xxxYxx\x2009xxxZxxx"); // U+2009 THIN SPACE
    }
    for (const wstring& pattern : {LR"(Y[\D]*Z)", LR"(Y[\W\D]*Z)"}) {
        const test_wregex neg_d_regex(&g_regexTester, pattern);
        neg_d_regex.should_search_match(L"xxxYxx\x0078xxxZxxx", L"Yxx\x0078xxxZ"); // U+0078 LATIN SMALL LETTER X
        neg_d_regex.should_search_match(L"xxxYxx\x03C7xxxZxxx", L"Yxx\x03C7xxxZ"); // U+03C7 GREEK SMALL LETTER CHI
        neg_d_regex.should_search_match(L"xxxYxx xxxZxxx", L"Yxx xxxZ");
        neg_d_regex.should_search_match(L"xxxYxx\x2009xxxZxxx", L"Yxx\x2009xxxZ"); // U+2009 THIN SPACE
        neg_d_regex.should_search_fail(L"xxxYxx3xxxZxxx");
        neg_d_regex.should_search_fail(L"xxxYxx\x0662xxxZxxx"); // U+0662 ARABIC-INDIC DIGIT TWO
    }
    for (const wstring& pattern : {LR"(Y[\W\S]*Z)", LR"(Y[\S\D]*Z)", LR"(Y[\W\S\D]*Z)"}) {
        const test_wregex class_matches_all_regex(&g_regexTester, pattern);
        class_matches_all_regex.should_search_match(
            L"xxxYxx\x0078xxxZxxx", L"Yxx\x0078xxxZ"); // U+0078 LATIN SMALL LETTER X
        class_matches_all_regex.should_search_match(
            L"xxxYxx\x03C7xxxZxxx", L"Yxx\x03C7xxxZ"); // U+03C7 GREEK SMALL LETTER CHI
        class_matches_all_regex.should_search_match(L"xxxYxx xxxZxxx", L"Yxx xxxZ");
        class_matches_all_regex.should_search_match(L"xxxYxx\x2009xxxZxxx", L"Yxx\x2009xxxZ"); // U+2009 THIN SPACE
        class_matches_all_regex.should_search_match(L"xxxYxx3xxxZxxx", L"Yxx3xxxZ");
        class_matches_all_regex.should_search_match(
            L"xxxYxx\x0662xxxZxxx", L"Yxx\x0662xxxZ"); // U+0662 ARABIC-INDIC DIGIT TWO
    }
    {
        const test_wregex neg_w_regex_skip(&g_regexTester, LR"([\W])");
        neg_w_regex_skip.should_search_match(L"xxxx\x2009xxxx", L"\x2009"); // U+2009 THIN SPACE
        neg_w_regex_skip.should_search_fail(L"xxxx\x03C7xxxx"); // U+03C7 GREEK SMALL LETTER CHI
        neg_w_regex_skip.should_search_fail(L"xxxx\x0662xxxx"); // U+0662 ARABIC-INDIC DIGIT TWO
    }
    {
        const test_wregex neg_s_regex_skip(&g_regexTester, LR"([\S])");
        neg_s_regex_skip.should_search_match(L"  \x03C7  ", L"\x03C7"); // U+03C7 GREEK SMALL LETTER CHI
        neg_s_regex_skip.should_search_match(L"  \x0662  ", L"\x0662"); // U+0662 ARABIC-INDIC DIGIT TWO
        neg_s_regex_skip.should_search_fail(L"  \x2009  "); // U+2009 THIN SPACE
    }
    {
        const test_wregex neg_d_regex_skip(&g_regexTester, LR"([\D])");
        neg_d_regex_skip.should_search_match(L"1623\x03C7"s + L"253", L"\x03C7"); // U+03C7 GREEK SMALL LETTER CHI
        neg_d_regex_skip.should_search_match(L"1623\x2009"s + L"253", L"\x2009"); // U+2009 THIN SPACE
        neg_d_regex_skip.should_search_fail(L"1623\x0662"s + L"253"); // U+0662 ARABIC-INDIC DIGIT TWO
    }
    {
        const test_wregex double_negative_w(&g_regexTester, LR"([^\W])");
        double_negative_w.should_search_match(L"\x03C7", L"\x03C7"); // U+03C7 GREEK SMALL LETTER CHI
        double_negative_w.should_search_match(L"\x0662", L"\x0662"); // U+0662 ARABIC-INDIC DIGIT TWO
        double_negative_w.should_search_fail(L"\x2009"); // U+2009 THIN SPACE
    }
    {
        const test_wregex double_negative_s(&g_regexTester, LR"([^\S])");
        double_negative_s.should_search_fail(L"\x03C7"); // U+03C7 GREEK SMALL LETTER CHI
        double_negative_s.should_search_fail(L"\x0662"); // U+0662 ARABIC-INDIC DIGIT TWO
        double_negative_s.should_search_match(L"\x2009", L"\x2009"); // U+2009 THIN SPACE
    }
    {
        const test_wregex double_negative_d(&g_regexTester, LR"([^\D])");
        double_negative_d.should_search_fail(L"\x03C7"); // U+03C7 GREEK SMALL LETTER CHI
        double_negative_d.should_search_match(L"\x0662", L"\x0662"); // U+0662 ARABIC-INDIC DIGIT TWO
        double_negative_d.should_search_fail(L"\x2009"); // U+2009 THIN SPACE
    }
    for (const wstring& pattern : {LR"([\w\W])", LR"([\s\S])", LR"([\d\D])"}) {
        const test_wregex omni_regex(&g_regexTester, pattern);
        omni_regex.should_search_match(L"\x03C7", L"\x03C7"); // U+03C7 GREEK SMALL LETTER CHI
        omni_regex.should_search_match(L"\x0662", L"\x0662"); // U+0662 ARABIC-INDIC DIGIT TWO
        omni_regex.should_search_match(L"\x2009", L"\x2009"); // U+2009 THIN SPACE
    }
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

void test_gh_4995() {
    // GH-4995: R"([\d-e])" should be rejected
    g_regexTester.should_throw(R"([\d-e])", error_range);
    g_regexTester.should_throw(R"([e-\d])", error_range);
    g_regexTester.should_throw(R"([\w-\d])", error_range);
    g_regexTester.should_throw("[[:digit:]-e]", error_range);
    g_regexTester.should_throw("[e-[:digit:]]", error_range);
    g_regexTester.should_throw("[[:alpha:]-[:digit:]]", error_range);
#ifndef _M_CEE_PURE
    g_regexTester.should_throw("[[=a=]-e]", error_range, ECMAScript | regex::collate);
    g_regexTester.should_throw("[e-[=a=]]", error_range, ECMAScript | regex::collate);
    g_regexTester.should_throw("[[=a=]-[=b=]]", error_range, ECMAScript | regex::collate);
#endif // ^^^ !defined(_M_CEE_PURE) ^^^

    // Test valid cases:
    g_regexTester.should_not_match("b", R"([\d-])");
    g_regexTester.should_match("5", R"([\d-])");
    g_regexTester.should_match("-", R"([\d-])");

    g_regexTester.should_not_match("b", R"([-\d])");
    g_regexTester.should_match("5", R"([-\d])");
    g_regexTester.should_match("-", R"([-\d])");

    g_regexTester.should_match("b", R"([a-c\d])");
    g_regexTester.should_match("5", R"([a-c\d])");
    g_regexTester.should_not_match("-", R"([a-c\d])");

    g_regexTester.should_match("b", R"([\da-c])");
    g_regexTester.should_match("5", R"([\da-c])");
    g_regexTester.should_not_match("-", R"([\da-c])");
}

void test_gh_5058() {
    // GH-5058 "<regex>: Small cleanups" changed some default constructors to be defaulted.
    // Verify that <regex> types are still const-default-constructible (N4993 [dcl.init.general]/8).
    {
        const regex r;
        assert(!regex_match("cats", r));
    }
    {
        const csub_match csm;
        assert(!csm.matched);
        assert(csm.first == nullptr);
        assert(csm.second == nullptr);
    }
    {
        const ssub_match ssm;
        assert(!ssm.matched);
        assert(ssm.first == string::const_iterator{});
        assert(ssm.second == string::const_iterator{});
    }
    {
        const cmatch cmr;
        assert(!cmr.ready());
        assert(cmr.size() == 0);
    }
    {
        const smatch smr;
        assert(!smr.ready());
        assert(smr.size() == 0);
    }
    {
        const cregex_iterator cri;
        assert(cri == cregex_iterator{});
    }
    {
        const sregex_iterator sri;
        assert(sri == sregex_iterator{});
    }
    {
        const cregex_token_iterator crti;
        assert(crti == cregex_token_iterator{});
    }
    {
        const sregex_token_iterator srti;
        assert(srti == sregex_token_iterator{});
    }
}

void test_gh_5160() {
    // GH-5160 fixed mishandled negated character class escapes
    // outside character class definitions
    {
        const test_wregex neg_w_regex(&g_regexTester, LR"(Y\W*Z)");
        neg_w_regex.should_search_match(L"xxxY      Zxxx", L"Y      Z");
        neg_w_regex.should_search_match(L"xxxY  \x2009   Zxxx", L"Y  \x2009   Z"); // U+2009 THIN SPACE
        neg_w_regex.should_search_fail(L"xxxY  \x0078   Zxxx"); // U+0078 LATIN SMALL LETTER X
        neg_w_regex.should_search_fail(L"xxxY  \x03C7   Zxxx"); // U+03C7 GREEK SMALL LETTER CHI
        neg_w_regex.should_search_fail(L"xxxY  3   Zxxx");
        neg_w_regex.should_search_fail(L"xxxY  \x0662   Zxxx"); // U+0662 ARABIC-INDIC DIGIT TWO
    }
    {
        const test_wregex neg_s_regex(&g_regexTester, LR"(Y\S*Z)");
        neg_s_regex.should_search_match(L"xxxYxx\x0078xxxZxxx", L"Yxx\x0078xxxZ"); // U+0078 LATIN SMALL LETTER X
        neg_s_regex.should_search_match(L"xxxYxx\x03C7xxxZxxx", L"Yxx\x03C7xxxZ"); // U+03C7 GREEK SMALL LETTER CHI
        neg_s_regex.should_search_match(L"xxxYxx3xxxZxxx", L"Yxx3xxxZ");
        neg_s_regex.should_search_match(L"xxxYxx\x0662xxxZxxx", L"Yxx\x0662xxxZ"); // U+0662 ARABIC-INDIC DIGIT TWO
        neg_s_regex.should_search_fail(L"xxxYxx xxxZxxx");
        neg_s_regex.should_search_fail(L"xxxYxx\x2009xxxZxxx"); // U+2009 THIN SPACE
    }
    {
        const test_wregex neg_d_regex(&g_regexTester, LR"(Y\D*Z)");
        neg_d_regex.should_search_match(L"xxxYxx\x0078xxxZxxx", L"Yxx\x0078xxxZ"); // U+0078 LATIN SMALL LETTER X
        neg_d_regex.should_search_match(L"xxxYxx\x03C7xxxZxxx", L"Yxx\x03C7xxxZ"); // U+03C7 GREEK SMALL LETTER CHI
        neg_d_regex.should_search_match(L"xxxYxx xxxZxxx", L"Yxx xxxZ");
        neg_d_regex.should_search_match(L"xxxYxx\x2009xxxZxxx", L"Yxx\x2009xxxZ"); // U+2009 THIN SPACE
        neg_d_regex.should_search_fail(L"xxxYxx3xxxZxxx");
        neg_d_regex.should_search_fail(L"xxxYxx\x0662xxxZxxx"); // U+0662 ARABIC-INDIC DIGIT TWO
    }
    {
        const test_wregex neg_w_regex_skip(&g_regexTester, LR"(\W)");
        neg_w_regex_skip.should_search_match(L"xxxx\x2009xxxx", L"\x2009"); // U+2009 THIN SPACE
        neg_w_regex_skip.should_search_fail(L"xxxx\x03C7xxxx"); // U+03C7 GREEK SMALL LETTER CHI
        neg_w_regex_skip.should_search_fail(L"xxxx\x0662xxxx"); // U+0662 ARABIC-INDIC DIGIT TWO
    }
    {
        const test_wregex neg_s_regex_skip(&g_regexTester, LR"(\S)");
        neg_s_regex_skip.should_search_match(L"  \x03C7  ", L"\x03C7"); // U+03C7 GREEK SMALL LETTER CHI
        neg_s_regex_skip.should_search_match(L"  \x0662  ", L"\x0662"); // U+0662 ARABIC-INDIC DIGIT TWO
        neg_s_regex_skip.should_search_fail(L"  \x2009  "); // U+2009 THIN SPACE
    }
    {
        const test_wregex neg_d_regex_skip(&g_regexTester, LR"(\D)");
        neg_d_regex_skip.should_search_match(L"1623\x03C7"s + L"253", L"\x03C7"); // U+03C7 GREEK SMALL LETTER CHI
        neg_d_regex_skip.should_search_match(L"1623\x2009"s + L"253", L"\x2009"); // U+2009 THIN SPACE
        neg_d_regex_skip.should_search_fail(L"1623\x0662"s + L"253"); // U+0662 ARABIC-INDIC DIGIT TWO
    }
}

void test_gh_5165_syntax_option(const syntax_option_type basic_or_grep) {
    g_regexTester.should_not_match("yx", "y[^x]", basic_or_grep);
    g_regexTester.should_match("yz", "y[^x]", basic_or_grep);
    g_regexTester.should_match("y^", "y[^x]", basic_or_grep);

    g_regexTester.should_match("yx", "y[x^]", basic_or_grep);
    g_regexTester.should_not_match("yz", "y[x^]", basic_or_grep);
    g_regexTester.should_match("y^", "y[x^]", basic_or_grep);

    g_regexTester.should_not_match("yx", "y[^x^]", basic_or_grep);
    g_regexTester.should_match("yz", "y[^x^]", basic_or_grep);
    g_regexTester.should_not_match("y^", "y[^x^]", basic_or_grep);

    {
        const test_regex no_anchor(&g_regexTester, "meo[wW]", basic_or_grep);
        no_anchor.should_search_match("meow_machine", "meow");
        no_anchor.should_search_match("homeowner", "meow");
    }
    {
        const test_regex beginning_anchor(&g_regexTester, "^meo[wW]", basic_or_grep);
        beginning_anchor.should_search_match("meow_machine", "meow");
        beginning_anchor.should_search_fail("homeowner");
    }
    {
        const test_regex middle_anchor(&g_regexTester, "me^o[wW]", basic_or_grep);
        middle_anchor.should_search_fail("meow_machine");
        middle_anchor.should_search_fail("homeowner");
        middle_anchor.should_search_match("home^owner", "me^ow");
    }
    {
        const test_regex double_carets(&g_regexTester, "^^meo[wW]", basic_or_grep);
        double_carets.should_search_fail("meow_machine");
        double_carets.should_search_fail("homeowner");
        double_carets.should_search_match("^meow_machine", "^meow");
        double_carets.should_search_fail("^^meow_machine");
        double_carets.should_search_fail("ho^meowner");
        double_carets.should_search_fail("ho^^meowner");
    }

    g_regexTester.should_not_match("me^ow", R"(me\(^o[wW]\))", basic_or_grep);
    g_regexTester.should_not_match("meow", R"(me\(^o[wW]\))", basic_or_grep);

    {
        const test_regex firstgroup_anchor(&g_regexTester, R"(\(^meo[wW]\))", basic_or_grep);
        firstgroup_anchor.should_search_match("meow_machine", "meow");
        firstgroup_anchor.should_search_fail("^meow_machine");
        firstgroup_anchor.should_search_fail("homeowner");
        firstgroup_anchor.should_search_fail("ho^meowner");
    }
    {
        const test_regex prefixedgroup_anchor(&g_regexTester, R"(.*\(^meo[wW]\))", basic_or_grep);
        prefixedgroup_anchor.should_search_match("meow_machine", "meow");
        prefixedgroup_anchor.should_search_fail("^meow_machine");
        prefixedgroup_anchor.should_search_fail("homeowner");
        prefixedgroup_anchor.should_search_fail("ho^meowner");
    }
    {
        const test_regex secondgroup_anchor(&g_regexTester, R"(\(.*\)\(^meo[wW]\))", basic_or_grep);
        secondgroup_anchor.should_search_match("meow_machine", "meow");
        secondgroup_anchor.should_search_fail("^meow_machine");
        secondgroup_anchor.should_search_fail("homeowner");
        secondgroup_anchor.should_search_fail("ho^meowner");
    }
    {
        const test_regex nested_anchor(&g_regexTester, R"(.*\(^\(^meo[wW]\)\))", basic_or_grep);
        nested_anchor.should_search_match("meow_machine", "meow");
        nested_anchor.should_search_fail("^meow_machine");
        nested_anchor.should_search_fail("^^meow_machine");
        nested_anchor.should_search_fail("homeowner");
        nested_anchor.should_search_fail("ho^meowner");
        nested_anchor.should_search_fail("ho^^meowner");
    }
    {
        const test_regex double_carets(&g_regexTester, R"(.*\(^^meo[wW]\))", basic_or_grep);
        double_carets.should_search_fail("meow_machine");
        double_carets.should_search_match("^meow_machine", "^meow");
        double_carets.should_search_fail("^^meow_machine");
        double_carets.should_search_fail("homeowner");
        double_carets.should_search_fail("ho^meowner");
        double_carets.should_search_fail("ho^^meowner");
    }

    // Validate correct handling of star at the
    // beginning of an expression (with or without optional caret).
    g_regexTester.should_match("*", "*", basic_or_grep);
    g_regexTester.should_not_match("**", "*", basic_or_grep);
    g_regexTester.should_match("****", "**", basic_or_grep);
    g_regexTester.should_throw("***", error_badrepeat, basic_or_grep);

    g_regexTester.should_match("*", "^*", basic_or_grep);
    g_regexTester.should_not_match("**", "^*", basic_or_grep);
    g_regexTester.should_not_match("^*", "^*", basic_or_grep);
    g_regexTester.should_match("****", "^**", basic_or_grep);
    g_regexTester.should_throw("^***", error_badrepeat, basic_or_grep);

    g_regexTester.should_match("*aa", "*a*", basic_or_grep);
    g_regexTester.should_match("*a", "*a*", basic_or_grep);
    g_regexTester.should_not_match("aa", "*a*", basic_or_grep);
    g_regexTester.should_not_match("*a*", "*a*", basic_or_grep);

    g_regexTester.should_match("*aa", "^*a*", basic_or_grep);
    g_regexTester.should_not_match("aa", "^*a*", basic_or_grep);
    g_regexTester.should_not_match("*a*", "^*a*", basic_or_grep);
    g_regexTester.should_not_match("^*a", "^*a*", basic_or_grep);
    g_regexTester.should_not_match("^*aa", "^*a*", basic_or_grep);
    g_regexTester.should_not_match("^*a*", "^*a*", basic_or_grep);

    g_regexTester.should_match("*", R"(\(*\))", basic_or_grep);
    g_regexTester.should_not_match("**", R"(\(*\))", basic_or_grep);
    g_regexTester.should_match("****", R"(\(**\))", basic_or_grep);
    g_regexTester.should_throw(R"(\(***\))", error_badrepeat, basic_or_grep);

    g_regexTester.should_match("*", R"(\(^*\))", basic_or_grep);
    g_regexTester.should_not_match("**", R"(\(^*\))", basic_or_grep);
    g_regexTester.should_not_match("^*", R"(\(^*\))", basic_or_grep);
    g_regexTester.should_match("***", R"(\(^**\))", basic_or_grep);
    g_regexTester.should_throw(R"(\(^***\))", error_badrepeat, basic_or_grep);

    g_regexTester.should_match("*aa", R"(\(*a*\))", basic_or_grep);
    g_regexTester.should_match("*a", R"(\(*a*\))", basic_or_grep);
    g_regexTester.should_not_match("aa", R"(\(*a*\))", basic_or_grep);
    g_regexTester.should_not_match("*a*", R"(\(*a*\))", basic_or_grep);

    g_regexTester.should_match("*aa", R"(\(^*a*\))", basic_or_grep);
    g_regexTester.should_not_match("aa", R"(\(^*a*\))", basic_or_grep);
    g_regexTester.should_not_match("*a*", R"(\(^*a*\))", basic_or_grep);
    g_regexTester.should_not_match("^*a", R"(\(^*a*\))", basic_or_grep);
    g_regexTester.should_not_match("^*aa", R"(\(^*a*\))", basic_or_grep);
    g_regexTester.should_not_match("^*a*", R"(\(^*a*\))", basic_or_grep);

    g_regexTester.should_match("*", R"(.*\(^*\))", basic_or_grep);
    g_regexTester.should_not_match("**", R"(.*\(^*\))", basic_or_grep);
    g_regexTester.should_not_match("^*", R"(.*\(^*\))", basic_or_grep);
    g_regexTester.should_match("***", R"(.*\(^**\))", basic_or_grep);
    g_regexTester.should_throw(R"(.*\(^***\))", error_badrepeat, basic_or_grep);

    g_regexTester.should_match("*aa", R"(.*\(^*a*\))", basic_or_grep);
    g_regexTester.should_not_match("aa", R"(.*\(^*a*\))", basic_or_grep);
    g_regexTester.should_not_match("*a*", R"(.*\(^*a*\))", basic_or_grep);
    g_regexTester.should_not_match("^*a", R"(.*\(^*a*\))", basic_or_grep);
    g_regexTester.should_not_match("^*aa", R"(.*\(^*a*\))", basic_or_grep);
    g_regexTester.should_not_match("^*a*", R"(.*\(^*a*\))", basic_or_grep);

    // Validate that there is no special behavior near bars,
    // as they are alternation operators in regex modes other than basic or grep.
    {
        const test_regex middle_bar(&g_regexTester, "^a|a", basic_or_grep);
        middle_bar.should_search_match("a|a", "a|a");
        middle_bar.should_search_fail("^a|a");
        middle_bar.should_search_fail("ba|a");
        middle_bar.should_search_fail("a");
    }
    {
        const test_regex group_middle_bar(&g_regexTester, R"(^\(a|a\))", basic_or_grep);
        group_middle_bar.should_search_match("a|a", "a|a");
        group_middle_bar.should_search_fail("^a|a");
        group_middle_bar.should_search_fail("ba|a");
        group_middle_bar.should_search_fail("a");
    }
    {
        const test_regex middle_bar_with_caret(&g_regexTester, "^a|^b", basic_or_grep);
        middle_bar_with_caret.should_search_match("a|^b", "a|^b");
        middle_bar_with_caret.should_search_fail("a|b");
        middle_bar_with_caret.should_search_fail("^a|^b");
        middle_bar_with_caret.should_search_fail("ca|^b");
        middle_bar_with_caret.should_search_fail("a");
        middle_bar_with_caret.should_search_fail("b");
    }
    {
        const test_regex group_middle_bar_with_caret(&g_regexTester, R"(^\(a|^b\))", basic_or_grep);
        group_middle_bar_with_caret.should_search_match("a|^b", "a|^b");
        group_middle_bar_with_caret.should_search_fail("a|b");
        group_middle_bar_with_caret.should_search_fail("^a|^b");
        group_middle_bar_with_caret.should_search_fail("ca|^b");
        group_middle_bar_with_caret.should_search_fail("a");
        group_middle_bar_with_caret.should_search_fail("b");
    }

    g_regexTester.should_match("ab", "a|*b", basic_or_grep);
    g_regexTester.should_match("a||b", "a|*b", basic_or_grep);
    g_regexTester.should_not_match("a|*b", "a|*b", basic_or_grep);
    g_regexTester.should_throw("a|**b", error_badrepeat, basic_or_grep);

    g_regexTester.should_match("ab", "^a|*b", basic_or_grep);
    g_regexTester.should_match("a||b", "^a|*b", basic_or_grep);
    g_regexTester.should_not_match("a|*b", "^a|*b", basic_or_grep);
    g_regexTester.should_throw("^a|**b", error_badrepeat, basic_or_grep);

    g_regexTester.should_match("a|b", "^a|^*b", basic_or_grep);
    g_regexTester.should_match("a|^^b", "^a|^*b", basic_or_grep);
    g_regexTester.should_not_match("a|*b", "^a|^*b", basic_or_grep);
    g_regexTester.should_not_match("a|^*b", "^a|^*b", basic_or_grep);
    g_regexTester.should_throw("^a|^**b", error_badrepeat, basic_or_grep);
}

void test_gh_5165_basic() {
    // test cases specific for basic regular expressions
    {
        const test_regex middle_nl(&g_regexTester, "^a\na", basic);
        middle_nl.should_search_match("a\na", "a\na");
        middle_nl.should_search_fail("^a\na");
        middle_nl.should_search_fail("ba\na");
        middle_nl.should_search_fail("a");
    }
    {
        const test_regex group_middle_nl(&g_regexTester, "^\\(a\na\\)", basic);
        group_middle_nl.should_search_match("a\na", "a\na");
        group_middle_nl.should_search_fail("^a\na");
        group_middle_nl.should_search_fail("ba\na");
        group_middle_nl.should_search_fail("a");
    }
    {
        const test_regex middle_nl_with_caret(&g_regexTester, "^a\n^b", basic);
        middle_nl_with_caret.should_search_match("a\n^b", "a\n^b");
        middle_nl_with_caret.should_search_fail("a\nb");
        middle_nl_with_caret.should_search_fail("^a\n^b");
        middle_nl_with_caret.should_search_fail("ca\n^b");
        middle_nl_with_caret.should_search_fail("a");
        middle_nl_with_caret.should_search_fail("b");
    }
    {
        const test_regex group_middle_nl_with_caret(&g_regexTester, "^\\(a\n^b\\)", basic);
        group_middle_nl_with_caret.should_search_match("a\n^b", "a\n^b");
        group_middle_nl_with_caret.should_search_fail("a\nb");
        group_middle_nl_with_caret.should_search_fail("^a\n^b");
        group_middle_nl_with_caret.should_search_fail("ca\n^b");
        group_middle_nl_with_caret.should_search_fail("a");
        group_middle_nl_with_caret.should_search_fail("b");
    }

    g_regexTester.should_match("ab", "a\n*b", basic);
    g_regexTester.should_match("a\n\nb", "a\n*b", basic);
    g_regexTester.should_not_match("a\n*b", "a\n*b", basic);
    g_regexTester.should_match("a\n\nb", "^a\n*b", basic);
    g_regexTester.should_throw("^a\n**b", error_badrepeat, basic);

    g_regexTester.should_match("a\nb", "^a\n^*b", basic);
    g_regexTester.should_match("a\n^^b", "^a\n^*b", basic);
    g_regexTester.should_not_match("a\n*b", "^a\n^*b", basic);
    g_regexTester.should_not_match("a\n^*b", "^a\n^*b", basic);
    g_regexTester.should_throw("^a\n^**b", error_badrepeat, basic);
}

void test_gh_5165_grep() {
    // test cases specific for grep mode
    {
        const test_regex middle_nl(&g_regexTester, "^a\na", grep);
        middle_nl.should_search_match("a\na", "a");
        middle_nl.should_search_match("^a\na", "a");
        middle_nl.should_search_match("ba\na", "a");
        middle_nl.should_search_match("a", "a");
        middle_nl.should_search_fail("b");
    }
    {
        // This regular expression is not accepted by POSIX grep, but currently the regex parser does not reject it.
        // If the parser is changed to reject it, adjust this test case.
        const test_regex group_middle_nl(&g_regexTester, "^\\(a\na\\)", grep);
        group_middle_nl.should_search_match("a\na", "a\na");
        group_middle_nl.should_search_fail("^a\na");
        group_middle_nl.should_search_fail("ba\na");
        group_middle_nl.should_search_fail("a");
    }
    {
        const test_regex middle_nl_with_caret(&g_regexTester, "^a\n^b", grep);
        middle_nl_with_caret.should_search_match("a\n^b", "a");
        middle_nl_with_caret.should_search_match("a\nb", "a");
        middle_nl_with_caret.should_search_match("ab", "a");
        middle_nl_with_caret.should_search_match("a", "a");
        middle_nl_with_caret.should_search_match("b", "b");
        middle_nl_with_caret.should_search_match("ba", "b");
        middle_nl_with_caret.should_search_fail("^a");
        middle_nl_with_caret.should_search_fail("ca");
        middle_nl_with_caret.should_search_fail("^b");
        middle_nl_with_caret.should_search_fail("cb");
    }
    {
        // This regular expression is not accepted by POSIX grep, but currently the regex parser does not reject it.
        // If the parser is changed to reject it, adjust this test case.
        const test_regex group_middle_nl_with_caret(&g_regexTester, "^\\(a\n^b\\)", grep);
        group_middle_nl_with_caret.should_search_match("a\n^b", "a\n^b");
        group_middle_nl_with_caret.should_search_fail("a\nb");
        group_middle_nl_with_caret.should_search_fail("^a\n^b");
        group_middle_nl_with_caret.should_search_fail("ca\n^b");
        group_middle_nl_with_caret.should_search_fail("a");
        group_middle_nl_with_caret.should_search_fail("b");
    }

    g_regexTester.should_not_match("ab", "a\n*b", grep);
    g_regexTester.should_not_match("a\n\nb", "a\n*b", grep);
    g_regexTester.should_not_match("a\n*b", "a\n*b", grep);
    g_regexTester.should_match("a", "a\n*b", grep);
    g_regexTester.should_match("*b", "a\n*b", grep);
    g_regexTester.should_match("a", "a\n**b", grep);
    g_regexTester.should_match("***b", "a\n**b", grep);

    g_regexTester.should_not_match("ab", "^a\n*b", grep);
    g_regexTester.should_not_match("a\n\nb", "^a\n*b", grep);
    g_regexTester.should_not_match("a\n*b", "^a\n*b", grep);
    g_regexTester.should_match("a", "^a\n*b", grep);
    g_regexTester.should_match("*b", "^a\n*b", grep);
    g_regexTester.should_match("a", "^a\n**b", grep);
    g_regexTester.should_match("****b", "^a\n**b", grep);

    g_regexTester.should_not_match("a\nb", "^a\n^*b", grep);
    g_regexTester.should_not_match("a\n^^b", "^a\n^*b", grep);
    g_regexTester.should_not_match("a\n*b", "^a\n^*b", grep);
    g_regexTester.should_not_match("a\n^*b", "^a\n^*b", grep);
    g_regexTester.should_not_match("^*b", "^a\n^*b", grep);
    g_regexTester.should_match("a", "^a\n^*b", grep);
    g_regexTester.should_match("*b", "^a\n^*b", grep);
    g_regexTester.should_not_match("**b", "^a\n^*b", grep);
    g_regexTester.should_match("a", "^a\n^**b", grep);
    g_regexTester.should_match("****b", "^a\n^**b", grep);
}

void test_gh_5165() {
    // GH-5165: Revise caret parsing in basic and grep mode
    test_gh_5165_syntax_option(basic);
    test_gh_5165_syntax_option(grep);

    test_gh_5165_basic();
    test_gh_5165_grep();
}

void test_gh_5167() {
    // GH-5167: Limit backreference parsing to single digit for basic regular expressions
    g_regexTester.should_match("abab0", R"(\(ab*\)\10)", basic);
    g_regexTester.should_match("abab0", R"(\(ab*\)\10)", grep);
    g_regexTester.should_match("abbcdccdc5abb8", R"(\(ab*\)\([cd]*\)\25\18)", basic);
    g_regexTester.should_match("abbcdccdc5abb8", R"(\(ab*\)\([cd]*\)\25\18)", grep);
    g_regexTester.should_not_match("abbcdccdc5abb8", R"(\(ab*\)\([cd]*\)\15\28)", basic);
    g_regexTester.should_not_match("abbcdccdc5abb8", R"(\(ab*\)\([cd]*\)\15\28)", grep);
    g_regexTester.should_throw(R"(abc\1d)", error_backref, basic);
    g_regexTester.should_throw(R"(abc\1d)", error_backref, grep);
    g_regexTester.should_throw(R"(abc\10)", error_backref, basic);
    g_regexTester.should_throw(R"(abc\10)", error_backref, grep);
}

void test_gh_5192() {
    // GH-5192: Correct characters not matched by special character dot
    for (const syntax_option_type option : {
             regex_constants::basic,
             regex_constants::extended,
             regex_constants::awk,
             regex_constants::grep,
             regex_constants::egrep,
         }) {
        const test_regex caretDotStar(&g_regexTester, "^.*", option);
        caretDotStar.should_search_match("abc\nd\re\0f"s, "abc\nd\re"s);
        caretDotStar.should_search_match("abcd\re\ngh\0i"s, "abcd\re\ngh"s);

        const test_wregex wCaretDotStar(&g_regexTester, L"^.*", option);
        wCaretDotStar.should_search_match(L"abc\nd\re\0f"s, L"abc\nd\re"s);
        wCaretDotStar.should_search_match(L"abcd\re\ngh\0i"s, L"abcd\re\ngh"s);
        wCaretDotStar.should_search_match(L"abc\u2028d\ne\0f"s, L"abc\u2028d\ne"s); // U+2028 LINE SEPARATOR
        wCaretDotStar.should_search_match(L"abc\u2029d\ne\0f"s, L"abc\u2029d\ne"s); // U+2029 PARAGRAPH SEPARATOR
    }

    for (const syntax_option_type option : {
             regex_constants::ECMAScript,
             syntax_option_type(),
         }) {
        const test_regex caretDotStar(&g_regexTester, "^.*", option);
        caretDotStar.should_search_match("ab\0c\nd\re\0f"s, "ab\0c"s);
        caretDotStar.should_search_match("ab\0cd\re\ngh\0i"s, "ab\0cd"s);

        const test_wregex wCaretDotStar(&g_regexTester, L"^.*", option);
        wCaretDotStar.should_search_match(L"abc\0\nd\re\0f"s, L"abc\0"s);
        wCaretDotStar.should_search_match(L"ab\0cd\re\ngh\0i"s, L"ab\0cd"s);
        wCaretDotStar.should_search_match(L"ab\0c\u2028d\ne\0f"s, L"ab\0c"s); // U+2028 LINE SEPARATOR
        wCaretDotStar.should_search_match(L"a\0bc\u2029d\ne\0f"s, L"a\0bc"s); // U+2029 PARAGRAPH SEPARATOR
    }
}

void test_gh_5214() {
    // GH-5214 makes negated character class escapes not match characters not included in the negated character class
    {
        const test_wregex neg_word_regex(&g_regexTester, LR"([\W])");
        neg_word_regex.should_search_fail(L"\u0100"); // U+0100 LATIN CAPITAL LETTER A WITH MACRON
    }

    {
        const test_wregex neg_space_regex(&g_regexTester, LR"([\S])");
        neg_space_regex.should_search_fail(L"\u2028"); // U+2028 LINE SEPARATOR
    }

    {
        const test_wregex neg_digit_regex(&g_regexTester, LR"([\D])");
        neg_digit_regex.should_search_fail(L"\u0662"); // U+0662 ARABIC-INDIC DIGIT TWO
    }
}

void test_gh_5243() {
    // GH-5243: <regex>: wregex with regular expression [\w\s] fails to match some spaces
    for (wstring pattern : {LR"([\w])", LR"([\w\w])"}) {
        const test_wregex word_regex(&g_regexTester, pattern);
        word_regex.should_search_match(L"a", L"a");
        word_regex.should_search_match(L"2", L"2");
        word_regex.should_search_match(L"_", L"_");
        word_regex.should_search_match(L"\u00e4", L"\u00e4"); // U+00E4 LATIN SMALL LETTER A WITH DIAERESIS
        word_regex.should_search_match(L"\u0662", L"\u0662"); // U+0662 ARABIC-INDIC DIGIT TWO
        word_regex.should_search_fail(L" ");
        word_regex.should_search_fail(L"\u2028"); // U+2028 LINE SEPARATOR
        word_regex.should_search_fail(L".");
        word_regex.should_search_fail(L"-");
        word_regex.should_search_fail(L"\u203d"); // U+203D INTERROBANG
    }
    {
        const test_wregex space_regex(&g_regexTester, LR"([\s])");
        space_regex.should_search_fail(L"a");
        space_regex.should_search_fail(L"2");
        space_regex.should_search_fail(L"_");
        space_regex.should_search_fail(L"\u00e4"); // U+00E4 LATIN SMALL LETTER A WITH DIAERESIS
        space_regex.should_search_fail(L"\u0662"); // U+0662 ARABIC-INDIC DIGIT TWO
        space_regex.should_search_match(L" ", L" ");
        space_regex.should_search_match(L"\u2028", L"\u2028"); // U+2028 LINE SEPARATOR
        space_regex.should_search_fail(L".");
        space_regex.should_search_fail(L"-");
        space_regex.should_search_fail(L"\u203d"); // U+203D INTERROBANG
    }
    for (wstring pattern : {LR"([\w\s])", LR"([\s\w])"}) {
        const test_wregex word_or_space_regex(&g_regexTester, pattern);
        word_or_space_regex.should_search_match(L"a", L"a");
        word_or_space_regex.should_search_match(L"2", L"2");
        word_or_space_regex.should_search_match(L"_", L"_");
        word_or_space_regex.should_search_match(L"\u00e4", L"\u00e4"); // U+00E4 LATIN SMALL LETTER A WITH DIAERESIS
        word_or_space_regex.should_search_match(L"\u0662", L"\u0662"); // U+0662 ARABIC-INDIC DIGIT TWO
        word_or_space_regex.should_search_match(L" ", L" ");
        word_or_space_regex.should_search_match(L"\u2028", L"\u2028"); // U+2028 LINE SEPARATOR
        word_or_space_regex.should_search_fail(L".");
        word_or_space_regex.should_search_fail(L"-");
        word_or_space_regex.should_search_fail(L"\u203d"); // U+203D INTERROBANG
    }
}


void test_gh_5245() {
    // GH-5245: <regex>: Successful negative lookahead assertions
    // sometimes mistakenly assign matches to capture groups
    {
        test_regex neg_assert(&g_regexTester, "^(?!(a)b)..$");
        neg_assert.should_search_fail("ab"); // rejected by the negative assertion
        neg_assert.should_search_match_capture_groups("ac", "ac", match_default, {{-1, -1}}); // test the fix
        neg_assert.should_search_match_capture_groups("cb", "cb", match_default, {{-1, -1}}); // never captures

        // These 3-character and 4-character tests verify that after a lookahead assertion, we reset the position:
        neg_assert.should_search_fail("abb");
        neg_assert.should_search_fail("acc");
        neg_assert.should_search_fail("cbb");
        neg_assert.should_search_fail("abab");
        neg_assert.should_search_fail("abcc");
        neg_assert.should_search_fail("accc");
    }

    {
        test_regex pos_assert(&g_regexTester, "^(?=(a)b)..$");
        pos_assert.should_search_match_capture_groups("ab", "ab", match_default, {{0, 1}}); // capture group retained
        pos_assert.should_search_fail("ac"); // rejected by the positive assertion midway through
        pos_assert.should_search_fail("cb"); // rejected by the positive assertion immediately

        // These 3-character and 4-character tests verify that after a lookahead assertion, we reset the position:
        pos_assert.should_search_fail("abb");
        pos_assert.should_search_fail("acc");
        pos_assert.should_search_fail("cbb");
        pos_assert.should_search_fail("abab");
        pos_assert.should_search_fail("abcc");
        pos_assert.should_search_fail("accc");
    }
}

void test_gh_5253() {
    // GH-5253 cleaned up parsing logic for quantifiers that were applied to single characters
    g_regexTester.should_match("abbb", "ab*");
    g_regexTester.should_not_match("abab", "ab*");
    g_regexTester.should_match("abbb", "(a)b*");
    g_regexTester.should_not_match("abab", "(a)b*");
    g_regexTester.should_match("abbb", "a(b)*");
    g_regexTester.should_not_match("abab", "a(b)*");
    g_regexTester.should_match("abbb", "(a)(b)*");
    g_regexTester.should_not_match("abab", "(a)(b)*");
    g_regexTester.should_not_match("abbb", "(ab)*");
    g_regexTester.should_match("abab", "(ab)*");
    g_regexTester.should_not_match("abbb", "(?:ab)*");
    g_regexTester.should_match("abab", "(?:ab)*");
    g_regexTester.should_match("aaaa", "a*");
    g_regexTester.should_not_match("b", "a*");
    g_regexTester.should_match("", "()*");
    g_regexTester.should_not_match("a", "()*");
}

void test_gh_5362_syntax_option(const syntax_option_type basic_or_grep) {
    {
        const test_regex ending_anchor(&g_regexTester, "meo[wW]$", basic_or_grep);
        ending_anchor.should_search_match("kitten_meow", "meow");
        ending_anchor.should_search_fail("homeowner");
    }
    {
        const test_regex middle_anchor(&g_regexTester, "me$o[wW]", basic_or_grep);
        middle_anchor.should_search_fail("kitten_meow");
        middle_anchor.should_search_fail("homeowner");
        middle_anchor.should_search_match("home$owner", "me$ow");
    }
    {
        const test_regex double_dollars(&g_regexTester, "meo[wW]$$", basic_or_grep);
        double_dollars.should_search_fail("kitten_meow");
        double_dollars.should_search_fail("homeowner");
        double_dollars.should_search_match("kitten_meow$", "meow$");
        double_dollars.should_search_fail("kitten_meow$$");
        double_dollars.should_search_fail("homeow$ner");
        double_dollars.should_search_fail("homeow$$ner");
    }

    g_regexTester.should_not_match("me$ow", R"(\(me$\)o[wW])", basic_or_grep);
    g_regexTester.should_not_match("meow", R"(\(me$\)o[wW])", basic_or_grep);

    {
        const test_regex singlegroup_anchor(&g_regexTester, R"(\(meo[wW]$\))", basic_or_grep);
        singlegroup_anchor.should_search_match("kitten_meow", "meow");
        singlegroup_anchor.should_search_fail("kitten_meow$");
        singlegroup_anchor.should_search_fail("homeowner");
        singlegroup_anchor.should_search_fail("homeow$ner");
    }
    {
        const test_regex suffixedgroup_anchor(&g_regexTester, R"(\(meo[wW]$\).*)", basic_or_grep);
        suffixedgroup_anchor.should_search_match("kitten_meow", "meow");
        suffixedgroup_anchor.should_search_fail("kitten_meow$");
        suffixedgroup_anchor.should_search_fail("homeowner");
        suffixedgroup_anchor.should_search_fail("homeow$ner");
    }
    {
        const test_regex firstgroup_anchor(&g_regexTester, R"(\(meo[wW]$\)\(.*\))", basic_or_grep);
        firstgroup_anchor.should_search_match("kitten_meow", "meow");
        firstgroup_anchor.should_search_fail("kitten_meow$");
        firstgroup_anchor.should_search_fail("homeowner");
        firstgroup_anchor.should_search_fail("homeow$ner");
    }
    {
        const test_regex nested_anchor(&g_regexTester, R"(\(\(meo[wW]$\)$\).*)", basic_or_grep);
        nested_anchor.should_search_match("kitten_meow", "meow");
        nested_anchor.should_search_fail("kitten_meow$");
        nested_anchor.should_search_fail("kitten_meow$$");
        nested_anchor.should_search_fail("homeowner");
        nested_anchor.should_search_fail("homeow$ner");
        nested_anchor.should_search_fail("homeow$$ner");
    }
    {
        const test_regex double_dollars(&g_regexTester, R"(\(meo[wW]$$\).*)", basic_or_grep);
        double_dollars.should_search_fail("kitten_meow");
        double_dollars.should_search_match("kitten_meow$", "meow$");
        double_dollars.should_search_fail("kitten_meow$$");
        double_dollars.should_search_fail("homeowner");
        double_dollars.should_search_fail("homeow$ner");
        double_dollars.should_search_fail("homeow$$ner");
    }

    // Validate that there is no special behavior near bars,
    // as they are alternation operators in regex modes other than basic or grep.
    {
        const test_regex middle_bar(&g_regexTester, "a|a$", basic_or_grep);
        middle_bar.should_search_match("a|a", "a|a");
        middle_bar.should_search_fail("a|a$");
        middle_bar.should_search_fail("a|ab");
        middle_bar.should_search_fail("a");
    }
    {
        const test_regex group_middle_bar(&g_regexTester, R"(\(a|a\)$)", basic_or_grep);
        group_middle_bar.should_search_match("a|a", "a|a");
        group_middle_bar.should_search_fail("a|a$");
        group_middle_bar.should_search_fail("a|ab");
        group_middle_bar.should_search_fail("a");
    }
    {
        const test_regex middle_bar_with_dollar(&g_regexTester, "a$|b$", basic_or_grep);
        middle_bar_with_dollar.should_search_match("a$|b", "a$|b");
        middle_bar_with_dollar.should_search_fail("a|b");
        middle_bar_with_dollar.should_search_fail("a$|b$");
        middle_bar_with_dollar.should_search_fail("a$|bc");
        middle_bar_with_dollar.should_search_fail("a");
        middle_bar_with_dollar.should_search_fail("b");
    }
    {
        const test_regex group_middle_bar_with_dollar(&g_regexTester, R"(\(a$|b\)$)", basic_or_grep);
        group_middle_bar_with_dollar.should_search_match("a$|b", "a$|b");
        group_middle_bar_with_dollar.should_search_fail("a|b");
        group_middle_bar_with_dollar.should_search_fail("a$|b$");
        group_middle_bar_with_dollar.should_search_fail("a$|bc");
        group_middle_bar_with_dollar.should_search_fail("a");
        group_middle_bar_with_dollar.should_search_fail("b");
    }
}

void test_gh_5362_basic() {
    // test cases specific for basic regular expressions
    {
        const test_regex middle_nl(&g_regexTester, "a\na$", basic);
        middle_nl.should_search_match("a\na", "a\na");
        middle_nl.should_search_fail("a\na$");
        middle_nl.should_search_fail("a\nab");
        middle_nl.should_search_fail("a");
    }
    {
        const test_regex group_middle_nl(&g_regexTester, "\\(a\na\\)$", basic);
        group_middle_nl.should_search_match("a\na", "a\na");
        group_middle_nl.should_search_fail("a\na$");
        group_middle_nl.should_search_fail("a\nab");
        group_middle_nl.should_search_fail("a");
    }
    {
        const test_regex middle_nl_with_dollar(&g_regexTester, "a$\nb$", basic);
        middle_nl_with_dollar.should_search_match("a$\nb", "a$\nb");
        middle_nl_with_dollar.should_search_fail("a\nb");
        middle_nl_with_dollar.should_search_fail("a$\nb$");
        middle_nl_with_dollar.should_search_fail("a$\nbc");
        middle_nl_with_dollar.should_search_fail("a");
        middle_nl_with_dollar.should_search_fail("b");
    }
    {
        const test_regex group_middle_nl_with_dollar(&g_regexTester, "\\(a$\nb\\)$", basic);
        group_middle_nl_with_dollar.should_search_match("a$\nb", "a$\nb");
        group_middle_nl_with_dollar.should_search_fail("a\nb");
        group_middle_nl_with_dollar.should_search_fail("a$\nb$");
        group_middle_nl_with_dollar.should_search_fail("a$\nbc");
        group_middle_nl_with_dollar.should_search_fail("a");
        group_middle_nl_with_dollar.should_search_fail("b");
    }
}

void test_gh_5362_grep() {
    // test cases specific for grep mode
    {
        const test_regex middle_nl(&g_regexTester, "a\na$", grep);
        middle_nl.should_search_match("a\na$", "a");
        middle_nl.should_search_match("a\nab", "a");
        middle_nl.should_search_match("a", "a");
        middle_nl.should_search_fail("b");
    }
    {
        // This regular expression is not accepted by POSIX grep, but currently the regex parser does not reject it.
        // If the parser is changed to reject it, adjust this test case.
        const test_regex group_middle_nl(&g_regexTester, "\\(a\na\\)$", grep);
        group_middle_nl.should_search_match("a\na", "a\na");
        group_middle_nl.should_search_fail("a\na$");
        group_middle_nl.should_search_fail("a\nac");
        group_middle_nl.should_search_fail("a");
    }
    {
        const test_regex middle_nl_with_dollar(&g_regexTester, "a$\nb$", grep);
        middle_nl_with_dollar.should_search_match("a$\nb", "b");
        middle_nl_with_dollar.should_search_match("a\nb", "b");
        middle_nl_with_dollar.should_search_match("ba", "a");
        middle_nl_with_dollar.should_search_match("a", "a");
        middle_nl_with_dollar.should_search_match("b", "b");
        middle_nl_with_dollar.should_search_match("ab", "b");
        middle_nl_with_dollar.should_search_fail("a$");
        middle_nl_with_dollar.should_search_fail("ac");
        middle_nl_with_dollar.should_search_fail("b$");
        middle_nl_with_dollar.should_search_fail("bc");
    }
    {
        // This regular expression is not accepted by POSIX grep, but currently the regex parser does not reject it.
        // If the parser is changed to reject it, adjust this test case.
        const test_regex group_middle_nl_with_dollar(&g_regexTester, "\\(a$\nb\\)$", grep);
        group_middle_nl_with_dollar.should_search_match("a$\nb", "a$\nb");
        group_middle_nl_with_dollar.should_search_fail("a\nb");
        group_middle_nl_with_dollar.should_search_fail("a$\nb$");
        group_middle_nl_with_dollar.should_search_fail("a$\nbc");
        group_middle_nl_with_dollar.should_search_fail("a");
        group_middle_nl_with_dollar.should_search_fail("b");
    }
}

void test_gh_5362() {
    // GH-5362: `<regex>`: Properly parse dollar anchors in basic and grep mode
    test_gh_5362_syntax_option(basic);
    test_gh_5362_syntax_option(grep);

    test_gh_5362_basic();
    test_gh_5362_grep();
}

void test_gh_5364() {
    // GH-5364 <regex>: Allow initial ] to start character ranges in POSIX regular expressions
    for (syntax_option_type option : {basic, extended, awk, grep, egrep}) {
        g_regexTester.should_match("]", "[]-_]", option);
        g_regexTester.should_match("^", "[]-_]", option);
        g_regexTester.should_match("_", "[]-_]", option);
        g_regexTester.should_not_match("-", "[]-_]", option);

        g_regexTester.should_not_match("]", "[^]-_]", option);
        g_regexTester.should_not_match("^", "[^]-_]", option);
        g_regexTester.should_not_match("_", "[^]-_]", option);
        g_regexTester.should_match("-", "[^]-_]", option);

        g_regexTester.should_match("]", "[]a]", option);
        g_regexTester.should_match("a", "[]a]", option);
        g_regexTester.should_not_match("_", "[]a]", option);
        g_regexTester.should_not_match("a]", "[]a]", option);
        g_regexTester.should_not_match("]a", "[]a]", option);
        g_regexTester.should_not_match("__", "[]a]", option);

        g_regexTester.should_not_match("]", "[^]a]", option);
        g_regexTester.should_not_match("a", "[^]a]", option);
        g_regexTester.should_match("_", "[^]a]", option);
        g_regexTester.should_not_match("a]", "[^]a]", option);
        g_regexTester.should_not_match("]a", "[^]a]", option);
        g_regexTester.should_not_match("__", "[^]a]", option);

        g_regexTester.should_throw("[]", error_brack, option);
        g_regexTester.should_throw("[^]", error_brack, option);
    }

    g_regexTester.should_throw("[]-_]", error_brack, ECMAScript);
    g_regexTester.should_throw("[^]-_]", error_brack, ECMAScript);
    g_regexTester.should_throw("[]a]", error_brack, ECMAScript);
    g_regexTester.should_throw("[^]a]", error_brack, ECMAScript);

    g_regexTester.should_not_match("c", "[]", ECMAScript);
    g_regexTester.should_match("c", "[^]", ECMAScript);
}

void test_gh_5365() {
    // GH-5365: <regex>: Implementation divergence for capture group behavior:
    // Capture groups were not correctly cleared at the beginning of repetitions in ECMAScript mode.
    for (string pattern : {"^(?:(a)|(b)|(c)|(d))+$", "^(?:(a)|(b)|(c)|(d))+?$", "^(?:(a)|(b)|(c)|(d)){4,}$"}) {
        test_regex captures_in_repeated_noncapturing_group(&g_regexTester, pattern);
        captures_in_repeated_noncapturing_group.should_search_match_capture_groups(
            "acbd", "acbd", match_default, {{-1, -1}, {-1, -1}, {-1, -1}, {3, 4}});
        captures_in_repeated_noncapturing_group.should_search_match_capture_groups(
            "adcba", "adcba", match_default, {{4, 5}, {-1, -1}, {-1, -1}, {-1, -1}});
    }

    {
        test_regex captures_in_repeated_noncapturing_group(&g_regexTester, "^(?:(a)|(b)|(c)|(d)){5}$");
        captures_in_repeated_noncapturing_group.should_search_fail("acbd");
        captures_in_repeated_noncapturing_group.should_search_match_capture_groups(
            "adcba", "adcba", match_default, {{4, 5}, {-1, -1}, {-1, -1}, {-1, -1}});
    }

    {
        test_regex captures_in_questionmark_quantifiers(&g_regexTester, "(z)((a+)?(b+)?(c))*");
        captures_in_questionmark_quantifiers.should_search_match_capture_groups(
            "zaacbbbcac", "zaacbbbcac", match_default, {{0, 1}, {8, 10}, {8, 9}, {-1, -1}, {9, 10}});
        captures_in_questionmark_quantifiers.should_search_match_capture_groups(
            "zaacbbbcbbc", "zaacbbbcbbc", match_default, {{0, 1}, {8, 11}, {-1, -1}, {8, 10}, {10, 11}});
        captures_in_questionmark_quantifiers.should_search_match_capture_groups(
            "zaacbbbcabbc", "zaacbbbcabbc", match_default, {{0, 1}, {8, 12}, {8, 9}, {9, 11}, {11, 12}});
    }
}

void test_gh_5371() {
    // GH-5371 <regex>: \b and \B are backwards on empty strings
    g_regexTester.should_not_match("", R"(\b)");
    g_regexTester.should_match("", R"(\B)");
}

void test_gh_5374() {
    // GH-5374: <regex>: Back-references to unmatched capture groups
    // should not match in POSIX basic regular expressions
    for (syntax_option_type option : {basic, grep}) {
        g_regexTester.should_not_match("", R"(\(.\)*\1)", option);
        g_regexTester.should_match("", R"(\(.*\)\1)", option);
        g_regexTester.should_not_match("bc", R"(\(a\)*b\1c)", option);
        g_regexTester.should_match("bc", R"(\(a*\)b\1c)", option);
    }

    // ECMAScript's behavior is different:
    g_regexTester.should_match("", R"((.)*\1)", ECMAScript);
    g_regexTester.should_match("", R"((.*)\1)", ECMAScript);
    g_regexTester.should_match("bc", R"((a)*b\1c)", ECMAScript);
    g_regexTester.should_match("bc", R"((a*)b\1c)", ECMAScript);
}

void test_gh_5377() {
    // GH-5377 <regex>: Do not reset matched capture groups in POSIX regexes
    for (syntax_option_type option : {extended, awk, egrep}) {
        test_regex abcd_regex(&g_regexTester, R"(^((a)|(b)|(c)|(d))+$)", option);
        abcd_regex.should_search_match_capture_groups(
            "abcd", "abcd", match_default, {{3, 4}, {0, 1}, {1, 2}, {2, 3}, {3, 4}});
        abcd_regex.should_search_match_capture_groups(
            "acbd", "acbd", match_default, {{3, 4}, {0, 1}, {2, 3}, {1, 2}, {3, 4}});
        abcd_regex.should_search_match_capture_groups(
            "dcba", "dcba", match_default, {{3, 4}, {3, 4}, {2, 3}, {1, 2}, {0, 1}});
    }

    for (syntax_option_type option : {basic, grep}) {
        test_regex abcd_regex(&g_regexTester, R"(^\(\(a\)*\(b\)*\(c\)*\(d\)*\)*$)", option);
        abcd_regex.should_search_match_capture_groups(
            "abcd", "abcd", match_default, {{0, 4}, {0, 1}, {1, 2}, {2, 3}, {3, 4}});
        abcd_regex.should_search_match_capture_groups(
            "acbd", "acbd", match_default, {{2, 4}, {0, 1}, {2, 3}, {1, 2}, {3, 4}});
        abcd_regex.should_search_match_capture_groups(
            "dcba", "dcba", match_default, {{3, 4}, {3, 4}, {2, 3}, {1, 2}, {0, 1}});

        test_regex backref_regex(&g_regexTester, R"(^\(\(a\)\{0,1\}\(\2b\)\{0,1\}\)*)", option);
        backref_regex.should_search_match_capture_groups("aaababb", "aaabab", match_default, {{4, 6}, {1, 2}, {4, 6}});
    }

    {
        // ECMAScript's behavior is different:
        test_regex abcd_regex(&g_regexTester, R"(^((a)|(b)|(c)|(d))+$)", ECMAScript);
        abcd_regex.should_search_match_capture_groups(
            "abcd", "abcd", match_default, {{3, 4}, {-1, -1}, {-1, -1}, {-1, -1}, {3, 4}});
        abcd_regex.should_search_match_capture_groups(
            "acbd", "acbd", match_default, {{3, 4}, {-1, -1}, {-1, -1}, {-1, -1}, {3, 4}});
        abcd_regex.should_search_match_capture_groups(
            "dcba", "dcba", match_default, {{3, 4}, {3, 4}, {-1, -1}, {-1, -1}, {-1, -1}});

        test_regex backref_regex(&g_regexTester, R"(^((a){0,1}(\2b){0,1})*)", ECMAScript);
        backref_regex.should_search_match_capture_groups(
            "aaababb", "aaababb", match_default, {{6, 7}, {-1, -1}, {6, 7}});
    }
}

void test_gh_5490() {
    // GH-5490: Optional empty repetitions are illegal

    // ECMA-262 15.10.2.5 "Term":
    // "If min is zero and y's endIndex is equal to x's endIndex, then return failure."
    // So if no additional repetition is required due to minimum requirements, the match should be rejected.

    // Similarly, POSIX 9.3.6 and 9.4.6 state that a null expression can only be matched if this is the only match or it
    // is necessary to satisfy the minimum number of repetitions.
    // Note the subtle difference that the empty match is allowed if it is the only match.
    for (string pattern : {"()*", "()?", "()*?", "()??", "(){0,}", "(){0,}?", "(){0,1}", "(){0,1}?"}) {
        test_regex quantified_empty_regex_ecma(&g_regexTester, pattern, ECMAScript);
        quantified_empty_regex_ecma.should_search_match_capture_groups("", "", match_default, {{-1, -1}});
        quantified_empty_regex_ecma.should_search_match_capture_groups("b", "", match_default, {{-1, -1}});

        // empty groups are not allowed in EREs
    }

    for (string pattern : {"()+", "()+?", "(){2,}?", "(){1}"}) {
        test_regex quantified_empty_regex_ecma(&g_regexTester, pattern, ECMAScript);
        quantified_empty_regex_ecma.should_search_match_capture_groups("", "", match_default, {{0, 0}});
        quantified_empty_regex_ecma.should_search_match_capture_groups("b", "", match_default, {{0, 0}});

        // empty groups are not allowed in EREs
    }

    for (auto option : {basic, grep}) {
        test_regex quantified_empty_regex_bre(&g_regexTester, R"(\(\)*)", option);
        quantified_empty_regex_bre.should_search_match_capture_groups("", "", match_default, {{0, 0}});
        quantified_empty_regex_bre.should_search_match_capture_groups("b", "", match_default, {{0, 0}});
    }

    for (auto option : {ECMAScript, extended, egrep, awk}) {
        test_regex simple_sequence_regex_ecma_or_ere(&g_regexTester, "(ab)*", option);
        simple_sequence_regex_ecma_or_ere.should_search_match_capture_groups("", "", match_default, {{-1, -1}});
        simple_sequence_regex_ecma_or_ere.should_search_match_capture_groups("b", "", match_default, {{-1, -1}});
        simple_sequence_regex_ecma_or_ere.should_search_match_capture_groups("ababcc", "abab", match_default, {{2, 4}});
    }

    for (auto option : {basic, grep}) {
        test_regex simple_sequence_regex_bre(&g_regexTester, R"(\(ab\)*)", option);
        simple_sequence_regex_bre.should_search_match_capture_groups("", "", match_default, {{-1, -1}});
        simple_sequence_regex_bre.should_search_match_capture_groups("b", "", match_default, {{-1, -1}});
        simple_sequence_regex_bre.should_search_match_capture_groups("ababcc", "abab", match_default, {{2, 4}});
    }

    for (string pattern : {"(ab(?=ab))*", "(ab(?!cc))*"}) {
        test_regex nested_assertion_regex_ecma(&g_regexTester, pattern, ECMAScript);
        nested_assertion_regex_ecma.should_search_match_capture_groups("", "", match_default, {{-1, -1}});
        nested_assertion_regex_ecma.should_search_match_capture_groups("b", "", match_default, {{-1, -1}});
        nested_assertion_regex_ecma.should_search_match_capture_groups("ababcc", "ab", match_default, {{0, 2}});
        nested_assertion_regex_ecma.should_search_match_capture_groups("abababcc", "abab", match_default, {{2, 4}});
    }

    for (string pattern : {"(a*)*", "(a?)*", "(a?)?"}) {
        test_regex nested_quantifier_regex_ecma(&g_regexTester, pattern, ECMAScript);
        nested_quantifier_regex_ecma.should_search_match_capture_groups("", "", match_default, {{-1, -1}});
        nested_quantifier_regex_ecma.should_search_match_capture_groups("b", "", match_default, {{-1, -1}});
        nested_quantifier_regex_ecma.should_search_match_capture_groups("a", "a", match_default, {{0, 1}});

        for (auto option : {extended, egrep, awk}) {
            test_regex nested_quantifier_regex_ere(&g_regexTester, pattern, option);
            nested_quantifier_regex_ere.should_search_match_capture_groups("", "", match_default, {{0, 0}});
            nested_quantifier_regex_ere.should_search_match_capture_groups("b", "", match_default, {{0, 0}});
            nested_quantifier_regex_ere.should_search_match_capture_groups("a", "a", match_default, {{0, 1}});
        }
    }

    for (string pattern : {R"(\(a*\)*)", R"(\(a\{0,1\}\)*)", R"(\(a\{0,1\}\)\{0,1\})"}) {
        for (auto option : {basic, grep}) {
            test_regex nested_quantifier_regex_bre(&g_regexTester, pattern, option);
            nested_quantifier_regex_bre.should_search_match_capture_groups("", "", match_default, {{0, 0}});
            nested_quantifier_regex_bre.should_search_match_capture_groups("b", "", match_default, {{0, 0}});
            nested_quantifier_regex_bre.should_search_match_capture_groups("a", "a", match_default, {{0, 1}});
        }
    }

    for (string pattern : {"(a*)+", "(a?)+"}) {
        for (auto option : {ECMAScript, extended, egrep, awk}) {
            test_regex plus_quantifier_regex_ecma_or_ere(&g_regexTester, pattern, option);
            plus_quantifier_regex_ecma_or_ere.should_search_match_capture_groups("", "", match_default, {{0, 0}});
            plus_quantifier_regex_ecma_or_ere.should_search_match_capture_groups("b", "", match_default, {{0, 0}});
            plus_quantifier_regex_ecma_or_ere.should_search_match_capture_groups("a", "a", match_default, {{0, 1}});
        }
    }

    for (string pattern : {R"(\(a*\)\{1,\})", R"(\(a\{0,1\}\)\{1,\})"}) {
        for (auto option : {basic, grep}) {
            test_regex plus_quantifier_regex_bre(&g_regexTester, pattern, option);
            plus_quantifier_regex_bre.should_search_match_capture_groups("", "", match_default, {{0, 0}});
            plus_quantifier_regex_bre.should_search_match_capture_groups("b", "", match_default, {{0, 0}});
            plus_quantifier_regex_bre.should_search_match_capture_groups("a", "a", match_default, {{0, 1}});
        }
    }

    for (string pattern : {"(a*){1}", "(a?){1}"}) {
        for (auto option : {ECMAScript, extended, egrep, awk}) {
            test_regex repeat_once_regex_ecma_or_ere(&g_regexTester, pattern, option);
            repeat_once_regex_ecma_or_ere.should_search_match_capture_groups("", "", match_default, {{0, 0}});
            repeat_once_regex_ecma_or_ere.should_search_match_capture_groups("b", "", match_default, {{0, 0}});
            repeat_once_regex_ecma_or_ere.should_search_match_capture_groups("a", "a", match_default, {{0, 1}});
        }
    }

    for (string pattern : {R"(\(a*\)\{1\})", R"(\(a\{0,1\}\)\{1\})"}) {
        for (auto option : {basic, grep}) {
            test_regex repeat_once_regex_bre(&g_regexTester, pattern, option);
            repeat_once_regex_bre.should_search_match_capture_groups("", "", match_default, {{0, 0}});
            repeat_once_regex_bre.should_search_match_capture_groups("b", "", match_default, {{0, 0}});
            // leftmost-longest rule according to Boost semantics
            repeat_once_regex_bre.should_search_match_capture_groups("a", "a", match_default, {{0, 1}});
        }
    }

    for (string pattern : {"(a*){2}", "(a?){2}"}) {
        test_regex repeat_twice_regex_ecma(&g_regexTester, pattern, ECMAScript);
        repeat_twice_regex_ecma.should_search_match_capture_groups("", "", match_default, {{0, 0}});
        repeat_twice_regex_ecma.should_search_match_capture_groups("b", "", match_default, {{0, 0}});
        repeat_twice_regex_ecma.should_search_match_capture_groups("a", "a", match_default, {{1, 1}});

        for (auto option : {extended, egrep, awk}) {
            test_regex repeat_twice_regex_ere(&g_regexTester, pattern, option);
            repeat_twice_regex_ere.should_search_match_capture_groups("", "", match_default, {{0, 0}});
            repeat_twice_regex_ere.should_search_match_capture_groups("b", "", match_default, {{0, 0}});
            // leftmost-longest rule according to Boost semantics
            repeat_twice_regex_ere.should_search_match_capture_groups("a", "a", match_default, {{0, 1}});
        }
    }

    for (string pattern : {R"(\(a*\)\{2\})", R"(\(a\{0,1\}\)\{2\})"}) {
        for (auto option : {basic, grep}) {
            test_regex repeat_twice_regex_bre(&g_regexTester, pattern, option);
            repeat_twice_regex_bre.should_search_match_capture_groups("", "", match_default, {{0, 0}});
            repeat_twice_regex_bre.should_search_match_capture_groups("b", "", match_default, {{0, 0}});
            // leftmost-longest rule according to Boost semantics
            repeat_twice_regex_bre.should_search_match_capture_groups("a", "a", match_default, {{0, 1}});
        }
    }

    for (string pattern : {"(a?a?){2}", "(a?a?)+"}) {
        test_regex repeated_double_question_regex_ecma(&g_regexTester, pattern, ECMAScript);
        repeated_double_question_regex_ecma.should_search_match_capture_groups("", "", match_default, {{0, 0}});
        repeated_double_question_regex_ecma.should_search_match_capture_groups("bbb", "", match_default, {{0, 0}});
        repeated_double_question_regex_ecma.should_search_match_capture_groups("aaa", "aaa", match_default, {{2, 3}});

        for (auto option : {extended, egrep, awk}) {
            test_regex repeated_double_question_regex_ere(&g_regexTester, pattern, option);
            repeated_double_question_regex_ere.should_search_match_capture_groups("", "", match_default, {{0, 0}});
            repeated_double_question_regex_ere.should_search_match_capture_groups("bbb", "", match_default, {{0, 0}});
            // leftmost-longest rule according to Boost semantics
            repeated_double_question_regex_ere.should_search_match_capture_groups(
                "aaa", "aaa", match_default, {{1, 3}});
        }
    }

    for (string pattern : {R"(\(a\{0,1\}a\{0,1\}\)\{2\})", R"(\(a\{0,1\}a\{0,1\}\)\{1,\})"}) {
        for (auto option : {basic, grep}) {
            test_regex repeated_double_question_regex_bre(&g_regexTester, pattern, option);
            repeated_double_question_regex_bre.should_search_match_capture_groups("", "", match_default, {{0, 0}});
            repeated_double_question_regex_bre.should_search_match_capture_groups("bbb", "", match_default, {{0, 0}});
            // leftmost-longest rule according to Boost semantics
            repeated_double_question_regex_bre.should_search_match_capture_groups(
                "aaa", "aaa", match_default, {{1, 3}});
        }
    }

    {
        test_regex backref_ecma(&g_regexTester, R"(a(b?)+c\1d)", ECMAScript);
        backref_ecma.should_search_fail("abcd");
        backref_ecma.should_search_match_capture_groups("acd", "acd", match_default, {{1, 1}});
        backref_ecma.should_search_match_capture_groups("abcbd", "abcbd", match_default, {{1, 2}});
    }

    for (auto option : {basic, grep}) {
        test_regex backref_bre(&g_regexTester, R"(a\(b\{0,1\}\)\{1,\}c\1d)", option);
        backref_bre.should_search_fail("abcd");
        backref_bre.should_search_match_capture_groups("acd", "acd", match_default, {{1, 1}});
        backref_bre.should_search_match_capture_groups("abcbd", "abcbd", match_default, {{1, 2}});
    }

    {
        test_regex backref_min_repeat_ecma(&g_regexTester, R"((a?){3,4}b\1c)", ECMAScript);
        backref_min_repeat_ecma.should_search_match_capture_groups("aabc", "aabc", match_default, {{2, 2}});
    }

    for (auto option : {basic, grep}) {
        test_regex backref_min_repeat_bre(&g_regexTester, R"(\(a\{0,1\}\)\{3,4\}b\1c)", option);
        backref_min_repeat_bre.should_search_match_capture_groups("aabc", "aabc", match_default, {{2, 2}});
    }
}

void test_gh_5509() {
    // GH-5509 extended the matcher's skip optimization
    // to regexes starting with a loop with at least one repetition,
    // speeding up searches for such regexes

    {
        test_regex char_plus_regex(&g_regexTester, "(a+)");
        char_plus_regex.should_search_match_capture_groups("blwerofaaweraf", "aa", match_default, {{7, 9}});
        char_plus_regex.should_search_fail("blwerofwerf");
    }

    {
        test_regex charclass_plus_regex(&g_regexTester, "([fa]+)");
        charclass_plus_regex.should_search_match_capture_groups("blwerofaaweraf", "faa", match_default, {{6, 9}});
        charclass_plus_regex.should_search_fail("blwerower");
    }

    {
        test_regex string_plus_regex(&g_regexTester, "((?:aw)+)");
        string_plus_regex.should_search_match_capture_groups("blwerofaawaweraf", "awaw", match_default, {{8, 12}});
        string_plus_regex.should_search_fail("blwerofaerwaf");
    }

    {
        test_regex anchored_string_plus_regex_multi(&g_regexTester, "((?:^aw)+)", multiline);
        anchored_string_plus_regex_multi.should_search_match_capture_groups(
            "blwerofa\nawaweraf", "aw", match_default, {{9, 11}});
        anchored_string_plus_regex_multi.should_search_fail("blwerof\naerwaf");
    }

    {
        test_regex anchored_string_plus_regex(&g_regexTester, "((?:^aw)+)");
        anchored_string_plus_regex.should_search_fail("blwerofa\nawaweraf");
        anchored_string_plus_regex.should_search_fail("blwerof\naerwaf");
    }

    {
        test_regex anchored_string_plus_regex_multi(&g_regexTester, "((?:$\naw)+)", multiline);
        anchored_string_plus_regex_multi.should_search_match_capture_groups(
            "blwerofa\nawaweraf", "\naw", match_default, {{8, 11}});
        anchored_string_plus_regex_multi.should_search_fail("blwerof\naerwaf");
    }

    {
        test_regex anchored_string_plus_regex(&g_regexTester, "((?:$\naw)+)");
        anchored_string_plus_regex.should_search_fail("blwerofa\nawaweraf");
        anchored_string_plus_regex.should_search_fail("blwerof\naerwaf");
    }

    {
        test_regex string_star_string_regex(&g_regexTester, "((?:aw)*fa)");
        string_star_string_regex.should_search_match_capture_groups(
            "blwerofaawawfaeraf", "fa", match_default, {{6, 8}});
        string_star_string_regex.should_search_match_capture_groups(
            "blweroawawfaeraf", "awawfa", match_default, {{6, 12}});
        string_star_string_regex.should_search_match("blwerofaerwaf", "fa");
        string_star_string_regex.should_search_fail("blweroerwaf");
    }
}

void test_gh_5576() {
    // GH-5576 sped up searches for regexes that start with assertions
    // by extending the skip heuristic in the matcher.
    // We test here that the skip heuristic is correct
    // for positive and negative lookahead assertions.
    g_regexTester.should_replace_to("AbGweEfFllLLlffflElF", "(?=[[:lower:]][[:upper:]])[fFlL]{2}", R"(X$&)",
        match_default, "AbGweEXfFlXlLLlffflEXlF");
    g_regexTester.should_replace_to("AbGweEfFllLLlffflElF", "(?![[:upper:]]|[[:lower:]]{2})[fFlL]{2}", R"(X$&)",
        match_default, "AbGweEXfFlXlLLlffflEXlF");
}

void test_gh_5672() {
    // GH-5672: Speed up skip optimization for default `regex_traits` in `collate` mode
    // The PR added a faster branch in the skip optimization when matching in collate mode
    // for default `regex_traits<char>` and `regex_traits<wchar_t>`.
    // The following tests check that searching still works correctly when the faster branch is engaged.
    {
        test_regex collating_re(&g_regexTester, "g", regex_constants::collate);

        collating_re.should_search_match("abcdefghijklmnopqrstuvwxyz", "g");
        collating_re.should_search_fail("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        collating_re.should_search_match("zyxwvutsrqponmlkjihgfedcba", "g");
        collating_re.should_search_fail("ZYXWVUTSRQPONMLKJIHGFEDCBA");
        collating_re.should_search_fail("zyxwvutsrqponmlkjihedcba");
    }

    {
        test_wregex collating_re(&g_regexTester, L"g", regex_constants::collate);

        collating_re.should_search_match(L"abcdefghijklmnopqrstuvwxyz", L"g");
        collating_re.should_search_fail(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        collating_re.should_search_match(L"zyxwvutsrqponmlkjihgfedcba", L"g");
        collating_re.should_search_fail(L"ZYXWVUTSRQPONMLKJIHGFEDCBA");
        collating_re.should_search_fail(L"zyxwvutsrqponmlkjihedcba");
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
    test_construction_from_nullptr_and_zero();
    test_gh_73();
    test_gh_731();
    test_gh_992();
    test_gh_993();
    test_gh_4995();
    test_gh_5058();
    test_gh_5160();
    test_gh_5165();
    test_gh_5167();
    test_gh_5192();
    test_gh_5214();
    test_gh_5243();
    test_gh_5245();
    test_gh_5253();
    test_gh_5362();
    test_gh_5364();
    test_gh_5365();
    test_gh_5371();
    test_gh_5374();
    test_gh_5377();
    test_gh_5490();
    test_gh_5509();
    test_gh_5576();
    test_gh_5672();

    return g_regexTester.result();
}
