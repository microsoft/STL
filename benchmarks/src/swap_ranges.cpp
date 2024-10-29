// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "skewed_allocator.hpp"

using namespace std;

template <size_t N, class T, template <class> class Padder>
void std_swap(benchmark::State& state) {
    Padder<T[N]> padded_a;
    auto& a = padded_a.value;
    memset(a, 'a', sizeof(a));
    Padder<T[N]> padded_b;
    auto& b = padded_b.value;
    memset(b, 'b', sizeof(b));

    for (auto _ : state) {
        swap(a, b);
        benchmark::DoNotOptimize(a);
        benchmark::DoNotOptimize(b);
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

BENCHMARK(std_swap<1, uint8_t, highly_aligned>);
BENCHMARK(std_swap<5, uint8_t, highly_aligned>);
BENCHMARK(std_swap<15, uint8_t, highly_aligned>);
BENCHMARK(std_swap<26, uint8_t, highly_aligned>);
BENCHMARK(std_swap<38, uint8_t, highly_aligned>);
BENCHMARK(std_swap<60, uint8_t, highly_aligned>);
BENCHMARK(std_swap<125, uint8_t, highly_aligned>);
BENCHMARK(std_swap<800, uint8_t, highly_aligned>);
BENCHMARK(std_swap<3000, uint8_t, highly_aligned>);
BENCHMARK(std_swap<9000, uint8_t, highly_aligned>);

BENCHMARK(std_swap<1, uint8_t, not_highly_aligned>);
BENCHMARK(std_swap<5, uint8_t, not_highly_aligned>);
BENCHMARK(std_swap<15, uint8_t, not_highly_aligned>);
BENCHMARK(std_swap<26, uint8_t, not_highly_aligned>);
BENCHMARK(std_swap<38, uint8_t, not_highly_aligned>);
BENCHMARK(std_swap<60, uint8_t, not_highly_aligned>);
BENCHMARK(std_swap<125, uint8_t, not_highly_aligned>);
BENCHMARK(std_swap<800, uint8_t, not_highly_aligned>);
BENCHMARK(std_swap<3000, uint8_t, not_highly_aligned>);
BENCHMARK(std_swap<9000, uint8_t, not_highly_aligned>);

BENCHMARK(std_swap_ranges<uint8_t, highly_aligned_allocator>)
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

BENCHMARK(std_swap_ranges<uint8_t, not_highly_aligned_allocator>)
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
