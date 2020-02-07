// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <array>
#include <assert.h>
#include <filesystem>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
using namespace std;

class BinaryFile {
public:
    explicit BinaryFile(const filesystem::path& filepath) {
        m_file = _wfopen(filepath.c_str(), L"rb");

        if (!m_file) {
            fwprintf(stderr, L"Validation failed: %s couldn't be opened.\n", filepath.c_str());
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

    BinaryFile(const BinaryFile&) = delete;
    BinaryFile& operator=(const BinaryFile&) = delete;

private:
    FILE* m_file{nullptr};
};

enum class TabPolicy : bool { Forbidden, Allowed };

void scan_file(const filesystem::path& filepath, const TabPolicy tab_policy, vector<unsigned char>& buffer) {
    constexpr char CR = '\r';
    constexpr char LF = '\n';

    bool has_cr       = false;
    bool has_lf       = false;
    bool has_crlf     = false;
    bool has_utf8_bom = false;

    size_t disallowed_characters     = 0;
    size_t tab_characters            = 0;
    size_t trailing_whitespace_lines = 0;

    unsigned char prev      = '@';
    unsigned char previous2 = '@';
    unsigned char previous3 = '@';

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
                    fwprintf(stderr, L"Validation failed: %s contains disallowed character 0x%02X.\n", filepath.c_str(),
                        static_cast<unsigned int>(ch));
                }
            }

            if ((prev == ' ' || prev == '\t') && (ch == CR || ch == LF)) {
                ++trailing_whitespace_lines;
            }

            previous3 = exchange(previous2, exchange(prev, ch));
        }
    }

    if (prev == CR) { // file ends with CR
        has_cr = true;
    }

    if (has_cr) {
        fwprintf(
            stderr, L"Validation failed: %s contains CR line endings (possibly damaged CRLF).\n", filepath.c_str());
    } else if (has_lf && has_crlf) {
        fwprintf(stderr, L"Validation failed: %s contains mixed line endings (both LF and CRLF).\n", filepath.c_str());
    } else if (has_lf) {
        fwprintf(stderr, L"Validation failed: %s contains LF line endings.", filepath.c_str());

        if (prev != LF) {
            fwprintf(stderr, L" Also, it doesn't end with a newline.\n");
        } else if (previous2 == LF) {
            fwprintf(stderr, L" Also, it ends with multiple newlines.\n");
        } else {
            fwprintf(stderr, L"\n");
        }
    } else if (has_crlf) {
        if (previous2 != CR || prev != LF) {
            fwprintf(stderr, L"Validation failed: %s doesn't end with a newline.\n", filepath.c_str());
        } else if (previous3 == LF) {
            fwprintf(stderr, L"Validation failed: %s ends with multiple newlines.\n", filepath.c_str());
        }
    } else {
        fwprintf(stderr, L"Validation failed: %s doesn't contain any newlines.\n", filepath.c_str());
    }

    if (has_utf8_bom) {
        fwprintf(stderr, L"Validation failed: %s contains UTF-8 BOM characters.\n", filepath.c_str());
    }

    if (tab_policy == TabPolicy::Forbidden && tab_characters != 0) {
        fwprintf(stderr, L"Validation failed: %s contains %zu tab characters.\n", filepath.c_str(), tab_characters);
    }

    if (trailing_whitespace_lines != 0) {
        fwprintf(stderr, L"Validation failed: %s contains %zu lines with trailing whitespace.\n", filepath.c_str(),
            trailing_whitespace_lines);
    }
}

int main() {
    static constexpr array skipped_directories{
        ".git"sv,
        ".vs"sv,
        ".vscode"sv,
        "llvm-project"sv,
        "out"sv,
        "vcpkg"sv,
    };

    static constexpr array skipped_extensions{
        ".dll"sv,
        ".exe"sv,
        ".obj"sv,
    };

    static constexpr array tabby_filenames{
        ".gitmodules"sv,
    };

    // TRANSITION, P0202R3, use constexpr is_sorted()
    assert(is_sorted(skipped_directories.begin(), skipped_directories.end()));
    assert(is_sorted(skipped_extensions.begin(), skipped_extensions.end()));
    assert(is_sorted(tabby_filenames.begin(), tabby_filenames.end()));

    vector<unsigned char> buffer; // reused for performance

    for (filesystem::recursive_directory_iterator rdi{"."}, last; rdi != last; ++rdi) {
        const string filename = rdi->path().filename().string();

        if (!rdi->is_regular_file()) {
            if (rdi->is_directory()) {
                if (binary_search(skipped_directories.begin(), skipped_directories.end(), filename)) {
                    rdi.disable_recursion_pending();
                }
            }

            continue;
        }

        const string extension = rdi->path().extension().string();

        if (binary_search(skipped_extensions.begin(), skipped_extensions.end(), extension)) {
            continue;
        }

        const TabPolicy tab_policy = binary_search(tabby_filenames.begin(), tabby_filenames.end(), filename)
                                         ? TabPolicy::Allowed
                                         : TabPolicy::Forbidden;

        scan_file(rdi->path(), tab_policy, buffer);
    }
}
