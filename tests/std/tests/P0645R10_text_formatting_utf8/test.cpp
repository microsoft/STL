// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <clocale>
#include <format>
#include <string_view>

#include "test_format_support.hpp"

using namespace std;

void test_multibyte_format_strings() {
    {
        // Filling with footballs ("\U0001f3c8" is U+1F3C8 AMERICAN FOOTBALL)
        assert(format("{:\U0001f3c8>4}"sv, 42) == "\U0001f3c8\U0001f3c8\x34\x32");

        assert(format("{:\U0001f3c8<4.2}", "1") == "\x31\U0001f3c8\U0001f3c8\U0001f3c8"sv);
        assert(format("{:\U0001f3c8^4.2}", "1") == "\U0001f3c8\x31\U0001f3c8\U0001f3c8"sv);
        assert(format("{:\U0001f3c8>4.2}", "1") == "\U0001f3c8\U0001f3c8\U0001f3c8\x31"sv);
    }

    {
        try {
            // Bad fill character encoding: missing lead byte before \x9f
            int arg = 42;
            (void) vformat("{:\x9f\x8f\x88<10}"sv, make_format_args(arg));
            assert(false);
        } catch (const format_error&) {
        }
    }
}

void test_parse_align() {
    auto parse_align_fn = _Parse_align<char, testing_callbacks<char>>;

    {
        test_parse_helper(parse_align_fn, "\U0001f3c8<X"sv, false, 5,
            {.expected_alignment = _Fmt_align::_Left, .expected_fill = "\U0001f3c8"sv});
        test_parse_helper(parse_align_fn, "\U0001f3c8>X"sv, false, 5,
            {.expected_alignment = _Fmt_align::_Right, .expected_fill = "\U0001f3c8"sv});
        test_parse_helper(parse_align_fn, "\U0001f3c8^X"sv, false, 5,
            {.expected_alignment = _Fmt_align::_Center, .expected_fill = "\U0001f3c8"sv});
    }

    {
        test_parse_helper(parse_align_fn, "\xC0\x80<X"sv, true);
        test_parse_helper(parse_align_fn, "\xF5\x80\x80\x80<X"sv, true);
        test_parse_helper(parse_align_fn, "\xE0\x80\x80<X"sv, true);
        test_parse_helper(parse_align_fn, "\xED\xA0\x80<X"sv, true);
        test_parse_helper(parse_align_fn, "\xF0\x80\x80\x80<X"sv, true);
        test_parse_helper(parse_align_fn, "\xF4\x90\x80\x80<X"sv, true);
        test_parse_helper(parse_align_fn, "\xE1\x80<X"sv, true);
        test_parse_helper(parse_align_fn, "\xE1\x7F\x80<X"sv, true);
        test_parse_helper(parse_align_fn, "\xE1\xC0\x80<X"sv, true);
        test_parse_helper(parse_align_fn, "\xE1\x80\x7F<X"sv, true);
        test_parse_helper(parse_align_fn, "\xE1\x80\xC0<X"sv, true);
    }
}

