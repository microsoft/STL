// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <string>

extern "C" {
const size_t* other_npos_address;
}

struct init_npos_address {
    init_npos_address() {
        other_npos_address = &std::string::npos;
    }
};

static init_npos_address before_main;
