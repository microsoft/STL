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
    static mt19937 gen;
    vector<bool> result(size);
    generate_n(result.begin(), size, [] { return bernoulli_distribution{0.5}(gen); });
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
