// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <locale>
#include <regex>
#include <string>

#include <test_regex_support.hpp>

// skip collation tests when linking to the DLL in case of
// _ITERATOR_DEBUG_LEVEL mismatch between code and linked DLL
#ifdef _DEBUG
#define DEFAULT_IDL_SETTING 2
#else
#define DEFAULT_IDL_SETTING 0
#endif

#if defined(_DLL) && _ITERATOR_DEBUG_LEVEL != DEFAULT_IDL_SETTING
#define SKIP_COLLATE_TESTS
#endif // defined(_DLL) && _ITERATOR_DEBUG_LEVEL != DEFAULT_IDL_SETTING

using namespace std;
using namespace std::regex_constants;

class test_wregex_locale {
private:
    regex_fixture* const fixture;
    const wstring pattern;
    const string locname;
    const syntax_option_type syntax;
    wregex r;

public:
    test_wregex_locale(regex_fixture* const fixture_, const wstring& pattern_, const string& locname_,
        const syntax_option_type syntax_ = ECMAScript)
        : fixture(fixture_), pattern(pattern_), locname(locname_), syntax(syntax_), r() {
        r.imbue(locale{locname});
        r.assign(pattern, syntax);
    }

    test_wregex_locale(const test_wregex_locale&)            = delete;
    test_wregex_locale& operator=(const test_wregex_locale&) = delete;

    void should_search_match(
        const wstring& subject, const wstring& expected, const match_flag_type match_flags = match_default) const {
        wsmatch mr;
        try {
            const bool search_result = regex_search(subject, mr, r, match_flags);
            if (!search_result || mr[0] != expected) {
                wprintf(LR"(Expected regex_search("%s", regex("%s", 0x%X), 0x%X) to find "%s" for locale "%hs", )",
                    subject.c_str(), pattern.c_str(), static_cast<unsigned int>(syntax),
                    static_cast<unsigned int>(match_flags), expected.c_str(), locname.c_str());
                if (search_result) {
                    wprintf(LR"(but it matched "%s")"
                            "\n",
                        mr.str().c_str());
                } else {
                    puts("but it failed to match");
                }

                fixture->fail_regex();
            }
        } catch (const regex_error& e) {
            wprintf(LR"(Failed to regex_search("%s", regex("%s", 0x%X), 0x%X))", subject.c_str(), pattern.c_str(),
                static_cast<unsigned int>(syntax), static_cast<unsigned int>(match_flags));
            printf(R"( for locale "%s": regex_error: "%s")"
                   "\n",
                locname.c_str(), e.what());
            fixture->fail_regex();
        }
    }

    void should_search_fail(const wstring& subject, const match_flag_type match_flags = match_default) const {
        wsmatch mr;
        try {
            if (regex_search(subject, mr, r, match_flags)) {
                wprintf(LR"(Expected regex_search("%s", regex("%s", 0x%X), 0x%X) to not match )"
                        LR"(for locale "%hs", but it found "%s")"
                        "\n",
                    subject.c_str(), pattern.c_str(), static_cast<unsigned int>(syntax),
                    static_cast<unsigned int>(match_flags), locname.c_str(), mr.str().c_str());
                fixture->fail_regex();
            }
        } catch (const regex_error& e) {
            wprintf(LR"(Failed to regex_search("%s", regex("%s", 0x%X), 0x%X))", subject.c_str(), pattern.c_str(),
                static_cast<unsigned int>(syntax), static_cast<unsigned int>(match_flags));
            printf(R"( for locale "%s": regex_error: "%s")"
                   "\n",
                locname.c_str(), e.what());
            fixture->fail_regex();
        }
    }
};

regex_fixture g_regexTester;

void regex_with_locale_should_throw(const wstring& pattern, const string& locname, const error_type expected,
    const syntax_option_type syntax = regex_constants::collate) {
    wregex r;
    r.imbue(locale{locname});
    try {
        r.assign(pattern, syntax);
        wprintf(LR"(regex r("%s", 0x%X) succeeded for locale "%hs" (which is bad).)"
                L"\n",
            pattern.c_str(), static_cast<unsigned int>(syntax), locname.c_str());
        g_regexTester.fail_regex();
    } catch (const regex_error& e) {
        if (e.code() != expected) {
            wprintf(LR"(regex r("%s", 0x%X) with locale "%hs" threw 0x%X; expected 0x%X)"
                    L"\n",
                pattern.c_str(), static_cast<unsigned int>(syntax), locname.c_str(),
                static_cast<unsigned int>(e.code()), static_cast<unsigned int>(expected));
            g_regexTester.fail_regex();
        }
    }
}