template <class CharT>
void test_width_estimation() {
    // Format strings of known width with a trailing delimiter using a precision large enough to
    // include all but the delimiter to validate the width estimation code.
    struct test_case {
        const CharT* str;
        int width;
    };
    constexpr test_case test_cases[] = {
        {TYPED_LITERAL(CharT, "\x58"), 1},
        {TYPED_LITERAL(CharT, "x\x58"), 2},

        // Test N4950 [format.string.std]/13
        {TYPED_LITERAL(CharT, "\u10ff\x58"), 2},
        {TYPED_LITERAL(CharT, "\u1100\x58"), 3},
        {TYPED_LITERAL(CharT, "\u115f\x58"), 3},
        {TYPED_LITERAL(CharT, "\u1160\x58"), 2},
        {TYPED_LITERAL(CharT, "\u2328\x58"), 2},
        {TYPED_LITERAL(CharT, "\u2329\x58"), 3},
        {TYPED_LITERAL(CharT, "\u232a\x58"), 3},
        {TYPED_LITERAL(CharT, "\u232b\x58"), 2},
        {TYPED_LITERAL(CharT, "\u2e7f\x58"), 2},
        {TYPED_LITERAL(CharT, "\u2e80\x58"), 3},
        {TYPED_LITERAL(CharT, "\u303e\x58"), 3},
        {TYPED_LITERAL(CharT, "\u303f\x58"), 2},
        {TYPED_LITERAL(CharT, "\ua4d0\x58"), 2},
        {TYPED_LITERAL(CharT, "\uabff\x58"), 2},
        {TYPED_LITERAL(CharT, "\uac00\x58"), 3},
        {TYPED_LITERAL(CharT, "\ud7a3\x58"), 3},
        {TYPED_LITERAL(CharT, "\ud7a4\x58"), 2},
        {TYPED_LITERAL(CharT, "\ud7ff\x58"), 2},

        // Skip over the surrogate pair range (\ud800-\udfff)

        {TYPED_LITERAL(CharT, "\ue000\x58"), 2},
        {TYPED_LITERAL(CharT, "\uf8ff\x58"), 2},
        {TYPED_LITERAL(CharT, "\uf900\x58"), 3},
        {TYPED_LITERAL(CharT, "\ufaff\x58"), 3},
        {TYPED_LITERAL(CharT, "\ufb00\x58"), 2},
        {TYPED_LITERAL(CharT, "\ufe0f\x58"), 2},
        {TYPED_LITERAL(CharT, "\ufe10\x58"), 3},
        {TYPED_LITERAL(CharT, "\ufe19\x58"), 3},
        {TYPED_LITERAL(CharT, "\ufe1a\x58"), 2},
        {TYPED_LITERAL(CharT, "\ufe2f\x58"), 2},
        {TYPED_LITERAL(CharT, "\ufe30\x58"), 3},
        {TYPED_LITERAL(CharT, "\ufe70\x58"), 2},
        {TYPED_LITERAL(CharT, "\ufeff\x58"), 2},
        {TYPED_LITERAL(CharT, "\uff60\x58"), 3},
        {TYPED_LITERAL(CharT, "\uff61\x58"), 2},
        {TYPED_LITERAL(CharT, "\uffdf\x58"), 2},
        {TYPED_LITERAL(CharT, "\uffe0\x58"), 3},
        {TYPED_LITERAL(CharT, "\uffe6\x58"), 3},
        {TYPED_LITERAL(CharT, "\uffe7\x58"), 2},
        {TYPED_LITERAL(CharT, "\U0001f2ff\x58"), 2},
        {TYPED_LITERAL(CharT, "\U0001f300\x58"), 3},
        {TYPED_LITERAL(CharT, "\U0001f64f\x58"), 3},
        {TYPED_LITERAL(CharT, "\U0001f650\x58"), 2},
        {TYPED_LITERAL(CharT, "\U0001f8ff\x58"), 2},
        {TYPED_LITERAL(CharT, "\U0001f900\x58"), 3},
        {TYPED_LITERAL(CharT, "\U0001f9ff\x58"), 3},
        {TYPED_LITERAL(CharT, "\U0001fa00\x58"), 2},
        {TYPED_LITERAL(CharT, "\U0001ffff\x58"), 2},
        {TYPED_LITERAL(CharT, "\U00020000\x58"), 3},
        {TYPED_LITERAL(CharT, "\U0002fffd\x58"), 3},
        {TYPED_LITERAL(CharT, "\U0002fffe\x58"), 2},
        {TYPED_LITERAL(CharT, "\U0002ffff\x58"), 2},
        {TYPED_LITERAL(CharT, "\U00030000\x58"), 3},
        {TYPED_LITERAL(CharT, "\U0003fffd\x58"), 3},
        {TYPED_LITERAL(CharT, "\U0003fffe\x58"), 2},
        {TYPED_LITERAL(CharT, "\U0010ffff\x58"), 2},

        // Pick "short" and "long" codepoints (\u2000 and \ufe40), then form all permutations of
        // 3-codepoint prefixes with the same fixed delimiter as above. This gives us coverage of
        // all adjacent pairings (short/short, short/long, long/short, long/long).
        {TYPED_LITERAL(CharT, "\u2000\u2000\u2000\x58"), 4},
        {TYPED_LITERAL(CharT, "\u2000\u2000\ufe40\x58"), 5},
        {TYPED_LITERAL(CharT, "\u2000\ufe40\u2000\x58"), 5},
        {TYPED_LITERAL(CharT, "\ufe40\u2000\u2000\x58"), 5},
        {TYPED_LITERAL(CharT, "\u2000\ufe40\ufe40\x58"), 6},
        {TYPED_LITERAL(CharT, "\ufe40\u2000\ufe40\x58"), 6},
        {TYPED_LITERAL(CharT, "\ufe40\ufe40\u2000\x58"), 6},
        {TYPED_LITERAL(CharT, "\ufe40\ufe40\ufe40\x58"), 7},

        {TYPED_LITERAL(CharT, "\u0061\u200D\x58"), 2},
        {TYPED_LITERAL(CharT, "\u0061\U0001F1E6\U0001F1E7\u200D\U0001F1E8\u0062\x58"), 5},

        // Test some codepoints affected by P2675R1
        // Used to be 2, now 1:
        {TYPED_LITERAL(CharT, "\u2e9a\x58"), 2},
        {TYPED_LITERAL(CharT, "\u2ef4\u2eff\x58"), 3},
        {TYPED_LITERAL(CharT, "\u2fd6\u2fef\x58"), 3},
        {TYPED_LITERAL(CharT, "\u2ffc\u2fff\x58"), 3},
        {TYPED_LITERAL(CharT, "\u3040\x58"), 2},
        {TYPED_LITERAL(CharT, "\u3097\u3098\x58"), 3},
        {TYPED_LITERAL(CharT, "\u3100\u3104\x58"), 3},
        {TYPED_LITERAL(CharT, "\u3130\x58"), 2},
        {TYPED_LITERAL(CharT, "\u318f\x58"), 2},
        {TYPED_LITERAL(CharT, "\u31e4\u31ef\x58"), 3},
        {TYPED_LITERAL(CharT, "\u321f\x58"), 2},
        {TYPED_LITERAL(CharT, "\u3248\u324f\x58"), 3},
        {TYPED_LITERAL(CharT, "\ua48d\ua48f\x58"), 3},
        {TYPED_LITERAL(CharT, "\ua4c7\ua4cf\x58"), 3},
        {TYPED_LITERAL(CharT, "\ufe53\x58"), 2},
        {TYPED_LITERAL(CharT, "\ufe67\x58"), 2},
        {TYPED_LITERAL(CharT, "\ufe6c\ufe6f\x58"), 3},
        {TYPED_LITERAL(CharT, "\uff00\x58"), 2},
        // Used to be 1, now 2:
        {TYPED_LITERAL(CharT, "\u23f0\x58"), 3},
        {TYPED_LITERAL(CharT, "\u23f3\x58"), 3},
        {TYPED_LITERAL(CharT, "\u267f\x58"), 3},
        {TYPED_LITERAL(CharT, "\u2693\x58"), 3},
        {TYPED_LITERAL(CharT, "\u26a1\x58"), 3},
        {TYPED_LITERAL(CharT, "\u26ce\x58"), 3},
        {TYPED_LITERAL(CharT, "\u26d4\x58"), 3},
        {TYPED_LITERAL(CharT, "\u26ea\x58"), 3},
        {TYPED_LITERAL(CharT, "\u26f5\x58"), 3},
        {TYPED_LITERAL(CharT, "\u26fa\x58"), 3},
        {TYPED_LITERAL(CharT, "\u26fd\x58"), 3},
        {TYPED_LITERAL(CharT, "\u2705\x58"), 3},
        {TYPED_LITERAL(CharT, "\u2728\x58"), 3},
        {TYPED_LITERAL(CharT, "\u274c\x58"), 3},
        {TYPED_LITERAL(CharT, "\u274e\x58"), 3},
        {TYPED_LITERAL(CharT, "\u2757\x58"), 3},
        {TYPED_LITERAL(CharT, "\u27b0\x58"), 3},
        {TYPED_LITERAL(CharT, "\u27bf\x58"), 3},
        {TYPED_LITERAL(CharT, "\u2b50\x58"), 3},
        {TYPED_LITERAL(CharT, "\u2b55\x58"), 3},
        {TYPED_LITERAL(CharT, "\U0001b132\x58"), 3},
        {TYPED_LITERAL(CharT, "\U0001b155\x58"), 3},
        {TYPED_LITERAL(CharT, "\U0001f004\x58"), 3},
        {TYPED_LITERAL(CharT, "\U0001f0cf\x58"), 3},
        {TYPED_LITERAL(CharT, "\U0001f18e\x58"), 3},
        {TYPED_LITERAL(CharT, "\U0001f6cc\x58"), 3},
        {TYPED_LITERAL(CharT, "\U0001f7f0\x58"), 3},
    };

    for (const auto& test : test_cases) {
        basic_string_view sv{test.str};
        sv = sv.substr(0, sv.size() - 1);
        assert(format(TYPED_LITERAL(CharT, "{:.{}}"), test.str, test.width - 1) == sv);
    }
}

void run_tests() {
    test_multibyte_format_strings();
    test_parse_align();
    test_width_estimation<char>();
    test_width_estimation<wchar_t>();
}

int main() {
    run_tests();

    assert(setlocale(LC_ALL, ".1252") != nullptr);
    run_tests();

    assert(setlocale(LC_ALL, ".932") != nullptr);
    run_tests();

    assert(setlocale(LC_ALL, ".UTF-8") != nullptr);
    run_tests();
}
