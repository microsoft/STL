// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <clocale>
#include <iomanip>
#include <iostream>
#include <string>

std::string set_locale(const std::string& locale_name) {
    const char* ret = std::setlocale(LC_ALL, locale_name.c_str());
    assert(ret != nullptr);
    return ret;
}

std::string query_locale() {
    const char* ret = std::setlocale(LC_ALL, nullptr);
    assert(ret != nullptr);
    return ret;
}

void assert_string_non_ascii(const std::string& str) {
    const auto char_not_ascii = [](const char c) { return (c & 0x80) != 0; };
    assert(std::any_of(str.begin(), str.end(), char_not_ascii));
}

void test_gh_5780() {
    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/language-strings#supported-language-strings
    std::string locale_name = set_locale("norwegian-bokmal.437");
    assert_string_non_ascii(locale_name);

    std::cerr.imbue(std::locale::classic());
    std::cerr << std::setprecision(2) << 0.1 << std::endl;

    assert(query_locale() == locale_name);
}

int main() {
    test_gh_5780();
}
