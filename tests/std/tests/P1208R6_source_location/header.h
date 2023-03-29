// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#include <cassert>
#include <source_location>
#include <string_view>

constexpr void header_test() {
    using namespace std;
    const auto x = source_location::current();
    assert(x.line() == __LINE__ - 1);
#ifdef __clang__
    assert(x.column() == 20);
#else // ^^^ defined(__clang__) / !defined(__clang__) vvv
    assert(x.column() == 37);
#endif // ^^^ !defined(__clang__) ^^^
#if defined(__clang__) || defined(__EDG__) // TRANSITION, DevCom-10199227 and LLVM-58951
    assert(x.function_name() == "header_test"sv);
#else // ^^^ workaround / no workaround vvv
    assert(x.function_name() == "void __cdecl header_test(void)"sv);
#endif // TRANSITION, DevCom-10199227 and LLVM-58951
    assert(string_view{x.file_name()}.ends_with("header.h"sv));
}
