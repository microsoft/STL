// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "test.hpp"

#include <format>
#include <locale>
#include <print>
#include <sstream>
#include <string_view>

#include <iostream>

const locale& get_utf8_locale() {
#pragma warning(push)
#pragma warning(disable: 4640) // construction of local static object is not thread-safe
    static const locale utf8_locale{"en_US.utf8"};
#pragma warning(pop)

    return utf8_locale;
}

wstring string_to_wstring(const string_view str) {
    using facet_type = codecvt<wchar_t, char, mbstate_t>;
    const facet_type& facet{use_facet<facet_type>(get_utf8_locale())};

    mbstate_t conversion_state{};
    const size_t num_chars_required =
        static_cast<size_t>(facet.length(conversion_state, str.data(), (str.data() + str.size()), (numeric_limits<size_t>::max)()));

    wstring output_str;
    output_str.resize_and_overwrite(num_chars_required, [&](wchar_t* const dest_str_ptr, const size_t output_size) {
        const char* src_end_ptr;
        wchar_t* dest_end_ptr;
        const codecvt_base::result conversion_result = facet.in(conversion_state, str.data(), (str.data() + str.size()),
            src_end_ptr, dest_str_ptr, (dest_str_ptr + output_size), dest_end_ptr);

        assert(conversion_result == codecvt_base::ok);

        return (dest_end_ptr - dest_str_ptr);
    });

    return output_str;
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

void maybe_flush_console_file_stream(const test::win_console& console)
{
    // std::print() and std::println() should automatically flush the stream if the Unicode
    // API is being used, according to the C++ specifications. So, as an additional check,
    // we'll only call std::fflush() if the ordinary literal encoding is *NOT* UTF-8. This
    // should work fine, assuming that our implementation is correct.
    if constexpr (!_Is_ordinary_literal_encoding_utf8) {
        std::fflush(console.get_file_stream());
    }
}

void test_print_optimizations() {
    // The function signatures of std::print() and std::println() do not exactly match
    // those specified in the C++ standard. Specifically, rather than taking a std::format_string,
    // these functions take a std::_Print_string as a parameter which privately inherits from
    // std::basic_format_string. This is done to implement an optimization related to printing
    // strings which don't have any formatting arguments (see <__msvc_print.hpp>).
    //
    // This does mean that we need to make sure that our program behaves "as-if" we didn't
    // make these changes.

    test::win_console test_console{};
    FILE* const console_file_stream = test_console.get_file_stream();

    const auto get_last_console_line_closure = [&test_console]() {
        maybe_flush_console_file_stream(test_console);
        
        static size_t curr_line_number = 0;
        return wstring_to_string(test_console.get_console_line(curr_line_number++));
    };

    stringstream test_str_stream{};

    // Even if a string has no formatting arguments, std::format() will still replace escaped
    // brace characters (i.e., {{ and }}) with a single brace character. We need to make sure
    // that we do the same.
    {
        constexpr string_view escaped_braces_str{"[{{ }}]"};
        constexpr string_view expected_str{"[{ }][{ }]"};

        print(console_file_stream, escaped_braces_str);
        println(console_file_stream, escaped_braces_str);

        assert(get_last_console_line_closure() == expected_str);

        print(test_str_stream, escaped_braces_str);
        println(test_str_stream, escaped_braces_str);

        string str_stream_line;
        getline(test_str_stream, str_stream_line);

        assert(str_stream_line == expected_str);
    }

    // We use std::_Print_string as a parameter, but we must still be able to pass std::format_string
    // values to calls to std::print() and std::println().
    {
        constexpr format_string<int, string_view> test_format_str{"2 + 2 = {0}? {1}"};
        constexpr format_string<int, string_view> test_format_str_newline{"2 + 2 = {0}? {1}\n"};

        using namespace literals::string_view_literals;

        print(console_file_stream, test_format_str_newline, 3, "Nope."sv);
        assert(get_last_console_line_closure() == "2 + 2 = 3? Nope.");

        println(console_file_stream, test_format_str, 7, "Still wrong..."sv);
        assert(get_last_console_line_closure() == "2 + 2 = 7? Still wrong...");

        print(test_str_stream, test_format_str_newline, -32, "Really?"sv);

        string str_stream_line;

        std::getline(test_str_stream, str_stream_line);
        assert(str_stream_line == "2 + 2 = -32? Really?");

        println(test_str_stream, test_format_str, 4, "Finally."sv);

        std::getline(test_str_stream, str_stream_line);
        assert(str_stream_line == "2 + 2 = 4? Finally.");
    }
}

void test_invalid_code_points_console() {
    test::win_console test_console{};
    FILE* const console_file_stream = test_console.get_file_stream();
    size_t curr_line_number         = 0;

    using printed_string_type = _Print_string<>;

    const auto test_valid_sequence_closure = [&](const printed_string_type printed_str) {
        println(console_file_stream, printed_str);
        maybe_flush_console_file_stream(test_console);

        const wstring console_line{test_console.get_console_line(curr_line_number++)};
        std::cout << "printed_str: " << printed_str._Get() << "\n";
        std::cout << "console_line: " << wstring_to_string(console_line) << "\n";
        assert(wstring_to_string(console_line) == printed_str._Get());
    };

    const auto test_invalid_sequence_closure = [&](const printed_string_type printed_str) {
        println(console_file_stream, printed_str);
        maybe_flush_console_file_stream(test_console);

        const wstring console_line{test_console.get_console_line(curr_line_number++)};
        const bool contains_replacement_character = console_line.contains(L'\uFFFD');

        if constexpr (_Is_ordinary_literal_encoding_utf8) {
            // It isn't necessarily well-documented how MultiByteToWideChar() (used internally by
            // __std_print_to_unicode_console()) replaces invalid code points, except for the fact
            // that if an invalid code point is encountered, then some amount of characters are
            // replaced with a single U+FFFD character. So, we instead check that the string written
            // to the console has at least one U+FFFD character if it is invalid. (The documentation
            // for the function also implies that if the string provided to MultiByteToWideChar() isn't
            // empty, then the string created by it also won't be empty.)
            assert(printed_str._Get().empty() || !console_line.empty());
            assert(contains_replacement_character);
        } else {
            // When UTF-8 is not the ordinary literal encoding, calls to std::fputs() (used internally
            // by std::print() et al.) on a FILE stream associated with a console do, in fact, seem to
            // drop invalid characters when they get displayed. They should never be replaced with 
            // U+FFFD. It isn't specified which characters get dropped.
            //
            // Since the C++ specifications state that invalid code points should only be replaced with
            // U+FFFD when the native Unicode API is being used, we make sure that the string we read
            // from the console does *NOT* contain any.
            assert(!contains_replacement_character);
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
    FILE* temp_file_stream;

    {
        const errno_t tmpfile_result = tmpfile_s(&temp_file_stream);
        assert(tmpfile_result == 0);
    }

    using printed_string_type = _Print_string<>;

    const auto test_sequence_closure = [&](const printed_string_type printed_str) {
        rewind(temp_file_stream);
        print(temp_file_stream, printed_str);
        rewind(temp_file_stream);

        string file_line_str;
        file_line_str.resize_and_overwrite(
            printed_str._Get().size() + 1, [&](char* const dest_str_ptr, const size_t output_size) {
                fgets(dest_str_ptr, static_cast<int>(output_size), temp_file_stream);
                return (output_size - 1);
            });

        assert(file_line_str == printed_str._Get());
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

int main() {
    //locale::global(get_utf8_locale());
    SetConsoleOutputCP(CP_UTF8);
    
    test_print_optimizations();

    test_invalid_code_points_console();
    test_invalid_code_points_file();
}