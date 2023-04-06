// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <clocale>
#include <format>
#include <string_view>

#include "test_format_support.hpp"

using namespace std;

void test_multibyte_format_strings() {
    const auto s = "\x93\xfa\x96{\x92\x6e\x90}"sv; // Note the use of `{` and `}` as continuation bytes (from GH-1576)
    assert(format("\x93\xfa\x96{\x92\x6e\x90}"sv) == s);

    assert(format("{:.2}", s) == "\x93\xfa"sv);
    assert(format("{:4.2}", s) == "\x93\xfa  "sv);

    assert(format("{:<4.2}", s) == "\x93\xfa  "sv);
    assert(format("{:^4.2}", s) == " \x93\xfa "sv);
    assert(format("{:>4.2}", s) == "  \x93\xfa"sv);

    assert(format("{:\x90}<4.2}", s) == "\x93\xfa\x90}\x90}"sv);
    assert(format("{:\x90}^4.2}", s) == "\x90}\x93\xfa\x90}"sv);
    assert(format("{:\x90}>4.2}", s) == "\x90}\x90}\x93\xfa"sv);

    assert(format("{:.3}", s) == "\x93\xfa"sv);
    assert(format("{:4.3}", s) == "\x93\xfa  "sv);

    assert(format("{:<4.3}", s) == "\x93\xfa  "sv);
    assert(format("{:^4.3}", s) == " \x93\xfa "sv);
    assert(format("{:>4.3}", s) == "  \x93\xfa"sv);

    assert(format("{:\x90}<4.3}", s) == "\x93\xfa\x90}\x90}"sv);
    assert(format("{:\x90}^4.3}", s) == "\x90}\x93\xfa\x90}"sv);
    assert(format("{:\x90}>4.3}", s) == "\x90}\x90}\x93\xfa"sv);
}

void test_parse_align() {
    const auto parse_align_fn = _Parse_align<char, testing_callbacks<char>>;

    test_parse_helper(parse_align_fn, "\x93\xfa<X"sv, false, 3, //
        {.expected_alignment = _Fmt_align::_Left, .expected_fill = "\x93\xfa"sv});
    test_parse_helper(parse_align_fn, "\x96\x7b>X"sv, false, 3,
        {.expected_alignment = _Fmt_align::_Right, .expected_fill = "\x96\x7b"sv});
    test_parse_helper(parse_align_fn, "\x92\x6e^X"sv, false, 3,
        {.expected_alignment = _Fmt_align::_Center, .expected_fill = "\x92\x6e"sv});

    test_parse_helper(parse_align_fn, "\x92\x30<X"sv, true);
}

void test_width_estimation() {
    // Format strings of known width with a trailing delimiter using a precision large enough to
    // include all but the delimiter to validate the width estimation code.
    struct test_case {
        const char* str;
        int width;
    };
    constexpr test_case test_cases[] = {
        {"\x58", 1},
        {"x\x58", 2},

        // Pick "short" and "long" codepoints (\x20 and \x96\x7b), then form all permutations of
        // 3-codepoint prefixes with the same fixed delimiter as above. This gives us coverage of
        // all adjacent pairings (short/short, short/long, long/short, long/long).
        {"\x20\x20\x20\x58", 4},
        {"\x20\x20\x96\x7b\x58", 5},
        {"\x20\x96\x7b\x20\x58", 5},
        {"\x96\x7b\x20\x20\x58", 5},
        {"\x20\x96\x7b\x96\x7b\x58", 6},
        {"\x96\x7b\x20\x96\x7b\x58", 6},
        {"\x96\x7b\x96\x7b\x20\x58", 6},
        {"\x96\x7b\x96\x7b\x96\x7b\x58", 7},
    };

    for (const auto& test : test_cases) {
        string_view sv{test.str};
        sv = sv.substr(0, sv.size() - 1);
        assert(format("{:.{}}", test.str, test.width - 1) == sv);
    }
}

int main() {
    test_multibyte_format_strings();
    test_parse_align();
    test_width_estimation();
}
