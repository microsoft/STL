// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// #include <cctype>
#include <string>

// This file tests a nonstandard assumption that <string> includes <cctype>.

int main() {
    if (std::tolower('a') != 'a') {
        return 1;
    }

    if (::tolower('a') != 'a') {
        return 1;
    }
}
