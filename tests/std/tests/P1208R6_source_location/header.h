// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#include <cassert>
#include <source_location>
#include <string_view>

using namespace std;

constexpr void header_test() {
    const auto x = source_location::current();
    assert(x.line() == __LINE__ - 1);
    assert(x.column() == 14);
    assert(x.function_name() == "header_test"sv);
    assert(string_view{x.file_name()}.ends_with("header.h"sv)); // TRANSITION LLVM-45731
}
