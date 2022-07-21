// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <sstream>

int main() {
    {
        std::stringstream ss;
        int i = 0;

        // the C locale should not have any numeric grouping defined,
        // so the comma after 2 should not be confused with a thousand
        // separator.
        ss << "2,";

        // imbuing the "C" locale workaround the problem
        // ss.imbue(std::locale("C"));

        // reading from the stream should not fail
        assert(ss >> i);
    }

    {
        // test for floats

        std::stringstream ss;
        float f = 0;

        // the C locale should not have any numeric grouping defined,
        // so the comma after 2 should not be confused with a thousand
        // separator.
        ss << "2,";

        // reading from the stream should not fail
        assert(ss >> f);
    }
}
