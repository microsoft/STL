// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <regex>

using namespace std;

// GH-2168 <regex>: integer overflow on large backreference value
int main() {
    try {
        // 4294967297 = 1 mod 2^32, so this will succeed if we don't check for overflow.
        regex testRegex{R"((a)\4294967297)", regex_constants::ECMAScript};
        assert(false);
    } catch (const regex_error& e) {
        assert(e.code() == regex_constants::error_backref);
    }

    try {
        regex testRegex{"a{100000000000000000}", regex_constants::ECMAScript};
        assert(false);
    } catch (const regex_error& e) {
        assert(e.code() == regex_constants::error_badbrace);
    }

    try {
        regex testRegex{"a{100,10000000000000000}", regex_constants::ECMAScript};
        assert(false);
    } catch (const regex_error& e) {
        assert(e.code() == regex_constants::error_badbrace);
    }

    try {
        // 4294967296 = 0 mod 2^32, so this will succeed if we don't check for overflow.
        regex testRegex{"[1-\\4294967296]", regex_constants::ECMAScript};
        assert(false);
    } catch (const regex_error& e) {
        assert(e.code() == regex_constants::error_escape);
    }
}
