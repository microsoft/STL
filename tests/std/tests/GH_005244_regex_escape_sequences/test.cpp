// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstddef>
#include <cstdio>
#include <regex>
#include <string>

#include <test_regex_support.hpp>

using namespace std;
using namespace std::regex_constants;

regex_fixture g_regexTester;

template <class charT>
class test_regex_traits {
private:
    using rx_traits = regex_traits<charT>;

public:
    using char_type       = typename rx_traits::char_type;
    using string_type     = typename rx_traits::string_type;
    using locale_type     = typename rx_traits::locale_type;
    using char_class_type = typename rx_traits::char_class_type;

    test_regex_traits() = default;

    static size_t length(const charT* p) {
        return rx_traits::length(p);
    }

    charT translate(const charT c) const {
        return inner.translate(c);
    }

    charT translate_nocase(const charT c) const {
        return inner.translate_nocase(c);
    }

    template <class FwdIt>
    string_type transform(FwdIt first, FwdIt last) const {
        return inner.transform(first, last);
    }

    template <class FwdIt>
    string_type transform_primary(FwdIt first, FwdIt last) const {
        return inner.transform_primary(first, last);
    }

    template <class FwdIt>
    string_type lookup_collatename(FwdIt first, FwdIt last) const {
        return inner.lookup_collatename(first, last);
    }

    template <class FwdIt>
    char_class_type lookup_classname(FwdIt first, FwdIt last, bool icase = false) const {
        FwdIt next = first;
        ++next;
        if (next == last && (*first == 'z' || *first == 'Z')) {
            const charT space_class = 's';
            return inner.lookup_classname(&space_class, &space_class + 1, icase);
        }
        return inner.lookup_classname(first, last, icase);
    }

    bool isctype(charT c, char_class_type f) const {
        return inner.isctype(c, f);
    }

    int value(charT ch, int radix) const {
        return inner.value(ch, radix);
    }

    locale_type imbue(locale_type l) {
        return inner.imbue(l);
    }

    locale_type getloc() const {
        return inner.getloc();
    }

private:
    regex_traits<charT> inner;
};

template <class Rx>
void check_match(const string& subject, const string& pattern, const Rx& re, match_flag_type flags, bool matches) {
    if (regex_match(subject, re, flags) != matches) {
        printf(R"(Expected regex_match("%s", regex("%s", 0x%X)) to be %s.)", subject.c_str(), pattern.c_str(),
            re.flags(), matches ? "true" : "false");
        g_regexTester.fail_regex();
    }
}

template <class Rx>
void check_match(const string& subject, const string& pattern, const Rx& re, match_flag_type flags = match_default) {
    check_match(subject, pattern, re, flags, true);
}

template <class Rx>
void check_no_match(const string& subject, const string& pattern, const Rx& re, match_flag_type flags = match_default) {
    check_match(subject, pattern, re, flags, false);
}

void check_atomescape_controlescape(const string& expected, const string& c, syntax_option_type option) {
    string pattern = "\\" + c;
    g_regexTester.should_match(expected, pattern, option);
    g_regexTester.should_not_match("g", pattern, option);
    g_regexTester.should_not_match(c, pattern, option);
    g_regexTester.should_not_match("\\", pattern, option);
    g_regexTester.should_not_match(pattern, pattern, option);
}

void check_classescape_controlescape(const string& expected, const string& c, syntax_option_type option) {
    string pattern = "[\\" + c + "]";
    g_regexTester.should_match(expected, pattern, option);
    g_regexTester.should_not_match("g", pattern, option);
    g_regexTester.should_not_match(c, pattern, option);
    g_regexTester.should_not_match("\\", pattern, option);
}

void check_atomescape_identityescape(const string& c, syntax_option_type option) {
    string pattern = "\\" + c;
    g_regexTester.should_match(c, pattern, option);
    g_regexTester.should_not_match("g", pattern, option);
    g_regexTester.should_not_match("\\", pattern, option);
    g_regexTester.should_not_match(pattern, pattern, option);
}

void check_classescape_identityescape(const string& c, syntax_option_type option) {
    string pattern = "[\\" + c + "]";
    g_regexTester.should_match(c, pattern, option);
    g_regexTester.should_not_match("g", pattern, option);
    g_regexTester.should_not_match("\\", pattern, option);
}

void check_classescape_noescape(const string& c, syntax_option_type option) {
    string pattern = "[\\" + c + "]";
    g_regexTester.should_match(c, pattern, option);
    g_regexTester.should_match("\\", pattern, option);
    g_regexTester.should_not_match("g", pattern, option);
    g_regexTester.should_not_match("\\" + c, pattern, option);
}

