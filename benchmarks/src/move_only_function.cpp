// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
#include <functional>

using namespace std;

void mof_none(benchmark::State& state) {
    move_only_function<void()> mof;
    for (auto _ : state) {
        benchmark::DoNotOptimize(mof);
    }
}

void mof_construct(benchmark::State& state) {
    for (auto _ : state) {
        move_only_function<void()> mof;
        benchmark::DoNotOptimize(mof);
    }
}

void mof_move(benchmark::State& state) {
    move_only_function<void()> mof;
    for (auto _ : state) {
        auto mof_move = move(mof);
        benchmark::DoNotOptimize(mof_move);
    }
}

void mof_construct_and_move(benchmark::State& state) {
    for (auto _ : state) {
        move_only_function<void()> mof{};
        benchmark::DoNotOptimize(mof);
        auto mof_move = move(mof);
        benchmark::DoNotOptimize(mof_move);
    }
}

BENCHMARK(mof_none);
BENCHMARK(mof_construct);
BENCHMARK(mof_move);
BENCHMARK(mof_construct_and_move);

BENCHMARK_MAIN();
