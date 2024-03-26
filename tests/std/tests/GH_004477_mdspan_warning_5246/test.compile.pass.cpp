// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cstddef>
#include <mdspan>

using namespace std;

// Test GH-4477 "Using std::mdspan results in multiple warnings (C5246)"
void test_gh_4477() {
    array test_data{0, 1, 2, 3, 4, 5};
    mdspan<int, dextents<size_t, 2>> test_span(test_data.data(), 2, 3);
    (void) test_span;
}