void test_gh_5244_atomescape_ecmascript() {

    // AtomEscape :: DecimalEscape
    // ECMAScript standard says:
    // \ followed by decimal whose first digit is not zero is a backreference.
    // It is an error if a backreference does not refer to a capture group.
    // \0 refers to literal NUL and must not be followed by another digit.

    // literal NUL tests
    g_regexTester.should_match("\0"s, R"(\0)", ECMAScript);
    g_regexTester.should_not_match("0", R"(\0)", ECMAScript);
    g_regexTester.should_not_match("", R"(\0)", ECMAScript);
    g_regexTester.should_not_match("\\", R"(\0)", ECMAScript);
    g_regexTester.should_throw(R"(\00)", error_escape, ECMAScript);
    g_regexTester.should_throw(R"(\01)", error_escape, ECMAScript);
    g_regexTester.should_throw(R"((a)\01)", error_escape, ECMAScript);

    // backreference tests
    g_regexTester.should_throw(R"(\1)", error_backref, ECMAScript);
    g_regexTester.should_throw(R"((a)\10)", error_backref, ECMAScript);
    g_regexTester.should_match("aa", R"((a)\1)", ECMAScript);
    g_regexTester.should_not_match("aa\\1", R"((a)\1)", ECMAScript);
    g_regexTester.should_not_match("aa\\", R"((a)\1)", ECMAScript);
    g_regexTester.should_not_match("aa1", R"((a)\1)", ECMAScript);
    g_regexTester.should_match("aaaaaaaaabb", R"((.)(.)(.)(.)(.)(.)(.)(.)(.)(b)\10)");
    g_regexTester.should_not_match("aaaaaaaaaba", R"((.)(.)(.)(.)(.)(.)(.)(.)(.)(.)\10)");
    g_regexTester.should_not_match("aaaaaaaaaba0", R"((.)(.)(.)(.)(.)(.)(.)(.)(.)(.)\10)");

    // AtomEscape :: CharacterEscape :: ControlEscape
    check_atomescape_controlescape("\f", "f", ECMAScript);
    check_atomescape_controlescape("\n", "n", ECMAScript);
    check_atomescape_controlescape("\r", "r", ECMAScript);
    check_atomescape_controlescape("\t", "t", ECMAScript);
    check_atomescape_controlescape("\v", "v", ECMAScript);

    // AtomEscape :: CharacterEscape :: 'c' ControlLetter
    g_regexTester.should_match("\x1", R"(\cA)", ECMAScript);
    g_regexTester.should_match("\x1a", R"(\cZ)", ECMAScript);
    g_regexTester.should_match("\x1", R"(\ca)", ECMAScript);
    g_regexTester.should_match("\x1a", R"(\cz)", ECMAScript);
    g_regexTester.should_not_match("\\", R"(\ca)", ECMAScript);
    g_regexTester.should_not_match("c", R"(\ca)", ECMAScript);
    g_regexTester.should_not_match("ca", R"(\ca)", ECMAScript);
    g_regexTester.should_throw(R"(\c0)", error_escape, ECMAScript);
    g_regexTester.should_throw(R"(\c)", error_escape, ECMAScript);
    g_regexTester.should_throw(R"(\c@)", error_escape, ECMAScript);
    g_regexTester.should_throw(R"(\c[)", error_escape, ECMAScript);
    g_regexTester.should_throw(R"(\c`)", error_escape, ECMAScript);
    g_regexTester.should_throw(R"(\c{)", error_escape, ECMAScript);
    g_regexTester.should_throw(L"\\c\u00C0", error_escape, ECMAScript); // U+00C0 LATIN CAPITAL LETTER A WITH GRAVE

    // AtomEscape :: CharacterEscape :: HexEscapeSequence
    g_regexTester.should_match("\x00"s, R"(\x00)", ECMAScript);
    g_regexTester.should_not_match("\\x00", R"(\x00)", ECMAScript);
    g_regexTester.should_match("\x00"s + "0", R"(\x000)", ECMAScript);
    g_regexTester.should_match("A", R"(\x41)", ECMAScript);
    g_regexTester.should_not_match("\\", R"(\x41)", ECMAScript);
    g_regexTester.should_match("\xff", R"(\xff)", ECMAScript);
    g_regexTester.should_throw(R"(\x)", error_escape, ECMAScript);
    g_regexTester.should_throw(R"(\x0)", error_escape, ECMAScript);

    // AtomEscape :: CharacterEscape :: UnicodeEscapeSequence
    g_regexTester.should_match("\u0000"s, R"(\u0000)", ECMAScript);
    g_regexTester.should_not_match("\\u0000", R"(\u0000)", ECMAScript);
    g_regexTester.should_match("\u0000"s + "0", R"(\u00000)", ECMAScript);
    g_regexTester.should_match("A", R"(\u0041)", ECMAScript);
    g_regexTester.should_not_match("\\", R"(\u0041)", ECMAScript);
    g_regexTester.should_throw(R"(\uffff)", error_escape, ECMAScript);
    g_regexTester.should_throw(R"(\u2600)", error_escape, ECMAScript); // U+2600 BLACK SUN WITH RAYS
    g_regexTester.should_throw(R"(\u)", error_escape, ECMAScript);
    g_regexTester.should_throw(R"(\u0)", error_escape, ECMAScript);
    g_regexTester.should_throw(R"(\u00)", error_escape, ECMAScript);
    g_regexTester.should_throw(R"(\u000)", error_escape, ECMAScript);

    // AtomEscape :: CharacterClassEscape
    g_regexTester.should_match("0", R"(\d)", ECMAScript);
    g_regexTester.should_match("5", R"(\d)", ECMAScript);
    g_regexTester.should_not_match("a", R"(\d)", ECMAScript);
    g_regexTester.should_not_match("_", R"(\d)", ECMAScript);
    g_regexTester.should_not_match("-", R"(\d)", ECMAScript);
    g_regexTester.should_not_match("\1", R"(\d)", ECMAScript);
    g_regexTester.should_not_match("\\", R"(\d)", ECMAScript);
    g_regexTester.should_not_match(" ", R"(\d)", ECMAScript);
    g_regexTester.should_not_match("0", R"(\D)", ECMAScript);
    g_regexTester.should_not_match("5", R"(\D)", ECMAScript);
    g_regexTester.should_match("a", R"(\D)", ECMAScript);
    g_regexTester.should_match("_", R"(\D)", ECMAScript);
    g_regexTester.should_match("-", R"(\D)", ECMAScript);
    g_regexTester.should_match("\1", R"(\D)", ECMAScript);
    g_regexTester.should_match("\\", R"(\D)", ECMAScript);
    g_regexTester.should_match(" ", R"(\D)", ECMAScript);

    g_regexTester.should_match("a", R"(\w)", ECMAScript);
    g_regexTester.should_match("A", R"(\w)", ECMAScript);
    g_regexTester.should_match("0", R"(\w)", ECMAScript);
    g_regexTester.should_match("_", R"(\w)", ECMAScript);
    g_regexTester.should_not_match("-", R"(\w)", ECMAScript);
    g_regexTester.should_not_match("\1", R"(\w)", ECMAScript);
    g_regexTester.should_not_match("\\", R"(\w)", ECMAScript);
    g_regexTester.should_not_match(" ", R"(\w)", ECMAScript);
    g_regexTester.should_not_match("a", R"(\W)", ECMAScript);
    g_regexTester.should_not_match("A", R"(\W)", ECMAScript);
    g_regexTester.should_not_match("0", R"(\W)", ECMAScript);
    g_regexTester.should_not_match("_", R"(\W)", ECMAScript);
    g_regexTester.should_match("-", R"(\W)", ECMAScript);
    g_regexTester.should_match("\1", R"(\W)", ECMAScript);
    g_regexTester.should_match("\\", R"(\W)", ECMAScript);
    g_regexTester.should_match(" ", R"(\W)", ECMAScript);

    g_regexTester.should_match(" ", R"(\s)", ECMAScript);
    g_regexTester.should_match("\t", R"(\s)", ECMAScript);
    g_regexTester.should_match("\n", R"(\s)", ECMAScript);
    g_regexTester.should_not_match("a", R"(\s)", ECMAScript);
    g_regexTester.should_not_match("0", R"(\s)", ECMAScript);
    g_regexTester.should_not_match("_", R"(\s)", ECMAScript);
    g_regexTester.should_not_match("-", R"(\s)", ECMAScript);
    g_regexTester.should_not_match("\1", R"(\s)", ECMAScript);
    g_regexTester.should_not_match("\\", R"(\s)", ECMAScript);
    g_regexTester.should_not_match(" ", R"(\S)", ECMAScript);
    g_regexTester.should_not_match("\t", R"(\S)", ECMAScript);
    g_regexTester.should_not_match("\n", R"(\S)", ECMAScript);
    g_regexTester.should_match("a", R"(\S)", ECMAScript);
    g_regexTester.should_match("0", R"(\S)", ECMAScript);
    g_regexTester.should_match("_", R"(\S)", ECMAScript);
    g_regexTester.should_match("-", R"(\S)", ECMAScript);
    g_regexTester.should_match("\1", R"(\S)", ECMAScript);
    g_regexTester.should_match("\\", R"(\S)", ECMAScript);

    // AtomEscape :: CharacterEscape :: IdentityEscape
    g_regexTester.should_match("\\", R"(\\)", ECMAScript);
    g_regexTester.should_not_match("g", R"(\\)", ECMAScript);
    g_regexTester.should_not_match("\\\\", R"(\\)", ECMAScript);
    check_atomescape_identityescape("a", ECMAScript);
    check_atomescape_identityescape("-", ECMAScript);
    check_atomescape_identityescape(" ", ECMAScript);
    check_atomescape_identityescape("(", ECMAScript);
    check_atomescape_identityescape(")", ECMAScript);
    check_atomescape_identityescape("[", ECMAScript);
    check_atomescape_identityescape("]", ECMAScript);
    check_atomescape_identityescape("{", ECMAScript);
    check_atomescape_identityescape("}", ECMAScript);
    check_atomescape_identityescape("*", ECMAScript);
    check_atomescape_identityescape("?", ECMAScript);
    check_atomescape_identityescape("+", ECMAScript);
    check_atomescape_identityescape("^", ECMAScript);
    check_atomescape_identityescape("$", ECMAScript);
    check_atomescape_identityescape(".", ECMAScript);

    {
        string pattern = R"(\z)";
        basic_regex<char, test_regex_traits<char>> custom_charclass_regex{pattern, ECMAScript};
        check_match("z", pattern, custom_charclass_regex);
        check_no_match("\\", pattern, custom_charclass_regex);
        check_no_match(" ", pattern, custom_charclass_regex);
    }
    {
        string pattern = R"(\Z)";
        basic_regex<char, test_regex_traits<char>> custom_charclass_regex{pattern, ECMAScript};
        check_match("Z", pattern, custom_charclass_regex);
        check_no_match("\\", pattern, custom_charclass_regex);
        check_no_match("A", pattern, custom_charclass_regex);
        check_no_match(" ", pattern, custom_charclass_regex);
    }
}

