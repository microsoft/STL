// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "benchmark/benchmark.h"
#include <chrono>

void locate_zone(benchmark::State& state) {
    auto&& tzdb = std::chrono::get_tzdb();
    for (auto _ : state) {
        for (std::size_t i = 0; i != tzdb.zones.size(); ++i) {
            auto* res = tzdb.locate_zone(tzdb.zones[i].name());
            benchmark::DoNotOptimize(res);
        }
    }
}

BENCHMARK(locate_zone);

BENCHMARK_MAIN();
