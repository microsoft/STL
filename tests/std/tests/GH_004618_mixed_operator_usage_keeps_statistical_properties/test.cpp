// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <random>
#include <vector>

constexpr std::size_t N = 1000000;

void check_results(const std::vector<double>& generated, const double expected_mean, const double expected_variance) {
    const double actual_mean = std::accumulate(generated.begin(), generated.end(), 0.0) / generated.size();
    assert(std::abs((actual_mean - expected_mean) / expected_mean) < 0.01);

    double actual_variance = 0;
    double actual_skew     = 0;
    for (const auto& value : generated) {
        const auto deviation = value - actual_mean;
        actual_variance += deviation * deviation;
        actual_skew += deviation * deviation * deviation;
    }
    actual_variance /= generated.size();
    assert(std::abs((actual_variance - expected_variance) / expected_variance) < 0.01);

    actual_skew /= generated.size() * actual_variance * std::sqrt(actual_variance);
    assert(std::abs(actual_skew) < 0.01);
}

int main() {
    // The basic idea is to generate values from two different distributions,
    // one defined through the type's constructor, one through the params
    // overload of operator(). The generated values are then checked for
    // their expected statistical properties.
    std::mt19937 rng;
    std::normal_distribution<> dist(5.0, 4.0);
    using dist_params = std::normal_distribution<>::param_type;
    const dist_params params(50.0, 0.5);
    std::vector<double> dist_results;
    dist_results.reserve(N);
    std::vector<double> param_results;
    param_results.reserve(N);

    // Make sure that we get some first and some second values for both
    // generated distributions
    for (std::size_t i = 0; i < N; i += 2) {
        dist_results.push_back(dist(rng));
        param_results.push_back(dist(rng, params));
        param_results.push_back(dist(rng, params));
        dist_results.push_back(dist(rng));
    }

    check_results(dist_results, dist.mean(), dist.stddev() * dist.stddev());
    check_results(param_results, params.mean(), params.stddev() * params.stddev());
}
