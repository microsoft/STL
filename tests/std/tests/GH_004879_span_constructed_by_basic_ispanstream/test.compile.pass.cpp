// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// GH-4879 <spanstream>: The span constructed by basic_ispanstream's range constructor may be ill-formed

#include <spanstream>

struct R {
    char* begin();
    std::unreachable_sentinel_t end();
    operator std::span<const char>();
};

int main() {
    R r{};
    std::basic_ispanstream<char> is{ r };
    is.span(r);
}
