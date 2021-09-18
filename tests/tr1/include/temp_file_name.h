// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <random>
#include <string>

[[nodiscard]] inline std::string temp_file_name() {
    std::string ret{"temp_file_"};

    std::random_device rd;

    for (int i = 0; i < 64; ++i) { // 64 hexits = 256 bits of entropy
        ret.push_back("0123456789ABCDEF"[rd() % 16]);
    }

    ret += ".tmp";

    return ret;
}