void test_gh_5244_classescape_ecmascript() {

    // ClassEscape :: DecimalEscape
    // only accepts literal NUL (\0), all other decimal escapes result in error
    g_regexTester.should_match("\0"s, R"([\0])", ECMAScript);
    g_regexTester.should_not_match("0", R"([\0])", ECMAScript);
    g_regexTester.should_not_match("", R"([\0])", ECMAScript);
    g_regexTester.should_not_match("\\", R"([\0])", ECMAScript);
    g_regexTester.should_throw(R"([\00])", error_escape, ECMAScript);
    g_regexTester.should_throw(R"([\01])", error_escape, ECMAScript);
    g_regexTester.should_throw(R"([\1])", error_escape, ECMAScript);
    g_regexTester.should_throw(R"((a)[\10])", error_escape, ECMAScript);
    g_regexTester.should_throw(R"((a)[\1])", error_escape, ECMAScript);
    g_regexTester.should_throw(R"((a)[\01])", error_escape, ECMAScript);

    // ClassEscape :: 'b'
    check_classescape_controlescape("\b", "b", ECMAScript);

    // ClassEscape :: CharacterEscape :: ControlEscape
    check_classescape_controlescape("\f", "f", ECMAScript);
    check_classescape_controlescape("\n", "n", ECMAScript);
    check_classescape_controlescape("\r", "r", ECMAScript);
    check_classescape_controlescape("\t", "t", ECMAScript);
    check_classescape_controlescape("\v", "v", ECMAScript);

    // ClassEscape :: CharacterEscape :: 'c' ControlLetter
    g_regexTester.should_match("\x1", R"([\cA])", ECMAScript);
    g_regexTester.should_match("\x1a", R"([\cZ])", ECMAScript);
    g_regexTester.should_match("\x1", R"([\ca])", ECMAScript);
    g_regexTester.should_match("\x1a", R"([\cz])", ECMAScript);
    g_regexTester.should_not_match("\\", R"([\ca])", ECMAScript);
    g_regexTester.should_not_match("c", R"([\ca])", ECMAScript);
    g_regexTester.should_not_match("ca", R"([\ca])", ECMAScript);
    g_regexTester.should_throw(R"([\c0])", error_escape, ECMAScript);
    g_regexTester.should_throw(R"([\c])", error_escape, ECMAScript);

    // ClassEscape :: CharacterEscape :: HexEscapeSequence
    g_regexTester.should_match("\x00"s, R"([\x00])", ECMAScript);
    g_regexTester.should_match("\x00"s, R"([\x000])", ECMAScript);
    g_regexTester.should_match("0", R"([\x000])", ECMAScript);
    g_regexTester.should_match("A", R"([\x41])", ECMAScript);
    g_regexTester.should_not_match("\\", R"([\x41])", ECMAScript);
    g_regexTester.should_match("\xff", R"([\xff])", ECMAScript);
    g_regexTester.should_throw(R"([\x])", error_escape, ECMAScript);
    g_regexTester.should_throw(R"([\x0])", error_escape, ECMAScript);

    // ClassEscape :: CharacterEscape :: UnicodeEscapeSequence
    g_regexTester.should_match("\u0000"s, R"([\u0000])", ECMAScript);
    g_regexTester.should_match("\u0000"s, R"([\u00000])", ECMAScript);
    g_regexTester.should_match("0", R"([\u00000])", ECMAScript);
    g_regexTester.should_match("A", R"([\u0041])", ECMAScript);
    g_regexTester.should_not_match("\\", R"([\u0041])", ECMAScript);
    g_regexTester.should_throw(R"([\uffff])", error_escape, ECMAScript);
    g_regexTester.should_throw(R"([\u2600])", error_escape, ECMAScript); // U+2600 BLACK SUN WITH RAYS
    g_regexTester.should_throw(R"([\u])", error_escape, ECMAScript);
    g_regexTester.should_throw(R"([\u0])", error_escape, ECMAScript);
    g_regexTester.should_throw(R"([\u00])", error_escape, ECMAScript);
    g_regexTester.should_throw(R"([\u000])", error_escape, ECMAScript);

    // ClassEscape :: CharacterClassEscape
    g_regexTester.should_match("0", R"([\d])", ECMAScript);
    g_regexTester.should_match("5", R"([\d])", ECMAScript);
    g_regexTester.should_not_match("a", R"([\d])", ECMAScript);
    g_regexTester.should_not_match("_", R"([\d])", ECMAScript);
    g_regexTester.should_not_match("-", R"([\d])", ECMAScript);
    g_regexTester.should_not_match("\1", R"([\d])", ECMAScript);
    g_regexTester.should_not_match("\\", R"([\d])", ECMAScript);
    g_regexTester.should_not_match(" ", R"([\d])", ECMAScript);
    g_regexTester.should_not_match("0", R"([\D])", ECMAScript);
    g_regexTester.should_not_match("5", R"([\D])", ECMAScript);
    g_regexTester.should_match("a", R"([\D])", ECMAScript);
    g_regexTester.should_match("_", R"([\D])", ECMAScript);
    g_regexTester.should_match("-", R"([\D])", ECMAScript);
    g_regexTester.should_match("\1", R"([\D])", ECMAScript);
    g_regexTester.should_match("\\", R"([\D])", ECMAScript);
    g_regexTester.should_match(" ", R"([\D])", ECMAScript);

    g_regexTester.should_match("a", R"([\w])", ECMAScript);
    g_regexTester.should_match("A", R"([\w])", ECMAScript);
    g_regexTester.should_match("0", R"([\w])", ECMAScript);
    g_regexTester.should_match("_", R"([\w])", ECMAScript);
    g_regexTester.should_not_match("-", R"([\w])", ECMAScript);
    g_regexTester.should_not_match("\1", R"([\w])", ECMAScript);
    g_regexTester.should_not_match("\\", R"([\w])", ECMAScript);
    g_regexTester.should_not_match(" ", R"([\w])", ECMAScript);
    g_regexTester.should_not_match("a", R"([\W])", ECMAScript);
    g_regexTester.should_not_match("A", R"([\W])", ECMAScript);
    g_regexTester.should_not_match("0", R"([\W])", ECMAScript);
    g_regexTester.should_not_match("_", R"([\W])", ECMAScript);
    g_regexTester.should_match("-", R"([\W])", ECMAScript);
    g_regexTester.should_match("\1", R"([\W])", ECMAScript);
    g_regexTester.should_match("\\", R"([\W])", ECMAScript);
    g_regexTester.should_match(" ", R"([\W])", ECMAScript);

    g_regexTester.should_match(" ", R"([\s])", ECMAScript);
    g_regexTester.should_match("\t", R"([\s])", ECMAScript);
    g_regexTester.should_match("\n", R"([\s])", ECMAScript);
    g_regexTester.should_not_match("a", R"([\s])", ECMAScript);
    g_regexTester.should_not_match("0", R"([\s])", ECMAScript);
    g_regexTester.should_not_match("_", R"([\s])", ECMAScript);
    g_regexTester.should_not_match("-", R"([\s])", ECMAScript);
    g_regexTester.should_not_match("\1", R"([\s])", ECMAScript);
    g_regexTester.should_not_match("\\", R"([\s])", ECMAScript);
    g_regexTester.should_not_match(" ", R"([\S])", ECMAScript);
    g_regexTester.should_not_match("\t", R"([\S])", ECMAScript);
    g_regexTester.should_not_match("\n", R"([\S])", ECMAScript);
    g_regexTester.should_match("a", R"([\S])", ECMAScript);
    g_regexTester.should_match("0", R"([\S])", ECMAScript);
    g_regexTester.should_match("_", R"([\S])", ECMAScript);
    g_regexTester.should_match("-", R"([\S])", ECMAScript);
    g_regexTester.should_match("\1", R"([\S])", ECMAScript);
    g_regexTester.should_match("\\", R"([\S])", ECMAScript);

    // ClassEscape :: CharacterEscape :: IdentityEscape
    g_regexTester.should_match("\\", R"([\\])", ECMAScript);
    g_regexTester.should_not_match("b", R"([\\])", ECMAScript);
    check_classescape_identityescape("a", ECMAScript);
    check_classescape_identityescape("-", ECMAScript);
    check_classescape_identityescape(" ", ECMAScript);
    check_classescape_identityescape("(", ECMAScript);
    check_classescape_identityescape(")", ECMAScript);
    check_classescape_identityescape("[", ECMAScript);
    check_classescape_identityescape("]", ECMAScript);
    check_classescape_identityescape("{", ECMAScript);
    check_classescape_identityescape("}", ECMAScript);
    check_classescape_identityescape("*", ECMAScript);
    check_classescape_identityescape("?", ECMAScript);
    check_classescape_identityescape("+", ECMAScript);
    check_classescape_identityescape("^", ECMAScript);
    check_classescape_identityescape("$", ECMAScript);
    check_classescape_identityescape(".", ECMAScript);
    check_classescape_identityescape("B", ECMAScript);

    {
        string pattern = R"([\z])";
        basic_regex<char, test_regex_traits<char>> custom_charclass_regex{pattern, ECMAScript};
        check_match("z", pattern, custom_charclass_regex);
        check_no_match("\\", pattern, custom_charclass_regex);
        check_no_match(" ", pattern, custom_charclass_regex);
    }
    {
        string pattern = R"([\Z])";
        basic_regex<char, test_regex_traits<char>> custom_charclass_regex{pattern, ECMAScript};
        check_match("Z", pattern, custom_charclass_regex);
        check_no_match("\\", pattern, custom_charclass_regex);
        check_no_match("A", pattern, custom_charclass_regex);
        check_no_match(" ", pattern, custom_charclass_regex);
    }
}

