// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <random>
#include <type_traits>
#include <vector>
using namespace std;

enum class alg_type { std_fn, rng };

template <class T, alg_type Alg>
void bm_sample(benchmark::State& state) {
    static_assert(is_unsigned_v<T>, "T must be unsigned so iota() doesn't have to worry about overflow.");

    const auto population_size = static_cast<size_t>(state.range(0));
    const auto sampled_size    = static_cast<size_t>(state.range(1));

    vector<T> population(population_size);
    vector<T> sampled(sampled_size);
    iota(population.begin(), population.end(), T{0});
    mt19937_64 urbg;

    for (auto _ : state) {
        benchmark::DoNotOptimize(population);
        if constexpr (Alg == alg_type::rng) {
            ranges::sample(population, sampled.begin(), sampled_size, urbg);
        } else {
            sample(population.begin(), population.end(), sampled.begin(), sampled_size, urbg);
        }
        benchmark::DoNotOptimize(sampled);
    }
}

void common_args(auto bm) {
    bm->Args({1 << 20, 1 << 15});
}

BENCHMARK(bm_sample<uint8_t, alg_type::std_fn>)->Apply(common_args);
BENCHMARK(bm_sample<uint16_t, alg_type::std_fn>)->Apply(common_args);
BENCHMARK(bm_sample<uint32_t, alg_type::std_fn>)->Apply(common_args);
BENCHMARK(bm_sample<uint64_t, alg_type::std_fn>)->Apply(common_args);

BENCHMARK(bm_sample<uint8_t, alg_type::rng>)->Apply(common_args);
BENCHMARK(bm_sample<uint16_t, alg_type::rng>)->Apply(common_args);
BENCHMARK(bm_sample<uint32_t, alg_type::rng>)->Apply(common_args);
BENCHMARK(bm_sample<uint64_t, alg_type::rng>)->Apply(common_args);

BENCHMARK_MAIN();
