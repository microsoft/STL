// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <string>

int main() {
    std::string t = "0123456789ABCDEF"; // large string
    t.replace(0, 30, 7, 'A');
}
