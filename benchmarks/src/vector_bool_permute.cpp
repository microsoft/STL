// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
//
#include <algorithm>
#include <cstddef>
#include <random>
#include <vector>

#include "utility.hpp"

using namespace std;

void perm_check_3(benchmark::State& state) {
    const auto size = static_cast<size_t>(state.range(0));
    vector<bool> v1 = random_vector<bool>(size);
    vector<bool> v2 = random_vector<bool>(size, 1u);

    for (auto _ : state) {
        benchmark::DoNotOptimize(v1);
        benchmark::DoNotOptimize(v2);
        bool r = is_permutation(v1.begin(), v1.end(), v2.begin());
        benchmark::DoNotOptimize(r);
    }
}

void perm_check_4(benchmark::State& state) {
    const auto size = static_cast<size_t>(state.range(0));
    vector<bool> v1 = random_vector<bool>(size);
    vector<bool> v2 = random_vector<bool>(size, 1u);

    for (auto _ : state) {
        benchmark::DoNotOptimize(v1);
        benchmark::DoNotOptimize(v2);
        bool r = is_permutation(v1.begin(), v1.end(), v2.begin(), v2.end());
        benchmark::DoNotOptimize(r);
    }
}

BENCHMARK(perm_check_3)->RangeMultiplier(64)->Range(64, 64 << 10);
BENCHMARK(perm_check_4)->RangeMultiplier(64)->Range(64, 64 << 10);

BENCHMARK_MAIN();
