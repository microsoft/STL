// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _SILENCE_CXX20_CODECVT_FACETS_DEPRECATION_WARNING
#include <algorithm>
#include <cassert>
#include <clocale>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <locale>
#include <string>

using namespace std;

struct encoding_test_char {
    const char* narrow;
    const wchar_t* wide;
};

// clang-format off
const encoding_test_char utf_test_cases[] = {
    {"\x61",         L"\u0061"}, // LATIN SMALL LETTER A
    {"\x7F",         L"\u007F"}, // DELETE, highest single encoding unit UTF-8
    {"\xC2\x80",     L"\u0080"}, // unspecified control character, lowest 2 encoding unit UTF-8
    {"\xC2\xA9",     L"\u00A9"}, // COPYRIGHT SIGN
    {"\xDF\xBF",     L"\u07FF"}, // unencoded from the nko block, highest 2 encoding unit UTF-8
    {"\xE0\xA0\x80", L"\u0800"}, // SAMARITAN LETTER ALAF, lowest 3 encoding unit UTF-8
    {"\xE2\x80\xB1", L"\u2031"}, // PER TEN THOUSAND SIGN
    {"\xED\x9F\xBF", L"\uD7FF"},
        // unencoded from the Hangul Jamo Extended-B block, last character before UTF-16 illegal region
    {"\xED\xA0\x80", nullptr}, // U+D800 beginning of surrogate range
    {nullptr,        L"\xD800"},
    {"\xED\xBF\xBF", nullptr}, // U+DFFF end of surrogate range
    {nullptr,        L"\xDFFF"},
    {"\xEE\x80\x80", L"\uE000"}, // unencoded from the private use area, first codepoint after surrogate range
    {"\xEF\xBF\xBD", L"\uFFFD"}, // REPLACEMENT CHARACTER
    // {"\xEF\xBF\xBF", L"\uFFFF"}, // TRANSITION, VSO-653129
        // NOT A CHARACTER, highest 3 encoding unit UTF-8 and highest non-surrogate UTF-16
    // ========================================
    // The following do not work today due to basic_filebuf's single wchar_t assumption, but should.
    // TRANSITION, VSO-653059
    {"\xF0\x90\x80\x80", nullptr}, // L"\U00010000"
        // U+10000 LINEAR B SYLLABLE B008 A, lowest 4 encoding unit UTF-8 and UTF-16 with surrogate pair
    {"\xF0\x90\x8D\x88", nullptr}, // L"\U00010348" HWAIR
    // ========================================
    {"\xC2\x61", nullptr}, // Wrong number of trailing bytes
    {"\xE0\xA0\x61", nullptr},
};
// clang-format on

void assert_empty_file(const wchar_t* const fileName) {
    ifstream emptyTest(fileName, ios::in | ios::binary);
    assert(emptyTest.seekg(0, ios::end).tellg() == 0);
}

void assert_empty_mbstate(const mbstate_t& tested) {
    mbstate_t valueInit{};
    assert(memcmp(&valueInit, &tested, sizeof(mbstate_t)) == 0);
}

void test_fstream(locale& loc) noexcept {
    const wchar_t* const testFile = L"VSO_644691_test.txt";
    string str;
    wstring wStr;
    wfstream wideStream;
    wideStream.imbue(loc);
    fstream narrowStream;
    for (const auto& testCase : utf_test_cases) {
        if (testCase.wide) {
            wideStream.open(testFile, ios::trunc | ios::out | ios::binary);
            if (testCase.narrow) {
                assert(static_cast<bool>(wideStream << testCase.wide));
                wideStream.close();
                narrowStream.open(testFile, ios::in | ios::binary);
                assert(static_cast<bool>(narrowStream >> str));
                narrowStream.close();
                assert(str == testCase.narrow);
                wideStream.open(testFile, ios::in | ios::binary);
                assert(static_cast<bool>(wideStream >> wStr));
                wideStream.close();
                assert(wStr == wStr);
            } else {
                assert(!(wideStream << testCase.wide));
                wideStream.close();
                assert_empty_file(testFile);
            }
        } else {
            assert(testCase.narrow != nullptr);
            narrowStream.open(testFile, ios::trunc | ios::out | ios::binary);
            assert(static_cast<bool>(narrowStream << testCase.narrow));
            narrowStream.close();
            wideStream.open(testFile, ios::in | ios::binary);
            wStr.clear();
            assert(!(wideStream >> wStr));
            assert(wStr.empty());
            wideStream.close();
        }
    }
}

