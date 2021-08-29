// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <regex>

using namespace std;

bool shouldThrow(const char* const regexString) {
    try {
        regex regex{regexString, regex_constants::ECMAScript};
        return false;
    } catch (const regex_error& e) {
        return e.code() == regex_constants::error_backref;
    }
}

int main() {
    assert(shouldThrow("\\3333333334"));
    assert(shouldThrow("\\2147483648"));
}