void test_gh_5244_atomescape_posix_common(syntax_option_type option) {
    // Sections on "BRE Special Characters" and "ERE Special Characters":
    // When special character is preceded by backslash, the special character matches itself
    g_regexTester.should_match("\\", R"(\\)", option);
    g_regexTester.should_not_match("b", R"(\\)", option);
    g_regexTester.should_not_match("\\\\", R"(\\)", option);
    check_atomescape_identityescape(".", option);
    check_atomescape_identityescape("[", option);
    check_atomescape_identityescape("*", option);
    check_atomescape_identityescape("^", option);
    check_atomescape_identityescape("$", option);

    // Even though [ is special, ] is not,
    // so the interpretation of the escape sequence \] is undefined
    // according to the POSIX standard referenced in the C++ standard.
    // But we treat \] as an identity escape in line with
    // more recent versions of the POSIX standard.
    check_atomescape_identityescape("]", option);

    // Sections on "BRE Special Characters" and "ERE Special Characters":
    // escaping ordinary characters is undefined -> reject
    g_regexTester.should_throw(R"(\B)", error_escape, option);
    g_regexTester.should_throw(R"(\c)", error_escape, option);
    g_regexTester.should_throw(R"(\ca)", error_escape, option);
    g_regexTester.should_throw(R"(\x000)", error_escape, option);
    g_regexTester.should_throw(R"(\u0000)", error_escape, option);
    g_regexTester.should_throw(R"(\d)", error_escape, option);
    g_regexTester.should_throw(R"(\D)", error_escape, option);
    g_regexTester.should_throw(R"(\w)", error_escape, option);
    g_regexTester.should_throw(R"(\W)", error_escape, option);
    g_regexTester.should_throw(R"(\s)", error_escape, option);
    g_regexTester.should_throw(R"(\S)", error_escape, option);
}

