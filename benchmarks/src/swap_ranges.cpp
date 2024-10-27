// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "alloc.hpp"

using namespace std;

template <size_t N, class T, template <class> class Padder>
void std_swap(benchmark::State& state) {
    Padder<T[N]> a;
    memset(a.value, 'a', sizeof(a.value));
    Padder<T[N]> b;
    memset(b.value, 'b', sizeof(b.value));

    for (auto _ : state) {
        swap(a.value, b.value);
        benchmark::DoNotOptimize(a.value);
        benchmark::DoNotOptimize(b.value);
    }
}

template <class T, template <class> class Alloc>
void std_swap_ranges(benchmark::State& state) {
    vector<T, Alloc<T>> a(static_cast<size_t>(state.range(0)), T{'a'});
    vector<T, Alloc<T>> b(static_cast<size_t>(state.range(0)), T{'b'});

    for (auto _ : state) {
        swap_ranges(a.begin(), a.end(), b.begin());
        benchmark::DoNotOptimize(a);
        benchmark::DoNotOptimize(b);
    }
}

BENCHMARK(std_swap<1, uint8_t, aligner>);
BENCHMARK(std_swap<5, uint8_t, aligner>);
BENCHMARK(std_swap<15, uint8_t, aligner>);
BENCHMARK(std_swap<26, uint8_t, aligner>);
BENCHMARK(std_swap<38, uint8_t, aligner>);
BENCHMARK(std_swap<60, uint8_t, aligner>);
BENCHMARK(std_swap<125, uint8_t, aligner>);
BENCHMARK(std_swap<800, uint8_t, aligner>);
BENCHMARK(std_swap<3000, uint8_t, aligner>);
BENCHMARK(std_swap<9000, uint8_t, aligner>);

BENCHMARK(std_swap<1, uint8_t, unaligner>);
BENCHMARK(std_swap<5, uint8_t, unaligner>);
BENCHMARK(std_swap<15, uint8_t, unaligner>);
BENCHMARK(std_swap<26, uint8_t, unaligner>);
BENCHMARK(std_swap<38, uint8_t, unaligner>);
BENCHMARK(std_swap<60, uint8_t, unaligner>);
BENCHMARK(std_swap<125, uint8_t, unaligner>);
BENCHMARK(std_swap<800, uint8_t, unaligner>);
BENCHMARK(std_swap<3000, uint8_t, unaligner>);
BENCHMARK(std_swap<9000, uint8_t, unaligner>);

BENCHMARK(std_swap_ranges<uint8_t, aligned_allocator>)
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

BENCHMARK(std_swap_ranges<uint8_t, unaligned_allocator>)
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
