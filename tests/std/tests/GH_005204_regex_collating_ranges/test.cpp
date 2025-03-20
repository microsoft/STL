// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstdio>
#include <cstdlib>
#include <locale>
#include <regex>
#include <string>

#include <test_regex_support.hpp>

// skip collation tests when linking to the DLL in case of
// * undefined _NATIVE_WCHAR_T_DEFINED due to GH-5236
// * _ITERATOR_DEBUG_LEVEL mismatch between code and linked DLL
#ifdef _DEBUG
#define DEFAULT_IDL_SETTING 2
#else
#define DEFAULT_IDL_SETTING 0
#endif

#ifdef _DLL
#ifndef _NATIVE_WCHAR_T_DEFINED // TRANSITION, GH-212 or GH-5236
#define SKIP_COLLATE_TESTS
#elif _ITERATOR_DEBUG_LEVEL != DEFAULT_IDL_SETTING
#define SKIP_COLLATE_TESTS
#endif // !defined(_NATIVE_WCHAR_T_DEFINED) || _ITERATOR_DEBUG_LEVEL != DEFAULT_IDL_SETTING
#endif // defined(_DLL)


using namespace std;
using namespace std::regex_constants;

class test_wregex_locale {
    regex_fixture* const fixture;
    const wstring pattern;
    const syntax_option_type syntax;
    const string locname;
    wregex r;

public:
    test_wregex_locale(
        regex_fixture* fixture, const wstring& pattern, const string& locname, syntax_option_type syntax = ECMAScript)
        : fixture(fixture), pattern(pattern), syntax(syntax), locname(locname), r() {
        r.imbue(locale(locname));
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
            printf(" for locale \"%s\": regex_error: \"%s\"\n", locname.c_str(), e.what());
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
            printf(" for locale \"%s\": regex_error: \"%s\"\n", locname.c_str(), e.what());
            fixture->fail_regex();
        }
    }
};

regex_fixture g_regexTester;

void regex_with_locale_should_throw(const wstring& pattern, const string& locname, error_type expected,
    syntax_option_type syntax = regex_constants::collate) {
    wregex r;
    r.imbue(locale(locname));
    try {
        r.assign(pattern, regex_constants::collate);
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
        }
    }
}

void test_collating_ranges_german() {

    // special characters in German (umlauts and sharp s)
    const wchar_t* special_characters[] = {
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
                for (const wchar_t* s : special_characters) {
                    nocollate_nolocale.should_search_fail(s);
                }
            }

            {
                test_wregex_locale nocollate_locale(&g_regexTester, pattern, "de-DE");
                for (const wchar_t* s : special_characters) {
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

    const wchar_t* special_characters_without_ae[] = {
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
        for (const wchar_t* s : special_characters_without_ae) {
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
        for (const wchar_t* s : special_characters_without_ae) {
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
        for (const wchar_t* s : special_characters_without_ae) {
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
        for (const wchar_t* s : special_characters_without_ae) {
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
        for (const wchar_t* s : special_characters_without_ae) {
            collate_a_to_b_regex.should_search_fail(s);
        }
    }

    const wchar_t* special_characters_without_sharp_s[] = {
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
        for (const wchar_t* s : special_characters_without_sharp_s) {
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
        for (const wchar_t* s : special_characters_without_sharp_s) {
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
        for (const wchar_t* s : special_characters_without_sharp_s) {
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
        for (const wchar_t* s : special_characters_without_sharp_s) {
            collate_s_to_t_regex.should_search_fail(s);
        }
    }

    {
        test_wregex_locale collate_A_to_z_regex(&g_regexTester, L"[A-z]", "de-DE", regex_constants::collate);
        collate_A_to_z_regex.should_search_fail(L"a");
        for (wchar_t ascii_upper = L'A'; ascii_upper < 'Z'; ++ascii_upper) {
            collate_A_to_z_regex.should_search_match(wstring(1, ascii_upper), wstring(1, ascii_upper));
        }
        for (wchar_t ascii_lower = L'b'; ascii_lower <= 'z'; ++ascii_lower) {
            collate_A_to_z_regex.should_search_match(wstring(1, ascii_lower), wstring(1, ascii_lower));
        }
        for (const wchar_t* s : special_characters) {
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

int main() {
    test_collating_ranges_german();

    return g_regexTester.result();
}
