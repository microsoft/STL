// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
#include <numeric>
#include <vector>

template <class T>
void bm(benchmark::State& state) {
    const size_t size = static_cast<size_t>(state.range(0));

    std::vector<T> a(size);

    for (auto _ : state) {
        std::iota(a.begin(), a.end(), T{22});
        benchmark::DoNotOptimize(a);
    }
}

void common_args(auto bm) {
    bm->Arg(7)->Arg(18)->Arg(43)->Arg(131)->Arg(315)->Arg(1212);
}

BENCHMARK(bm<std::uint32_t>)->Apply(common_args);
BENCHMARK(bm<std::uint64_t>)->Apply(common_args);

BENCHMARK_MAIN();
