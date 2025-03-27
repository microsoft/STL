// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <assert.h>
#include <benchmark/benchmark.h>
#include <bit>

#include <utility.hpp>

template <typename T>
void has_single_bit_if(benchmark::State& state) {
    const auto random_v = random_vector<T>(8);
    for (auto _ : state) {
        benchmark::DoNotOptimize(random_v);
        unsigned count_true  = 0;
        unsigned count_false = 0;
        for (const auto& x : random_v) {
            if (std::has_single_bit(x)) {
                benchmark::DoNotOptimize(count_true++);
            } else {
                benchmark::DoNotOptimize(count_false++);
            }
        }
    }
}

template <typename T>
void has_single_bit(benchmark::State& state) {
    const auto random_v = random_vector<T>(8);
    assert(random_v.size() % 4 == 0);
    for (auto _ : state) {
        benchmark::DoNotOptimize(random_v);
        unsigned r = 0;
        for (size_t i = 0; i < random_v.size(); i += 4) {
            r += std::has_single_bit(random_v[i + 0]);
            r += std::has_single_bit(random_v[i + 1]);
            r += std::has_single_bit(random_v[i + 2]);
            r += std::has_single_bit(random_v[i + 3]);
        }
        benchmark::DoNotOptimize(r);
    }
}

BENCHMARK(has_single_bit_if<uint16_t>);
BENCHMARK(has_single_bit_if<uint32_t>);
BENCHMARK(has_single_bit_if<uint64_t>);

BENCHMARK(has_single_bit<uint16_t>);
BENCHMARK(has_single_bit<uint32_t>);
BENCHMARK(has_single_bit<uint64_t>);

BENCHMARK_MAIN();
