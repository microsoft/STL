// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <cstdio>
#include <fcntl.h>
#include <filesystem>
#include <format>
#include <fstream>
#include <io.h>
#include <limits>
#include <locale>
#include <print>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include <Windows.h>

using namespace std;

namespace test {
    class win_console {
    public:
        win_console() {
            // There's a trick you can do to create a FILE stream for a Win32 console:
            //
            //   1. Call _open_osfhandle() to create a file descriptor for the console screen
            //      buffer handle.
            //
            //   2. Call _fdopen() to get a FILE* for the file descriptor.
            constexpr DWORD screen_buffer_access = GENERIC_READ | GENERIC_WRITE;
            console_handle =
                CreateConsoleScreenBuffer(screen_buffer_access, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);

            if (console_handle == INVALID_HANDLE_VALUE) [[unlikely]] {
                return;
            }

            const int console_fd = _open_osfhandle(reinterpret_cast<intptr_t>(console_handle), _O_TEXT);

            if (console_fd == -1) [[unlikely]] {
                return;
            }

            file_stream_ptr = _fdopen(console_fd, "w");
        }

        ~win_console() {
            delete_console();
        }

        win_console(const win_console&)            = delete;
        win_console& operator=(const win_console&) = delete;

        win_console(win_console&& rhs) noexcept
            : console_handle(rhs.console_handle), file_stream_ptr(rhs.file_stream_ptr) {
            rhs.console_handle  = nullptr;
            rhs.file_stream_ptr = nullptr;
        }

        win_console& operator=(win_console&& rhs) noexcept {
            delete_console();

            console_handle     = rhs.console_handle;
            rhs.console_handle = nullptr;

            file_stream_ptr     = rhs.file_stream_ptr;
            rhs.file_stream_ptr = nullptr;

            return *this;
        }

        FILE* get_file_stream() const {
            assert(is_console_valid());
            return file_stream_ptr;
        }

        wstring get_console_line(const size_t line_number) const {
            // We use the ReadConsoleOutputCharacterW() function to read lines of text which were written to
            // the console. The neat thing here is that if we write to the console using the FILE* returned by
            // win_console::get_file_stream() with std::print(), we can still use ReadConsoleOutputCharacterW()
            // to get that text! This allows us to verify the output being written to the console and check for,
            // e.g., invalid code point replacement.
            const size_t line_char_width = get_line_character_width();
            const COORD read_coords{.X = 0, .Y = static_cast<SHORT>(line_number)};

            wstring output_str;
            output_str.resize_and_overwrite(
                line_char_width, [this, read_coords](wchar_t* const dest_ptr, const size_t allocated_size) {
                    DWORD num_chars_read;
                    const BOOL read_output_result = ReadConsoleOutputCharacterW(
                        console_handle, dest_ptr, static_cast<DWORD>(allocated_size), read_coords, &num_chars_read);

                    assert(read_output_result && "ERROR: ReadConsoleOutputCharacterW() failed!");
                    return num_chars_read;
                });

            // By default, Windows fills console character cells with space characters. We want to remove
            // those space characters which appear after the user's text.
            const size_t lastValidChar = output_str.find_last_not_of(' ');

            if (lastValidChar == wstring::npos) [[unlikely]] {
                output_str.clear();
            } else [[likely]] {
                output_str = output_str.substr(0, lastValidChar + 1);
            }

            return output_str;
        }

    private:
        void delete_console() {
            if (is_console_valid()) [[likely]] {
                // According to the MSDN, we don't call CloseHandle() on handles passed to _open_osfhandle(),
                // and we don't call _close() on file descriptors passed to _fdopen(). So, our only clean-up
                // task is to call fclose().
                fclose(file_stream_ptr);

                console_handle  = nullptr;
                file_stream_ptr = nullptr;
            }
        }

        size_t get_line_character_width() const {
            CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;

            {
                const BOOL get_screen_buffer_info_result =
                    GetConsoleScreenBufferInfo(console_handle, &screen_buffer_info);
                assert(get_screen_buffer_info_result && "ERROR: GetConsoleScreenBufferInfo() failed!");
            }

            return static_cast<size_t>(screen_buffer_info.dwSize.X);
        }

        bool is_console_valid() const {
            return console_handle != nullptr && console_handle != INVALID_HANDLE_VALUE && file_stream_ptr != nullptr;
        }

    private:
        HANDLE console_handle;
        FILE* file_stream_ptr;
    };
} // namespace test

const locale& get_utf8_locale() {
#pragma warning(push)
#pragma warning(disable : 4640) // construction of local static object is not thread-safe
    static const locale utf8_locale{"en-US.UTF-8"};
#pragma warning(pop)

    return utf8_locale;
}

