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

void copy_block_aligned(benchmark::State& state) {
    const auto size           = static_cast<size_t>(state.range(0));
    const vector<bool> source = createRandomVector(size);
    vector<bool> dest(size, false);

    for (auto _ : state) {
        copy(source.cbegin(), source.cend(), dest.begin());
    }
}

void copy_source_misaligned(benchmark::State& state) {
    const auto size           = static_cast<size_t>(state.range(0));
    const vector<bool> source = createRandomVector(size);
    vector<bool> dest(size, false);

    for (auto _ : state) {
        copy(source.cbegin() + 1, source.cend(), dest.begin());
    }
}

void copy_dest_misaligned(benchmark::State& state) {
    const auto size           = static_cast<size_t>(state.range(0));
    const vector<bool> source = createRandomVector(size);
    vector<bool> dest(size, false);

    for (auto _ : state) {
        copy(source.cbegin(), source.cend() - 1, dest.begin() + 1);
    }
}

// Special benchmark for matching char alignment
void copy_matching_alignment(benchmark::State& state) {
    const auto size           = static_cast<size_t>(state.range(0));
    const vector<bool> source = createRandomVector(size);
    vector<bool> dest(size, false);

    for (auto _ : state) {
        copy(source.cbegin() + 5, source.cend(), dest.begin() + 5);
    }
}

// Special benchmarks for single block corner case
void copy_both_single_blocks(benchmark::State& state) {
    const vector<bool> source = createRandomVector(50);
    vector<bool> dest(50, false);

    const size_t length = 20;
    for (auto _ : state) {
        copy(source.cbegin() + 5, source.cbegin() + 5 + length, dest.begin() + 5);
    }
}

void copy_source_single_block(benchmark::State& state) {
    const vector<bool> source = createRandomVector(50);
    vector<bool> dest(50, false);

    const size_t length = 20;
    for (auto _ : state) {
        copy(source.cbegin() + 5, source.cbegin() + 5 + length, dest.begin() + 25);
    }
}

void copy_dest_single_block(benchmark::State& state) {
    const vector<bool> source = createRandomVector(50);
    vector<bool> dest(50, false);

    const size_t length = 20;
    for (auto _ : state) {
        copy(source.cbegin() + 25, source.cbegin() + 25 + length, dest.begin() + 5);
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
