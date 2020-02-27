// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <string>

int main() {
    try {
        throw std::string("meow");
    } catch (...) {
    }
}