void test_collating_ranges_german() {

    // special characters in German (umlauts and sharp s)
    static constexpr const wchar_t* special_characters[] = {
        L"\u00E4", // U+00E4 LATIN SMALL LETTER A WITH DIARESIS
        L"\u00C4", // U+00C4 LATIN CAPITAL LETTER A WITH DIARESIS
        L"\u00DF", // U+00DF LATIN SMALL LETTER SHARP S
        L"\u1E9E", // U+1E9E LATIN CAPITAL LETTER SHARP S
        L"\u00F6", // U+00F6 LATIN SMALL LETTER U WITH DIARESIS
        L"\u00D6", // U+00D6 LATIN CAPITAL LETTER U WITH DIARESIS
        L"\u00FC", // U+00FC LATIN SMALL LETTER O WITH DIARESIS
        L"\u00DC" // U+00DC LATIN CAPITAL LETTER O WITH DIARESIS
    };

    // sanity checks: collation not enabled, with or without imbued locale
    {
        // [a-z], [A-Z] and [A-z] should not match special German characters
        for (const wstring& pattern : {L"[a-z]", L"[A-Z]", L"[A-z]"}) {
            {
                test_wregex nocollate_nolocale(&g_regexTester, pattern);
                for (const auto& s : special_characters) {
                    nocollate_nolocale.should_search_fail(s);
                }
            }

            {
                test_wregex_locale nocollate_locale(&g_regexTester, pattern, "de-DE");
                for (const auto& s : special_characters) {
                    nocollate_locale.should_search_fail(s);
                }
            }
        }
    }

#ifndef SKIP_COLLATE_TESTS
    // de-DE collation order sorts as follows:
    // a, A,
    // U+00E4 LATIN SMALL LETTER A WITH DIARESIS,
    // U+00C4 LATIN CAPITAL LETTER A WITH DIARESIS,
    // b, B, ..., o, O,
    // U+00F6 LATIN SMALL LETTER O WITH DIARESIS,
    // U+00D6 LATIN CAPITAL LETTER O WITH DIARESIS,
    // p, P, ..., s, S,
    // U+00DF LATIN SMALL LETTER SHARP S
    // U+1E9E LATIN CAPITAL LETTER SHARP S,
    // t, T, u, U,
    // U+00FC LATIN SMALL LETTER U WITH DIARESIS,
    // U+00DC LATIN CAPITAL LETTER U WITH DIARESIS,
    // v, V, ..., z, Z

    static constexpr const wchar_t* special_characters_without_ae[] = {
        L"\u00DF", // U+00DF LATIN SMALL LETTER SHARP S
        L"\u1E9E", // U+1E9E LATIN CAPITAL LETTER SHARP S
        L"\u00F6", // U+00F6 LATIN SMALL LETTER U WITH DIARESIS
        L"\u00D6", // U+00D6 LATIN CAPITAL LETTER U WITH DIARESIS
        L"\u00FC", // U+00FC LATIN SMALL LETTER O WITH DIARESIS
        L"\u00DC" // U+00DC LATIN CAPITAL LETTER O WITH DIARESIS
    };

    {
        test_wregex_locale collate_a_to_a_regex(&g_regexTester, L"[a-a]", "de-DE", regex_constants::collate);
        collate_a_to_a_regex.should_search_match(L"a", L"a");
        collate_a_to_a_regex.should_search_fail(L"A");
        collate_a_to_a_regex.should_search_fail(L"\u00E4"); // U+00E4 LATIN SMALL LETTER A WITH DIARESIS
        collate_a_to_a_regex.should_search_fail(L"\u00C4"); // U+00C4 LATIN CAPITAL LETTER A WITH DIARESIS
        collate_a_to_a_regex.should_search_fail(L"b");
        for (const auto& s : special_characters_without_ae) {
            collate_a_to_a_regex.should_search_fail(s);
        }
    }

    {
        test_wregex_locale collate_a_to_A_regex(&g_regexTester, L"[a-A]", "de-DE", regex_constants::collate);
        collate_a_to_A_regex.should_search_match(L"a", L"a");
        collate_a_to_A_regex.should_search_match(L"A", L"A");
        collate_a_to_A_regex.should_search_fail(L"\u00E4"); // U+00E4 LATIN SMALL LETTER A WITH DIARESIS
        collate_a_to_A_regex.should_search_fail(L"\u00C4"); // U+00C4 LATIN CAPITAL LETTER A WITH DIARESIS
        collate_a_to_A_regex.should_search_fail(L"b");
        for (const auto& s : special_characters_without_ae) {
            collate_a_to_A_regex.should_search_fail(s);
        }
    }

    {
        test_wregex_locale collate_a_to_ae_regex(&g_regexTester,
            L"[a-\u00E4]", // U+00E4 LATIN SMALL LETTER A WITH DIARESIS
            "de-DE", regex_constants::collate);
        collate_a_to_ae_regex.should_search_match(L"a", L"a");
        collate_a_to_ae_regex.should_search_match(L"A", L"A");
        collate_a_to_ae_regex.should_search_match(L"\u00E4", L"\u00E4"); // U+00E4 LATIN SMALL LETTER A WITH DIARESIS
        collate_a_to_ae_regex.should_search_fail(L"\u00C4"); // U+00C4 LATIN CAPITAL LETTER A WITH DIARESIS
        collate_a_to_ae_regex.should_search_fail(L"b");
        for (const auto& s : special_characters_without_ae) {
            collate_a_to_ae_regex.should_search_fail(s);
        }
    }

    {
        test_wregex_locale collate_a_to_Ae_regex(&g_regexTester,
            L"[a-\u00C4]", // U+00C4 LATIN CAPITAL LETTER A WITH DIARESIS
            "de-DE", regex_constants::collate);
        collate_a_to_Ae_regex.should_search_match(L"a", L"a");
        collate_a_to_Ae_regex.should_search_match(L"A", L"A");
        collate_a_to_Ae_regex.should_search_match(L"\u00E4", L"\u00E4"); // U+00E4 LATIN SMALL LETTER A WITH DIARESIS
        collate_a_to_Ae_regex.should_search_match(L"\u00C4", L"\u00C4"); // U+00C4 LATIN CAPITAL LETTER A WITH DIARESIS
        collate_a_to_Ae_regex.should_search_fail(L"b");
        for (const auto& s : special_characters_without_ae) {
            collate_a_to_Ae_regex.should_search_fail(s);
        }
    }

    {
        test_wregex_locale collate_a_to_b_regex(&g_regexTester, L"[a-b]", "de-DE", regex_constants::collate);
        collate_a_to_b_regex.should_search_match(L"a", L"a");
        collate_a_to_b_regex.should_search_match(L"A", L"A");
        collate_a_to_b_regex.should_search_match(L"\u00E4", L"\u00E4"); // U+00E4 LATIN SMALL LETTER A WITH DIARESIS
        collate_a_to_b_regex.should_search_match(L"\u00C4", L"\u00C4"); // U+00C4 LATIN CAPITAL LETTER A WITH DIARESIS
        collate_a_to_b_regex.should_search_match(L"b", L"b");
        for (const auto& s : special_characters_without_ae) {
            collate_a_to_b_regex.should_search_fail(s);
        }
    }

    static constexpr const wchar_t* special_characters_without_sharp_s[] = {
        L"\u00E4", // U+00E4 LATIN SMALL LETTER A WITH DIARESIS
        L"\u00C4", // U+00C4 LATIN CAPITAL LETTER A WITH DIARESIS
        L"\u00F6", // U+00F6 LATIN SMALL LETTER U WITH DIARESIS
        L"\u00D6", // U+00D6 LATIN CAPITAL LETTER U WITH DIARESIS
        L"\u00FC", // U+00FC LATIN SMALL LETTER O WITH DIARESIS
        L"\u00DC" // U+00DC LATIN CAPITAL LETTER O WITH DIARESIS
    };

    {
        test_wregex_locale collate_s_to_S_regex(&g_regexTester, L"[s-S]", "de-DE", regex_constants::collate);
        collate_s_to_S_regex.should_search_fail(L"r");
        collate_s_to_S_regex.should_search_match(L"s", L"s");
        collate_s_to_S_regex.should_search_match(L"S", L"S");
        collate_s_to_S_regex.should_search_fail(L"\u00DF"); // U+00DF LATIN SMALL LETTER SHARP S
        collate_s_to_S_regex.should_search_fail(L"\u1E9E"); // U+1E9E LATIN CAPITAL LETTER SHARP S
        collate_s_to_S_regex.should_search_fail(L"t");
        collate_s_to_S_regex.should_search_fail(L"u");
        for (const auto& s : special_characters_without_sharp_s) {
            collate_s_to_S_regex.should_search_fail(s);
        }
    }

    {
        test_wregex_locale collate_s_to_sharp_s_regex(&g_regexTester,
            L"[s-\u00DF]", // U+00DF LATIN SMALL LETTER SHARP S
            "de-DE", regex_constants::collate);
        collate_s_to_sharp_s_regex.should_search_fail(L"r");
        collate_s_to_sharp_s_regex.should_search_match(L"s", L"s");
        collate_s_to_sharp_s_regex.should_search_match(L"S", L"S");
        collate_s_to_sharp_s_regex.should_search_match(L"\u00DF", L"\u00DF"); // U+00DF LATIN SMALL LETTER SHARP S
        collate_s_to_sharp_s_regex.should_search_fail(L"\u1E9E"); // U+1E9E LATIN CAPITAL LETTER SHARP S
        collate_s_to_sharp_s_regex.should_search_fail(L"t");
        collate_s_to_sharp_s_regex.should_search_fail(L"u");
        for (const auto& s : special_characters_without_sharp_s) {
            collate_s_to_sharp_s_regex.should_search_fail(s);
        }
    }

    {
        test_wregex_locale collate_s_to_Sharp_S_regex(&g_regexTester, L"[s-\u1E9E]", // LATIN CAPITAL LETTER SHARP S
            "de-DE", regex_constants::collate);
        collate_s_to_Sharp_S_regex.should_search_fail(L"r");
        collate_s_to_Sharp_S_regex.should_search_match(L"s", L"s");
        collate_s_to_Sharp_S_regex.should_search_match(L"S", L"S");
        collate_s_to_Sharp_S_regex.should_search_match(L"\u00DF", L"\u00DF"); // U+00DF LATIN SMALL LETTER SHARP S
        collate_s_to_Sharp_S_regex.should_search_match(L"\u1E9E", L"\u1E9E"); // U+1E9E LATIN CAPITAL LETTER SHARP S
        collate_s_to_Sharp_S_regex.should_search_fail(L"t");
        collate_s_to_Sharp_S_regex.should_search_fail(L"u");
        for (const auto& s : special_characters_without_sharp_s) {
            collate_s_to_Sharp_S_regex.should_search_fail(s);
        }
    }

    {
        test_wregex_locale collate_s_to_t_regex(&g_regexTester, L"[s-t]", "de-DE", regex_constants::collate);
        collate_s_to_t_regex.should_search_fail(L"r");
        collate_s_to_t_regex.should_search_match(L"s", L"s");
        collate_s_to_t_regex.should_search_match(L"S", L"S");
        collate_s_to_t_regex.should_search_match(L"\u00DF", L"\u00DF"); // U+00DF LATIN SMALL LETTER SHARP S
        collate_s_to_t_regex.should_search_match(L"\u1E9E", L"\u1E9E"); // U+1E9E LATIN CAPITAL LETTER SHARP S
        collate_s_to_t_regex.should_search_match(L"t", L"t");
        collate_s_to_t_regex.should_search_fail(L"u");
        for (const auto& s : special_characters_without_sharp_s) {
            collate_s_to_t_regex.should_search_fail(s);
        }
    }

    {
        test_wregex_locale collate_A_to_z_regex(&g_regexTester, L"[A-z]", "de-DE", regex_constants::collate);
        collate_A_to_z_regex.should_search_fail(L"a");
        for (wchar_t ascii_upper = L'A'; ascii_upper < L'Z'; ++ascii_upper) {
            collate_A_to_z_regex.should_search_match(wstring(1, ascii_upper), wstring(1, ascii_upper));
        }
        for (wchar_t ascii_lower = L'b'; ascii_lower <= L'z'; ++ascii_lower) {
            collate_A_to_z_regex.should_search_match(wstring(1, ascii_lower), wstring(1, ascii_lower));
        }
        for (const auto& s : special_characters) {
            collate_A_to_z_regex.should_search_match(s, s);
        }
        collate_A_to_z_regex.should_search_fail(L"Z");
    }

    regex_with_locale_should_throw(L"[A-a]", "de-DE", error_range);
    regex_with_locale_should_throw(L"[\u00DF-S]", // U+00DF LATIN SMALL LETTER SHARP S
        "de-DE", error_range);
    regex_with_locale_should_throw(
        L"[\u1E9E-\u00DF]", // U+1E9E LATIN CAPITAL LETTER SHARP S, U+00DF LATIN SMALL LETTER SHARP S
        "de-DE", error_range);
#endif // !defined(SKIP_COLLATE_TESTS)
}