wstring string_to_wstring(const string_view str) {
    using facet_type = codecvt<wchar_t, char, mbstate_t>;
    const facet_type& facet{use_facet<facet_type>(get_utf8_locale())};

    mbstate_t conversion_state{};
    const size_t num_chars_required = static_cast<size_t>(
        facet.length(conversion_state, str.data(), str.data() + str.size(), (numeric_limits<size_t>::max)()));

    wstring output_str;
    output_str.resize_and_overwrite(num_chars_required, [&](wchar_t* const dest_str_ptr, const size_t output_size) {
        const char* src_end_ptr;
        wchar_t* dest_end_ptr;
        const codecvt_base::result conversion_result = facet.in(conversion_state, str.data(), str.data() + str.size(),
            src_end_ptr, dest_str_ptr, dest_str_ptr + output_size, dest_end_ptr);

        assert(conversion_result == codecvt_base::ok);

        return dest_end_ptr - dest_str_ptr;
    });

    return output_str;
}

string wstring_to_string(const wstring_view wide_str) {
    using facet_type = codecvt<wchar_t, char, mbstate_t>;
    const facet_type& facet{use_facet<facet_type>(get_utf8_locale())};

    mbstate_t conversion_state{};
    const size_t max_chars_required = wide_str.size() * facet.max_length();

    string output_str;
    output_str.resize_and_overwrite(max_chars_required, [&](char* const dest_str_ptr, const size_t output_size) {
        const wchar_t* src_end_ptr;
        char* dest_end_ptr;
        const codecvt_base::result conversion_result = facet.out(conversion_state, wide_str.data(),
            wide_str.data() + wide_str.size(), src_end_ptr, dest_str_ptr, dest_str_ptr + output_size, dest_end_ptr);

        assert(conversion_result == codecvt_base::ok);

        return dest_end_ptr - dest_str_ptr;
    });

    return output_str;
}

void maybe_flush_console_file_stream(const test::win_console& console) {
    // std::print() and std::println() should automatically flush the stream if the Unicode
    // API is being used, according to N4928 [print.fun]/7. So, as an additional check,
    // we'll only call std::fflush() if the ordinary literal encoding is *NOT* UTF-8. This
    // should work fine, assuming that our implementation is correct.
    if constexpr (!_Is_ordinary_literal_encoding_utf8()) {
        fflush(console.get_file_stream());
    }
}

void initialize_console() {
    // We want to do things like alter the output code page of our console, but we
    // don't want to affect other tests which run concurrently. So, we want to detach this
    // process from its current console, create a new console, and modify that one instead.

    const BOOL free_console_result = FreeConsole();
    assert(free_console_result);

    const BOOL attach_console_result = AllocConsole();
    assert(attach_console_result);

    const BOOL set_output_cp_result = SetConsoleOutputCP(CP_UTF8);
    assert(set_output_cp_result);
}

void test_print_optimizations() {
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
        constexpr string_view escaped_braces_str{"[{{a{{{{b c}}}}d}}]"};
        constexpr string_view expected_str{"[{a{{b c}}d}][{a{{b c}}d}]"};

        print(console_file_stream, escaped_braces_str);
        println(console_file_stream, escaped_braces_str);

        assert(get_last_console_line_closure() == expected_str);

        print(test_str_stream, escaped_braces_str);
        println(test_str_stream, escaped_braces_str);

        string str_stream_line;
        getline(test_str_stream, str_stream_line);

        assert(str_stream_line == expected_str);
    }

    // When writing out to a Unicode console, we transcode the string in segments of 8,192 bytes
    // each. Splitting up the strings into segments requires ending each segment on a valid code
    // point (if applicable). We need to make sure that the actual string is getting printed
    // appropriately.
    //
    // (TODO: Add this test!)
}

