// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstdlib>
#include <print>

#include <Windows.h>

int main() {
    std::print("SVE: {}", IsProcessorFeaturePresent(PF_ARM_SVE_INSTRUCTIONS_AVAILABLE));
    std::print("SVE 2: {}", IsProcessorFeaturePresent(PF_ARM_SVE2_INSTRUCTIONS_AVAILABLE));
    std::print("SVE 2.1: {}", IsProcessorFeaturePresent(PF_ARM_SVE2_1_INSTRUCTIONS_AVAILABLE));
    std::abort();
}
