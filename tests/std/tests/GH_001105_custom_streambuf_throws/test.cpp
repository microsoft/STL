// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <istream>
#include <stdexcept>

struct bad_buf : std::streambuf {
    bad_buf() {
        throw std::runtime_error("throw in constructor!");
    }
};

struct custom_stream : std::istream {
    custom_stream() : std::istream(new bad_buf{}) {}
};

int main() {

    {
        // GH-1105 std::istream destructor should not crash if custom streambuf implementation throws.
        try {
            custom_stream f{};
        } catch (const std::runtime_error&) {
            assert(true);
        }
    }

    return 0;
}
