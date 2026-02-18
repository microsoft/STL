// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <clocale>
#include <iomanip>
#include <iostream>
#include <locale>
#include <string>
using namespace std;

string set_locale(const string& locale_name) {
    const char* ret = setlocale(LC_ALL, locale_name.c_str());
    assert(ret != nullptr);
    return ret;
}

string query_locale() {
    const char* ret = setlocale(LC_ALL, nullptr);
    assert(ret != nullptr);
    return ret;
}

void assert_string_non_ascii(const string& str) {
    const auto char_not_ascii = [](const char c) { return (c & 0x80) != 0; };
    assert(any_of(str.begin(), str.end(), char_not_ascii));
}

// GH-5780 <iostream>: Printing with std::cout changes the global locale permanently
void test_gh_5780() {
    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/language-strings#supported-language-strings
    string locale_name = set_locale("norwegian-bokmal.437");
    assert_string_non_ascii(locale_name);

    cerr.imbue(locale::classic());
    cerr << setprecision(2) << 0.1 << endl;

    assert(query_locale() == locale_name);
}

int main() {
    test_gh_5780();
}