void test_gh_5244_atomescape_posix_not_awk(syntax_option_type option) {
    test_gh_5244_atomescape_posix_common(option);

    // reject awk-only escapes
    g_regexTester.should_throw(R"(\a)", error_escape, option);
    g_regexTester.should_throw(R"(\b)", error_escape, option);
    g_regexTester.should_throw(R"(\f)", error_escape, option);
    g_regexTester.should_throw(R"(\n)", error_escape, option);
    g_regexTester.should_throw(R"(\r)", error_escape, option);
    g_regexTester.should_throw(R"(\t)", error_escape, option);
    g_regexTester.should_throw(R"(\v)", error_escape, option);
    g_regexTester.should_throw(R"(\")", error_escape, option);
    g_regexTester.should_throw(R"(\/)", error_escape, option);
}

void test_gh_5244_atomescape_basic_or_grep(syntax_option_type option) {
    test_gh_5244_atomescape_posix_not_awk(option);

    // Section on "BREs Matching Multiple Characters":
    // \ plus digit is backreference to previously completed subexpression
    g_regexTester.should_throw(R"(\0)", error_escape, option);
    g_regexTester.should_throw(R"(\1)", error_backref, option);
    g_regexTester.should_match("aa", R"(\(a\)\1)", option);
    g_regexTester.should_throw(R"(\(a\)\0)", error_escape, option);
    g_regexTester.should_throw(R"(\1\(a\))", error_backref, option);

    // check that the parser rejects escaped characters
    // that are only special in extended regexes or awk
    g_regexTester.should_throw(R"(\+)", error_escape, option);
    g_regexTester.should_throw(R"(\?)", error_escape, option);
    g_regexTester.should_throw(R"(\|)", error_escape, option);
    g_regexTester.should_throw(R"(\")", error_escape, option);
    g_regexTester.should_throw(R"(\/)", error_escape, option);
}

void test_gh_5244_atomescape_extended_egrep_awk(syntax_option_type option) {
    // check that the parser accepts escaped characters
    // that are only special in extended regexes
    check_atomescape_identityescape("+", option);
    check_atomescape_identityescape("?", option);
    check_atomescape_identityescape("|", option);
    check_atomescape_identityescape("(", option);
    check_atomescape_identityescape(")", option);
    check_atomescape_identityescape("{", option);

    // Even though { is special, } is not,
    // so the interpretation of the escape sequence \} is undefined
    // according to the POSIX standard referenced in the C++ standard.
    // But we treat \} as an identity escape in line with
    // more recent versions of the POSIX standard.
    check_atomescape_identityescape("}", option);
}

void test_gh_5244_atomescape_extended_or_egrep(syntax_option_type option) {
    test_gh_5244_atomescape_extended_egrep_awk(option);
    test_gh_5244_atomescape_posix_not_awk(option);

    // there are no backreferences in extended regexes
    g_regexTester.should_throw(R"(\0)", error_escape, option);
    g_regexTester.should_throw(R"(\1)", error_escape, option);
    g_regexTester.should_throw(R"((a)\1)", error_escape, option);
    g_regexTester.should_throw(R"((a)\0)", error_escape, option);
    g_regexTester.should_throw(R"(\1(a))", error_escape, option);
}

