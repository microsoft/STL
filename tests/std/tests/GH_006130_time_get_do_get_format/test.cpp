// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <ctime>
#include <iomanip>
#include <sstream>

int main() {
    {
        std::tm t{};
        std::stringstream ss{"%"};
        ss >> std::get_time(&t, "%%");
        assert(!ss.fail() && ss.eof());
    }
    {
        std::tm t{};
        std::stringstream ss{"% "};
        ss >> std::get_time(&t, "%%");
        assert(!ss.fail() && ss.tellg() == 1);
    }
}
