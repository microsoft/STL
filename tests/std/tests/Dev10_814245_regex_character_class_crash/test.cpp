// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <regex>
#include <string>

#include <test_regex_support.hpp>

using namespace std;
using namespace std::regex_constants;

regex_fixture g_regexTester;

string escape(const char c, const bool first) {
    switch (c) {
    case '^':
        return first ? R"(\^)" : "^";
    case '\\':
        return R"(\\)";
    case ']':
        return R"(\])";
    default:
        return string(1, c);
    }
}

const size_t characterCount = 256;
string g_firstCharacters[characterCount];
string g_secondCharacters[characterCount];
string g_inputs[characterCount];

void init_character_strings() {
    for (size_t idx = 0; idx < characterCount; ++idx) {
        g_firstCharacters[idx]  = "[" + escape(static_cast<char>(idx), true);
        g_secondCharacters[idx] = escape(static_cast<char>(idx), false) + "]";
        g_inputs[idx]           = string(1, static_cast<char>(idx));
    }
}

// Tests a character class pattern.
// `generator` is a functor taking a size_t lower and upper, and generates a
// character class pattern for that range.
template <typename Functor>
void test_ranges(const Functor& generator) {
    for (size_t upper = 0; upper < characterCount; ++upper) {
        for (size_t lower = 0; lower <= upper; ++lower) {
            const string pattern(generator(lower, upper));
            const regex r(pattern);
            for (size_t c = 0; c < lower; ++c) {
                g_regexTester.should_not_match(g_inputs[c], pattern, r);
            }

            for (size_t c = lower; c <= upper; ++c) {
                g_regexTester.should_match(g_inputs[c], pattern, r);
            }

            for (size_t c = upper + 1; c < characterCount; ++c) {
                g_regexTester.should_not_match(g_inputs[c], pattern, r);
            }
        }

        for (size_t lower = upper + 1; lower < characterCount; ++lower) {
            g_regexTester.should_throw(generator(lower, upper), error_range);
        }
    }
}

void test_dev10_814245_character_class_should_not_crash() {
    test_ranges([](size_t lower, size_t upper) { return g_firstCharacters[lower] + "-" + g_secondCharacters[upper]; });
}

void test_dev10_723057_normal_to_high_bit_ranges_should_not_throw_error_range() {
    test_ranges([](size_t lower, size_t upper) {
        char buf[12] = {};
        sprintf_s(buf, R"([\x%02zX-\x%02zX])", lower, upper);
        return string(buf);
    });
}

void test_VSO_153556_singular_classes_can_have_high_bit_set() {
    for (size_t cReg = 0; cReg < characterCount; ++cReg) {
        string pattern(g_firstCharacters[cReg]);
        pattern.push_back(']');
        const regex r(pattern);
        for (size_t cInput = 0; cInput < cReg; ++cInput) {
            g_regexTester.should_not_match(g_inputs[cInput], pattern, r);
        }

        g_regexTester.should_match(g_inputs[cReg], pattern, r);

        for (size_t cInput = cReg + 1; cInput < characterCount; ++cInput) {
            g_regexTester.should_not_match(g_inputs[cInput], pattern, r);
        }
    }
}

// Also test VSO-984741 "Splitting a string with a regex returns seemingly wrong results"
void test_VSO_984741_splitting_a_string_with_a_regex() {
    // We were skipping a non-match character after a zero-length match to avoid repeat matches, resulting in incorrect
    // behavior when tokenizing with a regex to match the delimiters between tokens.

    const string text = "cute fluffy\t zombie-kittens";
    const regex boundary{R"(\s*\b\s*)"};
    static constexpr const char* tokens[] = {"", "cute", "", "fluffy", "", "zombie", "-", "kittens"};

    const sregex_token_iterator i{text.begin(), text.end(), boundary, -1};
    assert(equal(i, sregex_token_iterator{}, begin(tokens), end(tokens)));
}

int main() {
    init_character_strings();
    test_dev10_814245_character_class_should_not_crash();
    test_dev10_723057_normal_to_high_bit_ranges_should_not_throw_error_range();
    test_VSO_153556_singular_classes_can_have_high_bit_set();
    test_VSO_984741_splitting_a_string_with_a_regex();

    return g_regexTester.result();
}