void test_gh_5244_atomescape_awk() {
    test_gh_5244_atomescape_extended_egrep_awk(awk);
    test_gh_5244_atomescape_posix_common(awk);

    // awk-only escapes
    check_atomescape_controlescape("\a", "a", awk);
    check_atomescape_controlescape("\b", "b", awk);
    check_atomescape_controlescape("\f", "f", awk);
    check_atomescape_controlescape("\n", "n", awk);
    check_atomescape_controlescape("\r", "r", awk);
    check_atomescape_controlescape("\t", "t", awk);
    check_atomescape_controlescape("\v", "v", awk);
    check_atomescape_identityescape("\"", awk);
    check_atomescape_identityescape("/", awk);

    // awk supports octal sequences
    g_regexTester.should_match("\1", R"(\1)", awk);
    g_regexTester.should_not_match("1", R"(\1)", awk);
    g_regexTester.should_not_match("\\1", R"(\1)", awk);
    g_regexTester.should_match("\11", R"(\11)", awk);
    g_regexTester.should_not_match("1", R"(\11)", awk);
    g_regexTester.should_not_match("11", R"(\11)", awk);
    g_regexTester.should_not_match("\\11", R"(\11)", awk);
    g_regexTester.should_match("\111", R"(\111)", awk);
    g_regexTester.should_not_match("1", R"(\111)", awk);
    g_regexTester.should_not_match("11", R"(\111)", awk);
    g_regexTester.should_not_match("111", R"(\111)", awk);
    g_regexTester.should_not_match("\\111", R"(\111)", awk);
    g_regexTester.should_match("\111"s + "1", R"(\1111)", awk);
    g_regexTester.should_not_match("\111", R"(\1111)", awk);
    g_regexTester.should_not_match("1", R"(\1111)", awk);
    g_regexTester.should_not_match("11", R"(\1111)", awk);
    g_regexTester.should_not_match("111", R"(\1111)", awk);
    g_regexTester.should_not_match("1111", R"(\1111)", awk);
    g_regexTester.should_not_match("\\1111", R"(\1111)", awk);
    g_regexTester.should_match("A", R"(\101)", awk);
    g_regexTester.should_match("Aa", R"(\101a)", awk);
    g_regexTester.should_match("\33", R"(\033)", awk);
    g_regexTester.should_match("\33a", R"(\033a)", awk);
    g_regexTester.should_match("\33", R"(\33)", awk);
    g_regexTester.should_match("\33a", R"(\33a)", awk);
    g_regexTester.should_match("\1", R"(\001)", awk);
    g_regexTester.should_match("\1a", R"(\001a)", awk);
    g_regexTester.should_match("\1", R"(\01)", awk);
    g_regexTester.should_match("\1a", R"(\01a)", awk);
    g_regexTester.should_match("\1", R"(\1)", awk);
    g_regexTester.should_match("\1a", R"(\1a)", awk);
    g_regexTester.should_throw(R"(\8)", error_escape, awk);
    g_regexTester.should_match("\1"s + "8", R"(\18)", awk);
    g_regexTester.should_match("\12"s + "9", R"(\129)", awk);

    // octal sequences evaluating to 0 are considered undefined by the standard
    g_regexTester.should_throw(R"(\0)", error_escape, awk);
    g_regexTester.should_throw(R"(\00)", error_escape, awk);
    g_regexTester.should_throw(R"(\000)", error_escape, awk);
}

void test_gh_5244_classescape_posix_not_awk(syntax_option_type option) {
    // Sections "BRE Bracket Expressions" and "ERE Bracket Expressions":
    // Backslash shall lose its special meaning, so \c should always match \ + c

    // common special characters outside character classes
    check_classescape_noescape(".", option);
    check_classescape_noescape("[", option);
    check_classescape_noescape("*", option);
    check_classescape_noescape("^", option);
    check_classescape_noescape("$", option);
    check_classescape_noescape("B", option);
    check_classescape_noescape("c", option);

    // special characters outside character classes in extended regexes
    check_classescape_noescape("+", option);
    check_classescape_noescape("?", option);
    check_classescape_noescape("|", option);

    check_classescape_noescape("(", option);
    check_classescape_noescape(")", option);
    check_classescape_noescape("{", option);

    // closing characters that are not considered special
    g_regexTester.should_match("\\]", R"([\]])", option);
    g_regexTester.should_not_match("]", R"([\]])", option);
    g_regexTester.should_not_match("\\", R"([\]])", option);
    check_classescape_noescape("}", option);

    // awk escape sequences
    check_classescape_noescape("a", option);
    check_classescape_noescape("b", option);
    check_classescape_noescape("f", option);
    check_classescape_noescape("n", option);
    check_classescape_noescape("r", option);
    check_classescape_noescape("t", option);
    check_classescape_noescape("v", option);
    check_classescape_noescape("\"", option);
    check_classescape_noescape("/", option);

    // awk octal sequences
    check_classescape_noescape("0", option);
    check_classescape_noescape("1", option);
    g_regexTester.should_match("1", R"([\101])", option);
    g_regexTester.should_match("0", R"([\101])", option);
    g_regexTester.should_match("\\", R"([\101])", option);
    g_regexTester.should_match("1", R"([\101a])", option);
    g_regexTester.should_match("0", R"([\101a])", option);
    g_regexTester.should_match("\\", R"([\101a])", option);
    g_regexTester.should_match("a", R"([\101a])", option);
    g_regexTester.should_match("3", R"([\033a])", option);
    g_regexTester.should_match("0", R"([\033a])", option);
    g_regexTester.should_match("\\", R"([\033a])", option);
    g_regexTester.should_match("a", R"([\033a])", option);
    g_regexTester.should_match("3", R"([\33a])", option);
    g_regexTester.should_match("\\", R"([\33a])", option);
    g_regexTester.should_match("a", R"([\33a])", option);
    g_regexTester.should_match("1", R"([\001a])", option);
    g_regexTester.should_match("0", R"([\001a])", option);
    g_regexTester.should_match("\\", R"([\001a])", option);
    g_regexTester.should_match("a", R"([\001a])", option);

    // ECMAScript escape sequences
    g_regexTester.should_match("c", R"([\ca])", option);
    g_regexTester.should_match("a", R"([\ca])", option);
    g_regexTester.should_match("\\", R"([\ca])", option);
    g_regexTester.should_match("x", R"([\x000])", option);
    g_regexTester.should_match("0", R"([\x000])", option);
    g_regexTester.should_match("\\", R"([\x000])", option);
    g_regexTester.should_match("u", R"([\u0000])", option);
    g_regexTester.should_match("0", R"([\u0000])", option);
    g_regexTester.should_match("\\", R"([\u0000])", option);
    check_classescape_noescape("d", option);
    check_classescape_noescape("D", option);
    check_classescape_noescape("w", option);
    check_classescape_noescape("W", option);
    check_classescape_noescape("s", option);
    check_classescape_noescape("S", option);
}

