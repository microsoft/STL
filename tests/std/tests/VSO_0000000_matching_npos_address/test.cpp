// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstddef>
#include <string>

using namespace std;

extern "C" {
extern const size_t* other_npos_address;
}

int main() {
    static_assert(string::npos == static_cast<size_t>(-1), "");
    assert(&string::npos == other_npos_address);
}
