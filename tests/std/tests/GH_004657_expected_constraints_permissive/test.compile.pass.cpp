// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <expected>
#include <locale>
#include <string>
#include <utility>

using namespace std;

expected<int, string> test_expected_int_constraints() {
    return 42;
}

expected<pair<int, int>, string> test_expected_pair_constraints() {
    return pair<int, int>{42, 1729};
}

expected<void, string> test_expected_void_constraints() {
    return {};
}

unexpected<string> test_unexpected_constraints() {
    return unexpected<string>{string{}};
}
