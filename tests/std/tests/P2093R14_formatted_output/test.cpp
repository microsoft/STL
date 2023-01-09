// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <filesystem>
#include <print>
#include <string_view>

#include "test.hpp"

void test_invalid_code_points_console() {
    test::win_console temp_console{};
    FILE* const console_file_stream = temp_console.get_file_stream();
    size_t curr_line_number         = 0;

    const auto test_sequence_closure = [&](const string_view printed_str, const wstring_view expected_str) {
        println(console_file_stream, printed_str);
        const wstring console_line{temp_console.get_console_line(curr_line_number++)};
        assert(console_line == expected_str);
    };

    // Example UTF-8 Code Sequences from https://www.php.net/manual/en/reference.pcre.pattern.modifiers.php#54805

    // Valid ASCII
    test_sequence_closure("a", L"a");
    
    // Valid 2 Octet Sequence
    test_sequence_closure("\xC3\xB1", L"\xC3\xB1");

    // Invalid 2 Octet Sequence
    test_sequence_closure("\xC3\x28", L"\xFFFD\x28");

    // Invalid Sequence Identifier
    test_sequence_closure("\xA0\xA1", L"\xFFFD\xFFFD");

    // Valid 3 Octet Sequence
    test_sequence_closure("\xE2\x82\xA1", L"\xE2\x82\xA1");

    // Invalid 3 Octet Sequence (in 2nd Octet)
    test_sequence_closure("\xE2\x28\xA1", L"\xFFFD\x28\xFFFD");

    // Invalid 3 Octet Sequence (in 3rd Octet)
    test_sequence_closure("\xE2\x82\x28", L"\xFFFD\x28");

    // Skipped: 0xF0 0x90 0x8C 0xBC (MultiByteToWideChar() correctly finds that this is a valid
    // UTF-8 string, but the call to WriteConsoleW() seems to erroneously replace it with U+FFFD.)

    // Invalid 4 Octet Sequence (in 2nd Octet)
    test_sequence_closure("\xF0\x28\x8C\xBC", L"\xFFFD\x28\xFFFD\xFFFD");

    // Invalid 4 Octet Sequence (in 3rd Octet)
    test_sequence_closure("\xF0\x90\x28\xBC", L"\xFFFD\x28\xFFFD");

    // Invalid 4 Octet Sequence (in 4th Octet)
    test_sequence_closure("\xF0\x28\x8C\x28", L"\xFFFD\x28\xFFFD\x28");
}

void test_invalid_code_points_file()
{
    // Unlike for the console API, invalid code points shouldn't be replaced when writing to a file
    // OR when the ordinary literal encoding is not UTF-8.
    FILE* temp_file_stream = tmpfile();


}

int main() {
    test_invalid_code_points_console();
}