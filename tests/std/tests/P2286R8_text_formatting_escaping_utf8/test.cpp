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

template <class TupleOrPair>
void test_tuple_or_pair_escaping(TupleOrPair&& input) {
    get<1>(input) = "hell\u00d6"; // U+00D6 LATIN CAPITAL LETTER O WITH DIAERESIS
    assert(format("{}", input) == "('*', \"hell\u00d6\")");
    assert(format("{:#^16}", input) == "#('*', \"hell\u00d6\")#");

    get<1>(input) = "hell\uff2f"; // U+FF2F FULLWIDTH LATIN CAPITAL LETTER O
    assert(format("{}", input) == "('*', \"hell\uff2f\")");
    assert(format("{:#^16}", input) == "('*', \"hell\uff2f\")#");
}

void run_test() {
    test_escaped_string();
    test_tuple_or_pair_escaping(make_pair('*', ""));
    test_tuple_or_pair_escaping(make_tuple('*', ""));
}

int main() {
    run_test();

    assert(setlocale(LC_ALL, ".1252") != nullptr);
    run_test();

    assert(setlocale(LC_ALL, ".932") != nullptr);
    run_test();

    assert(setlocale(LC_ALL, ".UTF-8") != nullptr);
    run_test();
}
