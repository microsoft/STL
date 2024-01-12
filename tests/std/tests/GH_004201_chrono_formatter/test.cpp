// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <chrono>
#include <format>
#include <string>

using namespace std::literals::chrono_literals;

int main() {
    assert(std::format("[{:20%T}]", 314159s) == "[87:15:59            ]");

    // std::formatter specializations for <chrono> types used to ignore dynamically provided width
    assert(std::format("[{:{}%T}]", 314159s, 20) == "[87:15:59            ]");
}
