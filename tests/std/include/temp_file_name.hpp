// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <random>
#include <string>

[[nodiscard]] inline std::string temp_file_name() {
    std::string ret{"msvc_stl_"};
    std::uniform_int_distribution<int> dist{0, 15};
    std::random_device rd;

    for (int i = 0; i < 32; ++i) { // 32 hexits = 128 bits of entropy
        ret.push_back("0123456789ABCDEF"[dist(rd)]);
    }

    ret += ".tmp";

    return ret;
}