void test_invalid_code_points_console() {
    test::win_console test_console{};
    FILE* const console_file_stream = test_console.get_file_stream();
    size_t curr_line_number         = 0;

    using printed_string_type = format_string<>;

    const auto test_valid_sequence_closure = [&](const printed_string_type printed_str) {
        println(console_file_stream, printed_str);
        maybe_flush_console_file_stream(test_console);

        const wstring console_line{test_console.get_console_line(curr_line_number++)};
        assert(wstring_to_string(console_line) == printed_str.get());
    };

    const auto test_invalid_sequence_closure = [&](const printed_string_type printed_str) {
        println(console_file_stream, printed_str);
        maybe_flush_console_file_stream(test_console);

        const wstring console_line{test_console.get_console_line(curr_line_number++)};
        const bool contains_replacement_character = console_line.contains(L'\uFFFD');

        if constexpr (_Is_ordinary_literal_encoding_utf8()) {
            // It isn't necessarily well-documented how MultiByteToWideChar() (used internally by
            // __std_print_to_unicode_console()) replaces invalid code points, except for the fact
            // that if an invalid code point is encountered, then some amount of characters are
            // replaced with a single U+FFFD character. So, we instead check that the string written
            // to the console has at least one U+FFFD character if it is invalid. (The documentation
            // for the function also implies that if the string provided to MultiByteToWideChar() isn't
            // empty, then the string created by it also won't be empty.)
            assert(printed_str.get().empty() || !console_line.empty());
            assert(contains_replacement_character);
        }

        // There seems to be some inconsistent behavior with what happens when std::fputs() (used internally
        // by std::print() et al. when writing to files or when the ordinary literal encoding isn't UTF-8) writes
        // invalid code sequences to a FILE stream associated with a console. On some systems, it seems that
        // these characters are dropped; on others, they seem to be replaced with U+FFFD. So, a check like
        // "assert(!contains_replacement_character)" might pass on some systems and fail on others.
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

    using printed_string_type = format_string<>;

    const auto test_sequence_closure = [&](const printed_string_type printed_str) {
        rewind(temp_file_stream);
        print(temp_file_stream, printed_str);
        rewind(temp_file_stream);

        string file_line_str;
        file_line_str.resize_and_overwrite(
            printed_str.get().size() + 1, [&](char* const dest_str_ptr, const size_t output_size) {
                fgets(dest_str_ptr, static_cast<int>(output_size), temp_file_stream);
                return output_size - 1;
            });

        assert(file_line_str == printed_str.get());
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

void test_stream_flush_console() {
    // If the ordinary literal encoding is UTF-8, then the FILE stream associated with
    // a console should always be flushed before writing output during a call to std::print()
    // and std::println(). Otherwise, the stream should *NOT* be flushed.
    test::win_console temp_console{};
    FILE* const console_file_stream = temp_console.get_file_stream();

    print(console_file_stream, "Hello,");

    {
        const wstring extractedStr{temp_console.get_console_line(0)};

        if constexpr (_Is_ordinary_literal_encoding_utf8()) {
            assert(extractedStr == L"Hello,");
        } else {
            assert(extractedStr.empty());
        }
    }

    println(console_file_stream, " world!");

    {
        const wstring extractedStr{temp_console.get_console_line(0)};

        if constexpr (_Is_ordinary_literal_encoding_utf8()) {
            assert(extractedStr == L"Hello, world!");
        } else {
            assert(extractedStr.empty());
        }
    }

    maybe_flush_console_file_stream(temp_console);

    {
        const wstring extractedStr{temp_console.get_console_line(0)};
        assert(extractedStr == L"Hello, world!");
    }
}

void test_stream_flush_file() {
    // Regardless of the ordinary literal encoding, neither std::print() nor std::println()
    // should flush file streams which do not refer to consoles.
    string temp_file_name_str;

    {
        ofstream output_file_stream;

        while (!output_file_stream.is_open()) {
            temp_file_name_str.resize_and_overwrite(L_tmpnam_s, [](char* const dest_ptr, const size_t allocated_size) {
                const errno_t tmpnam_result = tmpnam_s(dest_ptr, allocated_size);
                assert(tmpnam_result == 0);

                return allocated_size;
            });

            output_file_stream = ofstream{temp_file_name_str};
        }

        print(output_file_stream, "Hello, ");

        {
            ifstream input_file_stream = ifstream{temp_file_name_str};

            string extracted_line_str;
            getline(input_file_stream, extracted_line_str);

            assert(extracted_line_str.empty());
        }

        println(output_file_stream, "world!");

        {
            ifstream input_file_stream = ifstream{temp_file_name_str};

            string extracted_line_str;
            getline(input_file_stream, extracted_line_str);

            assert(extracted_line_str.empty());
        }

        output_file_stream.flush();

        {
            ifstream input_file_stream = ifstream{temp_file_name_str};

            string extracted_line_str;
            getline(input_file_stream, extracted_line_str);

            assert(extracted_line_str == "Hello, world!");
        }
    }

    filesystem::remove(temp_file_name_str);
}

int main() {
    initialize_console();

    test_print_optimizations();

    test_invalid_code_points_console();
    test_invalid_code_points_file();

    test_stream_flush_console();
    test_stream_flush_file();
}
