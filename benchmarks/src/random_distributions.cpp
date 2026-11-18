// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>

#include <random>
using namespace std;

template <class Distribution>
void bm(benchmark::State& state) {
    mt19937_64 gen;
    gen.discard(1'000);
    Distribution dist(0.0, 1.0);

    while (state.KeepRunning()) {
        benchmark::DoNotOptimize(dist(gen));
    }
}

BENCHMARK(bm<normal_distribution<float>>);
BENCHMARK(bm<normal_distribution<double>>);

BENCHMARK(bm<uniform_real_distribution<float>>);
BENCHMARK(bm<uniform_real_distribution<double>>);

BENCHMARK_MAIN();
