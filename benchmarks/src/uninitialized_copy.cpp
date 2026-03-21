// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>

#include "skewed_allocator.hpp"

using namespace std;

template <size_t N, class T, template <class> class Padder>
void bm_uninitialized_copy(benchmark::State& state) {
    Padder<T[N]> padded_a;
    auto a = &padded_a.value[0];
    memset(a, 'a', sizeof(T) * N);
    Padder<T[N]> padded_b;
    auto b = &padded_b.value[0];
    memset(b, 'b', sizeof(T) * N);

    for (auto _ : state) {
        benchmark::DoNotOptimize(a);
        uninitialized_copy(a, a + N, b);
        benchmark::DoNotOptimize(b);
    }
}

BENCHMARK(bm_uninitialized_copy<1, uint8_t, highly_aligned>);
BENCHMARK(bm_uninitialized_copy<5, uint8_t, highly_aligned>);
BENCHMARK(bm_uninitialized_copy<15, uint8_t, highly_aligned>);
BENCHMARK(bm_uninitialized_copy<26, uint8_t, highly_aligned>);
BENCHMARK(bm_uninitialized_copy<32, uint8_t, highly_aligned>);
BENCHMARK(bm_uninitialized_copy<38, uint8_t, highly_aligned>);
BENCHMARK(bm_uninitialized_copy<60, uint8_t, highly_aligned>);
BENCHMARK(bm_uninitialized_copy<64, uint8_t, highly_aligned>);
BENCHMARK(bm_uninitialized_copy<125, uint8_t, highly_aligned>);
BENCHMARK(bm_uninitialized_copy<800, uint8_t, highly_aligned>);
BENCHMARK(bm_uninitialized_copy<3000, uint8_t, highly_aligned>);
BENCHMARK(bm_uninitialized_copy<9000, uint8_t, highly_aligned>);

BENCHMARK(bm_uninitialized_copy<1, uint8_t, not_highly_aligned>);
BENCHMARK(bm_uninitialized_copy<5, uint8_t, not_highly_aligned>);
BENCHMARK(bm_uninitialized_copy<15, uint8_t, not_highly_aligned>);
BENCHMARK(bm_uninitialized_copy<26, uint8_t, not_highly_aligned>);
BENCHMARK(bm_uninitialized_copy<32, uint8_t, not_highly_aligned>);
BENCHMARK(bm_uninitialized_copy<38, uint8_t, not_highly_aligned>);
BENCHMARK(bm_uninitialized_copy<60, uint8_t, not_highly_aligned>);
BENCHMARK(bm_uninitialized_copy<64, uint8_t, not_highly_aligned>);
BENCHMARK(bm_uninitialized_copy<125, uint8_t, not_highly_aligned>);
BENCHMARK(bm_uninitialized_copy<800, uint8_t, not_highly_aligned>);
BENCHMARK(bm_uninitialized_copy<3000, uint8_t, not_highly_aligned>);
BENCHMARK(bm_uninitialized_copy<9000, uint8_t, not_highly_aligned>);

BENCHMARK_MAIN();
