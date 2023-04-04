// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <random>

void test() {
    // GH-3105 "<random>: std::piecewise_constant_distribution<float>::densities() gives warning C4244"
    (void) std::piecewise_constant_distribution<float>{}.densities();

    // Also affected:
    (void) std::piecewise_linear_distribution<float>{}.densities();

    // Not affected, but tested just in case:
    (void) std::piecewise_constant_distribution<float>{}.intervals();
    (void) std::piecewise_linear_distribution<float>{}.intervals();
}
