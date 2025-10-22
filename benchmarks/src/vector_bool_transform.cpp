// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
//
#include <algorithm>
#include <cstddef>
#include <functional>
#include <random>
#include <vector>

#include "skewed_allocator.hpp"
#include "utility.hpp"

using namespace std;

template <class Pred>
void transform_one_input_aligned(benchmark::State& state) {
    const auto size = static_cast<size_t>(state.range(0));
    auto source     = random_vector<bool, not_highly_aligned_allocator>(size);
    vector<bool> dest(size, false);

    for (auto _ : state) {
        benchmark::DoNotOptimize(source);
        transform(source.begin(), source.end(), dest.begin(), Pred{});
        benchmark::DoNotOptimize(dest);
    }
}

template <class Pred>
void transform_two_inputs_aligned(benchmark::State& state) {
    const auto size = static_cast<size_t>(state.range(0));
    auto source1    = random_vector<bool, not_highly_aligned_allocator>(size);
    auto source2    = random_vector<bool, not_highly_aligned_allocator>(size, 1729u);
    vector<bool> dest(size, false);

    for (auto _ : state) {
        benchmark::DoNotOptimize(source1);
        benchmark::DoNotOptimize(source2);
        transform(source1.begin(), source1.end(), source2.begin(), dest.begin(), Pred{});
        benchmark::DoNotOptimize(dest);
    }
}

void common_args(auto bm) {
    bm->RangeMultiplier(64)->Range(64, 64 << 10);
}

BENCHMARK(transform_two_inputs_aligned<logical_and<>>)->Apply(common_args);
BENCHMARK(transform_two_inputs_aligned<logical_or<>>)->Apply(common_args);
BENCHMARK(transform_one_input_aligned<logical_not<>>)->Apply(common_args);

BENCHMARK_MAIN();