class gh_994_regex_traits : public regex_traits<char> {
public:
    template <class FwdIt>
    string_type lookup_collatename(FwdIt first, FwdIt last) const {
        // from Hungarian
        const string_type collating_symbols[] = {"cs", "Cs", "dzs"};
        const string_type hyphen_name         = "hyphen";

        for (const string_type& coll_symbol : collating_symbols) {
            if (equal(first, last, begin(coll_symbol), end(coll_symbol))) {
                return coll_symbol;
            }
        }

        if (equal(first, last, begin(hyphen_name), end(hyphen_name))) {
            return "-";
        }

        return regex_traits::lookup_collatename(first, last);
    }
};

using gh_994_regex = basic_regex<char, gh_994_regex_traits>;

void gh_994_verify_match(
    const string& subject, const string& pattern, const bool correct, const syntax_option_type syntax = ECMAScript) {

    gh_994_regex r;
    try {
        r.assign(pattern, syntax);
    } catch (const regex_error& e) {
        printf(R"(Failed to construct regex("%s", 0x%X) for traits gh_994_regex_traits: "%s")"
               "\n",
            pattern.c_str(), static_cast<unsigned int>(syntax), e.what());
        g_regexTester.fail_regex();
        return;
    }

    try {
        if (regex_match(subject, r) != correct) {
            printf(R"(Expected regex_match("%s", regex("%s", 0x%X)) to be %s for traits gh_994_regex_traits.)"
                   "\n",
                subject.c_str(), pattern.c_str(), static_cast<unsigned int>(syntax), correct ? "true" : "false");
            g_regexTester.fail_regex();
        }
    } catch (const regex_error& e) {
        printf(R"(Failed to regex_match("%s", regex("%s", 0x%X)) for traits gh_994_regex_traits: regex_error: "%s")"
               "\n",
            subject.c_str(), pattern.c_str(), static_cast<unsigned int>(syntax), e.what());
        g_regexTester.fail_regex();
    }
}

