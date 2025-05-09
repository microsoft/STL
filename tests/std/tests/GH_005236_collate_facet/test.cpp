// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <locale>
#include <string>

// skip collate::transform() tests when linking to the DLL in case of
// _ITERATOR_DEBUG_LEVEL mismatch between code and linked DLL
#ifdef _DEBUG
#define DEFAULT_IDL_SETTING 2
#else
#define DEFAULT_IDL_SETTING 0
#endif

#if defined(_DLL) && _ITERATOR_DEBUG_LEVEL != DEFAULT_IDL_SETTING
#define SKIP_COLLATE_TRANSFORM_TESTS
#endif // defined(_DLL) && _ITERATOR_DEBUG_LEVEL != DEFAULT_IDL_SETTING

using namespace std;

// GH-5210 "std::collate<_Elem>::do_transform() should behave appropriately when _LStrxfrm() fails"
void test_gh_5210() {
#ifndef SKIP_COLLATE_TRANSFORM_TESTS
    {
        locale utf8_locale("en-US.UTF-8");
        const auto& coll = use_facet<collate<char>>(utf8_locale);

        const string test = "this i\xA0s a very brok\x80n utf-8\xC8string";
        assert(coll.transform(test.data(), test.data() + test.size()) == string{});
    }

    {
        locale en_us_locale("en-US");
        const auto& coll = use_facet<collate<char>>(en_us_locale);

        {
            const string test1 = "fluffy kittens";
            const string test2 = "fluffy Kittens";
            assert(coll.transform(test1.data(), test1.data() + test1.size())
                   < coll.transform(test2.data(), test2.data() + test2.size()));
        }
        {
            const string test1 = "Riddle";
            const string test2 = "middle";
            assert(coll.transform(test1.data(), test1.data() + test1.size())
                   > coll.transform(test2.data(), test2.data() + test2.size()));
        }
    }

    {
        locale en_us_locale("en-US");
        const auto& coll = use_facet<collate<wchar_t>>(en_us_locale);

        {
            const wstring test1 = L"fluffy kittens";
            const wstring test2 = L"fluffy Kittens";
            assert(coll.transform(test1.data(), test1.data() + test1.size())
                   < coll.transform(test2.data(), test2.data() + test2.size()));
        }
        {
            const wstring test1 = L"Riddle";
            const wstring test2 = L"middle";
            assert(coll.transform(test1.data(), test1.data() + test1.size())
                   > coll.transform(test2.data(), test2.data() + test2.size()));
        }
    }

    {
        locale de_DE_phone_locale("de-DE_phoneb");
        const auto& coll = use_facet<collate<wchar_t>>(de_DE_phone_locale);

        {
            const wstring test1 = L"Strasse";
            const wstring test2 = L"Stra\u00DFe"; // U+00DF LATIN SMALL LETTER SHARP S

            // sharp s collates like "ss"
            assert(coll.transform(test1.data(), test1.data() + test1.size())
                   == coll.transform(test2.data(), test2.data() + test2.size()));
        }
        {
            const wstring test1 = L"Kachel";
            const wstring test2 = L"Kaetzchen";
            const wstring test3 = L"K\u00E4tzchen"; // U+00E4 LATIN SMALL LETTER A WITH DIAERESIS
            const wstring test4 = L"Kater";

            // umlaut a collates like "ae"
            assert(coll.transform(test1.data(), test1.data() + test1.size())
                   < coll.transform(test2.data(), test2.data() + test2.size()));
            assert(coll.transform(test2.data(), test2.data() + test2.size())
                   == coll.transform(test3.data(), test3.data() + test3.size()));
            assert(coll.transform(test3.data(), test3.data() + test3.size())
                   < coll.transform(test4.data(), test4.data() + test4.size()));
        }
    }
#endif // !defined(SKIP_COLLATE_TRANSFORM_TESTS)
}

void test_gh_5212_compare_hash(const collate<wchar_t>& coll, const wstring& string1, const wstring& string2) {
    assert(coll.hash(string1.data(), string1.data() + string1.size())
           == coll.hash(string2.data(), string2.data() + string2.size()));
}

// GH-5212: std::collate_byname<_Elem>::hash() yields different hashes for strings that collate the same
void test_gh_5212() {
    const locale loc("de-DE_phoneb");
    const auto& coll = use_facet<collate<wchar_t>>(loc);

    // sharp s collates like "ss"
    test_gh_5212_compare_hash(coll, L"Strasse", L"Stra\u00DFe"); // U+00DF LATIN SMALL LETTER SHARP S
    // umlaut a collates like "ae"
    test_gh_5212_compare_hash(coll, L"Kaetzchen", L"K\u00E4tzchen"); // U+00E4 LATIN SMALL LETTER A WITH DIAERESIS
    // umlaut A collates like "AE"
    test_gh_5212_compare_hash(coll, L"AErmel", L"\u00C4rmel"); // U+00C4 LATIN CAPITAL LETTER A WITH DIAERESIS
}

// GH-5236 "std::collate<wchar_t> does not respect collation order when compiled with /MD(d) /Zc:wchar_t-"
void test_gh_5236() {
    const wchar_t Ue = L'\u00DC'; // U+00DC LATIN CAPITAL LETTER U WITH DIARESIS
    const wchar_t U  = L'U';
    const wchar_t V  = L'V';

    // German phonebook order: "U+00DC" is sorted between U and V in collation order
    const locale loc("de-DE_phoneb");
    const auto& coll = use_facet<collate<wchar_t>>(loc);

    assert(coll.compare(&U, &U + 1, &Ue, &Ue + 1) < 0);
    assert(coll.compare(&V, &V + 1, &Ue, &Ue + 1) > 0);

#ifndef SKIP_COLLATE_TRANSFORM_TESTS
    assert(coll.transform(&U, &U + 1) < coll.transform(&Ue, &Ue + 1));
    assert(coll.transform(&V, &V + 1) > coll.transform(&Ue, &Ue + 1));
#endif // !defined(SKIP_COLLATE_TRANSFORM_TESTS)
}

int main() {
    test_gh_5210();
    test_gh_5212();
    test_gh_5236();
}
