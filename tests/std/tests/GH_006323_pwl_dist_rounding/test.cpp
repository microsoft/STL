// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <random>
#include <vector>

using namespace std;

int main() {
    constexpr double delta       = 1.0e-15;
    static constexpr double b[2] = {0.0, 1.0};
    static constexpr double p[2] = {1.0 - delta, 1.0 + delta};
    piecewise_linear_distribution<double> dist(b, b + 2, p);

    mt19937_64 urbg;

    constexpr size_t N = 100'000;
    vector<double> samples(N);
    for (size_t i = 0; i < N; ++i) {
        samples[i] = dist(urbg);
    }

    sort(samples.begin(), samples.end());

    // Given the empirical CDF for $n$ samples, $F_n(x)$, and a hypothesized CDF $F(x)$, the Kolmogorov-Smirnov
    // statistic is $D_n = \sup_x |F_n(x)-F(x)|$. For large samples, reject the null hypothesis (that the samples come
    // from the distribution) if $\sqrt{n}D_n > K(p)$.

    // Both $F_n$ and $F$ are monotone and $F_n$ is piecewise constant, so the extreme value occurs at one of the
    // discontinuities.

    double ks_stat = 0.0;
    for (size_t i = 0; i < N;) {
        const double x       = samples[i];
        const double ecdf_lo = i / static_cast<double>(N);

        while (++i < N && samples[i] == x) {
        }

        const double ecdf_hi = i / static_cast<double>(N);
        const double cdf     = x + x * (x - 1.0) * delta;

        const double max_diff = max(abs(cdf - ecdf_lo), abs(cdf - ecdf_hi));
        if (max_diff > ks_stat) {
            ks_stat = max_diff;
        }
    }

    constexpr double crit_val = 1.224; // critical value for p = 0.1
    assert(sqrt(N) * ks_stat < crit_val);

    return 0;
}
