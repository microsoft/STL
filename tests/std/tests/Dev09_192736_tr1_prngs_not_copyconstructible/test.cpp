// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>
#include <random>
#include <vector>

using namespace std;

template <typename Gen>
void test_generator_can_be_value_direct_initialized() {
    Gen gen;

    Gen other(gen);

    assert(other == gen);
}

template <typename Gen>
void test_generator_can_be_const_ref_direct_initialized() {
    Gen gen;
    const Gen& r = gen;

    Gen other(r);

    assert(other == gen);
}

template <typename Gen>
void test_generator_can_be_value_copy_initialized() {
    Gen gen;

    Gen other = gen;

    assert(other == gen);
}

template <typename Gen>
void test_generator_can_be_const_ref_copy_initialized() {
    Gen gen;
    const Gen& r = gen;

    Gen other = r;

    assert(other == gen);
}

template <typename Gen, typename ReseedFunc>
void test_generator_can_be_reseeded(ReseedFunc reseedFunc) {
    const auto iterations = 100u;
    Gen gen;
    reseedFunc(gen);
    vector<typename Gen::result_type> firstPassResults;
    generate_n(back_inserter(firstPassResults), iterations, ref(gen));
    reseedFunc(gen);
    vector<typename Gen::result_type> seededResults;
    generate_n(back_inserter(seededResults), iterations, ref(gen));
    assert(firstPassResults == seededResults);
}

template <typename Gen>
void test_engine() {
    test_generator_can_be_value_direct_initialized<Gen>();
    test_generator_can_be_const_ref_direct_initialized<Gen>();
    test_generator_can_be_value_copy_initialized<Gen>();
    test_generator_can_be_const_ref_copy_initialized<Gen>();
    test_generator_can_be_reseeded<Gen>([](Gen& gen) { gen.seed(); });
    test_generator_can_be_reseeded<Gen>([](Gen& gen) { gen.seed(1729); });
}

template <typename Dist>
void test_distribution_can_be_reset(Dist dist) {
    knuth_b gen;
    const auto iterations = 100u;
    vector<typename Dist::result_type> firstPassResults;
    generate_n(back_inserter(firstPassResults), iterations, [&]() { return dist(gen); });
    dist.reset();
    gen.seed();
    vector<typename Dist::result_type> resetResults;
    generate_n(back_inserter(resetResults), iterations, [&]() { return dist(gen); });
    assert(firstPassResults == resetResults);
}

template <typename Dist>
void test_distribution(Dist dist) {
    test_distribution_can_be_reset(dist);
}

template <template <typename> class Dist>
void test_integral_distribution() {
    test_distribution(Dist<unsigned short>{});
    test_distribution(Dist<unsigned int>{});
    test_distribution(Dist<unsigned long>{});
    test_distribution(Dist<unsigned long long>{});
    test_distribution(Dist<short>{});
    test_distribution(Dist<int>{});
    test_distribution(Dist<long>{});
    test_distribution(Dist<long long>{});
}

template <template <typename> class Dist>
void test_real_distribution() {
    test_distribution(Dist<float>{});
    test_distribution(Dist<double>{});
    test_distribution(Dist<long double>{});
}

int main() {
    // N4527 26.5.5 [rand.predef], in paragraph order:
    test_engine<minstd_rand0>(); // /1
    test_engine<minstd_rand>();
    test_engine<mt19937>();
    test_engine<mt19937_64>();
    test_engine<ranlux24_base>(); // /5
    test_engine<ranlux48_base>();
    test_engine<ranlux24>();
    test_engine<ranlux48>();
    test_engine<knuth_b>();
    test_engine<default_random_engine>(); // /10

    // independent_bits_engine is not in any of the [rand.predef] engines, so
    // test it explicitly:
    test_engine<independent_bits_engine<mt19937_64, 5, unsigned int>>();
    // random_device can't be copied or reseeded, so no reason to test it here.

    test_integral_distribution<uniform_int_distribution>();
    test_real_distribution<uniform_real_distribution>();
    test_distribution(bernoulli_distribution{});
    test_integral_distribution<binomial_distribution>();
    test_integral_distribution<geometric_distribution>();
    test_integral_distribution<negative_binomial_distribution>();
    test_integral_distribution<poisson_distribution>();
    test_real_distribution<exponential_distribution>();
    test_real_distribution<gamma_distribution>();
    test_real_distribution<weibull_distribution>();
    test_real_distribution<extreme_value_distribution>();
    test_real_distribution<normal_distribution>();
    test_real_distribution<lognormal_distribution>();
    test_real_distribution<chi_squared_distribution>();
    test_real_distribution<cauchy_distribution>();
    test_real_distribution<fisher_f_distribution>();
    test_real_distribution<student_t_distribution>();
    test_distribution(discrete_distribution<unsigned short>({1, 2, 3, 4, 5}));
    test_distribution(discrete_distribution<unsigned int>({1, 2, 3, 4, 5}));
    test_distribution(discrete_distribution<unsigned long>({1, 2, 3, 4, 5}));
    test_distribution(discrete_distribution<unsigned long long>({1, 2, 3, 4, 5}));
    test_distribution(discrete_distribution<short>({1, 2, 3, 4, 5}));
    test_distribution(discrete_distribution<int>({1, 2, 3, 4, 5}));
    test_distribution(discrete_distribution<long>({1, 2, 3, 4, 5}));
    test_distribution(discrete_distribution<long long>({1, 2, 3, 4, 5}));

    vector<float> rangesf{0.0f, 10.0f, 90.0f, 100.0f};
    vector<float> weightsf{1.0f, 0.0f, 0.0f, 1.0f};
    test_distribution(piecewise_constant_distribution<float>(begin(rangesf), end(rangesf), begin(weightsf)));
    test_distribution(piecewise_linear_distribution<float>(begin(rangesf), end(rangesf), begin(weightsf)));

    vector<double> ranges{0.0, 10.0, 90.0, 100.0};
    vector<double> weights{1.0, 0.0, 0.0, 1.0};
    test_distribution(piecewise_constant_distribution<double>(begin(ranges), end(ranges), begin(weights)));
    test_distribution(piecewise_linear_distribution<double>(begin(ranges), end(ranges), begin(weights)));

    vector<long double> rangesl{0.0l, 10.0l, 90.0l, 100.0l};
    vector<long double> weightsl{1.0l, 0.0l, 0.0l, 1.0l};
    test_distribution(piecewise_constant_distribution<long double>(begin(rangesl), end(rangesl), begin(weightsl)));
    test_distribution(piecewise_linear_distribution<long double>(begin(rangesl), end(rangesl), begin(weightsl)));
}
