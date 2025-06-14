// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
#include <functional>
#include <utility>

#ifdef __clang__
#pragma clang diagnostic ignored "-Wunqualified-std-cast-call"
#endif // defined(__clang__)

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
        benchmark::DoNotOptimize(mof);
        auto moved_mof = move(mof);
        benchmark::DoNotOptimize(moved_mof);
    }
}

void mof_construct_and_move(benchmark::State& state) {
    for (auto _ : state) {
        move_only_function<void()> mof;
        benchmark::DoNotOptimize(mof);
        auto moved_mof = move(mof);
        benchmark::DoNotOptimize(moved_mof);
    }
}

BENCHMARK(mof_none);
BENCHMARK(mof_construct);
BENCHMARK(mof_move);
BENCHMARK(mof_construct_and_move);

BENCHMARK_MAIN();
