// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <random>

int main() {
    (void) std::piecewise_constant_distribution<float>{}.densities();
}
