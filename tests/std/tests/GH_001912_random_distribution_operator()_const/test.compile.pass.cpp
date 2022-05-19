// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _ALLOW_RANDOM_DISTRIBUTION_CONST_OPERATOR

#include <random>

using namespace std;

template <typename Dist>
void test() {
    default_random_engine random;
    const Dist dist;
    const auto res = dist(random);
    (void) res;
}

void test_every_distribution() {
    test<uniform_int_distribution<int>>();
    test<uniform_real_distribution<double>>();
    test<bernoulli_distribution>();
    test<binomial_distribution<int>>();
    test<geometric_distribution<int>>();
    test<negative_binomial_distribution<int>>();
    test<poisson_distribution<int>>();
    test<exponential_distribution<double>>();
    test<gamma_distribution<double>>();
    test<weibull_distribution<double>>();
    test<extreme_value_distribution<double>>();
    // normal_distribution doesn't have const operator()
    test<lognormal_distribution<double>>();
    test<chi_squared_distribution<double>>();
    test<cauchy_distribution<double>>();
    test<fisher_f_distribution<double>>();
    test<student_t_distribution<double>>();
    test<discrete_distribution<int>>();
    test<piecewise_constant_distribution<double>>();
    test<piecewise_linear_distribution<double>>();
}

int main() {} // COMPILE-ONLY
