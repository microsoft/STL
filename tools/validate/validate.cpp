// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <array>
#include <assert.h>
#include <exception>
#include <filesystem>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
using namespace std;

class BinaryFile {
public:
    explicit BinaryFile(const string& filepath) {
        m_file = fopen(filepath.c_str(), "rb");

        if (!m_file) {
            throw runtime_error("fopen() failed for: " + filepath);
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
            puts("fclose() failed.");
            terminate();
        }
    }

    BinaryFile(const BinaryFile&) = delete;
    BinaryFile& operator=(const BinaryFile&) = delete;

private:
    FILE* m_file{nullptr};
};

void scan_file(const string& filepath, vector<unsigned char>& buffer) {
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
                has_utf8_bom = true;
            } else if (ch != CR && ch != LF && !(ch >= 0x20 && ch <= 0x7E)) {
                ++disallowed_characters;
                constexpr size_t MaxDisallowedCharacters = 10;
                if (disallowed_characters <= MaxDisallowedCharacters) {
                    printf("Validation failed: %s contains disallowed character 0x%02X.\n", filepath.c_str(),
                        static_cast<unsigned int>(ch));
                }
            }

            if ((prev == ' ' || prev == '\t') && (ch == CR || ch == LF)) {
                ++trailing_whitespace_lines;
            }

            (void) exchange(previous3, exchange(previous2, exchange(prev, ch)));
        }
    }

    if (prev == CR) { // file ends with CR
        has_cr = true;
    }

    if (has_cr) {
        printf("Validation failed: %s contains CR line endings (possibly damaged CRLF).\n", filepath.c_str());
    } else if (has_lf && has_crlf) {
        printf("Validation failed: %s contains mixed line endings (both LF and CRLF).\n", filepath.c_str());
    } else if (has_lf) {
        printf("Validation failed: %s contains LF line endings.", filepath.c_str());

        if (prev != LF) {
            printf(" Also, it doesn't end with a newline.\n");
        } else if (previous2 == LF) {
            printf(" Also, it ends with multiple newlines.\n");
        } else {
            printf("\n");
        }
    } else if (has_crlf) {
        if (previous2 != CR || prev != LF) {
            printf("Validation failed: %s doesn't end with a newline.\n", filepath.c_str());
        } else if (previous3 == LF) {
            printf("Validation failed: %s ends with multiple newlines.\n", filepath.c_str());
        }
    } else {
        printf("Validation failed: %s doesn't contain any newlines.\n", filepath.c_str());
    }

    if (has_utf8_bom) {
        printf("Validation failed: %s contains UTF-8 BOM characters.\n", filepath.c_str());
    }

    if (tab_characters != 0) {
        printf("Validation failed: %s contains %zu tab characters.\n", filepath.c_str(), tab_characters);
    }

    if (trailing_whitespace_lines != 0) {
        printf("Validation failed: %s contains %zu lines with trailing whitespace.\n", filepath.c_str(),
            trailing_whitespace_lines);
    }
}

int main() {
    static constexpr array skipped_directories{
        ".git"sv,
        ".vs"sv,
        ".vscode"sv,
        "out"sv,
        "vcpkg"sv,
    };

    static constexpr array skipped_extensions{
        ".dll"sv,
        ".exe"sv,
        ".obj"sv,
    };

    // TRANSITION, P0202R3, use constexpr is_sorted()
    assert(is_sorted(skipped_directories.begin(), skipped_directories.end()));
    assert(is_sorted(skipped_extensions.begin(), skipped_extensions.end()));

    vector<unsigned char> buffer; // reused for performance

    try {
        for (filesystem::recursive_directory_iterator rdi{"."}, last; rdi != last; ++rdi) {
            if (!rdi->is_regular_file()) {
                if (rdi->is_directory()) {
                    const string filename = rdi->path().filename().string();
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

            const string filepath = rdi->path().string();

            scan_file(filepath, buffer);
        }
    } catch (const exception& e) {
        printf("Validation failed: Exception: %s\n", e.what());
    } catch (...) {
        printf("Validation failed: Unknown exception.\n");
    }
}
