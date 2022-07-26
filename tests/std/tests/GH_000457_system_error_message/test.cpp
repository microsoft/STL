// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <yvals.h>

#include <__msvc_system_error_abi.hpp>
#include <cassert>

int main() {
    char example[] = "test string \r\n\t\0 test test";
    //                000000000011 1 1 1 11111222222
    //                012345678901 2 3 4 56789012345
    assert(__std_get_string_size_without_trailing_whitespace(example, 0) == 0);
    assert(__std_get_string_size_without_trailing_whitespace(example, 4) == 4);
    assert(__std_get_string_size_without_trailing_whitespace(example, 5) == 4);
    assert(__std_get_string_size_without_trailing_whitespace(example, 16) == 11);
    assert(__std_get_string_size_without_trailing_whitespace(example, 17) == 11);
    assert(__std_get_string_size_without_trailing_whitespace(example, 18) == 18);
    assert(__std_get_string_size_without_trailing_whitespace(example, sizeof(example)) == 26);

    char allWhitespace[] = "\r\n\t\0 ";
    assert(__std_get_string_size_without_trailing_whitespace(allWhitespace, sizeof(allWhitespace)) == 0);
}