void gh_994_should_throw(
    const string& pattern, const error_type expected_code, const syntax_option_type syntax = ECMAScript) {

    try {
        gh_994_regex r(pattern, syntax);

        printf(R"(regex r("%s", 0x%X) succeeded for traits gh_994_regex_traits (which is bad).)"
               "\n",
            pattern.c_str(), static_cast<unsigned int>(syntax));
        g_regexTester.fail_regex();
    } catch (const regex_error& e) {
        if (e.code() != expected_code) {
            printf(R"(regex r("%s", 0x%X) threw 0x%X for traits gh_994_regex_traits; expected 0x%X)"
                   "\n",
                pattern.c_str(), static_cast<unsigned int>(syntax), static_cast<unsigned int>(e.code()),
                static_cast<unsigned int>(expected_code));
            g_regexTester.fail_regex();
        }
    }
}

void test_gh_994() {
    // GH-994: Regex with collating symbol erroneously returns a match
    // PR fixed parsing and matching of collating symbols and equivalences in character classes

    g_regexTester.should_not_match("v", "[[.(.]a[a]");

    g_regexTester.should_not_match("(((v", "[[.(.]]*");
    g_regexTester.should_not_match("v", "[[.(.]]*");
    g_regexTester.should_not_match("vv", "[[.(.]]*");

    g_regexTester.should_match("xxx", "[[.(.]x]*");
    g_regexTester.should_match("x((x(", "[[.(.]x]*");
    g_regexTester.should_not_match("xxxv", "[[.(.]x]*");
    g_regexTester.should_not_match("xxxvv", "[[.(.]x]*");
    g_regexTester.should_not_match("x(xv", "[[.(.]x]*");
    g_regexTester.should_not_match("v", "[[.(.]x]*");
    g_regexTester.should_not_match("vv", "[[.(.]x]*");
    g_regexTester.should_not_match("xxxv", "[[.(.]x]*");

    g_regexTester.should_throw("[[.whatisthis.]]", error_collate);

    gh_994_verify_match("a", "[[.cs.]a]", true);
    gh_994_verify_match("c", "[[.cs.]a]", false);
    gh_994_verify_match("ca", "[[.cs.]a]", false);
    gh_994_verify_match("ct", "[[.cs.]a]", false);
    gh_994_verify_match("cs", "[[.cs.]a]", true);
    gh_994_verify_match("Cs", "[[.cs.]a]", false);
    gh_994_verify_match("dsz", "[[.cs.]a]", false);
    gh_994_should_throw("[[.CS.]]", error_collate);

    gh_994_verify_match("cs", "[[.cs.][.dzs.]]", true);
    gh_994_verify_match("dzs", "[[.cs.][.dzs.]]", true);
    gh_994_verify_match("dz", "[[.cs.][.dzs.]]", false);
    gh_994_verify_match("Cs", "[[.cs.][.dzs.]]", false);
    gh_994_verify_match("cdzs", "[[.cs.][.dzs.]]", false);
    gh_994_verify_match("csdzs", "[[.cs.][.dzs.]]", false);
    gh_994_verify_match("a", "[[.cs.][.dzs.]]", false);
    gh_994_verify_match("dzt", "[[.cs.][.dzs.]]", false);

    gh_994_verify_match("csa", "[[.cs.][.dzs.]]a", true);
    gh_994_verify_match("csb", "[[.cs.][.dzs.]]a", false);
    gh_994_verify_match("Csa", "[[.cs.][.dzs.]]a", false);
    gh_994_verify_match("dzsa", "[[.cs.][.dzs.]]a", true);
    gh_994_verify_match("dzsb", "[[.cs.][.dzs.]]a", false);
    gh_994_verify_match("dza", "[[.cs.][.dzs.]]a", false);
    gh_994_verify_match("cdzsa", "[[.cs.][.dzs.]]a", false);
    gh_994_verify_match("csdzsa", "[[.cs.][.dzs.]]a", false);
    gh_994_verify_match("a", "[[.cs.][.dzs.]]a", false);
    gh_994_verify_match("aa", "[[.cs.][.dzs.]]a", false);
    gh_994_verify_match("dzta", "[[.cs.][.dzs.]]a", false);

    gh_994_verify_match("dzscs", "[[.cs.][.dzs.]a]*", true);
    gh_994_verify_match("dzsacs", "[[.cs.][.dzs.]a]*", true);
    gh_994_verify_match("dzsbcsa", "[[.cs.][.dzs.]a]*", false);
    gh_994_verify_match("dzscsb", "[[.cs.][.dzs.]a]*", false);
    gh_994_verify_match("dzscsb", "[[.cs.][.dzs.]a]*b", true);
    gh_994_verify_match("dzsCsb", "[[.cs.][.dzs.]a]*b", false);
    gh_994_verify_match("bdzscs", "[[.cs.][.dzs.]a]*", false);
    gh_994_verify_match("bdzscs", "b[[.cs.][.dzs.]a]*", true);

    gh_994_verify_match("-", "[[.hyphen.]]", true);
    gh_994_verify_match("hyphen", "[[.hyphen.]]", false);
    gh_994_verify_match("h", "[[.hyphen.]]", false);
    gh_994_verify_match("y", "[[.hyphen.]]", false);
    gh_994_verify_match("n", "[[.hyphen.]]", false);

    gh_994_verify_match("cs", "[[.cs.]]", true, icase);
    gh_994_verify_match("Cs", "[[.cs.]]", true, icase);
    gh_994_verify_match("CS", "[[.cs.]]", true, icase);
    gh_994_verify_match("cs", "[[.Cs.]]", true, icase);
    gh_994_verify_match("Cs", "[[.Cs.]]", true, icase);
    gh_994_verify_match("CS", "[[.Cs.]]", true, icase);
    gh_994_should_throw("[[.CS.]]", error_collate, icase);
    gh_994_verify_match("dzscsb", "[[.Cs.][.dzs.]a]*", false, icase);
    gh_994_verify_match("dzscsb", "[[.cs.][.dzs.]a]*b", true, icase);
    gh_994_verify_match("dzsCsb", "[[.cs.][.dzs.]a]*b", true, icase);
    gh_994_verify_match("DzsCsb", "[[.cs.][.dzs.]a]*b", true, icase);

    gh_994_verify_match("cs", "[[.cs.]]", true, regex_constants::collate);
    gh_994_verify_match("Cs", "[[.cs.]]", false, regex_constants::collate);
    gh_994_verify_match("CS", "[[.cs.]]", false, regex_constants::collate);
    gh_994_verify_match("cs", "[[.Cs.]]", false, regex_constants::collate);
    gh_994_verify_match("Cs", "[[.Cs.]]", true, regex_constants::collate);
    gh_994_verify_match("CS", "[[.Cs.]]", false, regex_constants::collate);
    gh_994_should_throw("[[.CS.]]", error_collate, regex_constants::collate);
    gh_994_verify_match("dzscsb", "[[.cs.][.dzs.]a]*", false, regex_constants::collate);
    gh_994_verify_match("dzscsb", "[[.cs.][.dzs.]a]*b", true, regex_constants::collate);
    gh_994_verify_match("dzsCsb", "[[.cs.][.dzs.]a]*b", false, regex_constants::collate);
    gh_994_verify_match("DzsCsb", "[[.cs.][.dzs.]a]*b", false, regex_constants::collate);

    g_regexTester.should_match("b", "[[.b.]-f]");
    g_regexTester.should_match("f", "[[.b.]-f]");
    g_regexTester.should_not_match("a", "[[.b.]-f]");
    g_regexTester.should_not_match("g", "[[.b.]-f]");
    g_regexTester.should_match("b", "[b-[.f.]]");
    g_regexTester.should_match("f", "[b-[.f.]]");
    g_regexTester.should_not_match("a", "[b-[.f.]]");
    g_regexTester.should_not_match("g", "[b-[.f.]]");
    g_regexTester.should_match("b", "[[.b.]-[.f.]]");
    g_regexTester.should_match("f", "[[.b.]-[.f.]]");
    g_regexTester.should_not_match("a", "[[.b.]-[.f.]]");
    g_regexTester.should_not_match("g", "[[.b.]-[.f.]]");

    g_regexTester.should_match("bi", "[[.b.]-f]i");
    g_regexTester.should_match("fi", "[[.b.]-f]i");
    g_regexTester.should_not_match("ai", "[[.b.]-f]i");
    g_regexTester.should_not_match("gi", "[[.b.]-f]i");
    g_regexTester.should_not_match("i", "[[.b.]-f]i");
    g_regexTester.should_match("bi", "[b-[.f.]]i");
    g_regexTester.should_match("fi", "[b-[.f.]]i");
    g_regexTester.should_not_match("ai", "[b-[.f.]]i");
    g_regexTester.should_not_match("gi", "[b-[.f.]]i");
    g_regexTester.should_not_match("i", "[b-[.f.]]i");
    g_regexTester.should_match("bi", "[[.b.]-[.f.]]i");
    g_regexTester.should_match("fi", "[[.b.]-[.f.]]i");
    g_regexTester.should_not_match("ai", "[[.b.]-[.f.]]i");
    g_regexTester.should_not_match("gi", "[[.b.]-[.f.]]i");
    g_regexTester.should_not_match("i", "[[.b.]-[.f.]]i");

    g_regexTester.should_match("becdfi", "[[.b.]-[.f.]]*i");
    g_regexTester.should_not_match("becdfb", "[[.b.]-[.f.]]*i");
    g_regexTester.should_not_match("becdfj", "[[.b.]-[.f.]]*i");

    // TRANSITION, GH-5391
    gh_994_should_throw("[[.cs.]-f]", error_range);
    gh_994_should_throw("[a-[.cs.]]", error_range);
    gh_994_should_throw("[[.cs.]-[.dzs.]]", error_range);

#ifndef _M_CEE_PURE
    g_regexTester.should_throw("[[=a=]-c]", error_range);
    g_regexTester.should_throw("[c-[=z=]]", error_range);
    g_regexTester.should_throw("[[=a=]-[=z=]]", error_range);

    g_regexTester.should_match("a", "[[=a=]]");
    g_regexTester.should_not_match("A", "[[=a=]]");
    g_regexTester.should_not_match("b", "[[=a=]]");
    g_regexTester.should_not_match("B", "[[=a=]]");
    g_regexTester.should_not_match("z", "[[=Z=]]");
    g_regexTester.should_match("Z", "[[=Z=]]");
    g_regexTester.should_not_match("b", "[[=Z=]]");
    g_regexTester.should_not_match("B", "[[=Z=]]");

    g_regexTester.should_match("a", "[[=a=]]", icase);
    g_regexTester.should_match("A", "[[=a=]]", icase);
    g_regexTester.should_not_match("b", "[[=a=]]", icase);
    g_regexTester.should_not_match("B", "[[=a=]]", icase);
    g_regexTester.should_match("z", "[[=Z=]]", icase);
    g_regexTester.should_match("Z", "[[=Z=]]", icase);
    g_regexTester.should_not_match("b", "[[=Z=]]", icase);
    g_regexTester.should_not_match("B", "[[=Z=]]", icase);

    g_regexTester.should_match("ab", "[[=a=]]b");
    g_regexTester.should_not_match("Ab", "[[=a=]]b");
    g_regexTester.should_not_match("Ab", "[[=a=]]B");
    g_regexTester.should_not_match("b", "[[=a=]]b");
    g_regexTester.should_not_match("aab", "[[=a=]]b");
    g_regexTester.should_not_match("B", "[[=a=]]b");
    g_regexTester.should_not_match("ab", "[[=A=]]b");
    g_regexTester.should_match("Ab", "[[=A=]]b");
    g_regexTester.should_not_match("Ab", "[[=A=]]B");
    g_regexTester.should_not_match("b", "[[=A=]]b");
    g_regexTester.should_not_match("AAb", "[[=A=]]b");
    g_regexTester.should_not_match("B", "[[=A=]]b");

    g_regexTester.should_match("ab", "[[=a=]]b", icase);
    g_regexTester.should_match("Ab", "[[=a=]]b", icase);
    g_regexTester.should_match("Ab", "[[=a=]]B", icase);
    g_regexTester.should_not_match("b", "[[=a=]]b", icase);
    g_regexTester.should_not_match("aab", "[[=a=]]b", icase);
    g_regexTester.should_not_match("B", "[[=a=]]b", icase);
    g_regexTester.should_match("ab", "[[=A=]]b", icase);
    g_regexTester.should_match("Ab", "[[=A=]]b", icase);
    g_regexTester.should_match("Ab", "[[=A=]]B", icase);
    g_regexTester.should_not_match("b", "[[=A=]]b", icase);
    g_regexTester.should_not_match("AAb", "[[=A=]]b", icase);
    g_regexTester.should_not_match("B", "[[=A=]]b", icase);

    g_regexTester.should_not_match("AaAaaAaab", "[[=a=]]*b");
    g_regexTester.should_not_match("AaAaaAaab", "[[=a=]]*c");
    g_regexTester.should_not_match("AaAabcaAaad", "[[=a=]bc]*d");
    g_regexTester.should_match("AaAaaAaab", "[[=a=]]*b", icase);
    g_regexTester.should_not_match("AaAaaAaab", "[[=a=]]*c", icase);
    g_regexTester.should_match("AaAabcaAaad", "[[=a=]bc]*d", icase);
#endif // ^^^ !defined(_M_CEE_PURE) ^^^
}

