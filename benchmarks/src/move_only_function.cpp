// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
#include <functional>

using namespace std;

void construct(benchmark::State& state) {
    for (auto _ : state) {
        move_only_function<void()> mof;
        benchmark::DoNotOptimize(mof);
    }
}

void move(benchmark::State& state) {
    move_only_function<void()> mof;
    for (auto _ : state) {
        auto mof_move = move(mof);
        benchmark::DoNotOptimize(mof_move);
    }
}

void construct_and_move(benchmark::State& state) {
    for (auto _ : state) {
        move_only_function<void()> mof{};
        benchmark::DoNotOptimize(mof);
        auto mof_move = move(mof);
        benchmark::DoNotOptimize(mof_move);
    }
}

BENCHMARK(construct);
BENCHMARK(move);
BENCHMARK(construct_and_move);

BENCHMARK_MAIN();
