// Copyright(c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <vector>

using namespace std;

template <class T>
void bm(benchmark::State& state) {
    const size_t size = static_cast<size_t>(state.range(0));
    const size_t pos  = static_cast<size_t>(state.range(1));

    vector<T> v(size);

    for (size_t i = 0; i != size; ++i) {
        v[i] = static_cast<T>(i & 3);
    }

    if (pos == 0 || pos >= size) {
        abort();
    }

    v[pos] = v[pos - 1];

    for (auto _ : state) {
        benchmark::DoNotOptimize(v);
        benchmark::DoNotOptimize(adjacent_find(v.begin(), v.end()));
    }
}

void common_args(auto bm) {
    bm->ArgPair(2525, 1142);
}

BENCHMARK(bm<char>)->Apply(common_args);
BENCHMARK(bm<short>)->Apply(common_args);
BENCHMARK(bm<int>)->Apply(common_args);
BENCHMARK(bm<long long>)->Apply(common_args);

BENCHMARK_MAIN();