void test_gh_5244_classescape_basic_or_grep(syntax_option_type option) {
    test_gh_5244_classescape_posix_not_awk(option);

    // check no backreference handling
    g_regexTester.should_match("a1", R"(\(a\)[\1])", option);
    g_regexTester.should_match("a\\", R"(\(a\)[\1])", option);
    g_regexTester.should_not_match("aa", R"(\(a\)[\1])", option);
    g_regexTester.should_match("a0", R"(\(a\)[\0])", option);
    g_regexTester.should_match("a\\", R"(\(a\)[\0])", option);
    g_regexTester.should_match("1a", R"([\1]\(a\))", option);
    g_regexTester.should_match("\\a", R"([\1]\(a\))", option);
    g_regexTester.should_not_match("aa", R"([\1]\(a\))", option);
}

void test_gh_5244_classescape_extended_or_egrep(syntax_option_type option) {
    test_gh_5244_classescape_posix_not_awk(option);

    // check no backreference handling
    g_regexTester.should_match("a1", R"((a)[\1])", option);
    g_regexTester.should_match("a\\", R"((a)[\1])", option);
    g_regexTester.should_not_match("aa", R"((a)[\1])", option);
    g_regexTester.should_match("a0", R"((a)[\0])", option);
    g_regexTester.should_match("a\\", R"((a)[\0])", option);
    g_regexTester.should_match("1a", R"([\1](a))", option);
    g_regexTester.should_match("\\a", R"([\1](a))", option);
    g_regexTester.should_not_match("aa", R"([\1](a))", option);
}

void test_gh_5244_classescape_awk() {
    // awk-only sequences and backslash
    g_regexTester.should_match("\\", "[\\\\]", awk);
    g_regexTester.should_not_match("g", "[\\\\]", awk);
    check_classescape_controlescape("\a", "a", awk);
    check_classescape_controlescape("\b", "b", awk);
    check_classescape_controlescape("\f", "f", awk);
    check_classescape_controlescape("\n", "n", awk);
    check_classescape_controlescape("\r", "r", awk);
    check_classescape_controlescape("\t", "t", awk);
    check_classescape_controlescape("\v", "v", awk);
    check_classescape_identityescape("\"", awk);
    check_classescape_identityescape("/", awk);

    // awk supports octal sequences
    g_regexTester.should_match("\1", R"([\1])", awk);
    g_regexTester.should_not_match("1", R"([\1])", awk);
    g_regexTester.should_not_match("\\", R"([\1])", awk);
    g_regexTester.should_match("\11", R"([\11])", awk);
    g_regexTester.should_not_match("1", R"([\11])", awk);
    g_regexTester.should_not_match("\\", R"([\11])", awk);
    g_regexTester.should_match("\111", R"([\111])", awk);
    g_regexTester.should_not_match("0", R"([\111])", awk);
    g_regexTester.should_not_match("\\", R"([\111])", awk);
    g_regexTester.should_match("\111", R"([\1111])", awk);
    g_regexTester.should_match("1", R"([\1111])", awk);
    g_regexTester.should_not_match("\\", R"([\1111])", awk);
    g_regexTester.should_match("A", R"([\101])", awk);
    g_regexTester.should_not_match("\\", R"([\101])", awk);
    g_regexTester.should_not_match("1", R"([\101])", awk);
    g_regexTester.should_not_match("0", R"([\101])", awk);
    g_regexTester.should_not_match("\1", R"([\101])", awk);
    g_regexTester.should_not_match("\0"s, R"([\101])", awk);
    g_regexTester.should_match("A", R"([\101a])", awk);
    g_regexTester.should_match("a", R"([\101a])", awk);
    g_regexTester.should_not_match("\\", R"([\101a])", awk);
    g_regexTester.should_not_match("1", R"([\101a])", awk);
    g_regexTester.should_not_match("0", R"([\101a])", awk);
    g_regexTester.should_not_match("\1", R"([\101a])", awk);
    g_regexTester.should_not_match("\0"s, R"([\101a])", awk);
    g_regexTester.should_match("\33", R"([\033])", awk);
    g_regexTester.should_not_match("\\", R"([\033])", awk);
    g_regexTester.should_not_match("3", R"([\033])", awk);
    g_regexTester.should_not_match("0", R"([\033])", awk);
    g_regexTester.should_not_match("\3", R"([\033])", awk);
    g_regexTester.should_not_match("\0"s, R"([\033])", awk);
    g_regexTester.should_match("\33", R"([\033a])", awk);
    g_regexTester.should_match("a", R"([\033a])", awk);
    g_regexTester.should_not_match("\\", R"([\033a])", awk);
    g_regexTester.should_not_match("3", R"([\033a])", awk);
    g_regexTester.should_not_match("0", R"([\033a])", awk);
    g_regexTester.should_not_match("\3", R"([\033a])", awk);
    g_regexTester.should_not_match("\0"s, R"([\033a])", awk);
    g_regexTester.should_match("\33", R"([\33])", awk);
    g_regexTester.should_not_match("\\", R"([\33])", awk);
    g_regexTester.should_not_match("3", R"([\33])", awk);
    g_regexTester.should_not_match("\3", R"([\33])", awk);
    g_regexTester.should_match("\33", R"([\33a])", awk);
    g_regexTester.should_match("a", R"([\33a])", awk);
    g_regexTester.should_not_match("\\", R"([\33a])", awk);
    g_regexTester.should_not_match("3", R"([\33a])", awk);
    g_regexTester.should_not_match("\3", R"([\33a])", awk);
    g_regexTester.should_match("\1", R"([\001])", awk);
    g_regexTester.should_not_match("\\", R"([\001])", awk);
    g_regexTester.should_not_match("1", R"([\001])", awk);
    g_regexTester.should_not_match("0", R"([\001])", awk);
    g_regexTester.should_not_match("\0"s, R"([\001])", awk);
    g_regexTester.should_match("\1", R"([\001a])", awk);
    g_regexTester.should_match("a", R"([\001a])", awk);
    g_regexTester.should_not_match("\\", R"([\001a])", awk);
    g_regexTester.should_not_match("1", R"([\001a])", awk);
    g_regexTester.should_not_match("0", R"([\001a])", awk);
    g_regexTester.should_not_match("\0"s, R"([\001a])", awk);
    g_regexTester.should_match("\1", R"([\01])", awk);
    g_regexTester.should_not_match("\\", R"([\01])", awk);
    g_regexTester.should_not_match("1", R"([\01])", awk);
    g_regexTester.should_not_match("0", R"([\01])", awk);
    g_regexTester.should_not_match("\0"s, R"([\01])", awk);
    g_regexTester.should_match("\1", R"([\01a])", awk);
    g_regexTester.should_match("a", R"([\01a])", awk);
    g_regexTester.should_not_match("\\", R"([\01a])", awk);
    g_regexTester.should_not_match("1", R"([\01a])", awk);
    g_regexTester.should_not_match("0", R"([\01a])", awk);
    g_regexTester.should_not_match("\0"s, R"([\01a])", awk);
    g_regexTester.should_match("\1", R"([\1])", awk);
    g_regexTester.should_not_match("\\", R"([\1])", awk);
    g_regexTester.should_not_match("1", R"([\1])", awk);
    g_regexTester.should_match("\1", R"([\1a])", awk);
    g_regexTester.should_match("a", R"([\1a])", awk);
    g_regexTester.should_not_match("\\", R"([\1a])", awk);
    g_regexTester.should_not_match("1", R"([\1a])", awk);
    g_regexTester.should_throw(R"([\8])", error_escape, awk);
    g_regexTester.should_match("\1", R"([\18])", awk);
    g_regexTester.should_match("8", R"([\18])", awk);
    g_regexTester.should_not_match("\\", R"([\18])", awk);
    g_regexTester.should_not_match("1", R"([\18])", awk);
    g_regexTester.should_match("\12", R"([\129])", awk);
    g_regexTester.should_match("9", R"([\129])", awk);
    g_regexTester.should_not_match("\\", R"([\129])", awk);
    g_regexTester.should_not_match("1", R"([\129])", awk);
    g_regexTester.should_not_match("2", R"([\129])", awk);
    g_regexTester.should_not_match("\131", R"([\129])", awk);

    // octal sequences evaluating to 0 are considered undefined by the standard
    g_regexTester.should_throw(R"([\0])", error_escape, awk);
    g_regexTester.should_throw(R"([\00])", error_escape, awk);
    g_regexTester.should_throw(R"([\000])", error_escape, awk);

    // all other escapes (including the atom escapes of basic and extended regexes) are undefined
    g_regexTester.should_throw(R"([\.])", error_escape, awk);
    g_regexTester.should_throw(R"([\[])", error_escape, awk);
    g_regexTester.should_throw(R"([\*])", error_escape, awk);
    g_regexTester.should_throw(R"([\^])", error_escape, awk);
    g_regexTester.should_throw(R"([\$])", error_escape, awk);
    g_regexTester.should_throw(R"([\]])", error_escape, awk);
    g_regexTester.should_throw(R"([\+])", error_escape, awk);
    g_regexTester.should_throw(R"([\?])", error_escape, awk);
    g_regexTester.should_throw(R"([\|])", error_escape, awk);
    g_regexTester.should_throw(R"([\(])", error_escape, awk);
    g_regexTester.should_throw(R"([\)])", error_escape, awk);
    g_regexTester.should_throw(R"([\{])", error_escape, awk);
    g_regexTester.should_throw(R"([\}])", error_escape, awk);
    g_regexTester.should_throw(R"([\B])", error_escape, awk);
    g_regexTester.should_throw(R"([\c])", error_escape, awk);
    g_regexTester.should_throw(R"([\ca])", error_escape, awk);
    g_regexTester.should_throw(R"([\x000])", error_escape, awk);
    g_regexTester.should_throw(R"([\u0000])", error_escape, awk);
    g_regexTester.should_throw(R"([\d])", error_escape, awk);
    g_regexTester.should_throw(R"([\D])", error_escape, awk);
    g_regexTester.should_throw(R"([\w])", error_escape, awk);
    g_regexTester.should_throw(R"([\W])", error_escape, awk);
    g_regexTester.should_throw(R"([\s])", error_escape, awk);
    g_regexTester.should_throw(R"([\S])", error_escape, awk);
}

