// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
//
#include <algorithm>
#include <cstddef>
#include <random>
#include <vector>

using namespace std;

vector<bool> createRandomVector(const size_t size) {
    mt19937 gen;
    bernoulli_distribution dist{0.5};
    vector<bool> result(size);
    generate(result.begin(), result.end(), [&] { return dist(gen); });
    return result;
}

void count_aligned(benchmark::State& state) {
    const auto size = static_cast<size_t>(state.range(0));
    vector<bool> v  = createRandomVector(size);

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
