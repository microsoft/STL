// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "benchmark/benchmark.h"
#include <chrono>

void locate_zone(benchmark::State& state) {
    const auto& db = std::chrono::get_tzdb();
    for (auto _ : state) {
        for (const auto& z : db.zones) {
            auto res = db.locate_zone(z.name());
            benchmark::DoNotOptimize(res);
        }
    }
}

BENCHMARK(locate_zone);

BENCHMARK_MAIN();
