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
        // Filling with footballs ("\xf0\x9f\x8f\x88" is U+1F3C8 AMERICAN FOOTBALL)
        assert(format("{:\xf0\x9f\x8f\x88>4}"sv, 42) == "\xf0\x9f\x8f\x88\xf0\x9f\x8f\x88\x34\x32");

        assert(format("{:\xf0\x9f\x8f\x88<4.2}", "1") == "\x31\xf0\x9f\x8f\x88\xf0\x9f\x8f\x88\xf0\x9f\x8f\x88"sv);
        assert(format("{:\xf0\x9f\x8f\x88^4.2}", "1") == "\xf0\x9f\x8f\x88\x31\xf0\x9f\x8f\x88\xf0\x9f\x8f\x88"sv);
        assert(format("{:\xf0\x9f\x8f\x88>4.2}", "1") == "\xf0\x9f\x8f\x88\xf0\x9f\x8f\x88\xf0\x9f\x8f\x88\x31"sv);
    }

    {
        try {
            (void) format("{:\x9f\x8f\x88<10}"sv, 42); // Bad fill character encoding: missing lead byte before \x9f
            assert(false);
        } catch (const format_error&) {
        }
    }
}

void test_parse_align() {
    auto parse_align_fn = _Parse_align<char, testing_callbacks<char>>;

    {
        // "\xf0\x9f\x8f\x88" is U+1F3C8 AMERICAN FOOTBALL
        test_parse_helper(parse_align_fn, "\xf0\x9f\x8f\x88<X"sv, false, 5,
            {.expected_alignment = _Fmt_align::_Left, .expected_fill = "\xf0\x9f\x8f\x88"sv});
        test_parse_helper(parse_align_fn, "\xf0\x9f\x8f\x88>X"sv, false, 5,
            {.expected_alignment = _Fmt_align::_Right, .expected_fill = "\xf0\x9f\x8f\x88"sv});
        test_parse_helper(parse_align_fn, "\xf0\x9f\x8f\x88^X"sv, false, 5,
            {.expected_alignment = _Fmt_align::_Center, .expected_fill = "\xf0\x9f\x8f\x88"sv});
    }
}

void run_tests() {
    test_multibyte_format_strings();
    test_parse_align();
}

int main() {
    run_tests();

    assert(setlocale(LC_ALL, ".1252") != nullptr);
    run_tests();

    assert(setlocale(LC_ALL, ".932") != nullptr);
    run_tests();

#ifndef MSVC_INTERNAL_TESTING // TRANSITION, the Windows version on Contest VMs doesn't always understand ".UTF-8"
    assert(setlocale(LC_ALL, ".UTF-8") != nullptr);
    run_tests();
#endif
}
