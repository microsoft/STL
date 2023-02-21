// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <format>
#include <string>

using namespace std;

// GH-2558 <format>: Bogus compiler errors with /presetPadding
string test_gh_2558() {
    return format("I have {} cute {} kittens.\n", 1729, "fluffy");
}
