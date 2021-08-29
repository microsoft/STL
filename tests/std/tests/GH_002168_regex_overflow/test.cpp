// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <regex>

using namespace std;

int main() {
    for (const char* regexString : {"\\3333333334", "\\2147483648"}) {
        try {
            regex testRegex{regexString, regex_constants::ECMAScript};
            assert(false);
        } catch (const regex_error& e) {
            assert(e.code() == regex_constants::error_backref);
        }
    }
}
