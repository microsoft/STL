// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <clocale>
#include <format>

using namespace std;

void test_escaped_string() {
    assert(format("{:?}", "\u00A0") == "\"\\u{a0}\""); // U+00A0 NO-BREAK SPACE
    assert(format("{:?}", "\u0300") == "\"\\u{300}\""); // U+0300 COMBINING GRAVE ACCENT

    assert(format("{:?}", "\u00A1") == "\"\u00A1\""); // U+00A1 INVERTED EXCLAMATION MARK
    assert(format("{:?}", "\U00010000") == "\"\U00010000\""); // U+10000 LINEAR B SYLLABLE B008 A

    assert(format("{:?}", "\xC0\x80") == "\"\\x{c0}\\x{80}\""); // ill-formed code unit sequence

    assert(format("{:?}", "\u00A0\u0300") == "\"\\u{a0}\\u{300}\"");
    assert(format("{:?}", " \u0300") == "\" \u0300\"");
    assert(format("{:?}", "a\u0300") == "\"a\u0300\"");
}

int main() {
    test_escaped_string();

    assert(setlocale(LC_ALL, ".1252") != nullptr);
    test_escaped_string();

    assert(setlocale(LC_ALL, ".932") != nullptr);
    test_escaped_string();

    assert(setlocale(LC_ALL, ".UTF-8") != nullptr);
    test_escaped_string();
}
