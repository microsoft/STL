// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
#include <bit>
#include <cstdint>

#include <utility.hpp>

using namespace std;

template <typename T>
void bm_has_single_bit_if(benchmark::State& state) {
    auto random_v = random_vector<T>(8);
    for (auto _ : state) {
        benchmark::DoNotOptimize(random_v);
        unsigned int count_true  = 0;
        unsigned int count_false = 0;
        for (const auto& x : random_v) {
            if (has_single_bit(x)) {
                benchmark::DoNotOptimize(++count_true);
            } else {
                benchmark::DoNotOptimize(++count_false);
            }
        }
    }
}

template <typename T>
void bm_has_single_bit(benchmark::State& state) {
    auto random_v = random_vector<T>(8);
    for (auto _ : state) {
        benchmark::DoNotOptimize(random_v);
        unsigned int r = 0;
        for (const auto& x : random_v) {
            r += has_single_bit(x);
        }
        benchmark::DoNotOptimize(r);
    }
}

BENCHMARK(bm_has_single_bit_if<uint8_t>);
BENCHMARK(bm_has_single_bit_if<uint16_t>);
BENCHMARK(bm_has_single_bit_if<uint32_t>);
BENCHMARK(bm_has_single_bit_if<uint64_t>);

BENCHMARK(bm_has_single_bit<uint8_t>);
BENCHMARK(bm_has_single_bit<uint16_t>);
BENCHMARK(bm_has_single_bit<uint32_t>);
BENCHMARK(bm_has_single_bit<uint64_t>);

BENCHMARK_MAIN();