void assert_do_out_success(const codecvt<wchar_t, char, mbstate_t>& f, mbstate_t& state, const wchar_t* const wideFirst,
    const wchar_t* const wideLast, const char* const narrowFirst, const char* const narrowLast) {
    const wchar_t* wideNext = wideFirst;
    string narrowOut(static_cast<size_t>(narrowLast - narrowFirst + 1), '\0');
    char* const outFirst = &narrowOut[0];
    char* const outLast  = outFirst + narrowOut.size();
    char* outNext        = outFirst;
    assert(f.out(state, wideFirst, wideLast, wideNext, outFirst, outLast, outNext) == codecvt_base::ok);
    assert_empty_mbstate(state);
    assert(wideNext == wideLast);
    assert(outLast - outNext == 1);
    assert(equal(outFirst, outNext, narrowFirst, narrowLast));
}

void assert_do_out_failure(const codecvt<wchar_t, char, mbstate_t>& f, mbstate_t& state, const wchar_t* const wideFirst,
    const wchar_t* const wideLast, const wchar_t* const widePrefix, const string& narrowPrefix) {
    const wchar_t* wideNext = wideFirst;
    string narrowOut;
    narrowOut.resize(narrowPrefix.size() + 16); // 16 ought to be enough for anybody
    char* const outFirst = &narrowOut[0];
    char* const outLast  = outFirst + narrowOut.size();
    char* outNext        = outFirst;
    assert(f.out(state, wideFirst, wideLast, wideNext, outFirst, outLast, outNext) == codecvt_base::error);
    assert_empty_mbstate(state);
    assert(widePrefix == wideNext);
    assert(equal(outFirst, outNext, narrowPrefix.begin(), narrowPrefix.end()));
}

void assert_do_in_success(const codecvt<wchar_t, char, mbstate_t>& f, mbstate_t& state, const char* const narrowFirst,
    const char* const narrowLast, const wchar_t* const wideFirst, const wchar_t* const wideLast) {
    const char* narrowNext = narrowFirst;
    wstring wideIn(static_cast<size_t>(wideLast - wideFirst + 1), L'\0');
    wchar_t* const inFirst = &wideIn[0];
    wchar_t* const inLast  = inFirst + wideIn.size();
    wchar_t* inNext        = inFirst;
    assert(f.in(state, narrowFirst, narrowLast, narrowNext, inFirst, inLast, inNext) == codecvt_base::ok);
    assert_empty_mbstate(state);
    assert(narrowNext == narrowLast);
    assert(inLast - inNext == 1);
    assert(equal(inFirst, inNext, wideFirst, wideLast));

    fill(wideIn.begin(), wideIn.end(), L'\0');
    inNext                = inFirst;
    narrowNext            = narrowFirst;
    const char* lastOk[2] = {narrowFirst, narrowFirst};
    for (const char* narrowLimit = narrowFirst; narrowLimit < narrowLast; ++narrowLimit) {
        const auto beforeInNext     = inNext;
        const auto beforeNarrowNext = narrowNext;
        const auto result = f.in(state, beforeNarrowNext, narrowLimit, narrowNext, beforeInNext, inLast, inNext);
        assert_empty_mbstate(state);
        switch (result) {
        case codecvt_base::ok:
            lastOk[0] = exchange(lastOk[1], narrowNext);
            if (beforeNarrowNext != narrowLimit) {
                // 1 character was converted, so the wide characters should advance by 1 and the
                // narrow range should be consumed
                assert(beforeInNext + 1 == inNext);
                assert(narrowNext == narrowLimit);
                // everything converted so far should agree with what we expect the overall wide
                // string to be
                assert(equal(inFirst, inNext, wideFirst));
                if (inNext != inFirst) { // also test partial because the wide buffer is too small
                    wchar_t* const partialLast = inNext - 1;
                    wchar_t* partialNext       = inFirst;
                    narrowNext                 = narrowFirst;
                    assert(f.in(state, narrowFirst, narrowLimit, narrowNext, inFirst, partialLast, partialNext)
                           == codecvt_base::partial);
                    assert(narrowNext == beforeNarrowNext); // we should stop 1 wide character before
                    narrowNext = narrowLimit;
                    assert(partialNext == partialLast); // entire wide buffer consumed
                    assert(equal(inFirst, partialLast, wideFirst));
                }
            }
            break;
        case codecvt_base::partial:
            // no extra characters could be converted, so the iterator positions should be unchanged
            assert(beforeInNext == inNext);
            assert(beforeNarrowNext == narrowNext);
            // also test partial because partial character *and* wide buffer too small:
            if (inFirst != inNext) {
                wchar_t* const partialLast = inNext - 1;
                wchar_t* partialNext       = inFirst;
                narrowNext                 = narrowFirst;
                assert(f.in(state, narrowFirst, narrowLimit, narrowNext, inFirst, partialLast, partialNext)
                       == codecvt_base::partial);
                assert(lastOk[0] == narrowNext);
                narrowNext = beforeNarrowNext;
                assert(partialNext == partialLast);
                assert(equal(inFirst, partialLast, wideFirst));
            }
            break;
        default:
            abort();
        }
    }
}

