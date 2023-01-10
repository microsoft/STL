// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "test.hpp"

#include <filesystem>
#include <fstream>
#include <locale>
#include <print>
#include <string_view>

const locale& get_utf8_locale() {
    static const locale utf8_locale{"en_US.utf8"};
    return utf8_locale;
}

string wstring_to_string(const wstring_view wide_str) {
    using facet_type = codecvt<wchar_t, char, mbstate_t>;
    const facet_type& facet{use_facet<facet_type>(get_utf8_locale())};

    mbstate_t conversion_state{};
    const size_t max_chars_required = (wide_str.size() * facet.max_length());

    string output_str;
    output_str.resize_and_overwrite(max_chars_required, [&](char* const dest_str_ptr, const size_t output_size) {
        const wchar_t* src_end_ptr;
        char* dest_end_ptr;
        const codecvt_base::result conversion_result = facet.out(conversion_state, wide_str.data(),
            (wide_str.data() + wide_str.size()), src_end_ptr, dest_str_ptr, (dest_str_ptr + output_size), dest_end_ptr);

        assert(conversion_result == codecvt_base::ok);

        return (dest_end_ptr - dest_str_ptr);
    });

    return output_str;
}

void test_invalid_code_points_console() {
    test::win_console temp_console{};
    FILE* const console_file_stream = temp_console.get_file_stream();
    size_t curr_line_number         = 0;

    const auto test_valid_sequence_closure = [&](const string_view printed_str) {
        println(console_file_stream, printed_str);
        const string console_line{wstring_to_string(temp_console.get_console_line(curr_line_number++))};
        assert(console_line == printed_str);
    };

    const auto test_invalid_sequence_closure = [&](const string_view printed_str) {
        println(console_file_stream, printed_str);
        const string console_line{temp_console.get_console_line(curr_line_number++)};

        // Invalid code units shouldn't be replaced if the ordinary literal encoding isn't
        // UTF-8.
        if constexpr (_Is_ordinary_literal_encoding_utf8) {
            // It isn't necessarily well-documented how MultiByteToWideChar() (used internally by
            // __std_print_to_unicode_console()) replaces invalid code points, except for the fact
            // that if an invalid code point is encountered, then some amount of characters are
            // replaced with a single U+FFFD character. So, we instead check that for each character
            // present in the line printed out to the console, this character was either present in the
            // original printed_str or is equal to U+FFFD.
            for (const auto c : console_line) {
                assert(console_line.contains(c) || c == '\uFFFD');
            }
        } else {
            assert(console_line == printed_str);
        }
    };

    // Example UTF-8 Code Sequences from https://www.php.net/manual/en/reference.pcre.pattern.modifiers.php#54805

    // Valid ASCII
    test_valid_sequence_closure("a");

    // Valid 2 Octet Sequence
    test_valid_sequence_closure("\xC3\xB1");

    // Invalid 2 Octet Sequence
    test_invalid_sequence_closure("\xC3\x28");

    // Invalid Sequence Identifier
    test_invalid_sequence_closure("\xA0\xA1");

    // Valid 3 Octet Sequence
    test_valid_sequence_closure("\xE2\x82\xA1");

    // Invalid 3 Octet Sequence (in 2nd Octet)
    test_invalid_sequence_closure("\xE2\x28\xA1");

    // Invalid 3 Octet Sequence (in 3rd Octet)
    test_invalid_sequence_closure("\xE2\x82\x28");

    // Skipped: 0xF0 0x90 0x8C 0xBC (MultiByteToWideChar() correctly finds that this is a valid
    // UTF-8 code point, but the call to WriteConsoleW() seems to erroneously replace it with U+FFFD.)

    // Invalid 4 Octet Sequence (in 2nd Octet)
    test_invalid_sequence_closure("\xF0\x28\x8C\xBC");

    // Invalid 4 Octet Sequence (in 3rd Octet)
    test_invalid_sequence_closure("\xF0\x90\x28\xBC");

    // Invalid 4 Octet Sequence (in 4th Octet)
    test_invalid_sequence_closure("\xF0\x28\x8C\x25");
}

void test_invalid_code_points_file() {
    // Unlike for the console API when the ordinary literal encoding is UTF-8, invalid code points shouldn't
    // be replaced when writing to a file.
    FILE* temp_file_stream = tmpfile();

    const auto test_sequence_closure = [&](const string_view printed_str) {
        rewind(temp_file_stream);
        print(temp_file_stream, printed_str);
        rewind(temp_file_stream);

        string file_line_str;
        file_line_str.resize_and_overwrite(
            printed_str.size() + 1, [&](char* const dest_str_ptr, const size_t output_size) {
                fgets(dest_str_ptr, output_size, temp_file_stream);
                return (output_size - 1);
            });

        assert(file_line_str == printed_str);
    };

    // Example UTF-8 Code Sequences from https://www.php.net/manual/en/reference.pcre.pattern.modifiers.php#54805

    // Valid ASCII
    test_sequence_closure("a");

    // Valid 2 Octet Sequence
    test_sequence_closure("\xC3\xB1");

    // Invalid 2 Octet Sequence
    test_sequence_closure("\xC3\x28");

    // Invalid Sequence Identifier
    test_sequence_closure("\xA0\xA1");

    // Valid 3 Octet Sequence
    test_sequence_closure("\xE2\x82\xA1");

    // Invalid 3 Octet Sequence (in 2nd Octet)
    test_sequence_closure("\xE2\x28\xA1");

    // Invalid 3 Octet Sequence (in 3rd Octet)
    test_sequence_closure("\xE2\x82\x28");

    // Valid 4 Octet Sequence
    test_sequence_closure("\xF0\x90\x8C\xBC");

    // Invalid 4 Octet Sequence (in 2nd Octet)
    test_sequence_closure("\xF0\x28\x8C\xBC");

    // Invalid 4 Octet Sequence (in 3rd Octet)
    test_sequence_closure("\xF0\x90\x28\xBC");

    // Invalid 4 Octet Sequence (in 4th Octet)
    test_sequence_closure("\xF0\x28\x8C\x25");

    fclose(temp_file_stream);
}

void test_print_optimizations()
{
    // The function signatures of std::print() and std::println() do not exactly match
    // those specified in the C++ standard. Specifically, rather than taking a std::format_string,
    // these functions take a std::_Print_string as a parameter which privately inherits from
    // std::basic_format_string. This is done to implement an optimization related to printing
    // string which don't have any formatting arguments (see <__msvc_print.hpp>).
    //
    // This does mean that we need to make sure that our program behaves "as-if" we didn't
    // make these changes.

    test::win_console temp_console{};
    size_t curr_console_line_number = 0;


}

int main() {
    locale::global(get_utf8_locale());

    test_print_optimizations();

    test_invalid_code_points_console();
    test_invalid_code_points_file();
}

void test_print_call_parameters() { // COMPILE-ONLY
    
}