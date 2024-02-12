// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef _MSVC_INTERNAL_TESTING // TRANSITION, VS 2022 17.10 Preview 1
#include <__msvc_all_public_headers.hpp>
#else // ^^^ no workaround / workaround vvv
#include <assert.h> // intentionally not <cassert>
#endif // ^^^ workaround ^^^

import std.compat;

// INTENTIONALLY AVOIDED: using namespace std;

void test_include_all_then_import_std_compat() {
    // Verify that std::vector and std::ranges algorithms are available:
    std::vector<int> v{31, 41, 59, 26, 53, 58, 97, 93};
    assert(!std::ranges::is_sorted(v));
    std::ranges::sort(v);
    assert(std::ranges::is_sorted(v));
    const std::vector<int> sorted{26, 31, 41, 53, 58, 59, 93, 97};
    assert(v == sorted);

    // Verify that the Sufficient Additional Overloads are available for std::sqrt():
    assert(std::sqrt(25.0) == 5.0);
    assert(std::sqrt(25.0f) == 5.0f);
    assert(std::sqrt(25) == 5.0);
    static_assert(std::is_same_v<decltype(std::sqrt(25.0)), double>);
    static_assert(std::is_same_v<decltype(std::sqrt(25.0f)), float>);
    static_assert(std::is_same_v<decltype(std::sqrt(25)), double>);

    // Verify that the Sufficient Additional Overloads are available for ::sqrt():
    assert(::sqrt(25.0) == 5.0);
    assert(::sqrt(25.0f) == 5.0f);
    assert(::sqrt(25) == 5.0);
    static_assert(std::is_same_v<decltype(::sqrt(25.0)), double>);
    static_assert(std::is_same_v<decltype(::sqrt(25.0f)), float>);
    static_assert(std::is_same_v<decltype(::sqrt(25)), double>);
}