void test_gh_5435() {
    // GH-5435: <regex>: Equivalence classes have unexpected behavior with std::wregex
#ifndef _M_CEE_PURE
    {
        test_wregex_locale eq_a_regex(&g_regexTester, L"^[[=a=]]*b$", "en-US");
        eq_a_regex.should_search_match(L"A\u00c0ab", L"A\u00c0ab"); // U+00C0 LATIN CAPITAL LETTER A WITH GRAVE
        eq_a_regex.should_search_fail(L"Ab\u00c0ab"); // U+00C0 LATIN CAPITAL LETTER A WITH GRAVE
        eq_a_regex.should_search_match(L"A\u00e0ab", L"A\u00e0ab"); // U+00E0 LATIN SMALL LETTER A WITH GRAVE
        eq_a_regex.should_search_match(L"A\u00c1ab", L"A\u00c1ab"); // U+00C1 LATIN CAPITAL LETTER A WITH ACUTE
        eq_a_regex.should_search_match(L"A\u00e1ab", L"A\u00e1ab"); // U+00E1 LATIN SMALL LETTER A WITH ACUTE
        eq_a_regex.should_search_match(L"A\u00c2ab", L"A\u00c2ab"); // U+00C2 LATIN CAPITAL LETTER A WITH CIRCUMFLEX
        eq_a_regex.should_search_match(L"A\u00e2ab", L"A\u00e2ab"); // U+00E2 LATIN SMALL LETTER A WITH CIRCUMFLEX
        eq_a_regex.should_search_match(L"A\u00c3ab", L"A\u00c3ab"); // U+00C3 LATIN CAPITAL LETTER A WITH TILDE
        eq_a_regex.should_search_match(L"A\u00e3ab", L"A\u00e3ab"); // U+00E3 LATIN SMALL LETTER A WITH TILDE
        eq_a_regex.should_search_match(L"A\u00c4ab", L"A\u00c4ab"); // U+00C4 LATIN CAPITAL LETTER A WITH DIAERESIS
        eq_a_regex.should_search_match(L"A\u00e4ab", L"A\u00e4ab"); // U+00E4 LATIN SMALL LETTER A WITH DIAERESIS
        eq_a_regex.should_search_match(L"A\u00c5ab", L"A\u00c5ab"); // U+00C5 LATIN CAPITAL LETTER A WITH RING ABOVE
        eq_a_regex.should_search_match(L"A\u00e5ab", L"A\u00e5ab"); // U+00E5 LATIN SMALL LETTER A WITH RING ABOVE
    }
    {
        test_wregex_locale eq_e_regex(&g_regexTester, L"^[[=e=]]*b$", "en-US");
        eq_e_regex.should_search_match(L"e\u00c8Eb", L"e\u00c8Eb"); // U+00C8 LATIN CAPITAL LETTER E WITH GRAVE
        eq_e_regex.should_search_fail(L"eb\u00c8Eb"); // U+00C8 LATIN CAPITAL LETTER E WITH GRAVE
        eq_e_regex.should_search_match(L"e\u00e8Eb", L"e\u00e8Eb"); // U+00E8 LATIN SMALL LETTER E WITH GRAVE
        eq_e_regex.should_search_match(L"e\u00c9Eb", L"e\u00c9Eb"); // U+00C9 LATIN CAPITAL LETTER E WITH ACUTE
        eq_e_regex.should_search_match(L"e\u00e9Eb", L"e\u00e9Eb"); // U+00E9 LATIN SMALL LETTER E WITH ACUTE
        eq_e_regex.should_search_match(L"e\u00caEb", L"e\u00caEb"); // U+00CA LATIN CAPITAL LETTER E WITH CIRCUMFLEX
        eq_e_regex.should_search_match(L"e\u00eaEb", L"e\u00eaEb"); // U+00EA LATIN SMALL LETTER E WITH CIRCUMFLEX
        eq_e_regex.should_search_match(L"e\u00cbEb", L"e\u00cbEb"); // U+00CB LATIN CAPITAL LETTER E WITH DIAERESIS
        eq_e_regex.should_search_match(L"e\u00ebEb", L"e\u00ebEb"); // U+00EB LATIN SMALL LETTER E WITH DIAERESIS
        eq_e_regex.should_search_fail(L"e\u00ccEb"); // U+00CC LATIN CAPITAL LETTER I WITH GRAVE
        eq_e_regex.should_search_fail(L"e\u00ecEb"); // U+00EC LATIN SMALL LETTER I WITH GRAVE
    }
#endif // ^^^ !defined(_M_CEE_PURE) ^^^
}

