// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cmath>
#include <random>
#include <vector>

using namespace std;

int main() {
    constexpr int it_max  = 100'000;
    constexpr int n       = 25;
    constexpr double mean = 0.99;
    constexpr double p    = mean / n;
    constexpr double var  = n * p * (1.0 - p);

    mt19937 gen;
    binomial_distribution<> dist(n, p);

    vector<int> counts(n + 1);
    for (int i = 0; i < it_max; ++i) {
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

    assert(abs(sample_mean / mean - 1.0) < 0.01);
    assert(abs(sample_var / var - 1.0) < 0.01);
    return 0;
}
