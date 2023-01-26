// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <random>

using namespace std;

template <class Generator>
bool basic_test() {
    constexpr auto maximum     = 1ull << 60;
    constexpr auto num_bins    = 20ull; // Don't change this without looking up a new threshold below.
    constexpr auto bin_width   = maximum / num_bins; // except possibly the last bin
    constexpr auto bin_freq    = static_cast<double>(bin_width) / maximum;
    constexpr auto freq_rem    = static_cast<double>(maximum % bin_width) / maximum;
    constexpr double threshold = 31.410; // chi-squared critical value for d.f. = 20 and p = 0.05

    Generator gen;
    uniform_int_distribution<uint64_t> dist(0, maximum - 1u);


    const int N             = 20'000;
    int frequency[num_bins] = {};
    for (int i = 0; i < N; ++i) {
        ++frequency[min(dist(gen) / bin_width, num_bins - 1u)];
    }

    double chi_squared = 0.0;
    for (unsigned int i = 0; i < num_bins; ++i) {
        const auto expected = (bin_freq + (i == num_bins - 1u ? freq_rem : 0.0)) * N;
        const auto delta    = static_cast<double>(frequency[i] - expected);
        chi_squared += delta * delta / expected;
    }

    return chi_squared <= threshold;
}

bool test_modulus_bias() {
    // This test is designed to detect modulus bias. When generating random intergers in [0,s) with a URBG having range
    // [0,R), then R mod s values must be rejected to ensure uniformity. By making (R mod s)/R large, we can introduce a
    // large bias if the rejection is incorrect.

    constexpr int maximum      = 5; // Don't change this without looking up a new threshold below.
    constexpr double threshold = 11.07; // chi-squared critical value for d.f. = 5 and p = 0.05
    uniform_int_distribution<> rng(0, maximum - 1);
    independent_bits_engine<mt19937, 3, uint32_t> gen;

    const int N            = 1'000;
    int frequency[maximum] = {};
    for (int i = 0; i < N; ++i) {
        ++frequency[rng(gen)];
    }

    double chi_squared = 0.0;
    for (int i = 0; i < maximum; ++i) {
        const double expected = static_cast<double>(N) / maximum;
        const double delta    = frequency[i] - expected;
        chi_squared += delta * delta / expected;
    }

    return chi_squared <= threshold;
}

int main() {
    // Four cases tested below:
    // (1) URBG provides enough bits to completely fill the underlying type
    // (2) URBG provides enough bits for our upper bound, but not enough to fill the type
    // (3) URBG is called multiple times, but doesn't fill the type
    // (4) URBG is called multiple times and overflows the number of bits in the type
    assert((basic_test<mt19937_64>()));
    assert((basic_test<independent_bits_engine<mt19937_64, 61, uint64_t>>()));
    assert((basic_test<independent_bits_engine<mt19937, 31, uint32_t>>()));
    assert((basic_test<independent_bits_engine<mt19937, 25, uint32_t>>()));

    assert(test_modulus_bias());

    return 0;
}
