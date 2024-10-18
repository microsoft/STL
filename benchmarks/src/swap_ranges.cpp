// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <vector>

using namespace std;

template <size_t N, class T>
void std_swap(benchmark::State& state) {
    T a[N];
    memset(a, 'a', sizeof(a));
    T b[N];
    memset(b, 'b', sizeof(b));

    for (auto _ : state) {
        swap(a, b);
        benchmark::DoNotOptimize(a);
        benchmark::DoNotOptimize(b);
    }
}

template <class T>
void std_swap_ranges(benchmark::State& state) {
    vector<T> a(static_cast<size_t>(state.range(0)), T{'a'});
    vector<T> b(static_cast<size_t>(state.range(0)), T{'b'});

    for (auto _ : state) {
        swap_ranges(a.begin(), a.end(), b.begin());
        benchmark::DoNotOptimize(a);
        benchmark::DoNotOptimize(b);
    }
}

BENCHMARK(std_swap<1, uint8_t>);
BENCHMARK(std_swap<5, uint8_t>);
BENCHMARK(std_swap<15, uint8_t>);
BENCHMARK(std_swap<26, uint8_t>);
BENCHMARK(std_swap<38, uint8_t>);
BENCHMARK(std_swap<60, uint8_t>);
BENCHMARK(std_swap<125, uint8_t>);
BENCHMARK(std_swap<800, uint8_t>);
BENCHMARK(std_swap<3000, uint8_t>);
BENCHMARK(std_swap<9000, uint8_t>);

BENCHMARK(std_swap_ranges<uint8_t>)
    ->Arg(1)
    ->Arg(5)
    ->Arg(15)
    ->Arg(26)
    ->Arg(38)
    ->Arg(60)
    ->Arg(125)
    ->Arg(800)
    ->Arg(3000)
    ->Arg(9000);

BENCHMARK_MAIN();
