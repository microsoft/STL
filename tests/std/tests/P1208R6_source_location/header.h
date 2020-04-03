// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#include <cassert>
#include <source_location>
#include <string_view>

using namespace std;

constexpr void header_test() {
    auto x = source_location::current();
    assert(x.line() == __LINE__ - 1);
    assert(x.column() == 31);
    assert(x.function_name() == "header_test"sv);
    assert(string_view{x.file_name()}.ends_with(R"(tests\std\tests\P1208R6_source_location\header.h)"sv));
}
