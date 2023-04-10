// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <functional>
#include <string>

using namespace std;

void test_case_hash_string(const string& a, const string& b, const string& c) {
    const auto x = hash<string>()(a);
    const auto y = hash<string>()(b);
    const auto z = hash<string>()(c);

    assert(x != y && x != z && y != z);
}

void test_hash_string() {
    test_case_hash_string("aa", "a5", "az");

    test_case_hash_string("cata", "cat5", "catz");

    test_case_hash_string("fluffykittens", "fluffykitte47", "fluffykitte!!");
}

// P0272R1 "Give basic_string a non-const data()"
void test_non_const_data() {
#if _HAS_CXX17
    string s;
    assert(*s.data() == '\0');
    s.resize(2);
    s.data()[0] = 'a';
    s.data()[1] = 'b';
    assert(s == "ab");
#endif // _HAS_CXX17
}

void test_string_find_character_offset() {
    // smoke test for string::find(character, offset)
    string s;
    assert(s.find('h') == string::npos);
    assert(s.find('h', 42) == string::npos);
    s = "hungry EViL zombies";
    //   0123456789012345678
    //             111111111
    assert(s.find('L') == 10);
    assert(s.find('A') == string::npos);
    assert(s.find('i') == 9);
    assert(s.find('i', 8) == 9);
    assert(s.find('i', 9) == 9);
    assert(s.find('i', 10) == 16);
    assert(s.find('i', 17) == string::npos);
}

int main() {
    test_hash_string();
    test_non_const_data();
    test_string_find_character_offset();
}
