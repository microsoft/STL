// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
using namespace std;

constexpr size_t max_line_length = 120;

class BinaryFile {
public:
    explicit BinaryFile(const filesystem::path& filepath) {
        m_file = _wfopen(filepath.c_str(), L"rb");

        if (!m_file) {
            fwprintf(stderr, L"Validation failed: %ls couldn't be opened.\n", filepath.c_str());
        }
    }

    [[nodiscard]] bool read_next_block(vector<unsigned char>& buffer) {
        constexpr size_t BlockSize = 65536;

        buffer.resize(BlockSize);

        const size_t bytes_read = fread(buffer.data(), 1, BlockSize, m_file);

        buffer.resize(bytes_read);

        return !buffer.empty();
    }

    ~BinaryFile() {
        if (fclose(m_file) != 0) {
            fwprintf(stderr, L"fclose() failed.\n");
            abort();
        }
    }

    BinaryFile(const BinaryFile&)            = delete;
    BinaryFile& operator=(const BinaryFile&) = delete;

private:
    FILE* m_file{nullptr};
};

int validation_failure(
    bool& any_errors, const filesystem::path& filepath, _Printf_format_string_ const wchar_t* format, ...) {
    any_errors = true;

    fwprintf(stderr, L"##vso[task.logissue type=error;sourcepath=%ls;linenumber=1;columnnumber=1]Validation failed: ",
        filepath.c_str());

    va_list args;
    va_start(args, format);
    const int result = vfwprintf(stderr, format, args);
    va_end(args);

    fwprintf(stderr, L"\n");
    return result;
}

enum class TabPolicy : bool { Forbidden, Allowed };

void scan_file(
    bool& any_errors, const filesystem::path& filepath, const TabPolicy tab_policy, vector<unsigned char>& buffer) {
    constexpr char CR = '\r';
    constexpr char LF = '\n';

    bool has_cr       = false;
    bool has_lf       = false;
    bool has_crlf     = false;
    bool has_utf8_bom = false;

    size_t overlength_lines          = 0;
    size_t disallowed_characters     = 0;
    size_t tab_characters            = 0;
    size_t trailing_whitespace_lines = 0;

    unsigned char prev      = '@';
    unsigned char previous2 = '@';
    unsigned char previous3 = '@';

    size_t columns = 0;

    for (BinaryFile binary_file{filepath}; binary_file.read_next_block(buffer);) {
        for (const auto& ch : buffer) {
            if (prev == CR) {
                if (ch == LF) {
                    has_crlf = true;
                } else {
                    has_cr = true;
                }
            } else {
                if (ch == LF) {
                    has_lf = true;
                }
            }

            if (ch == '\t') {
                ++tab_characters;
            } else if (ch == 0xEF || ch == 0xBB || ch == 0xBF) {
                // 0xEF, 0xBB, and 0xBF are the UTF-8 BOM characters.
                // https://en.wikipedia.org/wiki/Byte_order_mark#UTF-8
                has_utf8_bom = true;
            } else if (ch != CR && ch != LF && !(ch >= 0x20 && ch <= 0x7E)) {
                // [0x20, 0x7E] are the printable characters, including the space character.
                // https://en.wikipedia.org/wiki/ASCII#Printable_characters
                ++disallowed_characters;
                constexpr size_t MaxErrorsForDisallowedCharacters = 10;
                if (disallowed_characters <= MaxErrorsForDisallowedCharacters) {
                    validation_failure(any_errors, filepath, L"file contains disallowed character 0x%02X.",
                        static_cast<unsigned int>(ch));
                }
            }

            if (ch == CR || ch == LF) {
                if (prev == ' ' || prev == '\t') {
                    ++trailing_whitespace_lines;
                }

                if (columns > max_line_length) {
                    ++overlength_lines;
                }
                columns = 0;
            } else {
                ++columns;
            }
            previous3 = exchange(previous2, exchange(prev, ch));
        }
    }

    if (prev == CR) { // file ends with CR
        has_cr = true;
    }

    if (has_cr) {
        validation_failure(any_errors, filepath, L"file contains CR line endings (possibly damaged CRLF).");
    } else if (has_lf && has_crlf) {
        validation_failure(any_errors, filepath, L"file contains mixed line endings (both LF and CRLF).");
    } else if (has_lf) {
        validation_failure(any_errors, filepath, L"file contains LF line endings.");

        if (prev != LF) {
            validation_failure(any_errors, filepath, L"file doesn't end with a newline.");
        } else if (previous2 == LF) {
            validation_failure(any_errors, filepath, L"file ends with multiple newlines.");
        }
    } else if (has_crlf) {
        if (previous2 != CR || prev != LF) {
            validation_failure(any_errors, filepath, L"file doesn't end with a newline.");
        } else if (previous3 == LF) {
            validation_failure(any_errors, filepath, L"file ends with multiple newlines.");
        }
    } else {
        validation_failure(any_errors, filepath, L"file doesn't contain any newlines.");
    }

    if (has_utf8_bom) {
        validation_failure(any_errors, filepath, L"file contains UTF-8 BOM characters.");
    }

    if (tab_policy == TabPolicy::Forbidden && tab_characters != 0) {
        validation_failure(any_errors, filepath, L"file contains %zu tab characters.", tab_characters);
    }

    if (trailing_whitespace_lines != 0) {
        validation_failure(
            any_errors, filepath, L"file contains %zu lines with trailing whitespace.", trailing_whitespace_lines);
    }

    if (overlength_lines != 0) {
        static constexpr array checked_extensions{
            // line length should be capped in files with these extensions:
            L""sv,
            L".cmd"sv,
            L".cpp"sv,
            L".h"sv,
            L".hpp"sv,
            L".md"sv,
            L".ps1"sv,
            L".py"sv,
            L".yml"sv,
        };
        static_assert(is_sorted(checked_extensions.begin(), checked_extensions.end()));

        if (binary_search(checked_extensions.begin(), checked_extensions.end(), filepath.extension().wstring())) {
            validation_failure(any_errors, filepath, L"file contains %zu lines with more than %zu columns.\n",
                overlength_lines, max_line_length);
        }
    }
}