void assert_do_in_failure(const codecvt<wchar_t, char, mbstate_t>& f, mbstate_t& state, const char* const narrowFirst,
    const char* const narrowLast, const char* const narrowPrefix, const wstring& widePrefix) {
    const char* narrowNext = narrowFirst;
    wstring wideIn;
    wideIn.resize(widePrefix.size() + 16); // 16 ought to be enough for anybody
    wchar_t* const inFirst = &wideIn[0];
    wchar_t* const inLast  = inFirst + wideIn.size();
    wchar_t* inNext        = inFirst;
    assert(f.in(state, narrowFirst, narrowLast, narrowNext, inFirst, inLast, inNext) == codecvt_base::error);
    assert_empty_mbstate(state);
    assert(narrowPrefix == narrowNext);
    assert(equal(inFirst, inNext, widePrefix.begin(), widePrefix.end()));
}

void test_codecvt_encoding(
    const codecvt<wchar_t, char, mbstate_t>& f, const string& narrowPrefix, const wstring& widePrefix) {
    mbstate_t state{};
    for (const auto& testCase : utf_test_cases) {
        if (testCase.narrow) {
            string narrow = narrowPrefix;
            narrow.append(testCase.narrow);
            const char* const narrowFirst = narrow.data();
            const char* const narrowLast  = narrowFirst + narrow.size();
            if (testCase.wide) {
                wstring wide = widePrefix;
                wide.append(testCase.wide);
                const wchar_t* const wideFirst = wide.data();
                const wchar_t* const wideLast  = wideFirst + wide.size();
                assert_do_out_success(f, state, wideFirst, wideLast, narrowFirst, narrowLast);
                assert_do_in_success(f, state, narrowFirst, narrowLast, wideFirst, wideLast);
                if (widePrefix.size() <= 2 && narrowPrefix.size() <= 8) {
                    test_codecvt_encoding(f, narrow, wide);
                }
            } else {
                assert_do_in_failure(f, state, narrowFirst, narrowLast, narrowFirst + narrowPrefix.size(), widePrefix);
            }
        } else {
            assert(testCase.wide != nullptr);
            _Analysis_assume_(testCase.wide);
            wstring wide = widePrefix;
            wide.append(testCase.wide);
            const wchar_t* const wideFirst = wide.data();
            const wchar_t* const wideLast  = wideFirst + wide.size();
            assert_do_out_failure(f, state, wideFirst, wideLast, wideFirst + widePrefix.size(), narrowPrefix);
        }
    }
}

void test_codecvt_encoding(const codecvt<wchar_t, char, mbstate_t>& f) {
    test_codecvt_encoding(f, string{}, wstring{});
    test_codecvt_encoding(f, "aaa", L"aaa");
}

int main() {
    try {
        locale loc("en-US.UTF-8");
        test_fstream(loc);
        test_codecvt_encoding(use_facet<codecvt<wchar_t, char, mbstate_t>>(loc));
    } catch (const exception& ex) {
        puts("Could not test UTF-8 on this platform, newer Windows 10 required.");
        puts(ex.what());
        assert(false);
    }
}
