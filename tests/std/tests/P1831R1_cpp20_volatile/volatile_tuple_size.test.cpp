// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <tuple>

int main() {
    static_assert(std::tuple_size_v<volatile std::tuple<int>> == 1);
}
