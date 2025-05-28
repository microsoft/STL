// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "benchmark/benchmark.h"
#include <chrono>

void locate_zone(benchmark::State& state) {
    auto&& tzdb = std::chrono::get_tzdb();
    for (auto _ : state) {
        for (const auto& z : tzdb.zones) {
            auto res = tzdb.locate_zone(z.name());
            benchmark::DoNotOptimize(res);
        }
    }
}

BENCHMARK(locate_zone);

BENCHMARK_MAIN();
