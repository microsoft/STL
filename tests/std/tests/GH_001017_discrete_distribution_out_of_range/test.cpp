// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <random>

#include <bad_random_engine.hpp>

int main() {
    std::discrete_distribution<int> dist{1, 1, 1, 1, 1, 1};
    bad_random_generator rng;

    while (!rng.has_cycled_through()) {
        const auto rand_value = dist(rng);
        assert(0 <= rand_value && rand_value < 6);
    }
}