int main() {
    static constexpr array skipped_directories{
        L".git"sv,
        L".vs"sv,
        L".vscode"sv,
        L"__pycache__"sv,
        L"boost-math"sv,
        L"build"sv,
        L"google-benchmark"sv,
        L"llvm-project"sv,
        L"out"sv,
    };

    static constexpr array skipped_extensions{
        L".dll"sv,
        L".exe"sv,
        L".obj"sv,
    };

    // make sure someone doesn't accidentally include a diff in the tree
    static constexpr array bad_extensions{
        L".diff"sv,
        L".patch"sv,
    };

    static constexpr array tabby_filenames{
        L".gitmodules"sv,
    };

    static_assert(is_sorted(skipped_directories.begin(), skipped_directories.end()));
    static_assert(is_sorted(skipped_extensions.begin(), skipped_extensions.end()));
    static_assert(is_sorted(bad_extensions.begin(), bad_extensions.end()));
    static_assert(is_sorted(tabby_filenames.begin(), tabby_filenames.end()));

    vector<unsigned char> buffer; // reused for performance
    bool any_errors = false;

    for (filesystem::recursive_directory_iterator rdi{"."}, last; rdi != last; ++rdi) {
        const filesystem::path& filepath = rdi->path();

        const wstring filename = filepath.filename().wstring();

        if (!rdi->is_regular_file()) {
            if (rdi->is_directory()) {
                if (binary_search(skipped_directories.begin(), skipped_directories.end(), filename)) {
                    rdi.disable_recursion_pending();
                }
            }

            continue;
        }

        const wstring& relative_path = filepath.native();

        constexpr size_t maximum_relative_path_length = 120;
        if (relative_path.size() > maximum_relative_path_length) {
            validation_failure(any_errors, filepath, L"filepath is too long (%zu characters; the limit is %zu).",
                relative_path.size(), maximum_relative_path_length);
        }

        if (relative_path.find(L' ') != wstring::npos) {
            validation_failure(any_errors, filepath, L"filepath contains spaces.");
        }

        const wstring extension = filepath.extension().wstring();

        if (binary_search(skipped_extensions.begin(), skipped_extensions.end(), extension)) {
            continue;
        }

        if (binary_search(bad_extensions.begin(), bad_extensions.end(), extension)) {
            validation_failure(any_errors, filepath, L"file should not be checked in.");
            continue;
        }

        const TabPolicy tab_policy = binary_search(tabby_filenames.begin(), tabby_filenames.end(), filename)
                                       ? TabPolicy::Allowed
                                       : TabPolicy::Forbidden;

        scan_file(any_errors, filepath, tab_policy, buffer);
    }

    if (any_errors) {
        fwprintf(
            stderr, L"##vso[task.logissue type=warning]If your build fails here, you need to fix the listed issues.\n");
        fwprintf(stderr, L"##vso[task.complete result=Failed]DONE\n");
    }
}
