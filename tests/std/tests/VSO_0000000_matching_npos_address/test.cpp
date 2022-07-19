// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <string>

extern "C" {
extern const size_t* other_npos_address;
}

int main() {
    static_assert(std::string::npos == static_cast<size_t>(-1), "");
    assert(&std::string::npos == other_npos_address);
}
