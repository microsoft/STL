// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <vector>

using namespace std;

template <class T>
void bm(benchmark::State& state) {
    vector<T> a(static_cast<size_t>(state.range(0)), T{'a'});
    vector<T> b(static_cast<size_t>(state.range(0)), T{'b'});

    for (auto _ : state) {
        swap_ranges(a.begin(), a.end(), b.begin());
        benchmark::DoNotOptimize(a);
        benchmark::DoNotOptimize(b);
    }
}

BENCHMARK(bm<uint8_t>)->Arg(1)->Arg(5)->Arg(15)->Arg(26)->Arg(38)->Arg(60)->Arg(125)->Arg(800)->Arg(3000)->Arg(9000);

BENCHMARK_MAIN();
