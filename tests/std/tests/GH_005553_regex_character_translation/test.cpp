// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstddef>
#include <regex>
#include <string>

#include <test_regex_support.hpp>

using namespace std;
using namespace std::regex_constants;

regex_fixture g_regexTester;

template <class charT>
class nonidempotent_translate_regex_traits : regex_traits<charT> {
private:
    using rx_traits = regex_traits<charT>;

public:
    using char_type       = typename rx_traits::char_type;
    using string_type     = typename rx_traits::string_type;
    using locale_type     = typename rx_traits::locale_type;
    using char_class_type = typename rx_traits::char_class_type;
    using uchar_type      = make_unsigned_t<charT>;

    // TRANSITION, GH-995
    using _Uelem = typename rx_traits::_Uelem;

    nonidempotent_translate_regex_traits() = default;

    using rx_traits::length;

    charT translate(const charT c) const {
        return static_cast<charT>(static_cast<uchar_type>(rx_traits::translate(c)) / 2U);
    }


    charT translate_nocase(const charT c) const {
        return static_cast<charT>(static_cast<uchar_type>(rx_traits::translate_nocase(c)) / 2U);
    }

    using rx_traits::getloc;
    using rx_traits::imbue;
    using rx_traits::isctype;
    using rx_traits::lookup_classname;
    using rx_traits::lookup_collatename;
    using rx_traits::transform;
    using rx_traits::transform_primary;
    using rx_traits::value;
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

template <class Rx>
void check_search_match(const string& subject, const string& expected, const string& pattern, const Rx& re,
    match_flag_type flags = match_default) {
    smatch match;
    const bool search_result = regex_search(subject, match, re, flags);
    if (!search_result || match[0] != expected) {
        printf(R"(Expected regex_search("%s", regex("%s", 0x%X), 0x%X) to find "%s", )", subject.c_str(),
            pattern.c_str(), static_cast<unsigned int>(re.flags()), static_cast<unsigned int>(flags), expected.c_str());
        if (search_result) {
            printf(R"(but it matched "%s")"
                   "\n",
                match.str().c_str());
        } else {
            puts("but it failed to match");
        }
        g_regexTester.fail_regex();
    }
}

template <class Rx>
void check_search_fail(
    const string& subject, const string& pattern, const Rx& re, match_flag_type flags = match_default) {
    smatch match;
    const bool search_result = regex_search(subject, match, re, flags);
    if (search_result) {
        printf(R"(Expected regex_search("%s", regex("%s", 0x%X), 0x%X) to not match, but it found "%s")"
               "\n",
            subject.c_str(), pattern.c_str(), static_cast<unsigned int>(re.flags()), static_cast<unsigned int>(flags),
            match.str().c_str());
        g_regexTester.fail_regex();
    }
}

void test_gh_5553() {
    // GH-5553 `<regex>`: Correct character translation in `icase` and `collate` mode
    {
        string pattern = "g";
        basic_regex<char, nonidempotent_translate_regex_traits<char>> charcompare_icase_pattern{pattern, icase};
        check_match("f", pattern, charcompare_icase_pattern);
        check_match("F", pattern, charcompare_icase_pattern);
        check_match("g", pattern, charcompare_icase_pattern);
        check_match("G", pattern, charcompare_icase_pattern);
        check_no_match("e", pattern, charcompare_icase_pattern);
        check_no_match("E", pattern, charcompare_icase_pattern);
        check_no_match("h", pattern, charcompare_icase_pattern);
        check_no_match("H", pattern, charcompare_icase_pattern);

        check_search_match("abcdefghijklmnopqrstuvwxyz", "f", pattern, charcompare_icase_pattern);
        check_search_match("ABCDEFGHIJKLMNOPQRSTUVWXYZ", "F", pattern, charcompare_icase_pattern);
        check_search_match("zyxwvutsrqponmlkjihgfedcba", "g", pattern, charcompare_icase_pattern);
        check_search_match("ZYXWVUTSRQPONMLKJIHGFEDCBA", "G", pattern, charcompare_icase_pattern);
        check_search_fail("zyxwvutsrqponmlkjihedcba", pattern, charcompare_icase_pattern);
        check_search_fail("ABCDEHIJKLMNOPQRSTUVWXYZ", pattern, charcompare_icase_pattern);
    }

    {
        string pattern = "g";
        basic_regex<char, nonidempotent_translate_regex_traits<char>> charcompare_collate_pattern{
            pattern, regex_constants::collate};
        check_match("f", pattern, charcompare_collate_pattern);
        check_no_match("F", pattern, charcompare_collate_pattern);
        check_match("g", pattern, charcompare_collate_pattern);
        check_no_match("G", pattern, charcompare_collate_pattern);
        check_no_match("e", pattern, charcompare_collate_pattern);
        check_no_match("E", pattern, charcompare_collate_pattern);
        check_no_match("h", pattern, charcompare_collate_pattern);
        check_no_match("H", pattern, charcompare_collate_pattern);

        check_search_match("abcdefghijklmnopqrstuvwxyz", "f", pattern, charcompare_collate_pattern);
        check_search_fail("ABCDEFGHIJKLMNOPQRSTUVWXYZ", pattern, charcompare_collate_pattern);
        check_search_match("zyxwvutsrqponmlkjihgfedcba", "g", pattern, charcompare_collate_pattern);
        check_search_fail("ZYXWVUTSRQPONMLKJIHGFEDCBA", pattern, charcompare_collate_pattern);
        check_search_fail("zyxwvutsrqponmlkjihedcba", pattern, charcompare_collate_pattern);
    }

    {
        string pattern = "[g]";
        basic_regex<char, nonidempotent_translate_regex_traits<char>> charclasscompare_icase_pattern{pattern, icase};
        check_match("f", pattern, charclasscompare_icase_pattern);
        check_match("F", pattern, charclasscompare_icase_pattern);
        check_match("g", pattern, charclasscompare_icase_pattern);
        check_match("G", pattern, charclasscompare_icase_pattern);
        check_no_match("e", pattern, charclasscompare_icase_pattern);
        check_no_match("E", pattern, charclasscompare_icase_pattern);
        check_no_match("h", pattern, charclasscompare_icase_pattern);
        check_no_match("H", pattern, charclasscompare_icase_pattern);
    }

    {
        string pattern = "[g]";
        basic_regex<char, nonidempotent_translate_regex_traits<char>> charclasscompare_collate_pattern{
            pattern, regex_constants::collate};
        check_match("f", pattern, charclasscompare_collate_pattern);
        check_no_match("F", pattern, charclasscompare_collate_pattern);
        check_match("g", pattern, charclasscompare_collate_pattern);
        check_no_match("G", pattern, charclasscompare_collate_pattern);
        check_no_match("e", pattern, charclasscompare_collate_pattern);
        check_no_match("E", pattern, charclasscompare_collate_pattern);
        check_no_match("h", pattern, charclasscompare_collate_pattern);
        check_no_match("H", pattern, charclasscompare_collate_pattern);
    }

    {
        string pattern = "[g-i]";
        basic_regex<char, nonidempotent_translate_regex_traits<char>> charrangecompare_icase_pattern{pattern, icase};
        check_match("f", pattern, charrangecompare_icase_pattern);
        check_match("F", pattern, charrangecompare_icase_pattern);
        check_match("g", pattern, charrangecompare_icase_pattern);
        check_match("G", pattern, charrangecompare_icase_pattern);
        check_match("i", pattern, charrangecompare_icase_pattern);
        check_match("I", pattern, charrangecompare_icase_pattern);
        check_no_match("e", pattern, charrangecompare_icase_pattern);
        check_no_match("E", pattern, charrangecompare_icase_pattern);
        check_no_match("j", pattern, charrangecompare_icase_pattern);
        check_no_match("J", pattern, charrangecompare_icase_pattern);
    }

    {
        string pattern = "[g-i]";
        basic_regex<char, nonidempotent_translate_regex_traits<char>> charrangecompare_collate_pattern{
            pattern, regex_constants::collate};
        check_match("f", pattern, charrangecompare_collate_pattern);
        check_no_match("F", pattern, charrangecompare_collate_pattern);
        check_match("g", pattern, charrangecompare_collate_pattern);
        check_no_match("G", pattern, charrangecompare_collate_pattern);
        check_match("i", pattern, charrangecompare_collate_pattern);
        check_no_match("I", pattern, charrangecompare_collate_pattern);
        check_no_match("e", pattern, charrangecompare_collate_pattern);
        check_no_match("E", pattern, charrangecompare_collate_pattern);
        check_no_match("j", pattern, charrangecompare_collate_pattern);
        check_no_match("J", pattern, charrangecompare_collate_pattern);
    }
}

int main() {
    test_gh_5553();

    return g_regexTester.result();
}
