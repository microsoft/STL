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

void count_aligned(benchmark::State& state) {
    const auto size = static_cast<size_t>(state.range(0));
    vector<bool> v  = random_vector<bool>(size);

    bool b = false;

    for (auto _ : state) {
        benchmark::DoNotOptimize(b);
        benchmark::DoNotOptimize(v);
        auto r = count(v.cbegin(), v.cend(), b);
        benchmark::DoNotOptimize(r);
        b = !b;
    }
}

BENCHMARK(count_aligned)->RangeMultiplier(64)->Range(64, 64 << 10);

BENCHMARK_MAIN();
