// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <ranges>
#include <vector>

using namespace std;

constexpr int64_t no_pos = -1;

template <class T>
void bm(benchmark::State& state) {
    vector<T> a(static_cast<size_t>(state.range(0)), T{'.'});
    vector<T> b(static_cast<size_t>(state.range(0)), T{'.'});

    if (state.range(1) != no_pos) {
        b.at(static_cast<size_t>(state.range(1))) = 'x';
    }

    for (auto _ : state) {
        benchmark::DoNotOptimize(ranges::mismatch(a, b));
    }
}

#define COMMON_ARGS Args({8, 3})->Args({24, 22})->Args({105, -1})->Args({4021, 3056})

BENCHMARK(bm<uint8_t>)->COMMON_ARGS;
BENCHMARK(bm<uint16_t>)->COMMON_ARGS;
BENCHMARK(bm<uint32_t>)->COMMON_ARGS;
BENCHMARK(bm<uint64_t>)->COMMON_ARGS;

BENCHMARK_MAIN();
