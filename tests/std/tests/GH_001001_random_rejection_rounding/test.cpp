// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cmath>
#include <cstdint>
#include <map>
#include <random>

void test_gh_1001() {
    // GH-1001 <random>: binomial_distribution is broken
    constexpr int N{1000};
    constexpr double p{.001238};
    constexpr int seed{12345};
    constexpr int iters{1'000'000};
    std::map<int, int> frequency;

    std::mt19937 mt_rand(seed);

    std::binomial_distribution<int> distribution(N, p);

    for (int i = 0; i < iters; ++i) {
        ++frequency[distribution(mt_rand)];
    }

    double mean_x{0.0};
    for (const auto& valueCountPair : frequency) {
        mean_x += valueCountPair.first * static_cast<double>(valueCountPair.second) / iters;
    }
    const double p0_x{static_cast<double>(frequency[0]) / iters};
    const double p1_x{static_cast<double>(frequency[1]) / iters};

    const double p0{std::pow(1.0 - p, static_cast<double>(N))};
    const double p1{1000.0 * p * std::pow(1.0 - p, static_cast<double>(N - 1))};
    const double mean{p * N};

    assert(std::abs(mean_x / mean - 1.0) < 0.01);
    assert(std::abs(p0_x / p0 - 1.0) < 0.01);
    assert(std::abs(p1_x / p1 - 1.0) < 0.01);
}

int main() {
    test_gh_1001();
}
