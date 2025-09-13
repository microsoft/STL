// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <ios>
#include <sstream>
using namespace std;

enum class Parse { Success, Failure };

struct TestCase {
    const char* str;
    ios_base::fmtflags flags;
    Parse expected;
    bool result;
};

constexpr TestCase test_cases[] = {
    {"0", ios_base::fmtflags{}, Parse::Success, false}, //
    {"1", ios_base::fmtflags{}, Parse::Success, true}, //
    {"2", ios_base::fmtflags{}, Parse::Failure, true}, // N4868 [facet.num.get.virtuals]/6
    {"WOOF", ios_base::fmtflags{}, Parse::Failure, false}, // N4868 [facet.num.get.virtuals]/3.6
    {"false", ios_base::boolalpha, Parse::Success, false}, //
    {"true", ios_base::boolalpha, Parse::Success, true}, //
    {"WOOF", ios_base::boolalpha, Parse::Failure, false}, // N4868 [facet.num.get.virtuals]/7
    {"FALSE", ios_base::boolalpha, Parse::Failure, false}, // GH-1541
    {"TRUE", ios_base::boolalpha, Parse::Failure, false}, // GH-1541
};

int main() {
    for (const auto& test : test_cases) {
        bool val = !test.result;
        istringstream iss(test.str);
        iss.setf(test.flags);
        iss >> val;
        assert(iss.fail() == (test.expected == Parse::Failure));
        assert(val == test.result);
    }
}
