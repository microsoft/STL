// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <variant>

int main() {
    static_assert(std::is_same_v<std::variant_alternative_t<0, volatile std::variant<int>>, volatile int>);
}
