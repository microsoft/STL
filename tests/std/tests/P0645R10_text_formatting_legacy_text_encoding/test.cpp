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
        assert(setlocale(LC_ALL, ".932") != nullptr);
        const auto s =
            "\x93\xfa\x96{\x92\x6e\x90}"sv; // Note the use of `{` and `}` as continuation bytes (from GH-1576)
        assert(format(s) == s);

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

    assert(setlocale(LC_ALL, "C") != nullptr);
}

void test_parse_align() {
    auto parse_align_fn = _Parse_align<char, testing_callbacks<char>>;

    {
        assert(setlocale(LC_ALL, ".932") != nullptr);
        test_parse_helper(parse_align_fn, "\x93\xfa<X"sv, false, 3,
            {.expected_alignment = _Fmt_align::_Left, .expected_fill = "\x93\xfa"sv});
        test_parse_helper(parse_align_fn, "\x96\x7b>X"sv, false, 3,
            {.expected_alignment = _Fmt_align::_Right, .expected_fill = "\x96\x7b"sv});
        test_parse_helper(parse_align_fn, "\x92\x6e^X"sv, false, 3,
            {.expected_alignment = _Fmt_align::_Center, .expected_fill = "\x92\x6e"sv});
    }

    assert(setlocale(LC_ALL, "C") != nullptr);
}

int main() {
    test_multibyte_format_strings();
    test_parse_align();
}
