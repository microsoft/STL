// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
//
#include <algorithm>
#include <cstddef>
#include <random>
#include <vector>

using namespace std;

vector<bool> createRandomVector(const size_t size) {
    static mt19937 gen;
    vector<bool> result(size);
    generate_n(result.begin(), size, [] { return bernoulli_distribution{0.5}(gen); });
    return result;
}

void copy_n_block_aligned(benchmark::State& state) {
    const auto size           = static_cast<size_t>(state.range(0));
    const vector<bool> source = createRandomVector(size);
    vector<bool> dest(size, false);

    for (auto _ : state) {
        copy_n(source.cbegin(), size, dest.begin());
    }
}

void copy_n_source_misaligned(benchmark::State& state) {
    const auto size           = static_cast<size_t>(state.range(0));
    const vector<bool> source = createRandomVector(size);
    vector<bool> dest(size, false);

    for (auto _ : state) {
        copy_n(source.cbegin() + 1, size - 1, dest.begin());
    }
}

void copy_n_dest_misaligned(benchmark::State& state) {
    const auto size           = static_cast<size_t>(state.range(0));
    const vector<bool> source = createRandomVector(size);
    vector<bool> dest(size, false);

    for (auto _ : state) {
        copy_n(source.cbegin(), size - 1, dest.begin() + 1);
    }
}

// Special benchmark for matching char alignment
void copy_n_matching_alignment(benchmark::State& state) {
    const auto size           = static_cast<size_t>(state.range(0));
    const vector<bool> source = createRandomVector(size);
    vector<bool> dest(size, false);

    for (auto _ : state) {
        copy_n(source.cbegin() + 5, size - 5, dest.begin() + 5);
    }
}

// Special benchmarks for single block corner case
void copy_n_both_single_blocks(benchmark::State& state) {
    const vector<bool> source = createRandomVector(50);
    vector<bool> dest(50, false);

    const size_t length = 20;
    for (auto _ : state) {
        copy_n(source.cbegin() + 5, length, dest.begin() + 5);
    }
}

void copy_n_source_single_block(benchmark::State& state) {
    const vector<bool> source = createRandomVector(50);
    vector<bool> dest(50, false);

    const size_t length = 20;
    for (auto _ : state) {
        copy_n(source.cbegin() + 5, length, dest.begin() + 25);
    }
}

void copy_n_dest_single_block(benchmark::State& state) {
    const vector<bool> source = createRandomVector(50);
    vector<bool> dest(50, false);

    const size_t length = 20;
    for (auto _ : state) {
        copy_n(source.cbegin() + 25, length, dest.begin() + 5);
    }
}

BENCHMARK(copy_n_block_aligned)->RangeMultiplier(64)->Range(64, 64 << 10);
BENCHMARK(copy_n_source_misaligned)->RangeMultiplier(64)->Range(64, 64 << 10);
BENCHMARK(copy_n_dest_misaligned)->RangeMultiplier(64)->Range(64, 64 << 10);
BENCHMARK(copy_n_matching_alignment)->RangeMultiplier(64)->Range(64, 64 << 10);

BENCHMARK(copy_n_both_single_blocks);
BENCHMARK(copy_n_source_single_block);
BENCHMARK(copy_n_dest_single_block);

BENCHMARK_MAIN();