void test_gh_5437_ECMAScript_or_collate(syntax_option_type ECMAScript_or_collate) {
    {
        test_wregex char_range(&g_regexTester, L"^[\u0001-\u0200]$", ECMAScript_or_collate);
        for (wchar_t ch = L'\u0001'; ch <= L'\u0200'; ++ch) {
            char_range.should_search_match(wstring(1, ch), wstring(1, ch));
        }
        char_range.should_search_fail(wstring(1, L'\u0000'));
        char_range.should_search_fail(wstring(1, L'\u0201'));
    }
    {
        test_wregex char_range(&g_regexTester, L"^[\u00FE-\u0100]$", ECMAScript_or_collate);
        for (wchar_t ch = L'\u00FE'; ch <= L'\u0100'; ++ch) {
            char_range.should_search_match(wstring(1, ch), wstring(1, ch));
        }
        char_range.should_search_fail(wstring(1, L'\u00FD'));
        char_range.should_search_fail(wstring(1, L'\u0101'));
    }
}

void test_gh_5437() {
    // GH-5437: make `wregex` handle small character ranges containing U+00FF and U+0100 correctly
    test_gh_5437_ECMAScript_or_collate(ECMAScript);
#ifndef SKIP_COLLATE_TESTS
    test_gh_5437_ECMAScript_or_collate(regex_constants::collate);
#endif // !defined(SKIP_COLLATE_TESTS)
}

int main() {
    test_collating_ranges_german();
    test_gh_994();
    test_gh_5435();
    test_gh_5437();

    return g_regexTester.result();
}
