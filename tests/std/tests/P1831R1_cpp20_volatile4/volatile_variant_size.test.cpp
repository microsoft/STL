// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <variant>

int main() {
    static_assert(std::variant_size_v<volatile std::variant<int>> == 1);
}
