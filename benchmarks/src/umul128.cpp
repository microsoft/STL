// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>

#include <__msvc_int128.hpp>
#include <cstdint>
#include <random>
#include <utility>
#include <vector>

using namespace std;

void bm_umul128(benchmark::State& state) {
    vector<pair<uint64_t, uint64_t>> vec(1'000'000);

    {
        mt19937_64 mt64;
        for (auto& [val1, val2] : vec) {
            val1 = mt64();
            val2 = mt64();
        }
    }

    auto it       = vec.begin();
    uint64_t low  = 0;
    uint64_t high = 0;

    for (auto _ : state) {
        low = _Base128::_UMul128(it->first, it->second, high);

        benchmark::DoNotOptimize(low);
        benchmark::DoNotOptimize(high);
        if (++it == vec.end()) {
            it = vec.begin();
        }
    }
}

BENCHMARK(bm_umul128);

BENCHMARK_MAIN();
