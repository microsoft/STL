// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>
#include <locale>
using namespace std;

// GH-3048 <locale>: Double-checked locking for locale::classic
static void BM_locale_classic(benchmark::State& state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(locale::classic());
    }
}
BENCHMARK(BM_locale_classic);

BENCHMARK_MAIN();
