// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cmath>
#include <cstddef>
#include <random>
#include <vector>

using namespace std;

template <class Generator>
void test_binomial(const int n, const double mean, Generator& gen) {
    const double p       = mean / n;
    const double var     = n * p * (1.0 - p);
    constexpr double tol = 0.01;
    constexpr double x   = 4.0; // Standard deviation of sample variance should be less than tol / x.
    const size_t it_max  = 2 * static_cast<size_t>(pow(var / (tol / x), 2.0));
    binomial_distribution<> dist(n, p);

    vector<size_t> counts(static_cast<size_t>(n) + 1);
    for (size_t i = 0; i < it_max; ++i) {
        ++counts[static_cast<size_t>(dist(gen))];
    }

    double sample_mean = 0.0;
    for (size_t i = 1; i < counts.size(); ++i) {
        sample_mean += static_cast<double>(i) * counts[i];
    }
    sample_mean /= it_max;

    double sample_var = 0.0;
    for (size_t i = 0; i < counts.size(); ++i) {
        sample_var += counts[i] * pow(i - sample_mean, 2);
    }
    sample_var /= it_max - 1;

    assert(abs(sample_mean / mean - 1.0) < tol);
    assert(abs(sample_var / var - 1.0) < tol);
}

int main() {
    mt19937 gen;
    constexpr int n = 25;
    test_binomial(n, 0.99, gen);
    test_binomial(n, n - 0.99, gen);
    return 0;
}
