// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <benchmark/benchmark.h>

#include <__msvc_int128.hpp>
#include <random>
#include <utility>
#include <vector>

using namespace std;

vector<pair<_Signed128, _Signed128>> init_test_set() {
    vector<pair<_Signed128, _Signed128>> vec(1'000'000);
    mt19937_64 mt64{};
    for (auto& [val1, val2] : vec) {
        val1._Word[0] = mt64();
        val1._Word[1] = mt64();
        val2._Word[0] = mt64();
        val2._Word[1] = mt64();
    }
    return vec;
}

void bm_signed128_mul(benchmark::State& state) {
    const auto vec = init_test_set();

    auto it        = vec.begin();
    _Signed128 res = 0;

    for (auto _ : state) {
        res = it->first * it->second;

        benchmark::DoNotOptimize(res);
        ++it;
        if (it == vec.end()) {
            it = vec.begin();
        }
    }
}

BENCHMARK(bm_signed128_mul);

BENCHMARK_MAIN();