void test_gh_5244() {
    // GH-5244: <regex>: Some escape sequences are mishandled
    test_gh_5244_atomescape_ecmascript();
    test_gh_5244_classescape_ecmascript();

    for (syntax_option_type basic_or_grep : {basic, grep}) {
        test_gh_5244_atomescape_basic_or_grep(basic_or_grep);
        test_gh_5244_classescape_basic_or_grep(basic_or_grep);
    }

    for (syntax_option_type extended_or_egrep : {extended, egrep}) {
        test_gh_5244_atomescape_extended_or_egrep(extended_or_egrep);
        test_gh_5244_classescape_extended_or_egrep(extended_or_egrep);
    }

    test_gh_5244_atomescape_awk();
    test_gh_5244_classescape_awk();
}

void test_gh_5379() {
    // GH-5379: Backslashes in character classes are sometimes not matched in basic regular expressions

    // Correct handling of these backslashes at the beginning of a character class is already covered by GH-5244 tests.
    // The following tests check that backslashes are handled correctly immediately after a bracketed character class.
    for (syntax_option_type syntax : {basic, grep}) {
        g_regexTester.should_throw(R"([a]\b)", error_escape, syntax);
        g_regexTester.should_match("a[b]", R"([a]\[b])", syntax);
        g_regexTester.should_match("a", R"(\([a]\))", syntax);
        g_regexTester.should_match("ab", R"([a]\(b\))", syntax);
        g_regexTester.should_match("a", R"([a]\{1\})", syntax);
        g_regexTester.should_throw(R"([a]\})", error_brace, syntax);

        // also check handling of identity escape "\]",
        // which is supported as an extension following more recent POSIX standards
        g_regexTester.should_match("a]", R"([a]\])", syntax);
    }
}

int main() {
    test_gh_5244();
    test_gh_5379();

    return g_regexTester.result();
}
