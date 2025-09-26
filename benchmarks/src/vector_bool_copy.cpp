// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
//
#include <algorithm>
#include <cstddef>
#include <random>
#include <vector>

#include "utility.hpp"

using namespace std;

void copy_block_aligned(benchmark::State& state) {
    const auto size     = static_cast<size_t>(state.range(0));
    vector<bool> source = random_vector<bool>(size);
    vector<bool> dest(size, false);

    for (auto _ : state) {
        benchmark::DoNotOptimize(source);
        copy(source.cbegin(), source.cend(), dest.begin());
        benchmark::DoNotOptimize(dest);
    }
}

void copy_source_misaligned(benchmark::State& state) {
    const auto size     = static_cast<size_t>(state.range(0));
    vector<bool> source = random_vector<bool>(size);
    vector<bool> dest(size, false);

    for (auto _ : state) {
        benchmark::DoNotOptimize(source);
        copy(source.cbegin() + 1, source.cend(), dest.begin());
        benchmark::DoNotOptimize(dest);
    }
}

void copy_dest_misaligned(benchmark::State& state) {
    const auto size     = static_cast<size_t>(state.range(0));
    vector<bool> source = random_vector<bool>(size);
    vector<bool> dest(size, false);

    for (auto _ : state) {
        benchmark::DoNotOptimize(source);
        copy(source.cbegin(), source.cend() - 1, dest.begin() + 1);
        benchmark::DoNotOptimize(dest);
    }
}

// Special benchmark for matching char alignment
void copy_matching_alignment(benchmark::State& state) {
    const auto size     = static_cast<size_t>(state.range(0));
    vector<bool> source = random_vector<bool>(size);
    vector<bool> dest(size, false);

    for (auto _ : state) {
        benchmark::DoNotOptimize(source);
        copy(source.cbegin() + 5, source.cend(), dest.begin() + 5);
        benchmark::DoNotOptimize(dest);
    }
}

// Special benchmarks for single block corner case
void copy_both_single_blocks(benchmark::State& state) {
    vector<bool> source = random_vector<bool>(50);
    vector<bool> dest(50, false);

    const size_t length = 20;
    for (auto _ : state) {
        benchmark::DoNotOptimize(source);
        copy(source.cbegin() + 5, source.cbegin() + 5 + length, dest.begin() + 5);
        benchmark::DoNotOptimize(dest);
    }
}

void copy_source_single_block(benchmark::State& state) {
    vector<bool> source = random_vector<bool>(50);
    vector<bool> dest(50, false);

    const size_t length = 20;
    for (auto _ : state) {
        benchmark::DoNotOptimize(source);
        copy(source.cbegin() + 5, source.cbegin() + 5 + length, dest.begin() + 25);
        benchmark::DoNotOptimize(dest);
    }
}

void copy_dest_single_block(benchmark::State& state) {
    vector<bool> source = random_vector<bool>(50);
    vector<bool> dest(50, false);

    const size_t length = 20;
    for (auto _ : state) {
        benchmark::DoNotOptimize(source);
        copy(source.cbegin() + 25, source.cbegin() + 25 + length, dest.begin() + 5);
        benchmark::DoNotOptimize(dest);
    }
}

BENCHMARK(copy_block_aligned)->RangeMultiplier(64)->Range(64, 64 << 10);
BENCHMARK(copy_source_misaligned)->RangeMultiplier(64)->Range(64, 64 << 10);
BENCHMARK(copy_dest_misaligned)->RangeMultiplier(64)->Range(64, 64 << 10);
BENCHMARK(copy_matching_alignment)->RangeMultiplier(64)->Range(64, 64 << 10);

BENCHMARK(copy_both_single_blocks);
BENCHMARK(copy_source_single_block);
BENCHMARK(copy_dest_single_block);

BENCHMARK_MAIN();
