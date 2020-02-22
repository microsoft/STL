// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <type_traits>
using namespace std;

const bool* test2_int_int() {
    return &is_same_v<int, int>;
}

const bool* test2_short_long() {
    return &is_same_v<short, long>;
}
