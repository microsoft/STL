// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <random>

#include "bad_random_engine.hpp"

template <class Distribution>
void Test_for_NaN_Inf(Distribution&& distribution) {
    for (bad_random_generator rng; !rng.has_cycled_through();) {
        const auto rand_value = distribution(rng);
        assert(!isnan(rand_value) && !isinf(rand_value));
    }
}

template <class T>
void Test_distributions() {
    // Additionally test GH-1174 "<random>: Some random number distributions could return NaN"
    Test_for_NaN_Inf(std::normal_distribution<T>{});
    Test_for_NaN_Inf(std::lognormal_distribution<T>{});
    Test_for_NaN_Inf(std::fisher_f_distribution<T>{});
    Test_for_NaN_Inf(std::student_t_distribution<T>{});
}

int main() {
    std::discrete_distribution<int> dist{1, 1, 1, 1, 1, 1};
    bad_random_generator rng;

    while (!rng.has_cycled_through()) {
        const auto rand_value = dist(rng);
        assert(0 <= rand_value && rand_value < 6);
    }

    Test_distributions<float>();
    Test_distributions<double>();
    Test_distributions<long double>();
}
