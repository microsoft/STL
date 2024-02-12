// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <format>

using namespace std;

void test_escaped_string() {
    assert(format("{:?}", "\x81\x40") == "\"\\u{3000}\""); // U+3000 IDEOGRAPHIC SPACE

    assert(format("{:?}", "\x81\x41") == "\"\x81\x41\"");

    assert(format("{:?}", "\x81") == "\"\\x{81}\"");
    assert(format("{:?}", "\xEB!") == "\"\\x{eb}!\"");

    assert(format("{:?}", "\x81\x40\x40\x81") == "\"\\u{3000}\x40\\x{81}\"");
}

int main() {
    test_escaped_string();
}
