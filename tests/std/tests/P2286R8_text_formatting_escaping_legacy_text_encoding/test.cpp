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

template <class TupleOrPair>
void test_tuple_or_pair_escaping(TupleOrPair&& input) {
    get<1>(input) = "hell\uff2f"; // U+FF2F FULLWIDTH LATIN CAPITAL LETTER O
    assert(format("{}", input) == "('*', \"hell\uff2f\")");
    assert(format("{:#^16}", input) == "('*', \"hell\uff2f\")#");
}

int main() {
    test_escaped_string();
    test_tuple_or_pair_escaping(make_pair('*', ""));
    test_tuple_or_pair_escaping(make_tuple('*', ""));
}
